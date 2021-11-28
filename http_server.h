#ifndef SERVER_H
#define SERVER_H

#include <utility>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using boost::asio::ip::tcp;

#define clearBuffer(data) bzero(data, sizeof(data))

class session:public std::enable_shared_from_this<session>{
public:
    session(tcp::socket socket):socket_(std::move(socket)){}
    void start();
private:
    void do_read();
    void httpParser(std::string requestContent);

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  std::vector< std::pair<std::string, std::string> > envVector;
};

class server
{
public:
  server(boost::asio::io_context& io_context, short port):acceptor_(io_context, tcp::endpoint(tcp::v4(), port)){
    do_accept();
  }

private:

  void do_accept();

  tcp::acceptor acceptor_;
};

#endif