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
### Functions
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
