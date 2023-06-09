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

//Note: Problems marked with OBS
//if html not found dont add to hashtable, or add even though only url found?

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
        this-> threadPool = new ThreadPool(numthreads, std::min(maxsize, 50000)); 

        //Initiialise hashtable
        hashtable = StripedHashSet<Website>(max_depth);

        //download html for first 
        //downloader::HTTP_reply first_html; 
        //int return_code;
        //return_code = download_webpage(first_link, &first_html, false);
        //char* actual_html = first_html.reply_buffer;

        //if(return_code != 0)
          //  std::cerr << "Could not download: " << first_link << " most likely cause: " << parse_url_errstr[return_code] << std::endl; 


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
            if(count >=MAX){
            //std::cout << "reached max, stopping..." << std::endl; 
            return ; 
            }   

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

                //std::cout << "==============================" << std::endl << "IMPORTANT LOG: found new link to crawl:  " << new_link  << std::endl << "==============================" << std::endl;
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
                    continue;
                }
            //download next link
            //downloader::HTTP_reply new_html;
            //int res = download_webpage(new_link, &new_html, false);   

                if(hashtable.contains(new_link)){
                    continue;
                }
            
             //this shoudl be the case, we only want to parse links which return correct thing
                //create website object next link    //DO PARENT
                    Website* new_Website = new Website(new_link);//, std::string(new_html.reply_buffer));
                    //Website new_Website(new_link);//Website(new_link);//, std::string(new_html.reply_buffer));


                //put in hash table 
                    Node<Website>* website_node = new Node<Website>(new_Website, curr); //curr is the parent, new_website is our new website,the depth will be one more
                    hashtable.add(website_node);
                    count.fetch_add(1); 

                //sequential
                //crawl_this_website(website_node); //this should spawn new thread

                //pararell

                    if(count < MAX){

                        threadPool->enqueue(([this, website_node]() {
                            crawl_this_website(website_node);
                            }));
                    }
                    else{
                        if(threadPool->is_empty()){
                            //delete html_download;
                            return; 
                        }
                        //std::cout << "THREADS WILL DIE!!" << std::endl; 
                        threadPool->killall(); 
                    }   
                    
                //crawl next link
                }
            
        }
        //delete html_download;
        //std::cout << "Finishing task on node " << curr << std::endl; 
        return ;
    }


    void crawl(){
        //std::cout << "Started main crawling function..." << std::endl; 
        Node<Website>* current = hashtable.get(initial_link);
        //std::cout << "current hashtable first (Pointer) " << current << std::endl;

        //int hash_first_link = std::hash<std::string>{}(initial_link); 
        
        //std::cout << "Hash of first link in crawl function: " << hash_first_link << std::endl; 
        //std::cout<< "we launch crawl" <<std::endl;
        

        //parallel
        crawl_this_website(current);
        /*
        threadPool->enqueue(([this, current]() {
                     crawl_this_website(current);
                     }));
                     */
        //finish_crawl();
        threadPool->stopAndJoin() ;
        
                        
        //test sequential
        //crawl_this_website(current); //will always be first link


        //std::cout << "Exiting crawling function, " << count << " links in the hashtable!" << std::endl;
    }

};





#endif