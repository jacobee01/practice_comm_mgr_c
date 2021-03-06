#include <pthread.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/poll.h>

#include "uart.h"
#include "types.h"


#define QUEUE_MAX   512

uint8 uart_buffer[QUEUE_MAX];
uint32 q_head = 0;


void enqueue_buffer(uint8 *data, uint8 size)
{
    int i = 0;

    for(i=0; i<size; i++)
    {
        uart_buffer[q_head] = data[i];
        if(++q_head>=QUEUE_MAX)
        {
            q_head = 0;
        }
    }
}




void *thread_uart(void *arg)
{
    int i,res,j;
    int fd_arg = *((int *)arg);

    /* poll */
    int ndx;
    int cnt;
    char buf[1024];
    struct pollfd poll_events;
    int poll_state;

    poll_events.fd = fd_arg;
    poll_events.events = POLLIN | POLLERR;
    poll_events.revents = 0;


    printf("thread uart start!!!\n");
    printf("fd : %d\n", fd_arg);

    while(1)
    {
        poll_state = poll((struct pollfd*)&poll_events, 1, 1000 );

        if(poll_state > 0)
        {
            if(poll_events.revents & POLLIN )
            {
                cnt = read(fd_arg, buf, 1024);
                enqueue_buffer(buf,cnt);
            }
            if(poll_events.revents & POLLERR)
            {
                printf("POLLERR\n");
                // break;
            }

        }
        else if (poll_state < 0)
        {
            printf("Uart poll error!\n");
            //break;
        }
        else if(poll_state == 0)
        {
            printf("wait uart...\n");
        } 
        

        // memset(buf, 0, sizeof(buf));
        // res = read(fd_arg, buf, 255);
        // if(res>0)
        // {
        //     printf("response : %d ", res);
        //     for(i=0; i < res; ++i)
        //     {
        //         printf("%x ", buf[i]);
        //     }
        //     printf("\n");
        //     enqueue_buffer(buf,res);
        //     printf("buffer : ");
        //     for(j=0; j<QUEUE_MAX;j++)
        //     {
        //         printf("%x ", uart_buffer[j]);
        //     }

        // }
    }
    close(fd_arg);
}

int uart_init(void)
{
    int fd;
    struct termios oldtio, newtio;
    

    fd = open("/dev/ttyHS1", O_RDWR | O_NOCTTY | O_NONBLOCK );

    if(fd<0)
    {
        printf("UART Open error");
        return -1;
    }

    tcgetattr(fd, &oldtio);
    newtio=oldtio;
    newtio.c_cflag = B460800;   // ?????? ?????? 460800
    newtio.c_cflag |= CS8;      // ????????? ?????? 8bit
    newtio.c_cflag |= CLOCAL;   // ?????? ????????? ???????????? ?????? ?????? ?????? ?????? ??????
    newtio.c_cflag |= CREAD;    // ????????? ??????, ????????? ????????????
    newtio.c_iflag = IGNPAR;    //parity bit ??????
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN]  = 1;

    tcflush(fd,TCIFLUSH);
    tcsetattr(fd,TCSANOW, &newtio);


    pthread_t thread;

    pthread_create(&thread, NULL, thread_uart, (void*)&fd );

    // printf("tid : %x\n" , pthread_self());
    printf("tid : %x\n");

}