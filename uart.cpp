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

// 1st App Implement
// 2nd Analysis on Linux UART API.



void *Uart_Tx_Task(void* argu)
{
  
     cout << "Uart Tx Task Get : ";
     cout << (char*)argu << endl; 
     
   //  printf("%s\n",(char*)argu); 

   while(1)
   {


        cout << "--- Uart Tx Task ---" << endl; 
        sleep(1);



   }



   return argu; 

   


}



void *Uart_Rx_Task(void *argu)
{
 
 
     cout << "Uart Rx Task Get : ";
     cout << (char*)argu << endl; 

       // printf("%s\n",(char*)argu); 

    while(1)
    {

          cout << "--- Uart Rx Task ---" << endl; 
          sleep(1); 


    }


       return argu; 



}

class UART
{

    // Field

private:
    struct termios2 UartTerm;
    string Target;

    int BaudRate;
    int fd;

    // Method
public:
    UART(int, int);
    ~UART();
    void Set_BaudRate(int);
    int Get_BaudRate();

    void Transmit(char *) const;
    void Receive(char *);
};

UART ::UART(int Number, int BaudRate)
{

    Target = "/dev/ttyS" + to_string(Number);

    fd = open(Target.c_str(), O_RDWR | O_NOCTTY);

    // it means what ?
    ioctl(fd, TCGETS2, &UartTerm);
    UartTerm.c_cflag &= ~CBAUD;
    UartTerm.c_cflag |= BOTHER;
    UartTerm.c_cflag |= CS8;
    UartTerm.c_cflag |= CLOCAL;
    UartTerm.c_cflag |= CREAD;
    UartTerm.c_ospeed = BaudRate;
    UartTerm.c_ispeed = BaudRate;
    ioctl(fd, TCSETS2, &UartTerm);
}

void UART ::Transmit(char *Buffer) const
{

    int state = write(fd, Buffer, strlen(Buffer));

    if (state == -1)
    {

        printf("Transmit Error Code = %d\n", state);
    }
}
void UART ::Receive(char *Buffer)
{

    int state = read(fd, Buffer, sizeof(Buffer));
    if (state == -1)
    {
        printf("Receive Error Code = %d\n", state);
    }
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

UART::~UART()
{

    close(fd);
}

int main(int argc, char *argv[])
{


     pthread_t Uart_Tx_TID;
     pthread_t Uart_Rx_TID; 

     

    
     char Tx_Task_Argument[1024];
     char Rx_Task_Argument[1024];

     char* p_Tx_Task_Argument = new char[1024];
     char* p_Rx_Task_Argument = new char[1024];

     void* p_retVal_Tx_Task;
     void* p_retVal_Rx_Task;  


    
    cout << "Input Argument For Uart Transmit Task : ";
    //cin >> Tx_Task_Argument;
    cin >> p_Tx_Task_Argument;
   
    cout << "Input Argument For Uart Receive Task : ";
    //cin >> Rx_Task_Argument; 
    cin >> p_Rx_Task_Argument;
   

    //pthread_create(&Uart_Tx_TID, NULL, &Uart_Tx_Task, &Tx_Task_Argument);
    pthread_create(&Uart_Tx_TID, NULL, &Uart_Tx_Task, p_Tx_Task_Argument);
   // pthread_create(&Uart_Rx_TID, NULL, &Uart_Rx_Task, &Rx_Task_Argument);
     pthread_create(&Uart_Rx_TID, NULL, &Uart_Rx_Task, p_Rx_Task_Argument);


   // pthread_join(Uart_Tx_TID,NULL);
       // pthread_join(Uart_Tx_TID,NULL); 
    pthread_join(Uart_Tx_TID, &p_retVal_Tx_Task);
    pthread_join(Uart_Rx_TID, &p_retVal_Rx_Task);


    delete [] (char*)p_retVal_Tx_Task;
    delete [] (char*)p_retVal_Rx_Task;

    return 0;
}
