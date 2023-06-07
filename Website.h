#ifndef website_h
#define website_h
#include <string>

class Website{
    public:
        std::string url;
        std::string html;

        Website(std::string url, std::string html){
            this->url = url;
            this->html = html;
        }

        Website(std::string& url){
            this->url = url;
            this->html = "";
        }

        Website(){};

        void set_html(std::string html){
            this->html = html; 
        }

        std::string get_html(){
            return this->html; 
        }
        std::string get_url(){
            return this->url; 
        }

        bool operator==(const Website &a){
            return ((url == a.url));
	        }

        int hash(){
            return std::hash<std::string>{}(url);
        }

        void print(){
            std::cout << url;
        }

        std::string toString(){
            return url;
        }
};

#endif 