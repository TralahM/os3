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
    FILE* channel = fdopen(sock, "r+");
    if (channel == NULL) {
        DieWithSystemMessage("fdopen failed");
    }
    FILE* fp;
    pid_t clientEncoder;
    if ((clientEncoder = fork()) < 0) {
        perror("Client Encoder create failed");
    } else if (clientEncoder > 0) {  // ClientDecoder Process
        /* sem_wait(&full); */
        waitpid(clientEncoder, NULL, 0);
        size_t respSize;
        uint8_t* returnBuffer = (uint8_t*)calloc(4, sizeof(uint8_t));
        char* output_file = "result.txt";
        FILE* of = fopen(output_file, "w");
        fclose(of);
        of = fopen(output_file, "a");
        while ((respSize = GetNextMsg(channel, returnBuffer, 8)) > 0) {
            char* reply = (char*)DecodeFrame(returnBuffer);
            fputs(reply, of);
            /* printf("\033[34m--->: %s\033[0m  len:%d\n", reply,
             * strlen(reply)); */
            if (strlen(reply) < 3) {
                break;
            }
        }
        fclose(of);
        /* printf("fclose(of);\n"); */
        fclose(channel);
        /* printf("fclose(channel);\n"); */
        close(sock);
        printf("close(sock);\n");
        printf("Results written to result.txt\n");
        exit(0);
    } else {  // Encoder Process
        /* printf("clientEncoder process."); */
        // read input file
        fp = fopen(input_file, "r");
        while (!feof(fp)) {
            MessageInfo* m_info = message_info_create();
            MessageFrame* m_frame = message_frame_create();
            fgets(buffer, 4, fp);
            /* printf("\nBuffer: %s\n", buffer); */
            strcpy(m_info->str, buffer);
            Frame(m_info, m_frame);
            uint8_t* e_f = (uint8_t*)(m_frame);
            char* sbuf = calloc(8, sizeof(char));
            /* char* nbuf = calloc(8, sizeof(char)); */
            /* char* tbuf = calloc(8, sizeof(char)); */
            /* nbuf = htonl(sbuf); */
            /* tbuf = ntohl(nbuf); */
            sprintf(sbuf, "%s", e_f);
            /* printf("Send3: %s  len: %d  \n", sbuf, strlen(sbuf)); */
            /* printf("Send: %s  len: %d\n", e_f, strlen(e_f)); */
            /* n = send(sock, &nbuf, 8, 0); */
            n = PutMsg((uint8_t*)sbuf, 8, channel);
            if (n < 1) {
                DieWithSystemMessage("SendMsg Failed\n");
            }
        }
        PutMsg(NULL, 0, channel);
        fclose(fp);
        /* printf("fclose(fp);\n"); */
        exit(0);
    }
}
