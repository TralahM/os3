#include <pthread.h>
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

pthread_cond_t comp_a = PTHREAD_COND_INITIALIZER;
pthread_cond_t comp_e = PTHREAD_COND_INITIALIZER;
pthread_cond_t comp_i = PTHREAD_COND_INITIALIZER;
pthread_cond_t comp_o = PTHREAD_COND_INITIALIZER;
pthread_cond_t comp_u = PTHREAD_COND_INITIALIZER;
pthread_cond_t comp_d = PTHREAD_COND_INITIALIZER;
pthread_cond_t comp_w = PTHREAD_COND_INITIALIZER;

pthread_t thread_charA, thread_charE, thread_charI, thread_charO, thread_charU,
    thread_digit, thread_writer;

pthread_mutex_t mutex_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_e = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_i = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_o = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_u = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_d = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_w = PTHREAD_MUTEX_INITIALIZER;

sem_t empty, empty_e, empty_i, empty_o, empty_u, empty_d, empty_w;
sem_t full, full_e, full_i, full_o, full_u, full_d, full_w;
sem_t cli_sock, cli_sock_free;

void* thread_handler(void* arg) {
    pthread_t id = pthread_self();

    if (pthread_equal(id, thread_charA)) {
        char* msg;
        /* printf("Pthread %lu  A\n", id); */
        pthread_mutex_lock(&mutex_a);
        /* printf("lock acquired\n"); */
        /* pthread_cond_wait(&comp_a, &mutex_a); */
        /* printf("comp_a, mutex_a\n"); */
        sem_wait(&empty);
        /* printf("sem_wait(empty);\n"); */
        while ((msg = deQueue(threadQa)) != NULL) {
            printf("Pthread %lu  A\n", id);
            char* modified_msg = replace(msg, "a", "A");
            /* printf("%s --> %s\n", msg, modified_msg); */
            enQueue(threadQe, modified_msg);
        }
        pthread_cond_signal(&comp_e);
        pthread_mutex_unlock(&mutex_a);
        sem_post(&full);
        pthread_exit(NULL);
    }

    if (pthread_equal(id, thread_charE)) {
        /* printf("Pthread %lu  E\n", id); */
        char* msg;
        pthread_mutex_lock(&mutex_e);
        pthread_cond_wait(&comp_e, &mutex_e);
        sem_wait(&full);
        while ((msg = deQueue(threadQe)) != NULL) {
            printf("Pthread %lu  E\n", id);
            char* modified_msg = replace(msg, "e", "E");
            /* printf("%s --> %s\n", msg, modified_msg); */
            enQueue(threadQi, modified_msg);
        }
        pthread_cond_signal(&comp_i);
        pthread_mutex_unlock(&mutex_e);
        /* sem_post(&empty); */
        sem_post(&full_e);
        pthread_exit(NULL);
    }

    if (pthread_equal(id, thread_charI)) {
        /* printf("Pthread %lu  I\n", id); */
        char* msg;
        pthread_mutex_lock(&mutex_i);
        pthread_cond_wait(&comp_i, &mutex_i);
        sem_wait(&full_e);

        while ((msg = deQueue(threadQi)) != NULL) {
            printf("Pthread %lu  I\n", id);
            char* modified_msg = replace(msg, "i", "I");
            /* printf("%s --> %s\n", msg, modified_msg); */
            enQueue(threadQo, modified_msg);
        }
        pthread_cond_signal(&comp_o);
        pthread_mutex_unlock(&mutex_i);
        sem_post(&full_i);
        pthread_exit(NULL);
    }

    if (pthread_equal(id, thread_charO)) {
        /* printf("Pthread %lu  O\n", id); */
        char* msg;
        pthread_mutex_lock(&mutex_o);
        pthread_cond_wait(&comp_o, &mutex_o);
        sem_wait(&full_i);
        while ((msg = deQueue(threadQo)) != NULL) {
            printf("Pthread %lu  O\n", id);
            char* modified_msg = replace(msg, "o", "O");
            /* printf("%s --> %s\n", msg, modified_msg); */
            enQueue(threadQu, modified_msg);
        }
        pthread_cond_signal(&comp_u);
        pthread_mutex_unlock(&mutex_o);
        sem_post(&full_o);
        pthread_exit(NULL);
    }

    if (pthread_equal(id, thread_charU)) {
        printf("Pthread %lu  U\n", id);
        char* msg;
        pthread_mutex_lock(&mutex_u);
        pthread_cond_wait(&comp_u, &mutex_u);
        sem_wait(&full_o);

        while ((msg = deQueue(threadQu)) != NULL) {
            printf("Pthread %lu  U\n", id);
            char* modified_msg = replace(msg, "u", "U");
            /* printf("%s --> %s\n", msg, modified_msg); */
            enQueue(threadQd, modified_msg);
        }
        pthread_cond_signal(&comp_d);
        pthread_mutex_unlock(&mutex_u);
        sem_post(&full_u);
        pthread_exit(NULL);
    }

    if (pthread_equal(id, thread_digit)) {
        /* printf("Pthread %lu  D\n", id); */
        char* msg;
        pthread_mutex_lock(&mutex_d);
        pthread_cond_wait(&comp_d, &mutex_d);
        sem_wait(&full_u);

        while ((msg = deQueue(threadQd)) != NULL) {
            printf("Pthread %lu  D\n", id);
            /* printf("%s --> %s\n", msg, msg); */
            int digits = 0;
            int position = -1;
            int space = -1;
            for (int i = 0; i <= strlen(msg); i++) {
                if (isdigit(msg[i])) {
                    if (position == -1) {
                        position = i;
                    }
                    digits += 1;
                    /* sum += msg[i] - '0'; */
                }
                if (msg[i] == ' ') {
                    space = i;
                }
            }
            if (space == 1) {
                sum += msg[0] - '0';
                if (digits == 2) {
                    sum += msg[2] - '0';
                }
            } else if (space == 0) {
                if (digits == 2) {
                    sum += atoi((msg + position));
                } else {
                    sum += msg[position] - '0';
                }
            } else {
                if (digits == 2) {
                    sum += msg[position] - '0';
                    sum += msg[1] - '0';
                } else if (digits == 3) {
                    sum += atoi(msg);
                } else {
                    sum += msg[position] - '0';
                }
            }
            enQueue(threadQw, msg);
        }
        printf("\033[32mSum of all digits in request: %d\033[0m\n\n", sum);
        pthread_cond_signal(&comp_w);
        pthread_mutex_unlock(&mutex_d);
        pthread_cond_signal(&comp_d);
        pthread_cond_signal(&comp_u);
        pthread_cond_signal(&comp_o);
        pthread_cond_signal(&comp_e);
        pthread_cond_signal(&comp_i);
        pthread_mutex_unlock(&mutex_u);
        pthread_mutex_unlock(&mutex_e);
        pthread_mutex_unlock(&mutex_i);
        pthread_mutex_unlock(&mutex_o);
        sem_post(&full_d);
        pthread_exit(NULL);
    }

    if (pthread_equal(id, thread_writer)) {
        /* printf("Pthread %lu  W\n", id); */
        char* msg;
        pthread_mutex_lock(&mutex_w);
        pthread_cond_wait(&comp_w, &mutex_w);
        sem_wait(&full_d);

        while ((msg = deQueue(threadQw)) != NULL) {
            printf("Pthread %lu  W\n", id);
            printf("%s", msg);
            write(pipefds[1], msg, 4);
        }
        printf("\n");
        write(pipefds[1], " ", 4);
        sem_post(&full_w);
        pthread_cond_signal(&comp_w);
        pthread_mutex_unlock(&mutex_w);
        pthread_exit(NULL);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) DieWithUserMessage("Parameter(s)", "<Server Port/Service>");
    int servSock = SetupTCPServerSocket(argv[1]);

    for (;;) {
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

        sum = 0;
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
            /* printf("Queue size: %d\n", QS); */
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

            if ((pthread_create(&thread_charA, NULL, &thread_handler, NULL)) !=
                0)
                DieWithSystemMessage("Failed to create thread A");
            if ((pthread_create(&thread_charE, NULL, &thread_handler, NULL)) !=
                0)
                DieWithSystemMessage("Failed to create thread E");
            if ((pthread_create(&thread_charI, NULL, &thread_handler, NULL)) !=
                0)
                DieWithSystemMessage("Failed to create thread I");
            if ((pthread_create(&thread_charO, NULL, &thread_handler, NULL)) !=
                0)
                DieWithSystemMessage("Failed to create thread O");
            /* sleep(1); */
            if ((pthread_create(&thread_charU, NULL, &thread_handler, NULL)) !=
                0)
                DieWithSystemMessage("Failed to create thread U");
            /* sleep(1); */
            if ((pthread_create(&thread_digit, NULL, &thread_handler, NULL)) !=
                0)
                DieWithSystemMessage("Failed to create thread D");
            if ((pthread_create(&thread_writer, NULL, &thread_handler, NULL)) !=
                0)
                DieWithSystemMessage("Failed to create thread W");

            pthread_join(thread_charA, NULL);
            pthread_cond_signal(&comp_a);
            pthread_join(thread_charE, NULL);
            pthread_join(thread_charI, NULL);
            pthread_join(thread_charO, NULL);
            pthread_join(thread_charU, NULL);
            pthread_join(thread_digit, NULL);
            pthread_join(thread_writer, NULL);

            pthread_mutex_destroy(&mutex_a);
            pthread_mutex_destroy(&mutex_e);
            pthread_mutex_destroy(&mutex_i);
            pthread_mutex_destroy(&mutex_o);
            pthread_mutex_destroy(&mutex_u);
            pthread_mutex_destroy(&mutex_d);
            pthread_mutex_destroy(&mutex_w);

            waitpid(serverDecoder, NULL, 0);
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
            close(clntSock);

        } else {  // Decoder Process
            char outBuf[BUFFER_SIZE];
            uint32_t* p;
            MessageInfo* m_info = message_info_create();
            MessageFrame* m_frame = message_frame_create();
            int numRead = 0;
            sem_wait(&full_w);

            while ((numRead = read(pipefds[0], outBuf, 4)) != 0) {
                p = (uint32_t*)outBuf;
                printf("outBuf: %s   %d  nread:%d  len:%d\n", outBuf, ntohl(*p),
                       numRead, strlen(outBuf));
                strcpy(m_info->str, outBuf);
                Frame(m_info, m_frame);
                /* mSize = Encode(cliMsg, outBuf, BUFFER_SIZE); */
                uint8_t* e_f = EncodeFrame(m_frame);
                if ((PutMsg(e_f, 8, channel)) < 0) {
                    fputs("Error framing/outputting message\n", stderr);
                    break;
                } else {
                    sem_post(&empty_w);
                }
                /* printf("length:%d\n", strlen(outBuf)); */
                if (strlen(outBuf) < 3) {
                    break;
                }
                sem_post(&empty_w);
                /* fflush(channel); */
            }
            printf("Processed %s.\n", "");
            fclose(channel);
            printf("closed channel");
            close(clntSock);
        }
    }  // Per Each Client
    close(servSock);
    return 0;
}
