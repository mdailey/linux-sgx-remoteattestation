#ifndef SERVER_H
#define SERVER_H

#include "Session.h"
#include "LogBase.h"

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

typedef function<void(Session *)> CallbackSessionStart;

class Server {

    typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

public:
    Server(boost::asio::io_service& io_service, int port);
    virtual ~Server();
    void handle_accept(Session* new_session, const boost::system::error_code& error);
    void start_accept();
    void setCallbackSessionStart(CallbackSessionStart css);

private:
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ssl::context context_;
    CallbackSessionStart callback_session_start;

};

#endif