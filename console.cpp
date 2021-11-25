#include "console.h"


using boost::asio::ip::tcp;


void controller::start(){
    this->do_resolve();
}

void controller::do_resolve(){
    user_t userData;
    userData = htmlGen::getInstance().userTable[stoi(id)];
    auto self(shared_from_this());
    resolver.async_resolve(userData.url, userData.port,
        [this, self](const boost::system::error_code &ec,
            const boost::asio::ip::tcp::resolver::results_type results){
            if(!ec){
                endpoints = results;
                do_connect();
            } else {
            socket_.close();
        }
        }
    );
}

void controller::do_connect(){

}

void controller::do_read(){
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
    [this, self](boost::system::error_code ec, std::size_t length){
        if(!ec){
        }
    });
}


int main(int argc, char* argv[]){
    htmlGen::getInstance().do_parseString();
    htmlGen::getInstance().sendHtmlFrame();
    try{
        boost::asio::io_context io_context;
        for(int i = 0; i < USER_NUM; i++){
            user_t* userTable = htmlGen::getInstance().userTable;
            if(userTable[i].url == ""){
                break;
            }else{
                /* TODO: send user frame */
                std::make_shared<controller>(io_context, std::to_string(i))->start();
            }
        }

        io_context.run();
    }
    catch(std::exception& e){
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}