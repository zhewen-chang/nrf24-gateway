#ifndef __DATA_SERVER_H__
#define __DATA_SERVER_H__

#include <mysql/mysql.h>

class Server {
public:
    Server(int gateway_number);

    bool init(void);
    int log(char *payload,int pipe);
    bool log(int id, char *level, char *sign, int pipe);
    int getId(void);
    int getpipe(void);
    bool regist(int id,char *level);
    

private:
    MYSQL *conn;
    int gateway_number;

    bool connect(void);
    void close(void);
    bool deregist(int id);
};

#endif