#include "http_server.h"

// http://140.113.235.222:7000/panel.cgi

using boost::asio::ip::tcp;

void session::start(){
    this->do_read();
}

void session::do_read(){
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
    [this, self](boost::system::error_code ec, std::size_t length){
        if(!ec){
            data2Msg(data_, requestContent, length);
            clearBuffer(data_);
            char *argv[] = { nullptr };
            httpParser(requestContent);
            std::string targetFile;
            pid_t pid = 0;
refork:
            switch ((int)(pid = fork())){
            case -1:
                while(waitpid(-1, NULL, WNOHANG) > 0){};
                goto refork;
            case 0:
                // escape question mark!
                targetFile = this->envVector[1].second + "?";
                targetFile = "." + targetFile.substr(0, targetFile.find('?', 0));
                for(auto env : this->envVector){
                    setenv(env.first.c_str(), env.second.c_str(), 1);
                }
                dup2(socket_.native_handle(), STDOUT_FILENO);
                socket_.close();
                std::cout << "HTTP/1.1 200 OK\r\n" << std::flush;
                execv(targetFile.c_str(), argv);
                exit(0);
            default:
                socket_.close();
                waitpid(pid, NULL, WNOHANG);
            }
        }
    });
}

void session::httpParser(std::string requestContent){
    /* parse http req and store into envVector for setenv purpose */
    this->envVector.clear();
    std::pair<std::string, std::string> envPair[9];
    int endOfFirstLine, 
    endOfSecLine, 
    start, 
    end;

    endOfFirstLine = requestContent.find('\n', 0); // GET /panel.cgi HTTP/1.1
    endOfSecLine = requestContent.find('\n', endOfFirstLine + 1); // Host: 140.113.235.222:7000
    std::string firstLine = requestContent.substr(0, endOfFirstLine - 1);
    std::string secLine = requestContent.substr(endOfFirstLine + 1, endOfSecLine - (endOfFirstLine + 2));
    start = 0;
    end = firstLine.find(' ', 0);

    envPair[0].first = "REQUEST_METHOD";
    envPair[0].second = firstLine.substr(start, end - start);

    start = end + 1;
    /* has query string? */
    if((end = firstLine.find('?', start)) != -1){
        end = firstLine.find('?', start);
        envPair[1].first = "REQUEST_URI";
        envPair[1].second = firstLine.substr(start, end - start);

        start = end + 1;
        end = firstLine.find(' ', start);
        envPair[2].first = "QUERY_STRING";
        envPair[2].second = firstLine.substr(start, end - start);
        envPair[1].second = envPair[1].second + "?" + envPair[2].second;
    }else{
        end = firstLine.find(' ', start);
        envPair[1].first = "REQUEST_URI";
        envPair[1].second = firstLine.substr(start, end - start);
        envPair[2].first = "QUERY_STRING";
        envPair[2].second = "";
    }

    start = end + 1;
    envPair[3].first = "SERVER_PROTOCOL";
    envPair[3].second = firstLine.substr(start);

    secLine = secLine.substr(secLine.find(' ', 0) + 1);
    envPair[4].first = "HTTP_HOST";
    envPair[4].second = secLine;
    envPair[5].first = "SERVER_ADDR";
    envPair[5].second = socket_.local_endpoint().address().to_string();
    envPair[6].first = "SERVER_PORT";
    envPair[6].second = secLine.substr(secLine.find(':', 0) + 1);
    envPair[7].first = "REMOTE_ADDR";
    envPair[7].second = socket_.remote_endpoint().address().to_string();
    envPair[8].first = "REMOTE_PORT";
    envPair[8].second = std::to_string(htons(socket_.remote_endpoint().port()));

    for(int i = 0; i < 9; i++){
        std::cout << envPair[i].first << ": " << envPair[i].second << std::endl;
        this->envVector.push_back(envPair[i]);
    }
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
            std::cerr << "Usage: http_server <port>\n";
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