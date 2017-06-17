#include <cmath>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <thread>
#include <future>

using namespace std;

static const int TIMEOUT_SECONDS = 3;
static const int TIMEOUT_USECONDS = 0;
static const int HIGH_INTEREST[] = { 
    22, 2222,   /* SSH */
    23, 2323,   /* Telnet */
    25,         /* SMTP */
    53,         /* DNS */
    80, 8080,   /* HTTP */
    445
    3306,       /* MySQL */
    3389        /* RDP */
};

static bool port_is_open(int port, string target)
{
    struct sockaddr_in address;
    short int sock = -1;
    fd_set fdset;
    struct timeval tv;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(target.c_str()); /* assign the address */
    address.sin_port = htons(port);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    connect(sock, (struct sockaddr*) &address, sizeof(address));

    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);
    tv.tv_sec = TIMEOUT_SECONDS;
    tv.tv_usec = TIMEOUT_USECONDS;

    if (select(sock + 1, NULL, &fdset, NULL, &tv) == 1) {
        int so_error = -1;
        socklen_t len = sizeof so_error;

        getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
        
        if (so_error == 0) {
            close(sock);
            return true;
        } else if (so_error != 111) {
            return false;
        } else {
            close(sock);
            return false;
        }
    }
    
    return false;
}

/** For asynchronous call in range_scan */
bool task(string ip, int port)
{
    return port_is_open(port, ip);
}

vector<bool> Scanner::range_scan(int lower, int higher)
{
    vector<bool> ports;
    
    for (int i = 0; i < lower - 1; i++)
        ports.push_back(false);

    string ip = this->target;
    for (int port = lower; port <= higher; port++) {
        auto future = async(task, ip, port);
        ports.push_back(future.get());
    }

    return ports;
}

vector<bool> Scanner::quick_scan()
{
    cout << "Quick scan on " << this->target << "..." << endl;
    return range_scan(0, 2000);
}

vector<bool> Scanner::high_interest_scan()
{
    cout << "High interest scan on " << this->target << "..." << endl;
    vector<bool> ports;

    for (int i = 0; i < HIGH_INTEREST[0] - 1; i++)
        ports.push_back(false);
    
    string ip = this->target;
    for (int port : HIGH_INTEREST) {
        auto future = async(task, ip, port);
        ports.push_back(future.get());
    }
    
    return ports;
}
