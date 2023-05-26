#ifndef link_extractor_h
#define link_extractor_h

#include <regex>
#include <vector> 
#include <string>
#include <iostream>

std::string extract_link(std::string html, std::sregex_token_iterator& begin, std::sregex_token_iterator& end){
	std::regex expression(".*\\..*"); // regex for URL within <a href> tag
	 std::cout << "Hello from extract_links" << std::endl; 
    const std::regex url_re(R"!!(<\s*A\s+[^>]*href\s*=\s*"([^"]*)")!!", std::regex_constants::icase);
    //Regular expression used to create chunks of the html file

    //auto begin = std::sregex_token_iterator(b, e, url_re, 1); 
    //auto end = std::sregex_token_iterator{}; 

	//if (begin == end) return "";
	//auto it = begin;
	//while (begin != nullptr){
	for (auto it = begin; it !=end; it++){
		//std::string a = *begin;
		std::string a = *it;
		std::cout << a <<std::endl;

        //Remove unecessary characters that are in links due to JS and HTML syntax        

		size_t t = a.find_first_of("?");
		if (t != std::string::npos)
			a.erase(t);

		t = a.find_first_of("#");
		if (t != std::string::npos)
			a.erase(t);

		t = a.find_first_of(";");
		if (t != std::string::npos)
			a.erase(t);

		t = a.find_first_of("=");
		if (t != std::string::npos)
			a.erase(t);

        //Test that the remaining string matches the regular expression <a href=....... />
        //Test that the string has more than 7 characters
        //Because we then test the first 8 characters are https://
        //Check that \\, <, >, {, } and ' ' are not in the string
        //Then we have found a link
		if (
				regex_match(a, expression) &&
				a.size() > 7 && 
				a.substr(0, 8) == "https://" &&
				a.find('\\') == std::string::npos &&
				a.find('<') == std::string::npos &&
				a.find('>') == std::string::npos &&
				a.find('{') == std::string::npos &&
				a.find('}') == std::string::npos &&
				a.find(' ') == std::string::npos)
		{				
			// a launch new thread, conditional veriable???, 
			return a;
		}
	}
	return "";
}


std::vector<std::string> extract_links(std::string html, int maxLinks) //html of parsed link, max_depth
{
    std::cout << "Hello from extract_links" << std::endl; 
    const std::regex url_re(R"!!(<\s*A\s+[^>]*href\s*=\s*"([^"]*)")!!", std::regex_constants::icase);
    //Regular expression used to create chunks of the html file

    auto begin = std::sregex_token_iterator(html.begin(), html.end(), url_re, 1); 
    auto end = std::sregex_token_iterator{}; 

	std::regex expression(".*\\..*"); // regex for URL within <a href> tag

	std::vector<std::string> links;

    for (auto it = begin; it !=end; it++)
	{
		std::string a = *it;

        //Remove unecessary characters that are in links due to JS and HTML syntax        

		size_t t = a.find_first_of("?");
		if (t != std::string::npos)
			a.erase(t);

		t = a.find_first_of("#");
		if (t != std::string::npos)
			a.erase(t);

		t = a.find_first_of(";");
		if (t != std::string::npos)
			a.erase(t);

		t = a.find_first_of("=");
		if (t != std::string::npos)
			a.erase(t);

        //Test that the remaining string matches the regular expression <a href=....... />
        //Test that the string has more than 7 characters
        //Because we then test the first 8 characters are https://
        //Check that \\, <, >, {, } and ' ' are not in the string
        //Then we have found a link

		if (
				regex_match(a, expression) &&
				a.size() > 7 && 
				a.substr(0, 8) == "https://" &&
				a.find('\\') == std::string::npos &&
				a.find('<') == std::string::npos &&
				a.find('>') == std::string::npos &&
				a.find('{') == std::string::npos &&
				a.find('}') == std::string::npos &&
				a.find(' ') == std::string::npos)
		{	
			
			// a launch new thread, conditional veriable???, 

			links.push_back(a);
			if (links.size() >= maxLinks)
			{
				break;
			}
		}
	}

    return links; 

}


#endif 