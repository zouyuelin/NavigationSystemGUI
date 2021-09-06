#ifndef TELNET_H
#define TELNET_H

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

class telnet
{
public:
    telnet(std::string ipAddress);
    ~telnet();
    int sock2;
    void connect();
    bool SendMess(std::string str);
    std::string reciveMess();
    bool HasConnect;

protected:
    void telnetnvt();
};

#endif // TELNET_H
