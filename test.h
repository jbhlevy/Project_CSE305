#ifndef test_h
#define test_h


#include "url_parser.h"
#include "downloader.h"
#include "link_extractor.h"
#include "crawler.h"

#include "thread_pool.h"

#include "Integer.h"

#include <fstream>
#include<sstream>

#include <set>

class TestHashtable{
    public:
        ThreadPool* threadPool;
        StripedHashSet<Integer> hashtable;
        int counter;
        std::mutex counterMutex;

        TestHashtable(int depth){
            this-> hashtable = StripedHashSet<Integer>(10);
            this-> threadPool = new ThreadPool(100);
            this->counter = 0;

        }

        void hash_this_int(){
        int c = 0;

        std::cout << "current counter =" << counter << std::endl;
        if (counter < 200){ 
            while (c <= 10){
                std::cout << "hashing :" << counter << std::endl; 
                Integer* integer = new Integer(counter);
                Node<Integer>* node = new Node<Integer>(integer);
                std::cout << "current1 counter =" << counter << std::endl;
                hashtable.add(node);
                std::unique_lock<std::mutex> lock(counterMutex);
                counter++;
                lock.unlock();
                std::cout << "current2 counter =" << counter << std::endl;
                //hash_this_int();

                threadPool->enqueue([this](){
                hash_this_int();
                });
                
                c++;

            }
        }
        else{
            std::cout << "WE EXIT: current counter =" << counter << std::endl;

        }
        }

