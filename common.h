/**
 * \file common.h
 * Common Header Files.
 */
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
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>/* for Socket data types */
#include <sys/wait.h>
#include <unistd.h>

#include "buffer.h"
/**
 * \typedef struct MessageInfo MessageInfo;
 */
typedef struct MessageInfo MessageInfo;

/**
 * \typedef struct MessageFrame MessageFrame;
 * the message frame type.
 * \struct MessageFrame;
 * the message frame.
 */
typedef struct __attribute__((__packed__)) MessageFrame {
    uint16_t SYN;   /*!< SYN Field*/
    uint8_t LENGTH; /*!< LENGTH Field*/
    uint32_t DATA;  /*!< DATA Field*/
} MessageFrame;

/**
 * \union MessageFrameConverter;
 * the converter.
 */
typedef union MessageFrameConverter {
    MessageFrame frame;                  /*!< frame Field*/
    uint8_t bytes[sizeof(MessageFrame)]; /*!< bytes Field*/
} MessageFrameConverter;

/**
 * \struct MessageInfo holds message information.
 */
struct MessageInfo {
    char* str; /*!< str Field with actual message*/
};

/**
 * Exit with user message.
 * @param msg the message.
 * @param detail the details.
 */
void DieWithUserMessage(const char* msg, const char* detail);
/**
 * Exit with syatem message.
 * @param msg the message.
 */
void DieWithSystemMessage(const char* msg);

/**
 * Handle a TCP Client.
 * @param clntSocket the client socket.
 */
void HandleTCPClient(int clntSocket);
/**
 * Print the socket addeess to this stream.
 * @param address the socket address.
 * @param stream the output stream.
 */
void PrintSocketAddress(const struct sockaddr* address, FILE* stream);
/**
 * Setup a TCP Client on this host and port.
 * @param host the host/ip to the server.
 * @param service the port to the server.
 */
int SetupTCPClientSocket(const char* host, const char* service);
/**
 * Setup a TCP Server on this porr.
 * @param service the port to listen on.
 */
int SetupTCPServerSocket(const char* service);
/**
 * Accept a TCP connection on this Socket.
 * @param servSock the socket of the server.
 */
int AcceptTCPConnection(int servSock);

/**
 * Read a message of this size into this buffer.
 * @param in the input file.
 * @param bufSize the size of the message.
 * @param buf the message buffer.
 */
int GetNextMsg(FILE* in, uint8_t* buf, size_t bufSize);
/**
 * Write the message in the buffer to this file.
 * @param buf the message buffer.
 * @param msgSize the message size.
 * @param out the output file.
 */
int PutMsg(uint8_t buf[], size_t msgSize, FILE* out);

/**
 * Replace the pattern in the original string with the replacement.
 * @param original the original string.
 * @param pattern the pattern string.
 * @param replacement the replacement string.
 */
char* replace(char const* const original, char const* const pattern,
              char const* const replacement);
/**
 * Get a byte from this value.
 * @param value the value.
 * @param start_bit the position to start slicing.
 */
uint8_t get_byte_from(uint32_t value, int start_bit);
/**
 * Count the set bits in n.
 * @param n the byte to count set bits.
 */
unsigned int count_set_bits(uint8_t n);
/**
 * Clear the bit at position i on n.
 * @param n the byte to clear the bit.
 * @param i the position at which to clear the bit.
 */
uint8_t clear_bit(uint8_t n, int i);
/**
 * Set the bit at position i on n.
 * @param n the byte to set the bit.
 * @param i the position at which to set the bit.
 */
uint8_t set_bit(uint8_t n, int i);
/**
 * Convert this char to binary.
 * @param c the char to convert to bin.
 */
uint8_t char2bin(char c);
/**
 * Convert this byte to char.
 * @param b the binary to convert to char.
 */
char bin2char(uint8_t b);
/**
 * Check if the the parity bit of this byte is set.
 * @param bits the byte to add the parity bit.
 *
 */
bool check_parity_bit(uint8_t bits);
/**
 * Add the parity bit of this byte.
 * @param bits the byte to add the parity bit.
 *
 */
uint8_t add_parity_bit(uint8_t bits);
/**
 * Remove the parity bit of this byte.
 * @param bits the byte to remove the parity bit.
 */
uint8_t remove_parity_bit(uint8_t bits);
/**
 * Return a binary string representation of this int.
 * @param a the integer to convert.
 * @param buffer where to store the string representation.
 * @param buf_size the size the buffer.
 */
char* int2bin(int a, char* buffer, int buf_size);
/**
 * Get the kth bit from this byte.
 * @param bits the byte to get from.
 * @param k the kth position to get.
 */
uint8_t get_kth_bit(uint8_t bits, int k);

/**
 * Create a MessageFrame object by allocating memory.
 */
MessageFrame* message_frame_create();
/**
 * Create a MessageInfo object by allocating memory.
 */
MessageInfo* message_info_create();
/**
 * Decode this frame into a MessageInfo object.
 * @param frame the frame to Decode.
 * @param msg where to store the message info object.
 */
bool DeFrame(MessageFrame* frame, MessageInfo* msg);
/**
 * Create a Frame of this messagw.
 * @param msg the message info to frame.
 * @param frame where to store the created frame.
 */
bool Frame(MessageInfo* msg, MessageFrame* frame);
/**
 * Decode this frame and return its message.
 * @param inBuf the byte array input buffer.
 */
uint8_t* DecodeFrame(uint8_t* inBuf);
/**
 * Encode Frame into a byte array.
 * @param frameBuf the frame object to encode.
 */
uint8_t* EncodeFrame(MessageFrame* frameBuf);
#endif
