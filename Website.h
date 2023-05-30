#ifndef website_h
#define website_h
#include <string>

class Website{
    public:
        std::string url;
        std::string html; //DOES THIS TAKE SHIT TON OF MEMORY

        Website(std::string url, std::string html){
            this->url = url;
            this->html = html;
        }

        Website(std::string& url){
            this->url = url;
            this->html = "";
        }

        Website(){};

        bool operator==(const Website &a){
            return ((url == a.url));// && (html == a.html));
	        }

        int hash(){
            return std::hash<std::string>{}(url);
        }

        void print(){
            std::cout << url << std::endl;
        }
};

#endif 