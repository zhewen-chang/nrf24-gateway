#include <mysql/mysql.h>
#include <iomanip>
#include <time.h>
#include "dataServer.h"
#include "dataServer_config.h"
#include <string.h>

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
}

bool Server::log(int id, char *level, char *sign, int pipe)
{
    char sql[256], times[100]; 
    time_t rawtime;
    struct tm *info;
    time( &rawtime );
    info = localtime( &rawtime );
    strftime(times, 100, "%Y-%m-%d %H:%M:%S", info);
    init();
    connect();
    sprintf(sql, "INSERT ignore INTO `log`(`id`, `level`, `time`, `sign`, `near_gateway`, `pipe`) VALUES (%d, '%s', '%s', '%s',%d,%d)",id,level,times,sign,gateway_number,pipe);
    int res = mysql_query(conn,sql);
    close();

    if (!res) {
        return true;
    } 

    return false;
}

bool Server::log(char *payload,int pipe)
{
    char *dp = strdup(payload);
    char level[5];
    char sign[10];
    char ids[4];
    int id;

    strncpy(ids,dp+1,3);
    ids[3]='\0';
    id=atoi(ids);

    if(dp[0]=='L')
        strcpy(level,"LOW");
    else if(dp[0]=='M')
        strcpy(level,"MID");
    else if(dp[0]=='H')
        strcpy(level,"HIGH");
    else {
        return false;
    }
    
    if(dp[4]=='A')
        strcpy(sign,"Alive");
    else if(dp[4]=='W')
        strcpy(sign,"Wakeup");
    else if(dp[4]=='S'){
        strcpy(sign,"Sleep");
        deregist(id);
    }
    else {
        return false;
    }

    strncpy(ids,dp+1,3);
    ids[3]='\0';
    id=atoi(ids);
    return log(id,level,sign,pipe);

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

    init(); 
    connect();

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

bool Server::regist(int id,char *level, int pipe)
{
    char sql[128]; 

    init();
    connect();

    sprintf (sql, "INSERT INTO `customer`(`id`, `level`, `pipe`) VALUES (%d, '%s', %d)", id, level, pipe);
    int res = mysql_query(conn,sql);

    close();

    return !res;
}

bool Server::deregist(int id)
{
    if(gateway_number!=1)
        return false;
    char sql[128];
    init(); 
    connect();
    sprintf (sql, "DELETE FROM `customer` WHERE `id`=%d",id);
    int res = mysql_query(conn,sql);
    close();
    return !res;
    
}

