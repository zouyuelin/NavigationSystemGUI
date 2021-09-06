#include <iostream>
#include "telnet.h"
#include <time.h>

#define WILL  251
#define WONT  252
#define DO    253
#define DONT  254
#define IAC   255

using namespace std;

telnet::telnet(string ipAddress):HasConnect(false)
{
    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    //????23???
    //server_addr.sin_addr.S_un.S_addr=INADDR_ANY;
    server_addr.sin_port = htons(IPPORT_TELNET);//IPPORT_TELNET
    server_addr.sin_addr.s_addr = inet_addr(ipAddress.c_str());

    if ((sock2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        cout<<"The socket created failed\n";
    if (::connect(sock2, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
        cout<<"pmac connect failed. Please check the pmac ip address!\n";
    else
    {
        cout<<"PMAC connected!\n";
    }
    usleep(3000);
    //协商
    telnetnvt();
}

telnet::~telnet()
{
    close(sock2);
}

void telnet::connect()
{
    usleep(500);
    SendMess("root");//root
    reciveMess();
    sleep(1);


    SendMess("deltatau");//deltatau
    sleep(1);
    reciveMess();

    SendMess("gpascii");//gpascii
    sleep(1);
    reciveMess();

}

bool telnet::SendMess(string str)
{
    //char pBuff[256]={0};
    //strncpy(pBuff, str.c_str(), str.length());

    if(send(sock2,str.c_str(),str.length(),0)<0)
    {
        cout<<"send fail:"<<str<<std::endl;
    }
    else if(!HasConnect)
    {
        HasConnect = true;
        cout<<"Send the massage successful\n";
    }

    send(sock2, "\r\n", 1, 0);
    return true;
}

string telnet::reciveMess()
{
    char msg[1024] = {0};
    memset(msg, 0, 1024);
    int res = recv(sock2, msg, 1024, 0);
    if (res == -1)
    {
        cout<<"can't recive the message\n";
        return 0;
    }
    if (res == 0)
    {
        usleep(10);
        cout<<"can't get the message!\n";
    }
    string recivemsg = string(msg);
    return recivemsg;
}
void telnet::telnetnvt()
{
    unsigned char strRecvBuf[3] = {0};
    while(1)
    {
        //3 个字节
        //IAC DONT ECHO
        //第一个 IAC 一个字节 (0xff)
        if(recv(sock2, (char*)(strRecvBuf), 1, 0)!=1)
            return;

        if(strRecvBuf[0] != IAC)
            break;

        if(recv(sock2, (char*)(strRecvBuf+1), 2, 0)!=2)
            return;

        switch(strRecvBuf[1])
        {
            case WILL:	//WILL
                strRecvBuf[1] = DO;	//DO
                break;

            case WONT:	//WONT
                strRecvBuf[1] = DONT;	//DONT
                break;

            case DO:	//DO
            case DONT:	//DONT
                strRecvBuf[1] = WONT;	//WONT
                break;

            default:
                return;
        }
        send(sock2, (char*)strRecvBuf, 3, 0);
    }
}
