/*
 * =====================================================================================
 *
 *       Filename:  common.c
 *
 * =====================================================================================
 */

#include "common.h"

#include <stdio.h>
#include <stdlib.h>

void DieWithSystemMessage(const char* msg) {
    perror(msg);
    exit(1);
}

void DieWithUserMessage(const char* msg, const char* detail) {
    fputs(msg, stderr);
    fputs(": ", stderr);
    fputs(detail, stderr);
    fputc('\n', stderr);
    exit(1);
}

void HandleTCPClient(int clntSocket) {
    char buffer[BUFFER_SIZE];  // Buffer for message
    // receive msg from client
    ssize_t numBytesRcvd = recv(clntSocket, buffer, 1024, 0);
    if (numBytesRcvd < 0) DieWithSystemMessage("recv() failed");
    // Send received string and receive again until end of stream
    while (numBytesRcvd > 0) {
        // Echo msg back to client
        ssize_t numBytesSent = send(clntSocket, buffer, numBytesRcvd, 0);
        if (numBytesSent < 0)
            DieWithSystemMessage("send() failed");
        else if (numBytesSent != numBytesRcvd)
            DieWithUserMessage("send()", "sent unexpected number of bytes");
        // see if there is more data to recv
        numBytesRcvd = recv(clntSocket, buffer, BUFFER_SIZE, 0);
        if (numBytesRcvd < 0) DieWithSystemMessage("recv() failed");
    }
    close(clntSocket);  // Close client Socket
}

int SetupTCPServerSocket(const char* service) {
    struct addrinfo addrCriteria;
    memset(&addrCriteria, 0, sizeof(addrCriteria));  // Zero out structure
    addrCriteria.ai_family = AF_UNSPEC;              // Any address family
    addrCriteria.ai_flags = AI_PASSIVE;      // Accept on any address/port
    addrCriteria.ai_socktype = SOCK_STREAM;  // Only stream sockets
    addrCriteria.ai_protocol = IPPROTO_TCP;  // Only TCP protocol

    struct addrinfo* servAddr;  // List of server addresses
    int rtnVal = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
    if (rtnVal != 0)
        DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));

    int servSock = -1;
    for (struct addrinfo* addr = servAddr; addr != NULL; addr = addr->ai_next) {
        // Create a TCP socket
        servSock = socket(servAddr->ai_family, servAddr->ai_socktype,
                          servAddr->ai_protocol);
        if (servSock < 0) continue;  // Socket creation failed; try next address

        // Bind to the local address and set socket to list
        if ((bind(servSock, servAddr->ai_addr, servAddr->ai_addrlen) == 0) &&
            (listen(servSock, 5) == 0)) {
            // Print local address of socket
            struct sockaddr_storage localAddr;
            socklen_t addrSize = sizeof(localAddr);
            if (getsockname(servSock, (struct sockaddr*)&localAddr, &addrSize) <
                0)
                DieWithSystemMessage("getsockname() failed");
            fputs("Binding to ", stdout);
            PrintSocketAddress((struct sockaddr*)&localAddr, stdout);
            fputc('\n', stdout);
            break;  // Bind and list successful
        }

        close(servSock);  // Close and try again
        servSock = -1;
    }
    // Free address list allocated by getaddrinfo()
    freeaddrinfo(servAddr);
    return servSock;
}

int AcceptTCPConnection(int servSock) {
    struct sockaddr_storage clntAddr;  // Client address
    // Set length of client address structure (in-out parameter)
    socklen_t clntAddrLen = sizeof(clntAddr);

    // Wait for a client to connect
    int clntSock = accept(servSock, (struct sockaddr*)&clntAddr, &clntAddrLen);
    if (clntSock < 0) DieWithSystemMessage("accept() failed");

    // clntSock is connected to a client!

    fputs("Handling client ", stdout);
    PrintSocketAddress((struct sockaddr*)&clntAddr, stdout);
    fputc('\n', stdout);

    return clntSock;
}

int SetupTCPClientSocket(const char* host, const char* service) {
    // Tell System what kind(s) of address info we want
    struct addrinfo addrCriteria;
    memset(&addrCriteria, 0, sizeof(addrCriteria));
    addrCriteria.ai_family = AF_UNSPEC;
    addrCriteria.ai_socktype = SOCK_STREAM;
    addrCriteria.ai_protocol = IPPROTO_TCP;
    // Get address(es)
    struct addrinfo* servAddr;
    int rtnVal = getaddrinfo(host, service, &addrCriteria, &servAddr);
    if (rtnVal != 0)
        DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));
    int sock = -1;
    for (struct addrinfo* addr = servAddr; addr != NULL; addr = addr->ai_next) {
        // Create a reliable, stream socket using TCP
        sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sock < 0) continue;  // Socket Creation Failed;try next address
        // Establish connection to the echo server
        if (connect(sock, addr->ai_addr, addr->ai_addrlen) == 0)
            break;    // connection succeeded ; break and return socket
        close(sock);  // Socket connection failed; try next address
        sock = -1;
    }
    freeaddrinfo(servAddr);
    return sock;
}

