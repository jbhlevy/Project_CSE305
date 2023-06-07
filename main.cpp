#include <iostream>

#include "url_parser.h"
#include "test.h"

int main(int argc, const char * argv[]){
    //freopen("output.txt","w",stdout);
    std::cout << "Hello World !" << std::endl; 

    //test::test_url_parsing(); 
    //test::test_downloading(); 
    //test::test_extracting();
    test::test_crawler();
    //test::test_hashtable();
    return 0; 

}