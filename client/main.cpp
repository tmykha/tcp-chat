#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "comm.pb.h"
#include "logger.h"
#include "comm.pb.h"
#include "client.h"
#include "config.h" // FIXME: Server header
#include "message_helpers.inl"

#define _SERVER_HOST_ "localhost"

int main()
{
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    try {
        // TODO: Add configuring host
        chat::client client(_SERVER_HOST_, std::to_string(_CHAT_SERVER_PORT_));
        std::ostringstream log;
        log << "Start client. Try to connect to " << _SERVER_HOST_ << ":" << _CHAT_SERVER_PORT_ << "...";
        LOG_GLOBAL_MSG(log.str())
        log.clear();

        // Set client read callback
        client.on_read_msg(
            [](chat::Message msg){
                std::ostringstream oss;
                oss << "[" 
                    << (msg.has_target() ? msg.target() : "ROOM")
                    << "] "
                    << msg.payload();
                std::cout << oss.str() << std::endl;
            }
        );

        // Start client in other thread
        std::thread client_thr(
            [&client](){ client.run(); }
        );

        // Setup client params
        std::string user_nickname, room_password;
        do {
            std::cout << "Please enter your nickname: ";
            std::cin >> user_nickname;
            std::cout << "Please enter the room password: ";
            std::cin >> room_password;
            // TODO: Add quit mechanism on this step
        } while (!client.join_room(user_nickname, room_password));

        log << "Enter the chat room as [" << user_nickname << "]";
        LOG_GLOBAL_MSG(log.str())
        log.clear();

        std::string msg_str;
        while (std::cin >> msg_str)
        {
            if (msg_str == "/q") {
                break;
            }
            else {
                chat::Message msg = chat::message::parce_from_string(msg_str);
                client.send(msg);
            }
        }

        client.end();
        client_thr.join();
    }
    catch (std::exception& e) {
        // TODO: add custom client exceptions
        std::cerr << "Exception: " << e.what() << "\n";
    }

    // Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}