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
#include "./uart.h"

using namespace std;


void *KeyButton_Rx_Task_Thread(void *);
void *UART_Tx_Task_Thread(void *argu);
void *UART_Rx_Task_Thread(void *argu);

UART *pUart;

int main(int argc, char *argv[])
{

    int BaudRate;
    pthread_t UART_Rx_Task_Thread_TID;
    pthread_t KeyButton_Rx_Task_Thread_TID;

    cout << "Input Any BaudRate : ";
    cin >> BaudRate;

    pUart = new UART(3, BaudRate);

    pUart->Transmit("Hello I'm UART ...!!!\n");

    cout << "pUart : " << (int *)pUart << endl;
    cout << "pUart->Get_This() : " << (int *)pUart->Get_This() << endl;

    pthread_create(&UART_Rx_Task_Thread_TID, NULL, &UART_Rx_Task_Thread, NULL);
    pthread_create(&KeyButton_Rx_Task_Thread_TID, NULL, &KeyButton_Rx_Task_Thread, NULL);

    pthread_join(UART_Rx_Task_Thread_TID, NULL);
    pthread_join(KeyButton_Rx_Task_Thread_TID, NULL);

    return 0;
}

void *KeyButton_Rx_Task_Thread(void *argu)
{

    cout << "---  KeyButton Receive Task Thread Entry Point ---" << endl;

    int idx = 0;
    char c;
    char *Buffer = new char[1024];
    memset(Buffer, 0x0, sizeof(char) * 1024);

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

            cout << "Console : " << Buffer;
            pUart->Transmit(Buffer);

            if (strcmp("quit\n", Buffer) == 0)
            {
                cout << "--- Exit Process ---" << endl;
                memset(Buffer, 0x0, sizeof(Buffer) * 1024);
                delete pUart;
                delete[] Buffer;

                exit(1);
            }
            memset(Buffer, 0x0, sizeof(Buffer) * 1024);
            idx = 0;
        }
    }

    return NULL;
}

void *UART_Rx_Task_Thread(void *argu)
{

    cout << "---  Uart Receive Task Thread Entry Point ---" << endl;
    int idx = 0;
    char *Buffer = new char[1024];
    char c;

    memset(Buffer, 0x0, 1024);

    while (1)
    {

        c = pUart->Receive();
        Buffer[idx++] = c;

        if (c == '\b')
        {
            putchar(' ');
        }
        else if (c == '\n')
        {

            cout << "From Vim : " << Buffer;

            if (strcmp("quit\n", Buffer) == 0)
            {
                cout << "--- Exit Process ---" << endl;

                memset(Buffer, 0x0, sizeof(Buffer) * 1024);
                delete pUart;
                delete[] Buffer;

                exit(1);
            }

            bzero(Buffer, sizeof(Buffer));
            idx = 0;
        }
    }

    return argu;
}
