#include "http_server.h"

using boost::asio::ip::tcp;

void session::start(){
    this->do_read();
}

void session::do_read(){
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
    [this, self](boost::system::error_code ec, std::size_t length){
        if(!ec){
            data_[length] = '\0';
            std::string requestContent = data_; 
            httpParser(requestContent);
            do_read();
        }
    });
}

void session::httpParser(std::string requestContent){
    /* template */
    // GET /xxx.htm HTTP/1.1
    // Host: nplinux2.cs.nctu.edu.tw:7000
    // Connection: keep-alive
    // Cache-Control: max-age=0
    // Upgrade-Insecure-Requests: 1
    // User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.55 Safari/537.36
    // Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9
    // Accept-Encoding: gzip, deflate
    // Accept-Language: zh-TW,zh;q=0.9,en-US;q=0.8,en;q=0.7
    // Cookie: _ga=GA1.3.1865436702.1624630189

    /* required envals */
    // REQUEST METHOD
    // REQUEST URI
    // QUERY STRING
    // SERVER PROTOCOL
    // HTTP HOST
    // SERVER ADDR
    // SERVER PORT
    // REMOTE ADDR
    // REMOTE PORT
    std::string targetFile; // panel.cgi or console.cgi
    
    // std::cout << requestContent << std::endl;
}

void server::do_accept(){
    acceptor_.async_accept(
    [this](boost::system::error_code ec, tcp::socket socket){
        if(!ec){
            /* if no error occur, server trans the ownership of socket to session */
            std::make_shared<session>(std::move(socket))->start();
        }
        /* ready to accept another connection request */  
        do_accept();
    });
  }

int main(int argc, char* argv[]){
    try{
        if(argc != 2){
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }
        boost::asio::io_context io_context;
        server s(io_context, std::atoi(argv[1]));
        io_context.run();
    }
    catch(std::exception& e){
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}