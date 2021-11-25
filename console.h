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

#define USER_NUM 5

using boost::asio::ip::tcp;

typedef struct user{
  std::string url;
  std::string port;
  std::string file;
}user_t;

class controller:public std::enable_shared_from_this<controller>{
public:
    controller(boost::asio::io_context& io_context,
            std::string id)
            :resolver(io_context), 
            socket_(io_context), 
            io_context(io_context), 
            id(id){}
    void start();
private:
    void do_read();
    void do_resolve();
    void do_connect();

    tcp::resolver resolver;
    tcp::socket socket_;
    boost::asio::io_context& io_context;
    std::string id;
    boost::asio::ip::tcp::resolver::results_type endpoints;
    enum { max_length = 4096 };
    char data_[max_length];
  
};

class htmlGen{
  public:
    void do_parseString();
    std::string getQueryString();
    static htmlGen& getInstance();
    /* content-type + head + body */
    void sendHtmlFrame();
    user_t userTable[USER_NUM];
  private:
    htmlGen() = default;
    ~htmlGen() = default;
};

#endif