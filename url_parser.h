#ifndef url_parser_h
#define url_parser_h

#include <iostream>
#include <stdio.h>
#include <string.h>

// parse_url error codes
#define PARSE_URL_OK 0
#define PARSE_URL_NO_SLASH 1
#define PARSE_URL_INVALID_PORT 2
#define PARSE_URL_PROTOCOL_UNKNOWN 3

// parse_url associated error strings
static const char *parse_url_errstr[] = { "no error" , "no trailing slash", "invalid port", "unknown protocol"};

struct url_info{
    char* protocol; 
    char* host; 
    int port; 
    char* path; 
}; 
typedef struct url_info URL_info; 

void print_url_info(URL_info *info){
    std::cout << "The URL contains the following information:" << std::endl << 
    "Protocol: " << info->protocol << std::endl << 
    "Host name: " << info->host << std::endl <<
    "Port No: " << info->port << std::endl << 
    "Path: " << info->path << std::endl; 
}

int parse_url(char url[], URL_info *info){
    char *colon_slash, *host_name_path, *protocol; 

    colon_slash = strstr(url, "://"); 
    if(colon_slash != NULL){
        *colon_slash = '\0'; 
        host_name_path = colon_slash + 3; 
        protocol = url; 
    } 
    else {
        host_name_path = url; 
        protocol = "http"; 
    }
    info->protocol = protocol; 

    if(strcmp(info->protocol, "http") and strcmp(info->protocol, "https"))
        return PARSE_URL_PROTOCOL_UNKNOWN; 

    char* slash = strchr(host_name_path, '/'); 
    if(slash == NULL)
        return PARSE_URL_NO_SLASH; 
    
    *slash = '\0'; 
    info->host = host_name_path; 
    info->path = slash + 1; 

    char *port = strchr(host_name_path, ':');
	if (port) {
		*port = '\0';
		port = port + 1;
		if (sscanf(port, "%d", &info->port) != 1) {
			return PARSE_URL_INVALID_PORT;
		}
	} else {
        if(strcmp(info->protocol, "https") ==0)
        {
            std::cout << "hit" << std::endl; 
            info->port = 443; 
        }
        else{
		    info->port = 80;
        }
	}

	//If everything went well, return 0.
	return PARSE_URL_OK;
}

#endif 