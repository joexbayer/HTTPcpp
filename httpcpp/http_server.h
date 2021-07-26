//
//  http_server.h
//  httpcpp
//
//  Created by Joe Bayer on 22/07/2021.
//

#ifndef http_server_h
#define http_server_h

#define MAX_ROUTES 50
#define MAX_CONNECTIONS 10

#define HTTP_BUFFER_SIZE 8192 // 8KB

//#define CONTEXT getCurrentContext

#include "syshead.h"
#include "thread_pool.hpp"

typedef enum
{
    GET,
    POST,
    HEAD,
    UNDEFINED

} method_t ;

// CONNECTION SECTION

struct http_context
{
    /* User (Identity) TODO */
    
    std::string cookies;
    std::string host;
    std::string connection;
    
    method_t method;
    std::string route;
};

struct http_connection
{
    struct http_context* context;
    std::string header;
    std::string content;
    std::string router;
    
    // socket
    int client_socket;
    struct sockaddr_in* client_addr;
    
};

// SERVER SECTION

struct http_config /* Config struct, needs to be filled manually or through a config file */
{
    int port;
    int debug;
    int thread_pool_size;
};

struct http_route
{
    std::string route;
    method_t method;
    void (*function)();
};

struct file_s
{
    char* content;
    off_t size;
};

class http_server /* HTTP server class */
{
    
public:
    http_server(struct http_config user_config );
    ~http_server();
    
    int add_route(const std::string& route_name, const method_t& method_def, void (*user_function)() );
    void add_authorization(std::string token);
    void run(); /* Main event handle loop, blocking. */
    void handle_thread_connection(struct http_connection* connection);
    
private:
    void read_handle_loop();
    int create_tcp_socket(uint32_t port);
    void cleanup();
    
    //pipeline
    void parse_connection_header(struct http_connection* connection);
    void parse_method_route(struct http_connection* connection);
    void handle_route(const std::string& route, const method_t& method);
    
    struct file_s* open_file(const std::string& file);
    struct http_connection* new_http_client();
    
    method_t get_method(const char* method_string);
    
    void asign_worker(struct http_connection* connection);

private:
    struct http_config* config;
    struct http_route* routes[MAX_ROUTES];
    int http_route_counter;
    
    int server_socket;
    struct sockaddr_in* server_addr;
    
    int use_authorization;
    std::string token;
    
    thread_pool worker_pool;
    
    std::string main_header;
};



#endif /* http_server_h */
