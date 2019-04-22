#pragma once
#include <boost/asio.hpp>
#include "room.hpp"
#include "logger.h"

using boost::asio::ip::tcp;

namespace chat {
    class server
        : private Loggable
    {
        LOG_MODULE("SRV")
    public:
        server(const server&) = delete;
        server& operator=(const server&) = delete;

        server(const unsigned int port);
        void run();
    
    private:
        void accept();

        const int _port;
        room _room;
        boost::asio::io_service _io;
        tcp::endpoint _endpoint;
        tcp::acceptor _acceptor;
        tcp::socket _socket;
    };
}
