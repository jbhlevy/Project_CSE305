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
				return firstLink;
			}
			return "bad"; 
        }
    }
    // Return an empty string if no link was found
    return "";
}

#endif 