        void test_hash(){    
            //hash_this_int();      
            threadPool->enqueue([this]() {
                hash_this_int();
            });

            threadPool->stopAndJoin() ;
            
        }
};


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
        else {
            print_url_info(&info); 
            std::cout << "ENDED TEST SUCESSFULLY" << std::endl << "==============================" << std::endl;
        }

        char url1[] = "google.com/"; 
        return_code = parse_url(url1, &info);
        if(return_code != 0)
            std::cerr << "Could not parse URL: " << url << " most likely cause: " << parse_url_errstr[return_code] << std::endl; 
        else {
            print_url_info(&info); 
            std::cout << "ENDED TEST SUCESSFULLY" << std::endl << "==============================" << std::endl;
        }


        char url2[] = "https://www.polytechnique.edu:80/index.php"; 
        return_code = parse_url(url2, &info);
        if(return_code != 0)
            std::cerr << "Could not parse URL: " << url << " most likely cause: " << parse_url_errstr[return_code] << std::endl << "TEST FAILLURE" << std::endl; 
        else{
            print_url_info(&info); 
            std::cout << "ENDED TEST SUCESSFULLY" << std::endl << "==============================" << std::endl; 
        }
    }


    void test_downloading(){
        int return_code; 
        std::cout << "testing..." << std::endl; 
        downloader::HTTP_reply reply; 
        std::string url = "https://fr.wikipedia.org/wiki/Wikip%C3%A9dia:Accueil_principal";
        return_code = download_webpage(url, &reply, true);
        if(return_code != 0)
            std::cerr << "Could not parse URL: " << url << " most likely cause: " << parse_url_errstr[return_code] << std::endl; 
        
        std::cout << "ENDED TEST SUCESSFULLY" << std::endl << "==============================" << std::endl; 

        std::string url1 = "www.google.com/"; 
        return_code = download_webpage(url1, &reply, true);
        if(return_code != 0)
            std::cerr << "Could not download: " << url << " most likely cause: " << parse_url_errstr[return_code] << std::endl; 
        std::cout << "ENDED TEST SUCESSFULLY" << std::endl << "==============================" << std::endl; 


        std::string url2 = "https://fr.wikipedia.org/"; 
        return_code = download_webpage(url2, &reply, true);
        if(return_code != 0)
            std::cerr << "Could not download: " << url << " most likely cause: " << parse_url_errstr[return_code] << std::endl; 
        std::cout << "ENDED TEST SUCESSFULLY" << std::endl << "==============================" << std::endl; 
    }


    void test_crawler(std::string first_link, int maxsize, int depth_hashtable, int num_threads){
        auto start = std::chrono::high_resolution_clock::now();
        //std::string first_link = "https://www.google.com/"; //the html here doesnt match 
        //std::string first_link = "https://fr.wikipedia.org/"; //good to test first step
        //std::string first_link = "https://everything.curl.dev/libcurl/callbacks/write";
        //std::string first_link =  "https://fr.wikipedia.org/wiki/Sp%C3%A9cial:Mes_discussions";
        //std::string first_link = "http://iamjmm.ovh/NSI/http/site/http.html";
        //std::string first_link = "https://hackingcpp.com/"; //gives invalid port
        //std::string first_link = "https://github.com/gperftools/gperftools/wiki";
        //std::string first_link = "https://www.mediawiki.org/wiki/Special:MyLanguage/Help:Contents"; 
        //Crawler my_crawler(first_link, 10);

        // int maxsize = 200;
        // int depth_hashtable = 40;
        // int num_threads = 8;
        Crawler my_crawler;
        my_crawler.init(first_link, depth_hashtable, num_threads, maxsize); 
        std::cout << "====================================" <<
        std::endl << "IMPORTANT LOG: Finished Crawler init" << 
        std::endl << "====================================" << std::endl; 

        //my_crawler.hashtable.printHashtable();


        //std::cout <<"CHECK THAT WE ADDED WEBSITE in test 1:"<< my_crawler.hashtable.table[std::hash<std::string>{}("http://www.google.com/")].front() << std::endl;
        //std::cout <<"CHECK THAT WE ADDED WEBSITE in test 2:"<< my_crawler.hashtable.contains(Website(first_link)) << std::endl; 

        my_crawler.crawl();
        //my_crawler.crawl_this_website();
        std::cout << "================================" << 
        std::endl << "IMPORTANT LOG: Finished Crawling" << 
        std::endl << "================================" << std::endl; 


        my_crawler.hashtable.printHashtable();
        //my_crawler.hashtable.writeHashtableToFile("output.txt");

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
        std::cout << "Duration: " << duration.count() / 1000. << "s" << std::endl;

    }


    void test_hashtable(){
        auto start = std::chrono::high_resolution_clock::now();
        TestHashtable testi(10);
        testi.test_hash(); 

        testi.hashtable.printHashtable();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
        std::cout << "Duration: " << duration.count() / 1000. << "s" << std::endl;


    }
    //gprof crawler gmon.out > analysis.txt


    void test_different_threads(std::string first_link, int maxsize){
        std::cout << "==========================================================" <<
        std::endl << "Running tests on different numnber of threads (1, 4, 8, 16)" << 
        std::endl << "==========================================================" << std::endl;
        std::vector<int> thread_numbers; 

        thread_numbers.push_back(1); 
        thread_numbers.push_back(4); 
        thread_numbers.push_back(8); 
        thread_numbers.push_back(16); 

        int depth_hashtable = 10; 

        for (size_t i = 0; i < thread_numbers.size()-1; i++)
        {
            test_crawler(first_link, maxsize, depth_hashtable, thread_numbers[i]); 
        }
        std::cout << "==========================================================" <<
        std::endl << "Finished running test: SUCESS !" << 
        std::endl << "==========================================================\n\n" << std::endl;
    }

    void test_different_depths(std::string first_link, int maxsize){
        std::cout << "==========================================================" <<
        std::endl << "Running tests on different depths of hashtable (20, 30, 40, 80) for " << maxsize << " maxsize" << 
        std::endl << "==========================================================" << std::endl;

        std::vector<int> depth_sizes; 

        int num_threads = 8; 

        depth_sizes.push_back(20); 
        depth_sizes.push_back(30); 
        depth_sizes.push_back(40); 
        depth_sizes.push_back(80); 

        if(maxsize >= 2000)
            depth_sizes.push_back(160);

        if(maxsize >= 10000){            
            depth_sizes.push_back(320);
            depth_sizes.push_back(640);
            depth_sizes.push_back(1280);
        } 

        for (size_t i = 0; i < depth_sizes.size()-1; i++)
        {
            test_crawler(first_link, maxsize, depth_sizes[i], num_threads); 
        }
        std::cout << "==========================================================" <<
        std::endl << "Finished running test: SUCESS !" << 
        std::endl << "==========================================================\n\n" << std::endl;

    }
    void run_tests(std::string first_link){
        std::cout << "==========================================================" <<
        std::endl << "Testing on maxisze = 200" << 
        std::endl << "==========================================================" << std::endl;
        test_different_threads(first_link, 200); 
        test_different_depths(first_link, 200); 
        std::cout << "==========================================================" <<
        std::endl << "Testing on maxisze = 2000" << 
        std::endl << "==========================================================" << std::endl;
        test_different_threads(first_link, 2000); 
        test_different_depths(first_link, 2000); 
        std::cout << "==========================================================" <<
        std::endl << "Testing on maxisze = 10 000" << 
        std::endl << "==========================================================" << std::endl;
        test_different_depths(first_link, 10000); 
        std::cout << "==========================================================" <<
        std::endl << "Finished all test with sucess, we are geniuses !!" << 
        std::endl << "==========================================================" << std::endl;
        
    }




    
} // namespace test

#endif