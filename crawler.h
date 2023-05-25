#ifndef crawler_h
#define crawler_h

#include <vector>
#include <string>
#include "HashTable.cpp"
#include "Website.h"

#include "url_parser.h"
#include "downloader.h"
#include "link_extractor.h"


class Crawler{
    public:

    StripedHashSet<Website> hashtable;
    int counter;

    Crawler(std::string first_link,int capacity){
        //initialise hashtable
        hashtable = StripedHashSet<Website>(capacity);

        //download html for first 
        downloader::HTTP_reply first_html; 
        int return_code;

        char* first_link_char = const_cast<char*>(first_link.c_str());
        return_code = download_webpage(first_link_char, &first_html, true);
        if(return_code != 0)
            std::cerr << "Could not download: " << first_link << " most likely cause: " << parse_url_errstr[return_code] << std::endl; 
        
        //create first website object
        Website first_website;
        first_website = Website(first_link ,first_html.reply_buffer);

        //put first object in hashtable //where do we put it in the hashtable
        hashtable.add(first_website);

        //set counter to 1;
        counter++;
    }

    int thread_spawn(std::string link){

    }

    int crawl(){

        thread_spawn(first_link);

        while (!(hashtable.capacity == counter)){


        }
        //put in hash table, remember parent

        //extract next link

        //download next link

        //create website object for next link 

        //put in hash table
    }


    void dowload_website(std::string url){
    }

};





#endif