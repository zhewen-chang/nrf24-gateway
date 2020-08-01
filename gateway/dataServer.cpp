#include <mysql/mysql.h>
#include <iomanip>
#include <time.h>
#include "dataServer.h"
#include "dataServer_config.h"
#include <string.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>


bool Server::init(void)
{    
    conn = mysql_init(NULL);

    if(conn == NULL) { 
        return false;
    }

    return true; 
}


Server::Server(int _gateway_number)
        :gateway_number(_gateway_number)
{
    conn = NULL;
    getIP();
    gateway_regist();
}

void Server::getIP(void)
{
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;  

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa ->ifa_addr->sa_family==AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            strcpy(IP, addressBuffer);
         } else if (ifa->ifa_addr->sa_family==AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
        } 
    }

    if (ifAddrStruct!=NULL) 
        freeifaddrs(ifAddrStruct);//remember to free ifAddrStruct
}

bool Server::check_counter(void)
{

    MYSQL_RES *res_ptr; 
    MYSQL_ROW result_row; 
    bool result = false;

    char sql[128];
    sprintf(sql,"SELECT `counter` FROM `gateway` where ip='%s'",IP);
    int res = mysql_query(conn,sql);

    if (!res) {
        res_ptr = mysql_store_result(conn);
        
        if (res_ptr) {
            int row = mysql_num_rows(res_ptr);
            if (row) {     
                result_row = mysql_fetch_row(res_ptr);
                result = result_row[0];     
            } 
        } 
    }
    
    return result;
}

bool Server::log(int id, char *sign)
{

    if(!init()) {
        return false;
    }

    if (!connect()) {
        return false;
    }

    char sql[256];

    sprintf(sql, "INSERT ignore INTO `log`(`id`,`sign`, `near_gateway`) VALUES (%d, '%s', '%s')",id,sign,IP);
    int res = mysql_query(conn,sql);

    if(check_counter()==false){
        sprintf(sql, "UPDATE `customer` SET `counter`=false WHERE id=%d",id);
        res = mysql_query(conn,sql);
    }

    close();

    if (!res) {
        return true;
    } 

    return false;
}

/*decode payload*/
int Server::log(char *payload)
{
    char *dp = strdup(payload);
    char level[5];
    char sign[10];
    char ids[4];
    int id;

    strncpy(ids,dp+1,3);
    ids[3]='\0';
    id=atoi(ids);

    if(dp[0]=='L') {
        strcpy(level,"LOW");
    } else if(dp[0]=='M') {
        strcpy(level,"MID");
    } else if(dp[0]=='H') {
        strcpy(level,"HIGH");
    } else {
        return false;
    }
    
    if(dp[4]=='A') {
        strcpy(sign,"Alive");
    } else if(dp[4]=='W') {
        strcpy(sign,"Wakeup");
    } else if(dp[4]=='S'){
        strcpy(sign,"Sleep");
        if(deregist(id)==true){
            return 5;
        }
        else{
            return 6;
        }
    } else {
        return false;
    }

    return log(id,sign);
}

bool Server::connect(void)
{
    conn = mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASSWORD, DB_TABLE, 0, NULL, 0);

    if (conn) {
        return true;
    } 

    return false;
}

void Server::close()
{
    mysql_close(conn);
}

int Server::getId(void)
{
    MYSQL_RES *res_ptr; 
    MYSQL_ROW result_row; 

    if (!init()) {
        return -1;
    }

    if (!connect()) {
        return -1;
    }

    char sql[] = "SELECT `id` FROM `customer` order by `id` desc";
    int res = mysql_query(conn,sql);
    int max = -1;

    if (!res) {
        res_ptr = mysql_store_result(conn);
        
        if (res_ptr) {
            int row = mysql_num_rows(res_ptr);
            if (row) {     
                result_row = mysql_fetch_row(res_ptr);
                max = atoi(result_row[0]);
                max = (max+1)%1000;
            } else {
                max = 0;
            }
        } 
    } 

    close();
    return max;
}

int Server::getpipe(void)
{
    MYSQL_RES *res_ptr; 
    MYSQL_ROW result_row; 

    if (!init()) {
        return -1;
    }

    if (!connect()) {
        return -1;
    }
    
    char sql[] = "SELECT `id`, `pipe` FROM `customer` order by `id` desc";
    int res = mysql_query(conn,sql);
    int max = -1;

    if (!res) {
        res_ptr = mysql_store_result(conn);
        
        if (res_ptr) {
            int row = mysql_num_rows(res_ptr);
            if (row) {     
                result_row = mysql_fetch_row(res_ptr);
                max = atoi(result_row[1]);
                max = (max+1)%6;
            } else {
                max = 0;
            }
        } 
    } 

    close();
    return max;
}

bool Server::regist(int id)
{
    char sql[128]; 

    if (!init()) {
        return false;
    }

    if (!connect()) {
        return false;
    }

    if (check_counter()==false){
        return false;
    }

    sprintf (sql, "INSERT INTO `customer`(`id`, `pipe`) VALUES (%d, %d)", id,  id%6);
    int res = mysql_query(conn,sql);

    close();

    return !res;
}

bool Server::deregist(int id)
{
    char sql[128];

    if (!init()) {
        return false;
    }

    if (!connect()) {
        return false;
    }

    if(check_counter()==false){
        return false;
    }

    sprintf (sql, "DELETE FROM `customer` WHERE `id`=%d",id);
    int res = mysql_query(conn,sql);

    sprintf (sql, "DELETE FROM `log` WHERE `id`=%d",id);
    res = mysql_query(conn,sql);

    close();

    return !res;
}

bool Server::gateway_regist(void)
{
    char sql[128]; 

    if (!init()) {
        return false;
    }

    if (!connect()) {
        return false;
    }

    sprintf (sql, "INSERT INTO `gateway`(`ip`, `counter`) VALUES ('%s', false)", IP);

    int res = mysql_query(conn,sql);

    close();

    return !res;
}
