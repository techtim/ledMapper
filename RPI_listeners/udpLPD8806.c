/*
 * udp_listen.c
 *
 * Tim Tavlintsev 2016 < tim@tvl.io >
 * 
 * Illustrate simple UDP connection
 * It opens a blocking socket and
 * listens for messages in a for loop.  It takes the name of the machine
 * that it will be listening on as argument.
 * Then messages sending to SPI GPIO pins using lpd8806 lib (https://github.com/eranrund/blinky-pants/tree/master/lpd)
 */

// compile using:
// sudo gcc  -lm ./lpd8806led.c ./udpLPD8806.c -o udpLPD8806

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "lpd8806led.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>

static const char *device = "/dev/spidev0.0";
static const int leds = 2000;
static const int message_size = 2048*3;
static int continue_looping;

#define SERVER_PORT 3000

void stop_program(int sig);
void HSVtoRGB(double h, double s, double v, double *r, double *g, double *b);


main(int argc, char *argv[]) {
  char message[message_size];
  int sock;
  struct sockaddr_in name;
  struct hostent *hp, *gethostbyname();
  int bytes;

  printf("Listen activating.\n");

  /* Create socket from which to read */
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)   {
    perror("Opening datagram socket");
    exit(1);
  }
  
  /* Bind our local address so that the client can send to us */
  bzero((char *) &name, sizeof(name));
  name.sin_family = AF_INET;
  name.sin_addr.s_addr = htonl(INADDR_ANY);
  name.sin_port = htons(SERVER_PORT);
  
  if (bind(sock, (struct sockaddr *) &name, sizeof(name))) {
    perror("binding datagram socket");
    exit(1);
  }
  
  printf("Socket has port number #%d\n", ntohs(name.sin_port));
  // -------- LPD8806 ----------
  lpd8806_buffer buf;
  int fd;
  int return_value;
  lpd8806_color *p;
  int i;
  double h, r, g, b;

  /* Open the device file using Low-Level IO */
  fd = open(device,O_WRONLY);
  if(fd<0) {
    fprintf(stderr,"Error %d: %s\n",errno,strerror(errno));
    exit(1);
  }

  /* Initialize the SPI bus for Total Control Lighting */
  return_value = spi_init(fd);
  if(return_value==-1) {
    fprintf(stderr,"SPI initialization error %d: %s\n",errno, strerror(errno));
    exit(1);
  }

  /* Initialize pixel buffer */
  if(lpd8806_init(&buf,leds)<0) {
    fprintf(stderr,"Pixel buffer initialization error: Not enough memory.\n");
    exit(1);
  }

  /* Set the gamma correction factors for each color */
  set_gamma(2.2,2.2,2.2);

  /* Blank the pixels */
  for(i=0;i<leds;i++) {
    write_gamma_color(&buf.pixels[i],0x00,0x00,0x00);
  }
  send_buffer(fd,&buf);
  signal(SIGINT,stop_program);

  continue_looping = 1;

  while (continue_looping) {
    while ((bytes = read(sock, message, message_size)) > 0) {
      // message[bytes] = '\0';
      // printf("recv: %s\n", message);
      printf("%d  %d  %d\n",message[i*3+0], message[i*3+1], message[i*3+2]);

      for (i=0; i < bytes/3; i++) {
        write_gamma_color(&buf.pixels[i], message[i*3+0], message[i*3+1], message[i*3+2]);
      }
      send_buffer(fd,&buf);

      usleep(1000);
    }
  }

  for(i=0;i<leds;i++) {
    write_gamma_color(&buf.pixels[i],0x00,0x00,0x00);
  }
  send_buffer(fd,&buf);

  close(sock);

  return 0;
}

void stop_program(int sig) {
  /* Ignore the signal */
  signal(sig,SIG_IGN);
  /* stop the looping */
  continue_looping = 0;
  /* Put the ctrl-c to default action in case something goes wrong */
  signal(sig,SIG_DFL);
}