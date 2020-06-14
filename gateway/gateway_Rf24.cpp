#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <RF24/RF24.h>
#include "gateway_RF24.h"
#include "dataServer.h"

using namespace std;

RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);
Server data(GATEWAY_NUMBER);

int main(int argc,char *argv[])
{
    radio.begin();
    radio.enableDynamicPayloads();
    radio.setPALevel(4);
    radio.openReadingPipe(0, (uint8_t*)"0pipe");
    radio.openReadingPipe(1, (uint8_t*)"1pipe");
    radio.openReadingPipe(2, (uint8_t*)"2");
    radio.openReadingPipe(3, (uint8_t*)"3");
    radio.openReadingPipe(4, (uint8_t*)"4");
    radio.openReadingPipe(5, (uint8_t*)"5");
    radio.openWritingPipe((uint8_t*)"gaway");
    radio.startListening();
    radio.printDetails();

    bcm2835_gpio_fsel(20, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_clr(20);    
    
    while(1){
        uint8_t pipeNo, payload[20]="";

        while(radio.available(&pipeNo)) {   

            bcm2835_gpio_set(20);
            
            radio.read(payload, 5); 
            cout << payload << endl;

            if(!strcmp((char*)payload,"RGS")){
                radio.stopListening();

                // getid
                int id=data.getId();
                char ids[10];
                sprintf(ids,"%03d|%01d",id);
                
                radio.write(ids,6);

                //reg
                if (!data.regist(id,"Low",pipe)) {
                    cout<< "Reg failed" <<endl;
                }
                    
                radio.startListening();
                bcm2835_gpio_clr(20);    
                continue;
            }

            int result=data.log((char *)payload, pipeNo);
            if(result==5){
                char S[]="Sleep";
                radio.stopListening();
                radio.write(S,6);
                radio.startListening();
            }
            else if(result==6){
                char S[]="NOSleep";
                radio.stopListening();
                radio.write(S,8);
                radio.startListening();
            }
            else if (result==0)
                cout<< "Log failed" <<endl;

            bcm2835_gpio_clr(20);    
        }
    }
}