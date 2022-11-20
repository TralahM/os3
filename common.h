#ifndef __PRACTICAL__H
#define __PRACTICAL__H
#include <arpa/inet.h>
#include <byteswap.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>/* for Socket data types */
#include <sys/wait.h>
#include <unistd.h>

#include "buffer.h"

typedef struct MessageInfo MessageInfo;

typedef struct __attribute__((__packed__)) MessageFrame {
    uint16_t SYN;
    uint8_t LENGTH;
    uint32_t DATA;
} MessageFrame;

typedef union MessageFrameConverter {
    MessageFrame frame;
    uint8_t bytes[sizeof(MessageFrame)];
} MessageFrameConverter;

struct MessageInfo {
    char* str;
};

void DieWithUserMessage(const char* msg, const char* detail);
void DieWithSystemMessage(const char* msg);

void HandleTCPClient(int clntSocket);
void PrintSocketAddress(const struct sockaddr* address, FILE* stream);
int SetupTCPClientSocket(const char* host, const char* service);
int SetupTCPServerSocket(const char* service);
int AcceptTCPConnection(int servSock);

int GetNextMsg(FILE* in, uint8_t* buf, size_t bufSize);
int PutMsg(uint8_t buf[], size_t msgSize, FILE* out);

char* replace(char const* const original, char const* const pattern,
              char const* const replacement);
uint8_t get_byte_from(uint32_t value, int start_bit);
unsigned int count_set_bits(uint8_t n);
uint8_t clear_bit(uint8_t n, int i);
uint8_t set_bit(uint8_t n, int i);
uint8_t char2bin(char c);
char bin2char(uint8_t b);
bool check_parity_bit(uint8_t bits);
uint8_t add_parity_bit(uint8_t bits);
uint8_t remove_parity_bit(uint8_t bits);
char* int2bin(int a, char* buffer, int buf_size);
uint8_t get_kth_bit(uint8_t bits, int k);

MessageFrame* message_frame_create();
MessageInfo* message_info_create();
bool DeFrame(MessageFrame* frame, MessageInfo* msg);
bool Frame(MessageInfo* msg, MessageFrame* frame);
uint8_t* DecodeFrame(uint8_t* inBuf);
uint8_t* EncodeFrame(MessageFrame* frameBuf);
#endif
