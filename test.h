#ifndef test_h
#define test_h


#include "url_parser.h"
#include "downloader.h"
#include "link_extractor.h"

#include <fstream>

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
        myfile.open("links_example.txt"); 
        std::string mystring; 
        if(myfile.is_open()){
            myfile >> mystring; 
            std::cout << mystring;
        }
        std::cout << " =============== " << std::endl << "Now running extractor function" << std::endl; 

        

        std::vector<std::string> links; 

        links = extract_links(mystring, 5); 

        std::cout << "Finished extracting..." << std::endl; 
        for (size_t i = 0; i < links.size(); i++){
            std::cout << i << " " << links[i] << std::endl; 
        }
    }


    
} // namespace test

#endif