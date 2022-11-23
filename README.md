---
title: Operating Systems Project 3
author: ""
date: 2022-11-23
---
# Introduction
# System Documentation
## Flow Diagram
## List of Routines
### Global Variables
struct Queue * 	threadQa
 	threadQa; the Queue for thread A.

struct Queue * 	threadQe
 	threadQe the Queue for thread E.

struct Queue * 	threadQi
 	threadQi the Queue for thread I.

struct Queue * 	threadQo
 	threadQo the Queue for thread O.

struct Queue * 	threadQu
 	threadQu the Queue for thread U.

struct Queue * 	threadQd
 	threadQd the Queue for thread Digit.

struct Queue * 	threadQw
 	threadQw the Queue for thread Writer.

int 	sum = 0
 	the sum of all digits.

int 	pipefds [2]
 	the pipe file descriptors.

pthread_cond_t 	comp_a = PTHREAD_COND_INITIALIZER
 	condition variable for thread A.

pthread_cond_t 	comp_e = PTHREAD_COND_INITIALIZER
 	condition variable for thread E.

pthread_cond_t 	comp_i = PTHREAD_COND_INITIALIZER
 	condition variable for thread I.

pthread_cond_t 	comp_o = PTHREAD_COND_INITIALIZER
 	condition variable for thread O.

pthread_cond_t 	comp_u = PTHREAD_COND_INITIALIZER
 	condition variable for thread U.

pthread_cond_t 	comp_d = PTHREAD_COND_INITIALIZER
 	condition variable for thread Digit.

pthread_cond_t 	comp_w = PTHREAD_COND_INITIALIZER
 	condition variable for thread Writer.

pthread_t 	thread_charA
 	thread A.

pthread_t 	thread_charE
 	thread E.

pthread_t 	thread_charI
 	thread I.

pthread_t 	thread_charO
 	thread O.

pthread_t 	thread_charU
 	thread U.

pthread_t 	thread_digit
 	thread D.

pthread_t 	thread_writer
 	thread W.

pthread_mutex_t 	mutex_a = PTHREAD_MUTEX_INITIALIZER
 	mutex A.

pthread_mutex_t 	mutex_e = PTHREAD_MUTEX_INITIALIZER
 	mutex E.

pthread_mutex_t 	mutex_i = PTHREAD_MUTEX_INITIALIZER
 	mutex I.

pthread_mutex_t 	mutex_o = PTHREAD_MUTEX_INITIALIZER
 	mutex O.

pthread_mutex_t 	mutex_u = PTHREAD_MUTEX_INITIALIZER
 	mutex U.

pthread_mutex_t 	mutex_d = PTHREAD_MUTEX_INITIALIZER
 	mutex D.

pthread_mutex_t 	mutex_w = PTHREAD_MUTEX_INITIALIZER
 	mutex W.

sem_t 	empty

sem_t 	empty_e
 	empty semaphore E.

sem_t 	empty_i
 	empty semaphore I.

sem_t 	empty_o
 	empty semaphore O.

sem_t 	empty_u
 	empty semaphore U.

sem_t 	empty_d
 	empty semaphore D.

sem_t 	empty_w
 	empty semaphore W.

sem_t 	full

sem_t 	full_e
 	full semaphore E.

sem_t 	full_i
 	full semaphore I.

sem_t 	full_o
 	full semaphore O.

sem_t 	full_u
 	full semaphore U.

sem_t 	full_d
 	full semaphore D.

sem_t 	full_w
 	full semaphore W.

sem_t 	cli_sock
 	client socket semaphore.

sem_t 	cli_sock_free
 	client socket free semaphore.
### Functions
Classes
union  	MessageFrameConverter
 	the converter. .

struct  	MessageInfo
 	information. .

struct 	__attribute__ ((__packed__)) MessageFrame
Functions

void 	DieWithUserMessage (const char *msg, const char *detail)
 	Exit with user message. .

void 	DieWithSystemMessage (const char *msg)
 	Exit with syatem message. .

void 	HandleTCPClient (int clntSocket)
 	Handle a TCP Client. .

void 	PrintSocketAddress (const struct sockaddr *address, FILE *stream)
 	Print the socket addeess to this stream. .

int 	SetupTCPClientSocket (const char *host, const char *service)
 	Setup a TCP Client on this host and port. .

int 	SetupTCPServerSocket (const char *service)
 	Setup a TCP Server on this porr. .

int 	AcceptTCPConnection (int servSock)
 	Accept a TCP connection on this Socket. .

int 	GetNextMsg (FILE *in, uint8_t *buf, size_t bufSize)
 	Read a message of this size into this buffer. .

int 	PutMsg (uint8_t buf[], size_t msgSize, FILE *out)
 	Write the message in the buffer to this file. .

char * 	replace (char const *const original, char const *const pattern, char const *const replacement)
 	Replace the pattern in the original string with the replacement. .

uint8_t 	get_byte_from (uint32_t value, int start_bit)
 	Get a byte from this value. .

unsigned int 	count_set_bits (uint8_t n)
 	Count the set bits in n. .

uint8_t 	clear_bit (uint8_t n, int i)
 	Clear the bit at position i on n. .

uint8_t 	set_bit (uint8_t n, int i)
 	Set the bit at position i on n. .

uint8_t 	char2bin (char c)
 	Convert this char to binary. .

char 	bin2char (uint8_t b)
 	Convert this byte to char. .

bool 	check_parity_bit (uint8_t bits)
 	Check if the the parity bit of this byte is set. .

uint8_t 	add_parity_bit (uint8_t bits)
 	Add the parity bit of this byte. .

uint8_t 	remove_parity_bit (uint8_t bits)
 	Remove the parity bit of this byte. .

char * 	int2bin (int a, char *buffer, int buf_size)
 	Return a binary string representation of this int. .

uint8_t 	get_kth_bit (uint8_t bits, int k)
 	Get the kth bit from this byte. .

MessageFrame * 	message_frame_create ()
 	Create a MessageFrame object by allocating memory.

MessageInfo * 	message_info_create ()
 	Create a MessageInfo object by allocating memory.

bool 	DeFrame (MessageFrame *frame, MessageInfo *msg)
 	Decode this frame into a MessageInfo object. .

bool 	Frame (MessageInfo *msg, MessageFrame *frame)
 	Create a Frame of this message. .

uint8_t * 	DecodeFrame (uint8_t *inBuf)
 	Decode this frame and return its message. .

uint8_t * 	EncodeFrame (MessageFrame *frameBuf)
 	Encode Frame into a byte array. More

## Implementation
# Test Documentation
To Compile the code first extract the zip file in a directory and run in terminal

```bash
$ make
$ ./server 12345 # run the server
$ ./client 127.0.0.1 12345 intext.txt # run the client

```
# User Documentation
The server runs in an infinite loop until a Interrupt signal is send `SIGINT`
and for each client connection creates 2 processes, one to receive client data,
decode it and queue it. The first process also creates 7 threads to process the
received messages. The last thread writes the processed messages to a pipe to be
read by the second process.

The second process frames the message and sends it back to the client.

The client reads the input file passed as a command line argument, then frames
the message and sends it to the server in one process. The second process is
responsible for reading the responses from the server and writing the received
messages to a file named `result.txt` then closes the connection and exits.
