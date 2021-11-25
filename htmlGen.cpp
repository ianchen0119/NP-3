#include "console.h"

htmlGen& htmlGen::getInstance(){
    static htmlGen instance;
    return instance;
}

std::string htmlGen::getQueryString(){
    return getenv("QUERY_STRING");
}

void htmlGen::do_parseString(){
    std::string envVal = this->getQueryString() + "&";
    std::string requestBlock;
    int start, end, index;
    start = 0;
    index = 0;
    while((end = envVal.find('&', start)) != -1){
        requestBlock = envVal.substr(start, end - start);
        if(requestBlock.length() == 3)
            break;
        if(index % 3 == 0){
            userTable[index/3].url = requestBlock.substr(3);
        }else if (index % 3 == 1){
            userTable[index/3].port = requestBlock.substr(3);
        }else{
            userTable[index/3].file = requestBlock.substr(3);
        }
        index++;
        start = end + 1;
    }
}

void htmlGen::sendHtmlFrame(){
    std::cout << "Content-type: text/html\r\n\r\n";
    std::cout << "\
    <!DOCTYPE html>\
    <html lang=\"en\">\
        <head>\
            <meta charset=\"UTF-8\" />\
            <title>NP Project 3 Console</title>\
            <link\
                rel=\"stylesheet\"\
                href=\"https://cdn.jsdelivr.net/npm/bootstrap@4.5.3/dist/css/bootstrap.min.css\"\
                integrity=\"sha384-TX8t27EcRE3e/ihU7zmQxVncDAy5uIKz4rEkgIXeMed4M0jlfIDPvg6uqKI2xXr2\"\
                crossorigin=\"anonymous\"\
            />\
            <link\
                href=\"https://fonts.googleapis.com/css?family=Source+Code+Pro\"\
                rel=\"stylesheet\"\
            />\
            <link\
                rel=\"icon\"\
                type=\"image/png\"\
                href=\"https://cdn0.iconfinder.com/data/icons/small-n-flat/24/678068-terminal-512.png\"\
            />\
            <style>\
            * {\
                font-family: 'Source Code Pro', monospace;\
                font-size: 1rem !important;\
            }\
            body {\
                background-color: #212529;\
            }\
            pre {\
                color: #cccccc;\
            }\
            b {\
                color: #01b468;\
            }\
            </style>\
        </head>\
        <body>\
            <table class=\"table table-dark table-bordered\">\
                <thead>\
                    <tr id=\"table_head\">\
                    </tr>\
                </thead>\
                <tbody>\
                    <tr id=\"table_body\">\
                    </tr>\
                </tbody>\
            </table>\
        </body>\
    </html>" << std::flush;
}