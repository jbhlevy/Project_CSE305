#ifndef test_h
#define test_h


#include "url_parser.h"
#include "downloader.h"
#include "link_extractor.h"
#include "crawler.h"

#include <fstream>
#include<sstream>

#include <set>


namespace test
{
    void test_url_parsing(){
        int return_code; 
        std::cout << "testing..." << std::endl; 
        URL_info info; 
        char url[] = "http://www.google.com/";
        return_code = parse_url(url, &info);
        if(return_code != 0)
            std::cerr << "Could not parse URL: " << url << " most likely cause: " << parse_url_errstr[return_code] << std::endl; 
        print_url_info(&info); 
        std::cout << "ENDED TEST SUCESSFULLY" << std::endl << "==============================" << std::endl; 

        char url1[] = "google.com/"; 
        return_code = parse_url(url1, &info);
        if(return_code != 0)
            std::cerr << "Could not parse URL: " << url << " most likely cause: " << parse_url_errstr[return_code] << std::endl; 
        print_url_info(&info); 
        std::cout << "ENDED TEST SUCESSFULLY" << std::endl << "==============================" << std::endl; 


        char url2[] = "https://www.polytechnique.edu:80/index.php"; 
        return_code = parse_url(url2, &info);
        if(return_code != 0)
            std::cerr << "Could not parse URL: " << url << " most likely cause: " << parse_url_errstr[return_code] << std::endl; 
        print_url_info(&info); 
        std::cout << "ENDED TEST SUCESSFULLY" << std::endl << "==============================" << std::endl; 
    }


    void test_downloading(){
        int return_code; 
        std::cout << "testing..." << std::endl; 
        downloader::HTTP_reply reply; 
        char url[] = "http://www.google.com/";
        return_code = download_webpage(url, &reply, true);
        if(return_code != 0)
            std::cerr << "Could not parse URL: " << url << " most likely cause: " << parse_url_errstr[return_code] << std::endl; 
        
        std::cout << "ENDED TEST SUCESSFULLY" << std::endl << "==============================" << std::endl; 

        char url1[] = "www.google.com/"; 
        return_code = download_webpage(url1, &reply, true);
        if(return_code != 0)
            std::cerr << "Could not download: " << url << " most likely cause: " << parse_url_errstr[return_code] << std::endl; 
        std::cout << "ENDED TEST SUCESSFULLY" << std::endl << "==============================" << std::endl; 


        char url2[] = "https://www.polytechnique.edu:80/index.php"; 
        return_code = download_webpage(url2, &reply, true);
        if(return_code != 0)
            std::cerr << "Could not download: " << url << " most likely cause: " << parse_url_errstr[return_code] << std::endl; 
        std::cout << "ENDED TEST SUCESSFULLY" << std::endl << "==============================" << std::endl; 
    }

    void test_extracting(){
        std::ifstream myfile; 
        myfile.open("some_website.txt"); 
        std::string mystring; 
        if(myfile.is_open()){
            std::ostringstream ss;
            ss << myfile.rdbuf(); 
            mystring = ss.str(); 
        }
        std::cout << " =============== " << std::endl << "Now running extractor function" << std::endl; 
        std::cout << "Input string is" << mystring << std::endl; 
        std::cout << " =============== " << std::endl; 

        

        std::vector<std::string> links; 

        links = extract_links(mystring, 10); 

        std::cout << "Finished extracting..." << std::endl; 
        int i = 0;
        for (auto it = links.begin(); it != links.end(); it++){
            std::cout << i << " " << *it << std::endl; 
            i += 1;
        }
    }

    void test_crawler(){
        Crawler my_crawler("http://www.google.com/", 10);
        my_crawler.crawl();
        //my_crawler.crawl_this_website();
    }


    
} // namespace test

#endif