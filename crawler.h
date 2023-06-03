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

int MAX = 2000; 

class Crawler{
    public:

    int count;
    std::string initial_link;
    std::string starting_link; 
    URL_info initial_link_info; 
    StripedHashSet<Website> hashtable;

    Crawler(){};

    Crawler(std::string& first_link, int max_depth){
        this->starting_link = first_link; 
        this->starting_link.pop_back(); 

        //Initiialise hashtable
        hashtable = StripedHashSet<Website>(max_depth);

        //download html for first 
        downloader::HTTP_reply first_html; 
        int return_code;

        std::cout << "Call from crawler init " <<  first_link << std::endl; 
        return_code = download_webpage(first_link, &first_html, true);
        std::cout << "First link " << first_link << std::endl; 

        char* actual_html = parse_reply(&first_html);


        if(return_code != 0)
            std::cerr << "Could not download: " << first_link << " most likely cause: " << parse_url_errstr[return_code] << std::endl; 


        std::cout << "Downloader managed to find html for the url " << first_link << " now creating website and node objects" << std::endl; 
        //create first website object
        Website* first_website = new Website(first_link, actual_html);
        Node<Website>* node_first_website = new Node<Website>(first_website); //the parent is its own parent
        //put first object in hashtable  
        hashtable.add(node_first_website); 

        if (hashtable.contains(first_website) && hashtable.contains(first_link)) {
            std::cout << "Successfull initialisation of crawler, contains node for first link: "<< first_website->url << std::endl;
        }
        //set counter to 1;
        count++;
        this->initial_link = first_link; 
    }

    

    int crawl_this_website(Node<Website>* curr, int count=0){
        std::cout<< "hello crawl this website"<<std::endl;
        std::cout << "address of current node " << curr << std::endl; 
        if(count ==2){
            std::cout << "Reached Max, stopping" << std::endl; 
            return 0; 
        }

        const std::regex url_re(R"!!(<\s*A\s+[^>]*href\s*=\s*"([^"]*)")!!", std::regex_constants::icase);
        //Regular expression used to create chunks of the html file

        std::string html = curr->value->get_html(); //make sure we dont modify curr.value.html
        //std::cout<< html <<std::endl;

        while (html != ""){ 
            //extract next link
            std::string new_link = fetchFirstLink(html);
            if (new_link == ""){ //there is no more link in this website
                std::cout << "Link is empty meaning, we break" << std::endl;
                break;
            }
            if(new_link == "bad"){
                continue; 
            }

            std::cout << "==============================" << std::endl << "IMPORTANT LOG: found new link to crawl:  " << new_link  << std::endl << "==============================" << std::endl;
            if(new_link.front() == '/'){

                std::cout << "initial parent link " << this->starting_link << std::endl;

                new_link = this->starting_link + new_link; 
                std::cout << "Changed linked to " << new_link << " length " << new_link.length() << std::endl; 
            }

            //Check that link is not already in the table
            if(hashtable.contains(new_link)){
                std::cout << "==============================" << std::endl << "IMPORTANT LOG: link " << new_link  << " already in the table, skipping to next link" << std::endl << "==============================" << std::endl;

                continue;
            }

            //download next link
            downloader::HTTP_reply new_html;
            int res = download_webpage(new_link, &new_html, true);  

            std::cout << "Downloading return code: " << res<< std::endl;

            if(hashtable.contains(new_link)){
                std::cout << "==============================" << std::endl << "IMPORTANT LOG: link " << new_link  << " already in the table, skipping to next link" << std::endl << "==============================" << std::endl;

                continue;
            }
            
            if (res == 0){ //this shoudl be the case, we only want to parse links which return correct thing, implement https allowance
                //create website object next link    //DO PARENT
                Website* new_Website = new Website(new_link, std::string(downloader::parse_reply(&new_html)));

                //put in hash table 
                Node<Website>* website_node = new Node<Website>(new_Website, curr); //curr is the parent, new_website is our new website,the depth will be one more
                hashtable.add(website_node); //is depth a thing
                std::cout<< "we launch next crawl" <<std::endl;
                std::cout << "Now about to recurse and crawl the website at node (Pointer) " << website_node <<  std::endl << "=======" << std::endl; 
                //crawl_this_website(website_node, count+1); //this should spawn new thread
                //crawl next link
            }
            else{
                std::cout << "Downloading didnt work" << std::endl; 
            }

            
        }
        std::cout << "Exited extraction loop for website at node (Pointer) " << curr << std::endl; 

        return 0;
    }

    int crawl(){
        std::cout << "Started main crawling function" << std::endl; 

        int index = std::hash<std::string>{}(initial_link) % hashtable.table.size();

        Node<Website>* current = hashtable.get(initial_link);
        std::cout << "current hashtable first (Pointer) " << current << std::endl;

        std::cout << "First link is " << initial_link << std::endl; 
        int hash_first_link = std::hash<std::string>{}(initial_link); 
        std::cout << "Hash of first link in crawl function: " << hash_first_link << std::endl; 

        std::cout <<"Check that we have first link added: " << std::endl << hashtable.contains(initial_link)<< std::endl;

        std::cout<< "we launch crawl" <<std::endl;
        crawl_this_website(current); //will always be first link

        //std::cout << current->value.html << std::endl;

        return 0;
    }

};





#endif