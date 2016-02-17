/*
 * udpWS281x.c
 *
 * WS281x lib by Jeremy Garff with UDP listener by Tim Tavlintsev <tim@tvl.io>
 * Copyright (c) 2014 Jeremy Garff <jer @ jers.net>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 *     1.  Redistributions of source code must retain the above copyright notice, this list of
 *         conditions and the following disclaimer.
 *     2.  Redistributions in binary form must reproduce the above copyright notice, this list
 *         of conditions and the following disclaimer in the documentation and/or other materials
 *         provided with the distribution.
 *     3.  Neither the name of the owner nor the names of its contributors may be used to endorse
 *         or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

// compile using: scons
// sudo apt-get install scons

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "clk.h"
#include "gpio.h"
#include "dma.h"
#include "pwm.h"

#include "ws2811.h"


#define ARRAY_SIZE(stuff)                       (sizeof(stuff) / sizeof(stuff[0]))
#define TARGET_FREQ                             WS2811_TARGET_FREQ
#define GPIO_PIN                                18
#define GPIO_PIN_2                              17
#define DMA                                     5
#define LED_COUNT                               1024
#define MESSAGE_SIZE                            1024*3

#define SERVER_PORT 3000

ws2811_t ledstring =
{
    .freq = TARGET_FREQ,
    .dmanum = DMA,
    .channel =
    {
        [0] =
        {
            .gpionum = GPIO_PIN,
            .count = LED_COUNT,
            .invert = 0,
            .brightness = 255,
        },
        [1] =
        {
            .gpionum = 0,
            .count = 0,
            .invert = 0,
            .brightness = 255,
        },
    },
};


static void ctrl_c_handler(int signum)
{
    ws2811_fini(&ledstring);
}

static void setup_handlers(void)
{
    struct sigaction sa =
    {
        .sa_handler = ctrl_c_handler,
    };

    sigaction(SIGKILL, &sa, NULL);
}

int main(int argc, char *argv[])
{
    int ret = 0;
    setup_handlers();

    // --- WS281x SETUP ---

    if (ws2811_init(&ledstring))
    {
        printf("ws2811_init failed!\n");
	return -1;
    }

    // --- UDP SETUP --- 
    char message[MESSAGE_SIZE];
    int sock;
    struct sockaddr_in name;
    //struct hostent *hp, *gethostbyname();
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

    int i;

    while (1)
    {
        while ((bytes = read(sock, message, MESSAGE_SIZE)) > 0) {
        // message[bytes] = '\0';
        // printf("recv: %s\n", message);
        // printf("%d  %d  %d\n",message[i*3+0], message[i*3+1], message[i*3+2]);

        for (i=0; i < bytes/3; i++) {
            ledstring.channel[0].leds[i] = (message[i*3+0] << 16) | (message[i*3+1] << 8) | message[i*3+2];
            // &buf.pixels[i], message[i*3+0], message[i*3+1], message[i*3+2]);
        }
        if (ws2811_render(&ledstring))
        {
            ret = -1;
            break;
        }

        // 15 frames /sec
        // usleep(1000000 / 15);
        usleep(1000000 / 60);

	}
    }

    ws2811_fini(&ledstring);

    return ret;
}




