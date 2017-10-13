#include "../include/client.h"

void parse_argvs(int argc, char **argv, uint8_t *dst_ip, uint16_t *dst_port, uint8_t *file_name) {
    switch (argc) {
        case 1:
        case 2:
            print_usage();
            printf("too few arguments to initialize the client\n");

            exit(EXIT_FAILURE);

        case 5:
            if ((strcmp(argv[1], "-d") == 0) && (strcmp(argv[3], "-f") == 0))
                if ((sscanf(argv[2], "%[^:]:%hu", dst_ip, dst_port) == 2) &&
                    (sscanf(argv[4], "%s", file_name) == 2))
                    return;

            print_usage();
            printf("invalid arguments syntax to initialize the client\n");

            exit(EXIT_FAILURE);

        default:
            print_usage();
            printf("too few or invalid arguments syntax to initialize the client\n");

            exit(EXIT_FAILURE);
    }
}

void print_usage() {
    printf("usage syntax: ./client -d [server destination address]:[port] -f [filename]\n");
    printf(" --- destination address must be provided\n");
    printf(" --- file must be provided\n\n");
}

void init_socket(uint32_t *sock_fd, struct sockaddr_in *dst_addr, uint8_t *dst_ip,
                        uint16_t *dst_port) {
    socklen_t s_len = 0;

    s_len = sizeof(struct sockaddr_in);
    memset(dst_addr, 0, s_len);

    *sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (*sock_fd == -1) {
        printf("socket() failed\n");

        close(*sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("initialization socket\n");

    dst_addr->sin_family = AF_INET;
    dst_addr->sin_addr.s_addr = inet_addr((char*)dst_ip);
    dst_addr->sin_port = htons(*dst_port);

    printf("initialization sockadrr_in\n");
}
