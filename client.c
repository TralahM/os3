#include <semaphore.h>

#include "common.h"
/* sem_t empty, full; */
/* pthread_mutex_t m; */

int main(int argc, char* argv[]) {
    if (argc < 4 || argc > 5)
        DieWithUserMessage("Parameter(s):", "<Server/IP> <Port> <InputFile>");
    char* server = argv[1];
    char* service = argv[2];
    char* input_file = argv[3];
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    /* pthread_mutex_init(&m); */
    int n;

    int sock = SetupTCPClientSocket(server, service);
    if (sock < 0)
        DieWithUserMessage("SetupTCPClientSocket() failed",
                           "unable to connect");
    FILE* fp;
    pid_t clientEncoder;
    if ((clientEncoder = fork()) < 0) {
        perror("Client Encoder create failed");
    } else if (clientEncoder > 0) {  // ClientDecoder Process
        size_t respSize;
        uint8_t* returnBuffer = (uint8_t*)calloc(4, sizeof(uint8_t));
        char* output_file = "result.txt";
        FILE* of = fopen(output_file, "a");
        while ((respSize = recv(sock, &returnBuffer, 4, 0)) > 0) {
            /* sem_wait(&full); */
            char* reply = (char*)DecodeFrame(returnBuffer);
            fputs(reply, of);
            printf("\033[34m--->: %s\033[0m\n", reply);
            /* sem_post(&empty); */
        }
        fclose(of);
        waitpid(clientEncoder, NULL, 0);
        close(sock);
    } else {  // Encoder Process
        /* printf("clientEncoder process."); */
        // read input file
        fp = fopen(input_file, "r");
        while (!feof(fp)) {
            MessageInfo* m_info = message_info_create();
            MessageFrame* m_frame = message_frame_create();
            fgets(buffer, 4, fp);
            strcpy(m_info->str, buffer);
            Frame(m_info, m_frame);
            printf("%s", EncodeFrame(m_frame));
            n = send(sock, EncodeFrame(m_frame), 4, 0);
            if (n < 1) {
                DieWithSystemMessage("SendMsg Failed\n");
            }
        }
        if (feof(fp)) {
            fclose(fp);
            exit(0);
        }
    }
}
