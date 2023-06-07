#ifndef downloader_h
#define downloader_h

#include <iostream>
#include <curl/curl.h>
#include <string>

char* convert_string(std::string& link){
    char* char_link = new char[link.length() + 1]; 
    strcpy(char_link, link.c_str()); 
    return char_link; 
}


namespace downloader
{

struct http_reply
{
    char* reply_buffer; 
    int length; 
};
typedef struct http_reply HTTP_reply;

// Callback function to write received data into a string
size_t WriteCallback(char* ptr, size_t size, size_t nmemb, std::string* data) {
    
    size_t totalSize = size * nmemb;
    data->append(ptr, totalSize);
    return totalSize;
}

int download_webpage(std::string& url, HTTP_reply *reply, bool verbose=false){
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Create a CURL handle
    CURL* curl = curl_easy_init();
    if (curl) {
        // Set the URL to fetch
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the write callback function
        std::string response;
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2); 
        curl_easy_setopt(curl, CURLOPT_SERVER_RESPONSE_TIMEOUT,  2); 
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT,  2); 
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 5);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            reply->reply_buffer = convert_string(response); 
            reply->length = strlen(convert_string(response)); 
            
            // Clean up
            curl_easy_cleanup(curl);

            // Clean up libcurl
            curl_global_cleanup();

            return res;

        } else {
            // Request failed, display the error message
            std::cerr << "Failed to fetch URL: " << curl_easy_strerror(res) << std::endl;

            // Clean up
            curl_easy_cleanup(curl);

            // Clean up libcurl
            curl_global_cleanup();

            return res;
        }

        // Clean up
        curl_easy_cleanup(curl);
    }

    // Clean up libcurl
    curl_global_cleanup();

    return 1;
}

}; 

#endif