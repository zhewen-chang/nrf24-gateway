#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <RF24/RF24.h>

using namespace std;

RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

int main(int argc,char *argv[])
{
    char str[1024];
    radio.begin();
    radio.enableDynamicPayloads();
    radio.setPALevel(4);
    radio.openReadingPipe(0, (uint8_t*)"0pipe");
    radio.openReadingPipe(1, (uint8_t*)"1pipe");
    radio.openReadingPipe(2, (uint8_t*)"2");
    radio.openReadingPipe(3, (uint8_t*)"3");
    radio.openReadingPipe(4, (uint8_t*)"4");
    radio.openReadingPipe(5, (uint8_t*)"5");
    radio.startListening();
    radio.printDetails();

    bcm2835_gpio_fsel(20, BCM2835_GPIO_FSEL_OUTP);

    while(1){
        uint8_t pipeNo, payload[20]="";

        while(radio.available(&pipeNo)) {   
            bcm2835_gpio_set(20);
            radio.read(payload, 5); 
            cout<<payload;
            printf(" %d",pipeNo);
            sprintf(str,"curl 'http://134.208.6.62/exec/setData.php?data=%s&code=DCLAB&gateway=%s&pipe=%d'",payload,argv[1],pipeNo);
            cout<<" send: "<<str<<" result: "<<system(str)<<endl;
            bcm2835_gpio_clr(20);    
        }
    }
}