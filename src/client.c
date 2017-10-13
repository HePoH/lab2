#include "../include/client.h"

int main(int argc, char **argv) {
	uint32_t sock_fd = 0, file_fd = 0, seq_num = 0, ack_num = 0;
    uint8_t dst_ip[IP_ADDR_SIZE], file_name[MAX_FP_SIZE], file_buf[MAX_BUF_SIZE], 
            seq_num_buf[MAX_SN_SIZE], ack_num_buf[MAX_SN_SIZE];
    uint16_t dst_port = 0;
	struct sockaddr_in dst_addr;
	struct hostent* srv_hst = NULL;
	socklen_t sock_len = 0;
	ssize_t send_bts = 0, recv_bts = 0, file_bts = 0;

	sock_len = sizeof(struct sockaddr_in);

    parse_argvs(argc, argv, dst_ip, &dst_port, file_name);

    if(access((const char *)file_name, F_OK) == -1) {
        printf("file not found\n");
        exit(EXIT_FAILURE);
    } 
    
    init_socket(&sock_fd, &dst_addr, dst_ip, &dst_port);

	srv_hst = gethostbyaddr((char *)&dst_addr.sin_addr.s_addr, 4, AF_INET);
    printf("udp file server on %s:%d (hsn: %s)\n",
            dst_ip, dst_port, ((srv_hst != NULL) ? srv_hst->h_name : ""));

    file_fd = open((const char *)file_name, O_RDONLY);
    if (file_fd == -1) {
        perror("open() failed");
        exit(EXIT_FAILURE);
    }

    /* syn sequence number  */
    snprintf(seq_num_buf, MAX_SN_SIZE, "%d", seq_num);
    send_bts = sendto(sock_fd, seq_num_buf, strlen((const char *)seq_num_buf), 0, 
                        (struct sockaddr*)&dst_addr, sock_len);
	if (send_bts == -1) {
		perror("sendto() failed");

		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	printf("send sync sequence number to server: '%s' (%d bytes)\n", 
            seq_num_buf, (int)send_bts);

	recv_bts = recvfrom(sock_fd, ack_num_buf, MAX_SN_SIZE, 0, NULL, NULL);
	if (recv_bts == -1) {
		perror("recvfrom() failed");

		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	printf("receive sync acknowledgment number from server: '%s' (%d bytes)\n\n", 
            ack_num_buf, (int)recv_bts);

    ack_num = strtoumax(ack_num_buf, NULL, 10);
    if (seq_num != ack_num) {
            perror("syn failed");
            exit(EXIT_FAILURE);
    }
    /* --- */

    while(1) {
        file_bts = read(file_fd, file_buf, MAX_BUF_SIZE);
        if (file_bts == -1) {
            perror("read() failed");
            exit(EXIT_FAILURE);
        }

        if (file_bts == 0) {
            printf("end\n");
            exit(EXIT_SUCCESS);
        }

        do {
		    send_bts = sendto(sock_fd, file_buf, file_bts, 0, 
                                (struct sockaddr*)&dst_addr, sock_len);
		    if (send_bts == -1) {
			    perror("sendto() failed");

			    close(sock_fd);
			    exit(EXIT_FAILURE);
		    }

		    printf("send file part to server: '%s' (%d bytes)\n", 
                    file_buf, (int)send_bts);

            recv_bts = recvfrom(sock_fd, ack_num_buf, MAX_SN_SIZE, 0, NULL, NULL);
	        if (recv_bts == -1) {
		        perror("recvfrom() failed");

		        close(sock_fd);
		        exit(EXIT_FAILURE);
	        }

        	printf("receive sync acknowledgment number from server: '%s' (%d bytes)\n\n", 
                ack_num_buf, (int)recv_bts);

            ack_num = strtoumax(ack_num_buf, NULL, 10);
        } while (seq_num != ack_num);
	}

    close(file_fd);
	exit(EXIT_SUCCESS);
}
