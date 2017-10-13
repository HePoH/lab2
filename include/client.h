#ifndef CLIENT_CORE_H
#define CLIENT_CORE_H

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h>

#define IP_ADDR_SIZE 15

#define MAX_SN_SIZE 32
#define MAX_BUF_SIZE 64
#define MAX_FP_SIZE 256

void parse_argvs(int argc, char **argv, uint8_t *dst_ip, uint16_t *dst_port, 
                    uint8_t *file_name);
void print_usage();
void init_socket(uint32_t *sock_fd, struct sockaddr_in *dst_addr, uint8_t *dst_ip,
                    uint16_t *dst_port);
#endif
