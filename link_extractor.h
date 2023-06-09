#ifndef link_extractor_h
#define link_extractor_h

#include <regex>
#include <vector> 
#include <string>
#include <iostream>

//used for only url in hashtable version
std::vector<std::string> extractLinksFromHTML(const std::string& html) {
    // Regular expression pattern for extracting URLs within <a href> tags
    const std::regex urlRegex("<\\s*a\\s+[^>]*href\\s*=\\s*\"([^\"]*)\"", std::regex_constants::icase);

    std::vector<std::string> links;

    // Iterator for iterating over matches
    std::sregex_iterator regexIt(html.begin(), html.end(), urlRegex);
    const std::sregex_iterator regexEnd;

    // Extract all matches and add them to the links vector
    for (; regexIt != regexEnd; ++regexIt) {
        const std::smatch& match = *regexIt;
        if (match.size() > 1) {
            std::string link = match[1].str();
            links.push_back(std::move(link));
        }
    }

    return links;
}







#endif 