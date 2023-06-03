#ifndef link_extractor_h
#define link_extractor_h

#include <regex>
#include <vector> 
#include <string>
#include <iostream>

std::string fetchFirstLink(std::string& html) //call on copy of string. 
{
    // Regular expression pattern for extracting the first URL within <a href> tag
    const std::regex url_re("<\\s*A\\s+[^>]*href\\s*=\\s*\"([^\"]*)\"", std::regex_constants::icase);

    // Search for the first match in the HTML string
    std::smatch match;
    if (std::regex_search(html, match, url_re) && match.size() > 1)
    {
        // Extract the first link from the match
        std::string firstLink = match[1].str();
		//std::cout << "fetchFirstLink function 0 " << firstLink << std::endl; 


		//std::cout << "fetchFirstLink function " << firstLink << std::endl; 




        // Find the position of the first link within the HTML string
        size_t linkPos = html.find(firstLink);
        if (linkPos != std::string::npos)
        { 
			// Erase the portion of the HTML string up to the end of the first link
			html.erase(0, linkPos + firstLink.length());

			size_t t = firstLink.find_first_of("?");
			if (t != std::string::npos)
				firstLink.erase(t);

			t = firstLink.find_first_of("#");
			if (t != std::string::npos)
				firstLink.erase(t);

			t = firstLink.find_first_of(";");
			if (t != std::string::npos)
				firstLink.erase(t);

			t = firstLink.find_first_of("=");
			if (t != std::string::npos)
				firstLink.erase(t);

			if((firstLink.substr(0, 8) == "https://" || firstLink.substr(0,7) == "http://" || firstLink.substr(0, 1) == "/")&&
				firstLink.find('\\') == std::string::npos &&
				firstLink.find('<') == std::string::npos &&
				firstLink.find('>') == std::string::npos &&
				firstLink.find('{') == std::string::npos &&
				firstLink.find('}') == std::string::npos &&
				firstLink.find(' ') == std::string::npos &&
				firstLink != "" && firstLink != ".")
			{
				// Return the first link
				std::cout << "fetchFirstLink function3 " << firstLink << std::endl; 
				return firstLink;
			}
			return "bad"; 
        }
    }
    // Return an empty string if no link was found
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