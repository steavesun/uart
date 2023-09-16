#define MAX 1024

#include <stdio.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
// #include <asm/termbits.h>
// #include <asm/termios.h>
#include <termios.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/poll.h>

#include <termios.h>

// Library for Uart

using namespace std;

void *Rx_Msg_Thread(void *);
void *Rx_Key_Thread(void *);
void Error_Handle();

class UART
{

    // Field

private:
    struct termios Term_Option;
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

    UartFD = open(File, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK); // Default Blocking //| O_NDELAY | O_NONBLOCK
    if (UartFD < 0)
    {
        Error_Handle();
        exit(0);
    }
    else
    {

        printf("Open Uart Device File : [ OK ]\n");
        printf("`-File Descriptor : %d\n", UartFD);
        memset(&Term_Option, 0x0, sizeof(Term_Option));

        //  Term_Option.c_cflag |= CS8 | CLOCAL | CREAD;
        /* Char Size : 8 , Ignore Moderm Signal , Read Char : OK */
        /* Input Mode Flag */
        // Term_Option.c_iflag |= IGNPAR; /* Ignore Parity Bits*/
        /* Output Mode Flag */
        // Term_Option.c_oflag |= 0x0;
        /* Local Mode Flag */
        // Term_Option.c_lflag |= 0x0;

        //  Term_Option.c_cc[VTIME] = 0;
        //   Term_Option.c_cc[VMIN] = 1;

        struct termios Origin_Term;
        tcgetattr(UartFD, &Origin_Term);

        Term_Option = Origin_Term;

        Term_Option.c_cflag |= CS8 | CLOCAL | CREAD & ~CSIZE;
        Term_Option.c_iflag = 0x1 & ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON | IXOFF | IXANY);

        Term_Option.c_lflag = 0x0;
        Term_Option.c_oflag = 0x0;

        Term_Option.c_cc[VTIME] = 0;
        Term_Option.c_cc[VMIN] = 1;

        switch (BaudRate)
        {

        case 115200:
            Term_Option.c_cflag |= B115200;
            printf("BuadRate is Set to B115200\n");
            break;

        case 57600:
            Term_Option.c_cflag |= B57600;
            printf("BuadRate is Set to B57600\n");
            break;

        case 38400:
            Term_Option.c_cflag |= B38400;
            printf("BuadRate is Set to B38400\n");
            break;

        case 19200:
            Term_Option.c_cflag |= B19200;
            printf("BuadRate is Set to B19200\n");
            break;

        case 9600:
            Term_Option.c_cflag |= B9600;
            printf("BuadRate is Set to B9600\n");
            break;

        default:
            Term_Option.c_cflag |= B115200;
            printf("You Enter Wrong BaudRate\n");
            printf("BuadRate is Set to B115200\n");
            break;
        }

        tcflush(UartFD, TCIOFLUSH);
        tcsetattr(UartFD, TCSANOW, &Term_Option);

        //  mmap(NULL,getpagesize(), PROT_READ | PROT_WRITE , MAP_SHARED,UartFD,0);
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
    if (close(UartFD) != 0)
        Error_Handle();
    else
        printf("Close UartFD [ OK ]\n", UartFD);
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

    // pUart->Transmit("Hello I'm UART ...!!!\n");

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

    if (pthread_join(Rx_Msg_Tid, NULL) != 0)
        Error_Handle();
    else
        printf("Join Rx_Msg_Thread Routine : [ OK ]\n");
    if (pthread_join(Rx_Key_Tid, NULL) != 0)
        Error_Handle();
    else
        printf("Join Rx_Key_Thread Routine : [ OK ]\n");

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
            tcflush(UartFD, TCIFLUSH);
            write(UartFD, Buffer, sizeof(Buffer));

            if (strcmp(Buffer, "quit\n") == 0)
            {

                printf("You Order Quit\n");
                if (pthread_cancel(Rx_Msg_Tid) == 0)
                    printf("Cancel Rx_Msg_Thread Routine : [ OK ]\n");
                else
                    Error_Handle();

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
    int idx = -1;
    char Buffer[1024];
    char c;
    int UartFD = pUart->Get_UartFD();
    bzero(Buffer, 1024);

    struct pollfd Poll_Event = {
        .fd = UartFD,
        .events = POLLIN | POLLERR,
        .revents = 0

    };

    while (1)
    {

        int Poll_State = poll(&Poll_Event, 1, 1000);

        if (Poll_State > 0)
        {

            if (Poll_Event.revents == POLLIN)
            {

                //   printf("Poll_Event.revents = 0x%x\n",Poll_Event.revents);
                int Status = read(UartFD, &c, 1);
                // putchar(c);

                if ((Status != 0) && (Status != -1))
                {
                    idx++;
                    Buffer[idx] = c;

                    if (c == '\b')
                    {
                        printf("Back Space\n");
                        Buffer[idx] = ' ';
                    }
                    else if (c == '\n')
                    {

                        printf("Receive : %s", Buffer);
                        if (strcmp(Buffer, "quit\n") == 0)
                        {

                            printf("Quit Message is Ordered\n");
                            printf("Quit The Program\n");
                        }
                        tcflush(UartFD, TCIFLUSH);
                        bzero(Buffer, sizeof(Buffer));
                        idx = -1;
                    }
                }
            }
            else if (Poll_Event.revents & POLLERR)
                Error_Handle();
        }
    }

    printf("End of Rx_Msg_Thread Routine\n");
}
void Error_Handle()
{

    char Buffer[1024];
    bzero(Buffer, 1024);
    sprintf(Buffer, "Error(%d) : %s", errno, strerror(errno));
    printf("%s\n", Buffer);
    exit(1);
}