void PrintSocketAddress(const struct sockaddr* address, FILE* stream) {
    if (address == NULL || stream == NULL) return;

    void* numericAddress;  // pointer to numeric address
    // Buffer to contain result (IPv6 sufficient to hold IPv4)
    char addrBuffer[INET6_ADDRSTRLEN];
    in_port_t port;  // port to print
    // Set pointer to address base on family
    switch (address->sa_family) {
        case AF_INET:
            numericAddress = &((struct sockaddr_in*)address)->sin_addr;
            port = ntohs(((struct sockaddr_in*)address)->sin_port);
            break;
        case AF_INET6:
            numericAddress = &((struct sockaddr_in6*)address)->sin6_addr;
            port = ntohs(((struct sockaddr_in6*)address)->sin6_port);
            break;
        default:
            fputs("[unknown type]", stream);  // Unhandled type
            return;
    }
    // Convert binary to printable address
    if (inet_ntop(address->sa_family, numericAddress, addrBuffer,
                  sizeof(addrBuffer)) == NULL)
        fputs("[Invalid address]", stream);  // Unable to convert
    else {
        fprintf(stream, "%s", addrBuffer);
        if (port != 0)  // Zero not valid in any socket addr
            fprintf(stream, " :%u", port);
    }
}

/* Read 2-byte length and place in big-endian order.
 * Then read the indicated number of bytes.
 * If the input buffer is too small for the data, truncate to fit and
 * return the negation of the *indicated* length. Thus a negative return
 * other than -1 indicates that the message was truncated.
 * (Ambiguity is possible only if the caller passes an empty buffer.)
 * Input stream is always left empty.
 */
int GetNextMsg(FILE* in, uint8_t* buf, size_t bufSize) {
    uint16_t mSize = 0;
    uint16_t extra = 0;

    if (fread(&mSize, sizeof(uint16_t), 1, in) != 1) return -1;
    mSize = ntohs(mSize);
    if (mSize > bufSize) {
        extra = mSize - bufSize;
        mSize = bufSize;  // Truncate
    }
    if (fread(buf, sizeof(uint8_t), mSize, in) != mSize) {
        fprintf(stderr, "Framing error: expected %d, read less\n", mSize);
        return -1;
    }
    if (extra > 0) {  // Message was truncated
        uint8_t waste[BUFSIZ];
        fread(waste, sizeof(uint8_t), extra, in);  // Try to flush the channel
        return -(mSize + extra);                   // Negation of indicated size
    } else
        return mSize;
}

/* Write the given message to the output stream, followed by
 * the delimiter. Precondition: buf[] is at least msgSize.
 * Returns -1 on any error.
 */
int PutMsg(uint8_t buf[], size_t msgSize, FILE* out) {
    if (msgSize > UINT16_MAX) return -1;
    uint16_t payloadSize = htons(msgSize);
    if ((fwrite(&payloadSize, sizeof(uint16_t), 1, out) != 1) ||
        (fwrite(buf, sizeof(uint8_t), msgSize, out) != msgSize))
        return -1;
    fflush(out);
    return msgSize;
}

char* replace(char const* const original, char const* const pattern,
              char const* const replacement) {
    size_t const replen = strlen(replacement);
    size_t const patlen = strlen(pattern);
    size_t const orilen = strlen(original);

    size_t patcnt = 0;
    const char* oriptr;
    const char* patloc;

    // find how many times the pattern occurs in the original string
    for (oriptr = original; (patloc = strstr(oriptr, pattern));
         oriptr = patloc + patlen) {
        patcnt++;
    }

    {
        // allocate memory for the new string
        size_t const retlen = orilen + patcnt * (replen - patlen);
        char* const returned = (char*)malloc(sizeof(char) * (retlen + 1));

        if (returned != NULL) {
            // copy the original string,
            // replacing all the instances of the pattern
            char* retptr = returned;
            for (oriptr = original; (patloc = strstr(oriptr, pattern));
                 oriptr = patloc + patlen) {
                size_t const skplen = patloc - oriptr;
                // copy the section until the occurence of the pattern
                strncpy(retptr, oriptr, skplen);
                retptr += skplen;
                // copy the replacement
                strncpy(retptr, replacement, replen);
                retptr += replen;
            }
            // copy the rest of the string.
            strcpy(retptr, oriptr);
        }
        return returned;
    }
}
