#ifndef downloader_h
#define downloader_h

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <openssl/ssl.h>


#include "url_parser.h"

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

char* next_line(char* buff, int len){
    if(len == 0)
        return NULL; 
    char* last = buff + len - 1;
    while(buff != last){
        if(*buff == '\r' && *(buff+1) == '\n')
            return buff; 
        ++buff; 
    }
    return NULL; 
}


char* create_http_request(URL_info *info){
    char *request_buffer = (char * ) malloc(100 + strlen(info->path) + strlen(info->host));
    snprintf(request_buffer, 1024, "GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", info->path, info->host); 
    return request_buffer;  
}

int get_reply_http(URL_info *info, HTTP_reply *reply, bool verbose=false){
    std::cout << "HTTP NOT S" << std::endl; 
    char *hostname = info->host; 
    if(verbose)
        std::cout << "About to send GET request to " << hostname << std::endl; 

    struct hostent *host; 
    struct in_addr **addr_list; 
    host = gethostbyname(hostname); 

    if(host == NULL){
        herror("gethostbyname"); 
        return 1; 
    }

    addr_list = (struct in_addr **) host->h_addr_list; 
    //std::cout << "Obtained the adresses:" << std::endl; 
    if(verbose){
        for (int i = 0; addr_list[i] != NULL; i++){
            std::cout << inet_ntoa(*addr_list[i]) << std::endl; 
        }
    }  

    char *request_buffer = create_http_request(info); 
    if(verbose)
        std::cout << "Created request buffer: " << request_buffer << "EOF" << std::endl; 

    struct sockaddr_in server_addr; 
    int socketf = socket(AF_INET, SOCK_STREAM, 0); 

    memset(&server_addr, 0, sizeof(server_addr)); 
    server_addr.sin_family = AF_INET; 
    memcpy(&server_addr.sin_addr.s_addr, addr_list[0], host->h_length); 
    server_addr.sin_port = htons(info->port); 

    if(connect(socketf, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        std::cerr << "Could not connect to socker :/" << std::endl; 
        return 2; 
    }

    if(verbose)
        std::cout << "Connected to socket :)" << std::endl; 
    
    int total = strlen(request_buffer); 
    int sent = 0; 
    int bytes; 
    if(verbose)
        std::cout << "Sending request..."; 

    while(sent < total){
        bytes = write(socketf, request_buffer + sent, total - sent); 
        if(bytes < 0)
            std::cerr << "Error writting to socket" << std::endl; 
        if (bytes == 0)
            break; 
        sent += bytes; 
        if(verbose)
            std::cout << "..."; 
    }
    std::cout << "Sent HTTP GET request to " << inet_ntoa(*addr_list[0]) << std::endl; 
    //Read response from server 
    int received = 0;
    total = 64000; 
    char* response_buffer = (char *) malloc(total);
    memset(response_buffer, 0, 64000); 
    while (received < total){
        bytes = read(socketf, response_buffer + received, total-received); 

        if(bytes < 0)
            std::cout << "Error reading from socket" << std::endl; 
        if(bytes == 0)
            break;
        
        received += bytes; 
        total -= bytes; //??? Not sure about this 


        if(total <= 0){
            std::cout << "Reached maximum buffer length" << std::endl; 
            total += 32000; 
            reply->reply_buffer = (char *) realloc(reply->reply_buffer, received + total); 
        }
    }
    reply->reply_buffer = response_buffer; 
    reply->length = strlen(response_buffer); 

    shutdown(socketf, SHUT_RDWR); 
    if(verbose)
        std::cout << "Shutdown socket" << std::endl; 
    return 0; 
}

int get_reply_https(URL_info *info, HTTP_reply *reply, bool verbose=false){
    char *hostname = info->host; 
    if(verbose)
        std::cout << "About to send GET request to " << hostname << std::endl; 

    struct hostent *host; 
    struct in_addr **addr_list; 
    host = gethostbyname(hostname);

    if(host == NULL){
        herror("gethostbyname"); 
        return 1; 
    }

    addr_list = (struct in_addr **) host->h_addr_list; 
    //std::cout << "Obtained the adresses:" << std::endl; 
    if(verbose){
        for (int i = 0; addr_list[i] != NULL; i++){
            std::cout << inet_ntoa(*addr_list[i]) << std::endl; 
        }
    }  

    char *request_buffer = create_http_request(info); 
    if(verbose)
        std::cout << "Created request buffer: " << request_buffer << "EOF" << std::endl; 

    struct sockaddr_in server_addr; 
    int socketf = socket(AF_INET, SOCK_STREAM, 0); 

    memset(&server_addr, 0, sizeof(server_addr)); 
    server_addr.sin_family = AF_INET; 
    memcpy(&server_addr.sin_addr.s_addr, addr_list[0], host->h_length); 
    server_addr.sin_port = htons(info->port); 

    if(connect(socketf, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        std::cerr << "Could not connect to socker :/" << std::endl; 
        return 2; 
    }

    if(verbose)
        std::cout << "Connected to socket :)" << std::endl; 

    SSL_load_error_strings();
	SSL_CTX *ssl_ctx = SSL_CTX_new(SSLv23_client_method());
	SSL_library_init();

    SSL *conn = SSL_new(ssl_ctx);
	SSL_set_fd(conn, socketf);

    int err = SSL_connect(conn);
	if (err != 1)
	{
        SSL_load_error_strings();
        std::cout << "SSL error" << std::endl; 
        err = SSL_get_error(conn, err); 
        std::cout << "Error code: " << err << std::endl; 
		return 2;
	}

    
    int total = strlen(request_buffer); 
    int sent = 0; 
    int bytes; 
    if(verbose)
        std::cout << "Sending request..."; 

    while(sent < total){
        bytes = SSL_write(conn, request_buffer + sent, total - sent); 
        if(bytes < 0)
            std::cerr << "Error writting to socket" << std::endl; 
        if (bytes == 0)
            break; 
        sent += bytes; 
        if(verbose)
            std::cout << "..."; 
    }
    //std::cout << "Sent HTTP GET request to " << inet_ntoa(*addr_list[0]) << std::endl; 

    int received = 0;
    total = 64000; 
    char* response_buffer = (char *) malloc(total);
    memset(response_buffer, 0, 64000); 
    while (received < total){
        bytes = SSL_read(conn, response_buffer + received, total-received); 

        if(bytes < 0)
            std::cout << "Error reading from socket" << std::endl; 
        if(bytes == 0)
            break;
        
        received += bytes; 
        total -= bytes; //??? Not sure about this 


        if(total <= 0){
            std::cout << "Reached maximum buffer length" << std::endl; 
            total += 32000; 
            reply->reply_buffer = (char *) realloc(reply->reply_buffer, received + total); 
        }
    }
    reply->reply_buffer = response_buffer; 
    reply->length = strlen(response_buffer); 


	SSL_shutdown(conn);
	SSL_free(conn);
    shutdown(socketf, SHUT_RDWR); 
    if(verbose)
        std::cout << "Shutdown socket" << std::endl; 
    return 0; 

}





char* parse_reply(HTTP_reply *reply){
    char* status_line = next_line(reply->reply_buffer, reply->length);
    if(status_line == NULL){
        //std::cerr << "Could not find status in http reply " << std::endl; 
        return NULL; 
    }
    *status_line = '\0'; 
    //std::cout << "About to parse the first line: " << reply->reply_buffer << std::endl; 

    //PDF 
    // char* type; 
    // type = strstr(status_line+1, "Content-Type:"); 
    // if(type != NULL){
    //     type += 14; 
    // }
    // else{
    //     std::cout << "YOU KIDDING" << std::endl; 
    // }
    // if(type != NULL){
    //     type = strstr(type, "html"); 
    // }
    // if(type == NULL){
    //     return NULL; 
    // }

    int status; double http_version;  
    int rv = sscanf(reply->reply_buffer, "HTTP/%lf %d", &http_version, &status); 
    if(rv != 2){
        //std::cerr << "Could not parse first line (rv= " << rv << std::endl; 
        return NULL; 
    }
    //std::cout << "Server returned status " << status << std::endl; 
    if(status != 200){
        //std::cout << "Taking care of redirection" << std::endl;
        if(status == 404){
            //std::cout << "Page doesn't exist" << std::endl; 
            return NULL; 
        }
        if(status == 403){
            //std::cout << "Request forbidden by server :(" << std::endl; 
            return NULL;
        }
        char* new_location; 
        //std::cout << "stat+1 " << status_line + 1 << std::endl; 
        new_location = strstr(status_line+1, "Location:"); 
        if(new_location != NULL){
            new_location += 10; 
        }
        else{
            //std::cout << "Big L Location pointer was null" << std::endl; 
            new_location = strstr(status_line+1, "location:");
            if(new_location != NULL){
                new_location += 10; 
            } 
            else{
                //std::cout << "THIS IS BULLSHIT" << std::endl; 
                return NULL; //JOHANNA ADDED THIS CHECK, this should not ahppen
            }
        }
        if(new_location != NULL){
            char* end = strstr(new_location, "\r"); 
            if(end != NULL){
                *end = '\0'; 
            }
        }        
        //std::cout << "FOUND NEW LOCATION: " << new_location << std::endl; 
        return new_location; 
    }
    char * buff = status_line + 2; 
    char* n_line; 
    while(true){
        n_line = next_line(buff, reply->length - (buff-reply->reply_buffer)); 
        if(n_line == buff)
            break; 
        buff = n_line + 2; 
    }
    return buff+ + 2; 
}


int download_webpage(std::string& string_url, HTTP_reply *reply, bool verbose=false){
    //Start by parsing the url 
    //std::cout << "Entered Downloading Function" << std::endl << "====" << std::endl;
    char* url = convert_string(string_url); 


    URL_info info; 
    int return_code = parse_url(url, &info); 
    if(return_code != 0){
        //std::cerr << "Could not parse URL: " << url << " most likely cause: " << parse_url_errstr[return_code] << std::endl; 
        return 1; 
    }

    if(verbose)
        print_url_info(&info); 

    if(strstr(info.protocol, "https") != 0){
        //HTTPS
        return_code = get_reply_https(&info, reply, verbose); 

    }
    else{
        //HTTP
        return_code = get_reply_http(&info, reply, verbose); 
    }

    if(return_code != 0){
        //c la merde
        return 2; 
    }
    char* http_reply = parse_reply(reply); 
    if(http_reply == NULL){
        //std::cout << "THERE WAS MAJOR PROBLEM SORRY" << std::endl; 
        return 3; 
    }
    if(*strstr(http_reply, "http") == *http_reply){
        //std::cout << "reply was another link..." << std::endl; 
        string_url = std::string(http_reply); 
        download_webpage(string_url, reply, verbose); 
    }


    // // if(verbose)
    // std::cout << "From download function, reply is: " << http_reply << std::endl;
    //std::cout << "Download page exited with return code 0." << std::endl << "====" << std::endl; 
    return 0; 
}


    

    
} // namespace downloader




#endif