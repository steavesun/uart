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

// 1st App Implement
// 2nd Analysis on Linux UART API.


void* KeyButton_Rx_Task_Thread(void* );
void *UART_Tx_Task_Thread(void *argu);
void *UART_Rx_Task_Thread(void *argu);





int main(int argc, char *argv[])
{


     pthread_t UART_Tx_Task_Thread_TID;
     pthread_t UART_Rx_Task_Thread_TID; 
     

     /*
     char* p_UART_Tx_Task_Thread_Argument = new char[1024];
     char* p_UART_Rx_Task_Thread_Argument = new char[1024];

     void* p_retVal_UART_Tx_Task_Thread;
     void* p_retVal_UART_Rx_Task_Thread;
     */ 
  


     UART* p_Uart = new UART ( 3, 115200 ); 

     p_Uart->Transmit("Hello I'm UART ...!!!\n"); 


    /*
    cout << "Input Argument For Uart Transmit Task : ";
    cin >> p_UART_Tx_Task_Thread_Argument;
   
    cout << "Input Argument For Uart Receive Task : ";
    cin >> p_UART_Rx_Task_Thread_Argument;
    */ 
   

  
  //  pthread_create(&UART_Tx_Task_Thread_TID, NULL, &UART_Tx_Task_Thread, p_UART_Tx_Task_Thread_Argument);
  //  pthread_create(&UART_Rx_Task_Thread_TID, NULL, &UART_Rx_Task_Thread, p_UART_Rx_Task_Thread_Argument);

    pthread_create(&UART_Tx_Task_Thread_TID, NULL, &UART_Tx_Task_Thread, NULL);
    pthread_create(&UART_Rx_Task_Thread_TID, NULL, &UART_Rx_Task_Thread, NULL);

    pthread_join(UART_Tx_Task_Thread_TID, NULL);
    pthread_join(UART_Rx_Task_Thread_TID, NULL);


    
    // pthread_join(UART_Tx_Task_Thread_TID, &p_retVal_UART_Tx_Task_Thread);
  //  pthread_join(UART_Rx_Task_Thread_TID, &p_retVal_UART_Rx_Task_Thread);


   // delete [] (char*)p_retVal_UART_Tx_Task_Thread;
   // delete [] (char*)p_retVal_UART_Rx_Task_Thread;

    return 0;
}





void* KeyButton_Rx_Task_Thread(void* argu)
{


    cout << "---It's a Entry Point of KeyButton Receive Task Thread---" << endl;

    char Buffer[1024];
    memset(Buffer,0x0,sizeof(Buffer)); 

    while(1)
    {


        cout << "\nUnder Infinite Loop of KeyButton_Rx_Task Thread..." << endl;
        scanf("%s",Buffer); 
        cout << "Console = " << Buffer << endl; 
        

       

        sleep(1); 




    }


    return NULL; 


    
}





void *UART_Tx_Task_Thread(void* argu)
{
    


   cout << "---It's a Entry Point of Uart Transmit Task Thread---" << endl;

   while(1)
   {


       cout << "\nUnder Infinite Loop of UART_Tx_Task Thread..." << endl;
       
       
        sleep(1);



   }



   return argu; 

   


}


void *UART_Rx_Task_Thread(void *argu)
{
 
    cout << "---It's a Entry Point of Uart Receive Task Thread---" << endl;
    
    while(1)
    {

          
          cout << "\nUnder Infinite Loop of UART_Rx_Task Thread..." << endl;
          sleep(1); 


    }

       return argu; 

}
