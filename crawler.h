#ifndef crawler_h
#define crawler_h

#include <vector>
#include <string>
#include "HashTable.cpp"
#include "Website.h"

#include "url_parser.h"
#include "downloader.h"
#include "link_extractor.h"

//Note: Problems marked with OBS

class Crawler{
    public:

    StripedHashSet<Website> hashtable;
    int count;

    Crawler(){};

    Crawler(std::string first_link,int max_depth){
        //We have initialise
        //initialise hashtable
        hashtable = StripedHashSet<Website>(max_depth);

        //download html for first 
        downloader::HTTP_reply first_html; 
        int return_code;

        char* first_link_char = const_cast<char*>(first_link.c_str());
        return_code = download_webpage(first_link_char, &first_html, true);
        char* actual_html = parse_reply(&first_html);
        if(return_code != 0)
            std::cerr << "Could not download: " << first_link << " most likely cause: " << parse_url_errstr[return_code] << std::endl; 
        
        //std::cout << actual_html <<std::endl;

        //create first website object
        Website* first_website = new Website(first_link, actual_html);
        Node<Website>* node_first_website = new Node<Website>(*first_website); //the parent is its own parent

        //put first object in hashtable 
        hashtable.add(node_first_website); //OBS!! BUG, this do not work properly, and we do not know where to put it.

        hashtable.contains()
        std::cout << hashtable.contains()<< std::enl;

        //std::cout << node_first_website->value.html << std::endl; //this works

        //set counter to 1;
        count++;
    }

    

    int crawl_this_website(Node<Website>& curr){
        std::cout<< "hello crawl this website"<<std::endl;

        const std::regex url_re(R"!!(<\s*A\s+[^>]*href\s*=\s*"([^"]*)")!!", std::regex_constants::icase);
        //Regular expression used to create chunks of the html file
        std::string html = curr.value.html;
        std::cout<< html <<std::endl;
        auto begin = std::sregex_token_iterator(html.begin(), html.end(), url_re, 1); 
        auto end = std::sregex_token_iterator{};




        while (html != ""){  //OBS!!HTML  EMPTY, hashtable access susupicios
            //extract next link
            std::cout<< "we extract link" <<std::endl;
            
            std::string new_link = extract_link(html, begin, end); //extract link

            std::cout << "NEWLINK" << new_link << std::endl;

            if (new_link == ""){ //there is no more link in this website
                break;
            }

            //download next link
            downloader::HTTP_reply new_html;
            char* new_link_char = const_cast<char*>(new_link.c_str());  
            download_webpage(new_link_char, &new_html, false);  

            //create website object next link    //DO PARENT
            Website* new_Website = new Website(new_link, new_html.reply_buffer);

            //put in hash table 
            Node<Website>* website_node = new Node<Website>(*new_Website, &curr); //curr is the parent, new_website is our new website,the depth will be one more
            hashtable.add(website_node); //is depth a thing

            //crawl next link
            std::cout<< "we launch next crawl" <<std::endl;
            crawl_this_website(*website_node); //this should spawn new thread
        }
    }

    int crawl(){
        /*
        while (!(hashtable.max_depth == hashtable.count)){ //i.e not full //Do in hashtable.
            std::unique_ptr<Node<Website>>& current =  hashtable.table[0].front();
            std::cout<< "we launch crawl" <<std::endl;
            crawl_this_website(*current); //will always be first link
        }*/

        std::unique_ptr<Node<Website>>& current =  hashtable.table[0].front(); //ACCESS PROBLEM
        std::cout<< "we launch crawl" <<std::endl;
        std::cout<< current->value.html <<  std::endl; //THIS OUTPUTS NOTHING
        crawl_this_website(*current); //will always be first link

        return 0;
    }

};





#endif