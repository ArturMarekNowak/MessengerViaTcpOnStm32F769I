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
#include "server-socket.h"
#include "cmsis_os.h"
#include "lcd_log.h"
//#include "uart_api.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SERVER_SOCKET_THREAD_PRIO (osPriorityHigh)
#define CLIENT_SOCKET_THREAD_PRIO (osPriorityHigh)

#define RX_BUFFER_SIZE 128
#define MSG_BUFFER_SIZE 128

static uint8_t msg_buf[MSG_BUFFER_SIZE + 2];
static int32_t msg_buf_cnt;

static int8_t client_recv_buf[MSG_BUFFER_SIZE + 2];
static int32_t client_recv_buf_cnt;
static volatile bool client_running;
static volatile bool msg_received;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void client_socket_write_thread(void *arg);
static void client_socket_read_thread(void *arg);

void socket_serve(int conn)
{
  while (1)
  {
    int n_rec;
    while (msg_received == true)
    {
    }

    /* Read in the request */
    n_rec = read(conn, msg_buf, MSG_BUFFER_SIZE);
    if (n_rec < 0)
      return;

    msg_buf[n_rec] = '\0';
    LCD_UsrLog("Otrzymano wiadomosc: %s\n", &msg_buf[0]);
    msg_buf[n_rec] = '\r';
    msg_buf[n_rec + 1] = '\n';
    msg_buf_cnt = n_rec + 2;

    // msg_received = true;
    write(conn, &msg_buf[0], n_rec + 2);

    /* Close connection socket */
    // close(conn);
  }
}

static void server_socket_thread(void *arg)
{
  int sock, newconn, size;
  struct sockaddr_in address, remotehost;

  msg_received = false;
  /* create a TCP socket */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    return;
  }

  /* bind to port 80 at any interface */
  address.sin_family = AF_INET;
  address.sin_port = htons(80);
  address.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock, (struct sockaddr *)&address, sizeof(address)) < 0)
  {
    return;
  }

  /* listen for incoming connections (TCP listen backlog = 5) */
  listen(sock, 5);

  size = sizeof(remotehost);

  while (1)
  {
    newconn = accept(sock, (struct sockaddr *)&remotehost, (socklen_t *)&size);
    socket_serve(newconn);
  }
}
static void client_socket_read_thread(void *arg)
{
  int sock = *((int *)arg);
  client_recv_buf_cnt = 0;
  while (client_running)
  {
    while (client_recv_buf_cnt > 0)
    {
    }
    client_recv_buf_cnt = read(sock, &client_recv_buf[0], MSG_BUFFER_SIZE);
  }

  osThreadTerminate(NULL);
}

static void client_socket_write_thread(void *arg)
{
  static int8_t uart_buf[RX_BUFFER_SIZE + 1];
  int32_t recv_cnt = 0;
  const int port = 1007;
  int sock;
  struct sockaddr_in chat_server_addr;
  client_running = true;

  /* create a TCP socket */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    LCD_UsrLog("client: Error creating socket\n");
    client_running = false;
    osThreadTerminate(NULL);
  }

  // bzero(&chat_server_addr, sizeof(chat_server_addr));
  memset(&chat_server_addr, 0, sizeof(chat_server_addr));

  chat_server_addr.sin_family = AF_INET;
  inet_pton(AF_INET, "192.168.10.5", &chat_server_addr.sin_addr);
  chat_server_addr.sin_port = htons(port);
  while (connect(sock, (struct sockaddr *)&chat_server_addr, sizeof(chat_server_addr)) < 0)
  {
    LCD_UsrLog("client: Error connecting\n");
    client_running = false;
    osThreadTerminate(NULL);
  }
  sys_thread_new("client_read", client_socket_read_thread, &sock, DEFAULT_THREAD_STACKSIZE * 2, CLIENT_SOCKET_THREAD_PRIO);
  while (1)
  {
    if (msg_received)
    {
      if (0 == strncmp(&msg_buf[0], "Close client", 12))
      {
        LCD_UsrLog("Closing client\n");
        close(sock);
        break;
      }
      LCD_UsrLog("Sending %ld chars as gateway\n", msg_buf_cnt);
      write(sock, &msg_buf[0], msg_buf_cnt);
      msg_received = false;
    }
    /*
    if ((recv_cnt = UartReadCommand(&uart_buf[0], RX_BUFFER_SIZE)) > 0)
    {
      uart_buf[recv_cnt] = '\0';
      LCD_UsrLog("UART: %s\r\n", uart_buf);
      uart_buf[recv_cnt] = '\r';
      uart_buf[recv_cnt + 1] = '\n';
      write(sock, &uart_buf[0], recv_cnt + 2);
    }
    */
    if (client_recv_buf_cnt > 0)
    {
      client_recv_buf[client_recv_buf_cnt] = '\0';
      LCD_UsrLog("Client read: %s\r\n", client_recv_buf);
      client_recv_buf[client_recv_buf_cnt] = '\r';
      client_recv_buf[client_recv_buf_cnt] = '\n';
      //  UartWriteCommand(&client_recv_buf[0], client_recv_buf_cnt + 2);
      client_recv_buf_cnt = 0;
    }
  }
  client_running = false;
  osThreadTerminate(NULL);
}

void server_socket_init()
{
  sys_thread_new("server", server_socket_thread, NULL, DEFAULT_THREAD_STACKSIZE * 2, SERVER_SOCKET_THREAD_PRIO);
  sys_thread_new("client_write", client_socket_write_thread, NULL, DEFAULT_THREAD_STACKSIZE * 2, CLIENT_SOCKET_THREAD_PRIO);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
