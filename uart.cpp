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

void *Rx_Msg_Thread(void *);
void *Rx_Key_Thread(void *);
void Error_Handle(); 

class UART
{

    // Field

private:
    struct termios2 UartTerm;
    char File[1024];
    int BaudRate;
    int UartFD;
    char RxBuffer[1024] = {
        0,
    };

    // Method
public:
    UART(char *File, int Baud);
    ~UART();
    UART *Get_This();

    void Set_BaudRate(int);
    int Get_BaudRate();
    void Transmit(char *) const;
    char Receive();

    int Get_UartFD();
};

UART ::UART(char *File, int BaudRate)
{

    printf("Constructiong Uart Instance\n");

    UartFD = open(File, O_RDWR); // Default Blocking
    if (UartFD == -1)
    {
        Error_Handle();
        exit(0);
    }
    else
    {

        printf("Open Uart Device File : [ OK ]\n");
        printf("UartFD : %d\n", UartFD);

        if (ioctl(UartFD, TCGETS2, &UartTerm) != 0)
            Error_Handle();
        else
        {

            UartTerm.c_cflag &= ~CBAUD;
            UartTerm.c_cflag |= BOTHER;
            UartTerm.c_cflag |= CS8;
            UartTerm.c_cflag |= CLOCAL;
            UartTerm.c_cflag |= CREAD;
            UartTerm.c_ospeed = BaudRate;
            UartTerm.c_ispeed = BaudRate;
            if (ioctl(UartFD, TCSETS2, &UartTerm) != 0)
                Error_Handle();
        }
    }
}

void UART ::Transmit(char *Buffer) const
{

    int state = write(UartFD, Buffer, strlen(Buffer));

    if (state == -1)
    {

        printf("Transmit Error Code = %d\n", state);
    }
}
char UART ::Receive()
{
    char Buffer;
    int state = read(UartFD, &Buffer, 1);
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

UART *UART ::Get_This()
{

    return this;
}

UART::~UART()
{

  printf("Destruct Uart Instance %x : [ OK ]\n");
  if( close(UartFD) != 0 ) Error_Handle();
  else printf("Close UartFD [ OK ]\n",UartFD); 
    
}

int UART ::Get_UartFD()
{

    return UartFD;
}

pthread_t Rx_Key_Tid;
pthread_t Rx_Msg_Tid;
UART *pUart;

int main(int argc, char *argv[])
{

    int BaudRate;
    
    char Device_File[1024];

    printf("Enter Uart Device File...\n");
    printf("For Example, If The File is /dev/ttyUSB0 Then Enter /dev/ttyUSB0\n");
    printf("For Example, If The File is /dev/tty0 Then Enter /dev/tty0\n");

    printf("Uart File : ");
    scanf("%s", Device_File);

    printf("You Enter Uart File [ %s ]\n", Device_File);

    printf("BaudRate : ");
    scanf("%d", &BaudRate);
    printf("You Enter BaudRate [ %d ]\n", BaudRate);

    pUart = new UART(Device_File, BaudRate);

    pUart->Transmit("Hello I'm UART ...!!!\n");

    cout << "pUart : " << (int *)pUart << endl;
    cout << "pUart->Get_This() : " << (int *)pUart->Get_This() << endl;

    if (pthread_create(&Rx_Msg_Tid, NULL, Rx_Msg_Thread, NULL) != 0)
        Error_Handle();
    else
        printf("Create Rx_Msg_Thread Routine : [ OK ]\n");
    if (pthread_create(&Rx_Key_Tid, NULL, Rx_Key_Thread, NULL) != 0)
        Error_Handle();
    else
        printf("Create Rx_Key_Thread Routine : [ OK ]\n");






    if (pthread_join(Rx_Msg_Tid, NULL) != 0) Error_Handle();
    else printf("Join Rx_Msg_Thread Routine : [ OK ]\n");
    if (pthread_join(Rx_Key_Tid, NULL) != 0) Error_Handle();
    else printf("Join Rx_Key_Thread Routine : [ OK ]\n");


    printf("Exit This Program\n");
    delete pUart;
    return 0;
}

void *Rx_Key_Thread(void *argu)
{
    int UartFD = pUart->Get_UartFD();
    int idx = 0;
    char c;
    char Buffer[1024];
    bzero(Buffer, 1024);

    getchar();

    while (1)
    {

        c = getchar();
        Buffer[idx++] = c;
        if (c == '\b')
        {

            putchar(' ');
        }
        else if (c == '\n')
        {

            
            printf("Console : %s", Buffer);
            write(UartFD, Buffer, strlen(Buffer));
            if (strcmp(Buffer, "quit\n") == 0)
            {

                printf("You Order Quit\n");
                if (pthread_cancel(Rx_Msg_Tid) != 0)
                    Error_Handle();
                else
                    printf("Cancel Rx_Msg_Thread Routine : [ OK ]\n");
                break;
            }

            bzero(Buffer, 1024);
            idx = 0;
        }
    }
    printf("End of Rx_Key_Thread Routine\n");
}

void *Rx_Msg_Thread(void *argu)
{

    printf("Under Rx_Msg_Thread Routine\n");
    int idx = 0;
    char Buffer[1024];
    char c;
    int UartFD = pUart->Get_UartFD();
    bzero(Buffer, 1024);

    while (1)
    {

        int Status = read(UartFD, &c, 1); // Blocking
        if ((Status != 0) && (Status != -1))
        {

            Buffer[idx++] = c;
            if (c == '\b')
            {
                putchar(' ');
            }
            else if (c == '\n')
            {
            
                printf("Receive : %s", Buffer);

                if (strcmp("quit\n", Buffer) == 0)
                {
                    printf("CounterPart Orders Quit\n");
                    if (pthread_cancel(Rx_Key_Tid) != 0) Error_Handle();
                    else printf("Cancel Key_Msg_Thread Routine : [ OK ]\n");

                    break;
                }
                bzero(Buffer, 1024);
                idx = 0;
            }
        }
        else  Error_Handle();
    }
          

    printf("End of Rx_Msg_Thread Routine\n");
}
void Error_Handle()
{

    char Buffer[1024];
    bzero(Buffer, 1024);
    sprintf(Buffer, "Error(%d) : %s\n", errno, strerror(errno));
    printf("%s\n",Buffer); 
   
}