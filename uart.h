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