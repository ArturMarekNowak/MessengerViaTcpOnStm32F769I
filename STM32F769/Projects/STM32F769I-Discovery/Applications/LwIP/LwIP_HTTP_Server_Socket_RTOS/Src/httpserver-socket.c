/**
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
 * All rights reserved.</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/apps/fs.h"
#include "lwip/netdb.h"
#include "string.h"
#include "httpserver-socket.h"
#include "cmsis_os.h"
#include "lcd_log.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define FIRST_CLIENT_SOCKET_THREAD_PRIO (osPriorityHigh)
#define SECOND_CLIENT_SOCKET_THREAD_PRIO (osPriorityHigh)

#define MSG_BUFFER_SIZE 128

static uint8_t msg_buf[MSG_BUFFER_SIZE + 2], client_recv_buf[MSG_BUFFER_SIZE + 2];
static int32_t msg_buf_cnt, client_recv_buf_cnt;
static volatile bool client_running, msg_received;
int Client1, Client2, ConnFirstSocket, ConnSecondSocket, Recived, size;

// FIRST SOCKET
// ============
void FirstSocket(int conn)
{
  while (1)
  {
    Recived = read(conn, msg_buf, MSG_BUFFER_SIZE);
    if (Recived < 0)
      return;

    msg_buf[Recived] = '\0';
    LCD_UsrLog("Klient A: %s\n", &msg_buf[0]); 
    msg_buf[Recived] = '\r';
    msg_buf_cnt = Recived + 2;

    write(ConnSecondSocket, &msg_buf[0], Recived + 2);
  }
}

// SECOND SOCKET
// =============
void SecondSocket(int conn)
{
  while (1)
  {
    Recived = read(conn, msg_buf, MSG_BUFFER_SIZE);
    if (Recived < 0)
      return;

    msg_buf[Recived] = '\0';
    LCD_UsrLog("Klient B: %s\n", &msg_buf[0]); 
    msg_buf[Recived] = '\r';
    msg_buf_cnt = Recived + 2;

    write(ConnFirstSocket, &msg_buf[0], Recived + 2);
  }
}

// First Socket Thread --> Port 12345
// ===============================
static void FirstClientThread(void *arg)
{
  struct sockaddr_in address, remotehost;

  if ((Client1 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    return;

  address.sin_family = AF_INET;
  address.sin_port = htons(12345);
  address.sin_addr.s_addr = INADDR_ANY;

  if (bind(Client1, (struct sockaddr *)&address, sizeof(address)) < 0)
    return;

  listen(Client1, 1);
  size = sizeof(remotehost);

  ConnFirstSocket = accept(Client1, (struct sockaddr *)&remotehost, (socklen_t *)&size);
  FirstSocket(ConnFirstSocket);
}

// Second Socket Thread --> Port 12346
// ================================
static void SecondClientThread(void *arg)
{
  struct sockaddr_in address, remotehost;

  if ((Client2 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    return;

  address.sin_family = AF_INET;
  address.sin_port = htons(12346);
  address.sin_addr.s_addr = INADDR_ANY;

  if (bind(Client2, (struct sockaddr *)&address, sizeof(address)) < 0)
    return;

  listen(Client2, 1);
  size = sizeof(remotehost);

  ConnSecondSocket = accept(Client2, (struct sockaddr *)&remotehost, (socklen_t *)&size);
  SecondSocket(ConnSecondSocket);
}

// Threads initialization
// ======================
void server_socket_init()
{
  sys_thread_new("FirstClient", FirstClientThread, NULL, DEFAULT_THREAD_STACKSIZE * 2, FIRST_CLIENT_SOCKET_THREAD_PRIO);
  sys_thread_new("SecondClient", SecondClientThread, NULL, DEFAULT_THREAD_STACKSIZE * 2, SECOND_CLIENT_SOCKET_THREAD_PRIO);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
