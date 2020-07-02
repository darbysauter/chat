#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <semaphore.h>
#include <pthread.h>

#include "list.h"

#define MAX_MSG_LEN 100

pthread_t kiThread, rdThread, sdThread, pmThread;

char kbbuf[MAX_MSG_LEN];
char printbuf[MAX_MSG_LEN*2];
char recvbuf[MAX_MSG_LEN];

LIST *send_queue, *print_queue;

char* hostname;
char* self_port;
char* target_port;

sem_t* wait_for_msg_to_send;
sem_t* wait_for_msg_to_print;
sem_t* send_queue_mutex;
sem_t* print_queue_mutex;

/* P -> sem_wait V -> sem_post */

void* receiveData(void* args) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    char* stringStorage;
    int stringSize;

    char h_name[MAX_MSG_LEN];
    int h_name_size;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; /* set to AF_INET to force IPv4 */
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; /* use my IP */

    if ((rv = getaddrinfo(NULL, self_port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return NULL;
    }

    /* loop through all the results and bind to the first we can */
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return NULL;
    }

    freeaddrinfo(servinfo);

    addr_len = sizeof their_addr;

    while(1) { /* receive loop */
        if ((numbytes = recvfrom(sockfd, recvbuf, MAX_MSG_LEN-1 , 0,
            (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }

        if (getnameinfo((struct sockaddr *)&their_addr, addr_len,
                    h_name, MAX_MSG_LEN-1, NULL, 0, 0) != 0){
            perror("getnameinfo");
            exit(1);
        }

        if (strstr(h_name, hostname) == 0)
            continue; /* not from target */

        h_name_size = strlen(h_name);
        stringSize = h_name_size + numbytes + 2;
        stringStorage = malloc(stringSize);
        if (!stringStorage) {
           perror("malloc()");
           continue;
        }
        memcpy(stringStorage, h_name, h_name_size);
        stringStorage[h_name_size] = ':';
        stringStorage[h_name_size+1] = ' ';
        memcpy(&stringStorage[h_name_size+2], recvbuf, numbytes);
        stringStorage[stringSize] = 0;

        sem_wait(print_queue_mutex);
        if (ListAppend(print_queue, stringStorage) == -1) {
            printf("Appending to list failed\n");
            sem_post(print_queue_mutex);
            continue;
        }
        sem_post(print_queue_mutex);
        sem_post(wait_for_msg_to_print);
    }

    close(sockfd);

    return NULL;
}

void* kbInput(void* args) {

    int fd, flags, print_string_size;
    ssize_t bytes_read;
    char* printString;
    struct timeval tv;

    fd = open("/dev/stdin", O_NONBLOCK);

    while (1) {
        usleep(50);
        bytes_read = read(fd, kbbuf, MAX_MSG_LEN);
        if (bytes_read < 1)
            continue;

        if (bytes_read == MAX_MSG_LEN)
            bytes_read--;
        kbbuf[bytes_read-1] = 0;

        if (strstr(kbbuf, "!leave"))
            break;

        gettimeofday(&tv,NULL);

        print_string_size = sprintf(printbuf,\
                "%s            > at time: %ld sec %d microsec",
                kbbuf, tv.tv_sec, tv.tv_usec);

        printString = malloc(print_string_size+1);
        if (!printString) {
            perror("malloc()");
            continue;
        }
        strcpy(printString, printbuf);

        sem_wait(send_queue_mutex);
        if (ListAppend(send_queue, printString) == -1) {
            printf("Appending to list failed\n");
            sem_post(send_queue_mutex);
            continue;
        }
        sem_post(send_queue_mutex);
        sem_post(wait_for_msg_to_send);
    }
    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        exit(1);
    flags &= ~O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);

    return NULL;
}

void* sendData(void* args) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    char* msg_to_send;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(hostname, target_port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return NULL;
    }

    /* loop through all the results and make a socket */
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return NULL;
    }

    while(1) { /* send loop */
        /* wait for message to send */
        sem_wait(wait_for_msg_to_send);
        sem_wait(send_queue_mutex);

        msg_to_send = ListFirst(send_queue);
        ListRemove(send_queue);
        sem_post(send_queue_mutex);

        if ((numbytes = sendto(sockfd, msg_to_send, strlen(msg_to_send), 0,
                p->ai_addr, p->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        }

        free(msg_to_send);
    }

    freeaddrinfo(servinfo);
    close(sockfd);

    return NULL;
}

void* printMessages(void* args) {
    char* msg_to_print;

    while(1) {
        sem_wait(wait_for_msg_to_print);
        sem_wait(print_queue_mutex);

        msg_to_print = ListFirst(print_queue);
        ListRemove(print_queue);
        sem_post(print_queue_mutex);

        printf("%s\n", msg_to_print);

        free(msg_to_print);
    }

    return NULL;
}


/*
 * Begins application by starting server thread
 */
int main(int argc, char* argv[]) {
    printf("chat started\n");

    if (argc != 4) {
        fprintf(stderr,"usage: %s self_port hostname target_port\n", argv[0]);
        exit(1);
    }

    send_queue = ListCreate();
    print_queue = ListCreate();

    wait_for_msg_to_send = sem_open("sem", O_CREAT | O_EXCL, 0644, 0);
    if (wait_for_msg_to_send == SEM_FAILED) {
        fprintf(stderr,"Failed to create: wait_for_msg_to_send");
        exit(1);
    }
    if (sem_unlink("sem")) {
        fprintf(stderr,"Failed to unlink: sem");
        exit(1);
    }

    wait_for_msg_to_print = sem_open("sem", O_CREAT | O_EXCL, 0644, 0);
    if (wait_for_msg_to_print == SEM_FAILED) {
        fprintf(stderr,"Failed to create: wait_for_msg_to_print");
        exit(1);
    }
    if (sem_unlink("sem")) {
        fprintf(stderr,"Failed to unlink: sem");
        exit(1);
    }

    send_queue_mutex = sem_open("sem", O_CREAT | O_EXCL, 0644, 1);
    if (send_queue_mutex == SEM_FAILED) {
        fprintf(stderr,"Failed to create: send_queue_mutex");
        exit(1);
    }
    if (sem_unlink("sem")) {
        fprintf(stderr,"Failed to unlink: sem");
        exit(1);
    }

    print_queue_mutex = sem_open("sem", O_CREAT | O_EXCL, 0644, 1);
    if (print_queue_mutex == SEM_FAILED) {
        fprintf(stderr,"Failed to create: print_queue_mutex");
        exit(1);
    }
    if (sem_unlink("sem")) {
        fprintf(stderr,"Failed to unlink: sem");
        exit(1);
    }

    self_port = malloc(strlen(argv[1]) + 1);
    if (!self_port) {
        fprintf(stderr,"Failed to malloc: self_port");
        exit(1);
    }
    strcpy(self_port, argv[1]);

    hostname = malloc(strlen(argv[2]) + 1);
    if (!hostname) {
        fprintf(stderr,"Failed to malloc: hostname");
        exit(1);
    }
    strcpy(hostname, argv[2]);

    target_port = malloc(strlen(argv[3]) + 1);
    if (!target_port) {
        fprintf(stderr,"Failed to malloc: target_port");
        exit(1);
    }
    strcpy(target_port, argv[3]);


    if (pthread_create(&rdThread, NULL, receiveData, NULL)) {
        fprintf(stderr,"Failed to create thread: rdThread");
        exit(1);
    }
    if (pthread_create(&kiThread, NULL, kbInput, NULL)) {
        fprintf(stderr,"Failed to create thread: kiThreadID");
        exit(1);
    }
    if (pthread_create(&sdThread, NULL, sendData, NULL)) {
        fprintf(stderr,"Failed to create thread: sdThreadID");
        exit(1);
    }
    if (pthread_create(&pmThread, NULL, printMessages, NULL)) {
        fprintf(stderr,"Failed to create thread: pmThreadID");
        exit(1);
    }

    pthread_join(rdThread, NULL);
    pthread_join(kiThread, NULL);
    pthread_join(sdThread, NULL);
    pthread_join(pmThread, NULL);

    return 0;
}
