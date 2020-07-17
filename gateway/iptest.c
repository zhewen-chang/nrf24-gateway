#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>

int main (int argc, const char * argv[]) {
struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;      

    getifaddrs(&ifAddrStruct);

    ifa = ifAddrStruct->ifa_next; 
    tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
    char addressBuffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
 
    printf("%s\n", addressBuffer);

    if (ifAddrStruct!=NULL) 
        freeifaddrs(ifAddrStruct);//remember to free ifAddrStruct
    return 0;
}