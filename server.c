#include <semaphore.h>

#include "common.h"
#include "queue.h"
#define MaxIt 10

struct Queue* threadQa;
struct Queue* threadQe;
struct Queue* threadQi;
struct Queue* threadQo;
struct Queue* threadQu;
struct Queue* threadQd;
struct Queue* threadQw;
int sum = 0;
int pipefds[2];

pthread_t thread_charA, thread_charE, thread_charI, thread_charO, thread_charU,
    thread_digit, thread_writer;
pthread_mutex_t mutex_e, mutex_i, mutex_o, mutex_u, mutex_d, mutex_w;
sem_t empty, empty_e, empty_i, empty_o, empty_u, empty_d, empty_w;
sem_t full, full_e, full_i, full_o, full_u, full_d, full_w;
sem_t cli_sock, cli_sock_free;

void* thread_handler(void* arg) {
    pthread_t id = pthread_self();
    printf("Pthread %lu\n", id);
    if (pthread_equal(id, thread_charA)) {
        char* msg;
        msg = deQueue(threadQa);
        sem_wait(&empty);
        pthread_mutex_lock(&mutex_e);
        if (msg == NULL) {
            sem_post(&full);
            return NULL;
        }
        char* modified_msg = replace(msg, "a", "A");
        printf("%s --> %s", msg, modified_msg);
        enQueue(threadQe, modified_msg);
        pthread_mutex_unlock(&mutex_e);
        sem_post(&full);
    }
    if (pthread_equal(id, thread_charE)) {
        char* msg;
        sem_wait(&full);
        sem_wait(&empty_e);
        pthread_mutex_lock(&mutex_e);
        pthread_mutex_lock(&mutex_i);
        msg = deQueue(threadQe);
        if (msg == NULL) {
            pthread_mutex_unlock(&mutex_e);
            pthread_mutex_unlock(&mutex_i);
            sem_post(&empty);
            sem_post(&full_e);
            return NULL;
        }
        char* modified_msg = replace(msg, "e", "E");
        printf("%s --> %s", msg, modified_msg);
        enQueue(threadQi, modified_msg);
        pthread_mutex_unlock(&mutex_e);
        pthread_mutex_unlock(&mutex_i);
        sem_post(&empty);
        sem_post(&full_e);
    }
    if (pthread_equal(id, thread_charI)) {
        char* msg;
        sem_wait(&full_e);
        sem_wait(&empty_i);
        pthread_mutex_lock(&mutex_i);
        pthread_mutex_lock(&mutex_o);
        msg = deQueue(threadQi);
        if (msg == NULL) {
            pthread_mutex_unlock(&mutex_o);
            pthread_mutex_unlock(&mutex_i);
            sem_post(&empty_e);
            sem_post(&full_i);
            return NULL;
        }
        char* modified_msg = replace(msg, "i", "I");
        printf("%s --> %s", msg, modified_msg);
        enQueue(threadQo, modified_msg);
        pthread_mutex_unlock(&mutex_o);
        pthread_mutex_unlock(&mutex_i);
        sem_post(&empty_e);
        sem_post(&full_i);
    }
    if (pthread_equal(id, thread_charO)) {
        char* msg;
        sem_wait(&full_i);
        sem_wait(&empty_o);
        pthread_mutex_lock(&mutex_u);
        pthread_mutex_lock(&mutex_o);
        msg = deQueue(threadQo);
        if (msg == NULL) {
            pthread_mutex_unlock(&mutex_o);
            pthread_mutex_unlock(&mutex_u);
            sem_post(&empty_i);
            sem_post(&full_o);
            return NULL;
        }
        char* modified_msg = replace(msg, "o", "O");
        printf("%s --> %s", msg, modified_msg);
        enQueue(threadQu, modified_msg);
        pthread_mutex_unlock(&mutex_o);
        pthread_mutex_unlock(&mutex_u);
        sem_post(&empty_i);
        sem_post(&full_o);
    }
    if (pthread_equal(id, thread_charU)) {
        char* msg;
        sem_wait(&full_o);
        sem_wait(&empty_u);
        pthread_mutex_lock(&mutex_u);
        pthread_mutex_lock(&mutex_d);
        msg = deQueue(threadQu);
        if (msg == NULL) {
            pthread_mutex_unlock(&mutex_u);
            pthread_mutex_unlock(&mutex_d);
            sem_post(&empty_o);
            sem_post(&full_u);
            return NULL;
        }
        char* modified_msg = replace(msg, "u", "U");
        printf("%s --> %s", msg, modified_msg);
        enQueue(threadQd, modified_msg);
        pthread_mutex_unlock(&mutex_u);
        pthread_mutex_unlock(&mutex_d);
        sem_post(&empty_o);
        sem_post(&full_u);
    }
    if (pthread_equal(id, thread_digit)) {
        char* msg;
        sem_wait(&full_u);
        sem_wait(&empty_d);
        pthread_mutex_lock(&mutex_w);
        pthread_mutex_lock(&mutex_d);
        msg = deQueue(threadQd);
        if (msg == NULL) {
            pthread_mutex_unlock(&mutex_d);
            pthread_mutex_unlock(&mutex_w);
            sem_post(&empty_u);
            sem_post(&full_d);
            printf("Sum of all digits in request: %d\n", sum);
            return NULL;
        }
        for (int i = 0; i <= strlen(msg); i++) {
            if (isdigit(msg[i])) {
                sum += msg[i] - '0';
            }
        }
        enQueue(threadQw, msg);
        pthread_mutex_unlock(&mutex_d);
        pthread_mutex_unlock(&mutex_w);
        sem_post(&empty_u);
        sem_post(&full_d);
        printf("Sum of all digits in request: %d\n", sum);
    }
    if (pthread_equal(id, thread_writer)) {
        char* msg;
        sem_wait(&full_d);
        sem_wait(&empty_w);
        pthread_mutex_lock(&mutex_w);
        msg = deQueue(threadQw);
        if (msg == NULL) {
            pthread_mutex_unlock(&mutex_w);
            sem_post(&empty_d);
            sem_post(&full_w);
            return NULL;
        }
        printf("%s", msg);
        write(pipefds[1], msg, strlen(msg));
        pthread_mutex_unlock(&mutex_w);
        sem_post(&empty_d);
        sem_post(&full_w);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) DieWithUserMessage("Parameter(s)", "<Server Port/Service>");

    int servSock = SetupTCPServerSocket(argv[1]);
    uint8_t* inBuf = (uint8_t*)calloc(4, sizeof(uint8_t));
    if (pipe(pipefds) == -1) {
        DieWithSystemMessage("pipe() failed");
    }

    // process semaphore
    sem_init(&cli_sock, 1, 1);

    sem_init(&full, 0, 1);
    sem_init(&empty, 0, 1);
    sem_init(&full_e, 0, 1);
    sem_init(&empty_e, 0, 1);
    sem_init(&full_i, 0, 1);
    sem_init(&empty_i, 0, 1);
    sem_init(&full_o, 0, 1);
    sem_init(&empty_o, 0, 1);
    sem_init(&full_u, 0, 1);
    sem_init(&empty_u, 0, 1);
    sem_init(&full_d, 0, 1);
    sem_init(&empty_d, 0, 1);
    sem_init(&full_w, 1, 1);
    sem_init(&empty_w, 1, 1);

    for (;;) {
        int clntSock = AcceptTCPConnection(servSock);
        FILE* channel = fdopen(clntSock, "r+");
        if (channel == NULL) {
            DieWithSystemMessage("fdopen failed");
        }
        int QS = 0;
        pid_t serverDecoder;
        threadQa = createQueue();
        /* enQueue(threadQa, "tralah"); */
        /* printf("deQueue: %s\n", deQueue(threadQa)); */
        threadQe = createQueue();
        threadQi = createQueue();
        threadQo = createQueue();
        threadQu = createQueue();
        threadQd = createQueue();
        threadQw = createQueue();

        if ((serverDecoder = fork()) < 0) {
            perror("serverDecoder fork failed");
        } else if (serverDecoder > 0) {
            // acquire cli_sock
            ssize_t numBytesRcvd;
            // Send received string and receive again until end of stream
            while ((numBytesRcvd = GetNextMsg(channel, inBuf, 8)) > 0) {
                printf("Recv: %s \n", inBuf);
                char* msg = (char*)DecodeFrame((inBuf));
                printf("Received message: %s  (%d bytes)\n", msg,
                       (int)numBytesRcvd);
                /* enQueue(threadQa, (char*)DecodeFrame(inBuf)); */
                enQueue(threadQa, msg);
                // see if there is more data to recv
                if (numBytesRcvd == 0) {
                    break;
                }
            }
            QS = threadQa->size;
            printf("Queue size: %d\n", QS);
            if (pthread_mutex_init(&mutex_e, NULL) != 0) {
                DieWithSystemMessage("Mutex e Init Failed\n");
            }
            if (pthread_mutex_init(&mutex_i, NULL) != 0) {
                DieWithSystemMessage("Mutex i Init Failed\n");
            }
            if (pthread_mutex_init(&mutex_o, NULL) != 0) {
                DieWithSystemMessage("Mutex o Init Failed\n");
            }
            if (pthread_mutex_init(&mutex_u, NULL) != 0) {
                DieWithSystemMessage("Mutex u Init Failed\n");
            }
            if (pthread_mutex_init(&mutex_d, NULL) != 0) {
                DieWithSystemMessage("Mutex d Init Failed\n");
            }
            if (pthread_mutex_init(&mutex_w, NULL) != 0) {
                DieWithSystemMessage("Mutex w Init Failed\n");
            }

            pthread_create(&thread_charA, NULL, &thread_handler, NULL);
            pthread_create(&thread_charE, NULL, &thread_handler, NULL);
            pthread_create(&thread_charI, NULL, &thread_handler, NULL);
            pthread_create(&thread_charO, NULL, &thread_handler, NULL);
            pthread_create(&thread_charU, NULL, &thread_handler, NULL);
            pthread_create(&thread_digit, NULL, &thread_handler, NULL);
            pthread_create(&thread_writer, NULL, &thread_handler, NULL);

            pthread_join(thread_charA, NULL);
            pthread_join(thread_charE, NULL);
            pthread_join(thread_charI, NULL);
            pthread_join(thread_charO, NULL);
            pthread_join(thread_charU, NULL);
            pthread_join(thread_digit, NULL);
            pthread_join(thread_writer, NULL);

            pthread_mutex_destroy(&mutex_e);
            pthread_mutex_destroy(&mutex_i);
            pthread_mutex_destroy(&mutex_o);
            pthread_mutex_destroy(&mutex_u);
            pthread_mutex_destroy(&mutex_d);
            pthread_mutex_destroy(&mutex_w);

            sem_destroy(&full);
            sem_destroy(&empty);
            sem_destroy(&full_e);
            sem_destroy(&empty_e);
            sem_destroy(&full_i);
            sem_destroy(&empty_i);
            sem_destroy(&full_o);
            sem_destroy(&empty_o);
            sem_destroy(&full_u);
            sem_destroy(&empty_u);
            sem_destroy(&full_d);
            sem_destroy(&empty_d);
            sem_destroy(&full_w);
            sem_destroy(&empty_w);
            sem_destroy(&cli_sock);
            sem_destroy(&cli_sock_free);
            waitpid(serverDecoder, NULL, 0);
            close(clntSock);
        } else {  // Decoder Process
            char outBuf[BUFFER_SIZE];
            uint32_t* p;
            MessageInfo* m_info = message_info_create();
            MessageFrame* m_frame = message_frame_create();
            int numRead = 0;
            /* sem_wait(&full_w); */
            while ((numRead = read(pipefds[0], outBuf, 4)) > 0) {
                p = (uint32_t*)outBuf;
                printf("outBuf: %s   %d\n", outBuf, ntohl(*p));
                strcpy(m_info->str, outBuf);
                Frame(m_info, m_frame);
                /* mSize = Encode(cliMsg, outBuf, BUFFER_SIZE); */
                uint8_t* e_f = EncodeFrame(m_frame);
                if ((PutMsg(e_f, 8, channel)) < 0) {
                    fputs("Error framing/outputting message\n", stderr);
                    break;
                } else {
                    printf("Processed %s.\n", "");
                }

                sem_post(&empty_w);
            }
            exit(0);
        }
    }  // Per Each Client
    close(servSock);
    return 0;
}
