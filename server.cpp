#include "ft_irc.hpp"

void create_server(int port)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    if (server_fd < 0)
    {
        std::cerr << "socket failed" << std::endl;
        return;
    }
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;

    int result = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (result < 0)
    {
        std::cerr << "bind failed" << std::endl;
        return;
    }
    if (listen(server_fd, 10) < 0)
    {
        std::cerr << "listen failed" << std::endl;
        return;
    }
    std::cout << "Server started on port : " << port << std::endl;
}

/*void *handle_client(void *arg)
{
    int client_fd = *((int *) arg);
    char buffer[200];

    ssize_t bytes_received = recv(client_fd, buffer, 200, 0);
    if (bytes_received > 0)
    {
        regex_t regex;
        regcomp(&regex, "^GET /([^ ]*) HTTP/1", REG_EXTENDED);
        regmatch_t matches[2];
    
        if (regexec(&regex, buffer, 2, matches, 0) == 0)
        {
            buffer[matches[1].rm_eo] = 0;
            const char *url_encoded_file_name = buffer + matches[1].rm_so;
            char *file_name = url_decode(url_encoded_file_name);
            char file_ext[32];
            std::strcpy(file_ext, get_file_extension(file_name));
            char *response = new char[400];
            size_t response_len;
            build_http_response(file_name, file_ext, response, &response_len);
            send(client_fd, response, response_len, 0);
            free(response);
            free(file_name);
        }
        regfree(&regex);
    }
    close(client_fd);
    free(arg);
    free(buffer);
    return NULL;
}

void handle_client(int server_fd)
{
    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd < 0)
        {
            std::cerr << "accept failed" << std::endl;
            continue;
        }
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, (void *)client_fd);
        pthread_detach(thread_id);
    }
}*/