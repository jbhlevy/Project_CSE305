#include <iostream>

#include "url_parser.h"
#include "test.h"

int main(int argc, const char * argv[]){
    freopen("test_output.txt","w",stdout);
    std::cout << "Hello World !" << std::endl; 

    //test::test_url_parsing(); 
    //test::test_downloading(); 
    //test::test_extracting();
    std::string first_link = "https://fr.wikipedia.org/"; 
    //test::test_crawler(first_link, 200, 40, 8);
    //test::test_crawler(first_link, 500000, 40, 16);
    //test::test_extractor();
    test::run_tests(first_link); 
    //test::test_hashtable();
    return 0; 

}