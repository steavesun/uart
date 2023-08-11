#define MAX 1024

#include <stdio.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <pthread.h>



// Library for Uart
#include <asm/termbits.h>

using namespace std; 


class UART
{

    // Field

private:
  
    struct termios2 UartTerm;
    char File[1024]; 
    int BaudRate;
    int Uartfd;
    char RxBuffer[1024] = {0,};

    // Method
public:
  
    UART(int, int);
    ~UART();
    UART* Get_This(); 
    
    void Set_BaudRate(int);
    int Get_BaudRate();
     void Transmit(char *) const;
     char Receive(); 

};




UART ::UART(int Number, int BaudRate)
{

    sprintf(File,"/dev/ttyS%d",Number); 
    Uartfd = open(File, O_RDWR | O_NOCTTY);
    ioctl(Uartfd, TCGETS2, &UartTerm);
    UartTerm.c_cflag &= ~CBAUD;
    UartTerm.c_cflag |= BOTHER;
    UartTerm.c_cflag |= CS8;
    UartTerm.c_cflag |= CLOCAL;
    UartTerm.c_cflag |= CREAD;
    UartTerm.c_ospeed = BaudRate;
    UartTerm.c_ispeed = BaudRate;
    ioctl(Uartfd, TCSETS2, &UartTerm);
}

void UART ::Transmit(char* Buffer) const
{

    int state = write(Uartfd, Buffer, strlen(Buffer));

    if (state == -1)
    {

        printf("Transmit Error Code = %d\n", state);
    }

}
char UART :: Receive()
{
   char Buffer;
   int state = read(Uartfd, &Buffer, 1);
   if (state == -1)
   {
        printf("Receive Error Code = %d\n", state);
   }

   return Buffer; 


}

void UART ::Set_BaudRate(int BaudRate)
{
    
    
    UartTerm.c_ospeed = BaudRate;
    UartTerm.c_ispeed = BaudRate;
    this->BaudRate = BaudRate;
}
int UART ::Get_BaudRate()
{

    return BaudRate;
}

UART* UART :: Get_This()
{

return this; 

}

UART::~UART()
{
    
    cout << "--- Destructing Object : " << this << " ---" << endl;  
     close(Uartfd);
    
  
}




