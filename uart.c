#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // file open read write close 

#include <fcntl.h> 
#include <sys/ioctl.h>
#include <string.h>
#include <pthread.h> 

// Library for Uart
#include <asm/termbits.h>


int fd; 

pthread_t tid; 


void Uart_Init(char* ,int );
char* Uart_Receive(const int );
void Uart_Transmit(const char* ,const size_t  );




void* Uart_Receive_Task(void* argument)
{

     int Message = *(int*)argument; 

     printf("Message = %d\n",Message);


    while(1)
    {
         
       
        char* Data = Uart_Receive(Message);
        printf("Uart Receive Data = %s",Data);  
      
      
        /*
        if ( strcmp(Data,"quit\n") == -1 )
        {


            return;

        }
        */

        free(Data);
        

        
            
            


        

       
          
                         

    }
    

}

int main(int argc, char* argv[])
{
 
   
          

     int Argument;
   
     Uart_Init("3",115200);    
      
  
    printf("Message For Uart Receive Task : ");scanf("%d",&Argument);
    
    pthread_create(&tid,NULL,&Uart_Receive_Task,&Argument); 



    while(1)
    {
        
                
        Uart_Transmit("0x0806\n",strlen("0x0806\n"));
        sleep(1);
           

    }
    
    pthread_join(tid,NULL); 
    
    


    return 0; 
}



void Uart_Init(char* Value,int BaudRate)
{


    struct termios2 UartTerminal;
    char Target[1024] = "/dev/ttyS";
    strcat(Target,Value); 
                 
    fd = open(Target, O_RDWR | O_NOCTTY );

    if ( fd == -1 )
    {
       printf("Error to Open The %s\n",Target); 
       exit(1); 


    }
    else printf("Succeed to Open The %s\n",Target); 
    
         
        
    // it means what ?

    ioctl(fd,TCGETS2,&UartTerminal);
  
    UartTerminal.c_cflag &= ~CBAUD;
    UartTerminal.c_cflag |= BOTHER;

    UartTerminal.c_cflag |= CS8;
    UartTerminal.c_cflag |= CLOCAL;
    UartTerminal.c_cflag |= CREAD;

    UartTerminal.c_ospeed = BaudRate;
    UartTerminal.c_ispeed = BaudRate;

    ioctl(fd,TCSETS2,&UartTerminal); 





}



void Uart_Transmit(const char* str,const size_t len )
{

     

    int state = write(fd,str,len);

    if( state == -1 )
    {
       
        printf("Error to Transmit Data\n");
        printf("write function return %d\n",state);    
        
        
    }
     
}


char* Uart_Receive(const int len)
{
    

    char* RxData = (char*)malloc(sizeof(char)*len); 
    bzero(RxData,sizeof(RxData));
    int state = read(fd,RxData,len);

    if( fd == -1)
    {
        printf("Error to Transmit Data\n");
        printf("read function return %d\n",state);    

    }
       

     
    return RxData;

   

}
