#include <boost/beast.hpp>
#include <iostream>
#ifdef WIN32
    #include <windows.h>
#else
    #include <cstdlib>
#endif
#include <variant.hpp>

#include "client.hpp"
#include "errors.hpp"
#include "mtx.hpp"

//
// Simple usage example of the /login & /sync endpoints which
// will print the stream of messages from all rooms as received by the client.
//

using namespace std;
using namespace mtx::client;
using namespace mtx::events;

using TimelineEvent = mtx::events::collections::TimelineEvents;

void
print_errors(RequestErr err)
{
    if (err->status_code != boost::beast::http::status::unknown)
        cout << err->status_code << "\n";
    if (!err->matrix_error.error.empty())
        cout << err->matrix_error.error << "\n";
    if (err->error_code)
        cout << err->error_code.message() << "\n";
}

// Check if the given event has a textual representation.
bool
is_room_message(const TimelineEvent &event)
{
    return mpark::holds_alternative<mtx::events::RoomEvent<msg::Audio>>(event) ||
           mpark::holds_alternative<mtx::events::RoomEvent<msg::Emote>>(event) ||
           mpark::holds_alternative<mtx::events::RoomEvent<msg::File>>(event) ||
           mpark::holds_alternative<mtx::events::RoomEvent<msg::Image>>(event) ||
           mpark::holds_alternative<mtx::events::RoomEvent<msg::Notice>>(event) ||
           mpark::holds_alternative<mtx::events::RoomEvent<msg::Text>>(event) ||
           mpark::holds_alternative<mtx::events::RoomEvent<msg::Video>>(event);
}

// Retrieves the fallback body value from the event.
std::string
get_body(const TimelineEvent &event)
{
    if (mpark::holds_alternative<RoomEvent<msg::Audio>>(event))
        return mpark::get<RoomEvent<msg::Audio>>(event).content.body;
    else if (mpark::holds_alternative<RoomEvent<msg::Emote>>(event))
        return mpark::get<RoomEvent<msg::Emote>>(event).content.body;
    else if (mpark::holds_alternative<RoomEvent<msg::File>>(event))
        return mpark::get<RoomEvent<msg::File>>(event).content.body;
    else if (mpark::holds_alternative<RoomEvent<msg::Image>>(event))
        return mpark::get<RoomEvent<msg::Image>>(event).content.body;
    else if (mpark::holds_alternative<RoomEvent<msg::Notice>>(event))
        return mpark::get<RoomEvent<msg::Notice>>(event).content.body;
    else if (mpark::holds_alternative<RoomEvent<msg::Text>>(event))
        return mpark::get<RoomEvent<msg::Text>>(event).content.body;
    else if (mpark::holds_alternative<RoomEvent<msg::Video>>(event))
        return mpark::get<RoomEvent<msg::Video>>(event).content.body;

    return "";
}

// Retrieves the sender of the event.
std::string
get_sender(const TimelineEvent &event)
{
    return mpark::visit([](auto e) { return e.sender; }, event);
}

// Simple print of the message contents.
void
print_message(const TimelineEvent &event)
{
    if (is_room_message(event))
        cout << get_sender(event) << ": " << get_body(event) << "\n";
}

// Callback to executed after a /sync request completes.
void
sync_handler(shared_ptr<Client> client, const mtx::responses::Sync &res, RequestErr err)
{
    if (err) {
        cout << "sync error:\n";
        print_errors(err);

        client->sync(
                "",
                client->next_batch_token(),
                false,
                30000,
                std::bind(&sync_handler, client, std::placeholders::_1, std::placeholders::_2));

        return;
    }

    for (const auto room : res.rooms.join) {
        for (const auto msg : room.second.timeline.events)
            print_message(msg);
    }

    client->set_next_batch_token(res.next_batch);

    client->sync(
            "",
            client->next_batch_token(),
            false,
            30000,
            std::bind(&sync_handler, client, std::placeholders::_1, std::placeholders::_2));
}

// Callback to executed after the first (initial) /sync request completes.
void
initial_sync_handler(shared_ptr<Client> client, const nlohmann::json &res, RequestErr err)
{
    if (err) {
        cout << "error during initial sync:\n";
        print_errors(err);

        if (err->status_code != boost::beast::http::status::ok) {
            cout << "retrying initial sync ...\n";
            client->sync("",
                         "",
                         false,
                         0,
                         std::bind(&initial_sync_handler,
                                   client,
                                   std::placeholders::_1,
                                   std::placeholders::_2));
        }

        return;
    }

    client->set_next_batch_token(res.at("next_batch"));

    client->sync(
            "",
            client->next_batch_token(),
            false,
            30000,
            std::bind(&sync_handler, client, std::placeholders::_1, std::placeholders::_2));
}

#ifdef WIN32
    string getpassword(const char *prompt, bool show_asterisk=true)
    {
        const char BACKSPACE=8;
        const char RETURN=13;

        string password;
        unsigned char ch=0;

        cout <<prompt<<endl;

        DWORD con_mode;
        DWORD dwRead;

        HANDLE hIn=GetStdHandle(STD_INPUT_HANDLE);

        GetConsoleMode( hIn, &con_mode );
        SetConsoleMode( hIn, con_mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT) );

        while(ReadConsoleA( hIn, &ch, 1, &dwRead, NULL) && ch !=RETURN)
        {
            if(ch==BACKSPACE)
            {
                if(password.length()!=0)
                {
                    if(show_asterisk)
                        cout <<"\b \b";
                    password.resize(password.length()-1);
                }
            }
            else
            {
                password+=ch;
                if(show_asterisk)
                    cout <<'*';
            }
        }
        cout <<endl;
        return password;
    }
#else
    string getpassword(const char *prompt, bool show_asterisk=true)
    {
        return getpass("Password: ");
    }
#endif

int
main()
{
    std::string username, server, password;

    cout << "Username: ";
    std::getline(std::cin, username);

    cout << "HomeServer: ";
    std::getline(std::cin, server);

    password = getpassword("Password: ");

    auto client = std::make_shared<Client>(server);

    client->login(
            username, password, [client](const mtx::responses::Login &res, RequestErr err) {
                if (err) {
                    cout << "There was an error during login: " << err->matrix_error.error
                         << "\n";
                    return;
                }

                cout << "Logged in as: " << res.user_id.to_string() << "\n";
                client->set_access_token(res.access_token);

                client->sync(
                        "",
                        "",
                        false,
                        0,
                        std::bind(
                                &initial_sync_handler, client, std::placeholders::_1, std::placeholders::_2));
            });

    client->close();

    return 0;
}