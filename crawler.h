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
//if html not found dont add to hashtable, or add even though only url found?

class Crawler{
    public:

    StripedHashSet<Website> hashtable;
    int count;

    Crawler(){};

    Crawler(std::string& first_link, int max_depth){

        //Initiialise hashtable
        hashtable = StripedHashSet<Website>(max_depth);

        //download html for first 
        downloader::HTTP_reply first_html; 
        int return_code;

        char* first_link_char = const_cast<char*>(first_link.c_str());
        return_code = download_webpage(first_link_char, &first_html, true);
        char* actual_html = parse_reply(&first_html);

        if(return_code != 0)
            std::cerr << "Could not download: " << first_link << " most likely cause: " << parse_url_errstr[return_code] << std::endl; 

        //create first website object
        Website* first_website = new Website(first_link, actual_html);
        Node<Website>* node_first_website = new Node<Website>(*first_website); //the parent is its own parent

        //put first object in hashtable 
        hashtable.add(node_first_website); 

        if (hashtable.contains(*first_website) &&hashtable.contains(Website(first_link))) {
            std::cout << "Successfull initialisation of crawler, contains node for first link: "<< first_link << std::endl;

        }

        //set counter to 1;
        count++;
    }

    

    int crawl_this_website(Node<Website>& curr){
        std::cout<< "hello crawl this website"<<std::endl;

        const std::regex url_re(R"!!(<\s*A\s+[^>]*href\s*=\s*"([^"]*)")!!", std::regex_constants::icase);
        //Regular expression used to create chunks of the html file
        std::string html = curr.value.html; //make sure we dont modify curr.value.html
        std::cout<< html <<std::endl;

        while (html != ""){ 
            //extract next link
            //std::string new_link = extract_link(html); //extract link
            std::string new_link = fetchFirstLink(html);


            std::cout << "NEWLINK: " << new_link << std::endl;

            if (new_link == ""){ //there is no more link in this website
                std::cout << "we break" << std::endl;
                break;
            }

            //download next link
            downloader::HTTP_reply new_html;
            char* new_link_char = const_cast<char*>(new_link.c_str());  
            int res = download_webpage(new_link_char, &new_html);  

            std::cout << res<< std::endl;
            
            //if (res == 0){ //this shoudl be the case, we only want to parse links which return correct thing, implement https allowance
                //create website object next link    //DO PARENT
                Website* new_Website = new Website(new_link, new_html.reply_buffer);

                //put in hash table 
                Node<Website>* website_node = new Node<Website>(*new_Website, &curr); //curr is the parent, new_website is our new website,the depth will be one more
                hashtable.add(website_node); //is depth a thing
                crawl_this_website(*website_node); //this should spawn new thread
                //crawl next link
                std::cout<< "we launch next crawl" <<std::endl;
            //}

            
        }
        //std::cout << curr.value.html << std::endl;
        return 0;
    }

    int crawl(std::string& first_link){
        /*
        while (!(hashtable.max_depth == hashtable.count)){ //i.e not full //Do in hashtable.
            std::unique_ptr<Node<Website>>& current =  hashtable.table[0].front();
            std::cout<< "we launch crawl" <<std::endl;
            crawl_this_website(*current); //will always be first link
        }*/

        int index = std::hash<std::string>{}(first_link) % hashtable.table.size();

        Node<Website>* current = hashtable.get(Website(first_link));

        Website first = Website(first_link);
        std::cout <<"Check that we have first link added:" << hashtable.contains(first)<< std::endl;

        std::cout<< "we launch crawl" <<std::endl;
        crawl_this_website(*current); //will always be first link

        //std::cout << current->value.html << std::endl;

        return 0;
    }

};





#endif