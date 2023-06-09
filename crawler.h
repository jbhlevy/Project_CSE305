#ifndef crawler_h
#define crawler_h

#include <vector>
#include <string>
#include <atomic>
#include "HashTable.cpp"
#include "Website.h"
#include "thread_pool.h"

#include "url_parser.h"
#include "downloader.h"
#include "link_extractor.h"

class Crawler{
    std::atomic<int> count;

    public:
    ThreadPool* threadPool;
    int MAX;
    std::string initial_link;
    std::string starting_link; 
    URL_info initial_link_info; 
    StripedHashSet<Website> hashtable;

    Crawler(){
        this->count = 0; 
    };

    void init(std::string& first_link, int max_depth, int numthreads, int maxsize){

        this->starting_link = first_link; 
        this->starting_link.pop_back(); 
        this->MAX = maxsize;
        this-> threadPool = new ThreadPool(numthreads); 

        //Initiialise hashtable
        hashtable = StripedHashSet<Website>(max_depth);

        //create first website object
        Website* first_website = new Website(first_link);
        Node<Website>* node_first_website = new Node<Website>(first_website); //the parent is its own parent

        //put first object in hashtable  
        hashtable.add(node_first_website); 

        count.fetch_add(1); 
        this->initial_link = first_link; 
    }
  

    void crawl_this_website(Node<Website>* curr) {
        downloader::HTTP_reply html_download;
        int res = download_webpage(curr->value->url, &html_download, false); 
        if (res == 0){
            if(count >=MAX) return ; 
              

            const std::regex url_re(R"!!(<\s*A\s+[^>]*href\s*=\s*"([^"]*)")!!", std::regex_constants::icase);
            //Regular expression used to create chunks of the html file

            std::string html = html_download.reply_buffer; //make sure we dont modify curr.value.html

            std::vector<std::string> links = extractLinksFromHTML(html);

            for (std::string& new_link : links) {
                if (count >= MAX) break;
            //while (html != "" && count < MAX ){ 
            //extract next link
                //std::string new_link = fetchFirstLink(html);
                if (new_link == ""){ //there is no more link in this website
                    break;
                }
                if(new_link == "bad"){
                    continue; 
                }

                if (new_link.find("wikipedia") != std::string::npos){
                    if( !(new_link.find("fr") != std::string::npos || new_link.find("en") != std::string::npos)){
                        continue; 
                    }
                }

                if(new_link.front() == '/'){

                    if(new_link.find("//") != std::string::npos){
                        new_link = "https:" + new_link; 
                    }
                    else{

                        URL_info parent_info; 
                        std::string parent_url_string = curr->value->get_url(); 
                        char* parent_url = convert_string(parent_url_string);
                        parse_url(parent_url, &parent_info); 

                        new_link =  std::string(parent_info.protocol) + "://" + std::string(parent_info.host) + new_link; 
                }
            }
                //Check that link is not already in the table
                if(hashtable.contains(new_link)){
                    //we ignore this link
                    continue;
                }

                //create website object next link    
                    Website* new_Website = new Website(new_link);//, std::string(new_html.reply_buffer));

                //put in hash table 
                    Node<Website>* website_node = new Node<Website>(new_Website, curr); //curr is the parent, new_website is our new website,the depth will be one more
                    hashtable.add(website_node);
                    count.fetch_add(1); 

                //pararell
                    if(count < MAX){

                        threadPool->enqueue(([this, website_node]() {
                            crawl_this_website(website_node);
                            }));
                    }
                    else{
                        if(threadPool->is_empty()){
                            return; 
                        }
                        threadPool->killall(); 
                    }       
                //crawl next link
                }
            
        }
        return ;
    }


    void crawl(){
        Node<Website>* current = hashtable.get(initial_link);

        //parallel
        crawl_this_website(current);
        /*
        threadPool->enqueue(([this, current]() {
                     crawl_this_website(current);
                     }));
                     */
        threadPool->stopAndJoin() ;
        
    }

};

#endif