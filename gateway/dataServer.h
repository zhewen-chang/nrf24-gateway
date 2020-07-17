#ifndef __DATA_SERVER_H__
#define __DATA_SERVER_H__

#include <mysql/mysql.h>

class Server {
public:
    Server(int gateway_number);

    bool init(void);
    int log(char *payload);
    bool log(int id, char *sign);
    int getId(void);
    int getpipe(void);
    bool regist(int id);
    bool gateway_regist(void);    

private:
    MYSQL *conn;
    int gateway_number;
    char IP[20];

    bool connect(void);
    void close(void);
    bool deregist(int id);
    void getIP(void);
    bool check_counter(void);
};

#endif