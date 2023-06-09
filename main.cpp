#include <iostream>

#include "url_parser.h"
#include "test.h"

int main(int argc, const char * argv[]){
    freopen("test_output.txt","w",stdout);
    std::cout << "Hello World !" << std::endl; 

    test::test_url_parsing(); 
    test::test_downloading(); 
    test::test_extractor();
     
    test::test_hashtable();


    std::string first_link = "https://fr.wikipedia.org/"; 
    test::run_tests(first_link);
    //test::test_crawler(first_link, 200, 40, 8);
    //test::test_crawler(first_link, 500000, 1000, 8);
    //test::run_tests(first_link);

    
    return 0; 

}