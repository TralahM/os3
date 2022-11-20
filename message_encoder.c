/*
 * =====================================================================================
 *
 *       Filename:  message_encoder.c
 *
 *    Description: Routines to encode and decode messages
 *
 *
 *       Compiler:  gcc
 *
 * =====================================================================================
 */

#include "common.h"
#define MAX_STR_LEN 4

/*
 * MessageFrame represents actual packet format.
 * This function initializes one such structure for transmission.
 */
MessageFrame* message_frame_create() {
    MessageFrame* m = (MessageFrame*)calloc(1, sizeof(MessageFrame));
    m->DATA = 0;
    return m;
}

/*
 * MessageInfo is a wrapper around a string to be sent.
 * This function initializes one such structure to hold a fixed size string of 4
 * bytes.
 */
MessageInfo* message_info_create() {
    MessageInfo* m = (MessageInfo*)malloc(sizeof(MessageInfo));
    m->str = (char*)calloc(MAX_STR_LEN, sizeof(char));
    return m;
}

/*
 * Convert a character to a byte type
 */
uint8_t char2bin(char c) { return (uint8_t)c; }

/*
 * convert a byte to a character
 */
char bin2char(uint8_t b) { return (char)b; }

/*
 * Return binary representation of an integer.
 * For debugging.
 */
char* int2bin(int a, char* buffer, int buf_size) {
    buffer += (buf_size - 1);

    for (int i = (buf_size - 1); i >= 0; i--) {
        *buffer-- = (a & 1) + '0';

        a >>= 1;
    }

    return buffer;
}

uint8_t get_kth_bit(uint8_t bits, int k) { return bits & (1 << k); }

/*
 * Check if parity bit is set or not
 */
bool check_parity_bit(uint8_t bits) {
    if (((count_set_bits(bits) % 2 != 0) &&
         !get_kth_bit((bits << 1), 1))) {  // all set bits are odd so no
        return true;
    }
    return false;
}

/*
 * Add odd parity bit to this byte if ones are even else do nothing.
 */
uint8_t add_parity_bit(uint8_t bits) {
    if (!get_kth_bit((bits << 1), 1)) {
        if (!check_parity_bit(bits)) {
            return set_bit((bits << 1), 1);
        }
    }
    if (count_set_bits(bits) % 2 == 0) {
        return ((set_bit((bits << 1), 1)));
    }
    return bits;
}

/*
 * Remove the parity bit to restore the original bits
 */
uint8_t remove_parity_bit(uint8_t bits) {
    if (check_parity_bit(bits)) {
        if (get_kth_bit((bits << 1), 1)) {
            return clear_bit(bits, 1);
        }
        return set_bit(bits, 1);
    }
    return bits;
}

/*
 *
 */
bool Frame(MessageInfo* msg, MessageFrame* frame) {
    uint8_t len = strlen(msg->str);
    char size_str[40];
    sprintf(size_str, "Got %d instead of %d\n", len, MAX_STR_LEN);
    if (len > MAX_STR_LEN) {
        DieWithUserMessage("Frame: Message Too Long: ", size_str);
    }
    frame->SYN = ((uint16_t)2222);
    frame->LENGTH = len;
    uint8_t* f = (uint8_t*)&(frame->DATA);
    for (int i = 0; i < len; i++) {
        uint8_t cb = char2bin(msg->str[i]);
        if (check_parity_bit(cb)) {
            cb = (add_parity_bit(cb) & remove_parity_bit(cb));
        }
        *f = cb;
        f++;
    }
    return true;
}

/*
 *
 */
bool DeFrame(MessageFrame* frame, MessageInfo* msg) {
    /* int l = ntohs(frame->LENGTH); */
    /* int d = ntohs(frame->DATA); */
    char* str = (char*)calloc(frame->LENGTH, sizeof(char));
    /* char str[l]; */
    uint8_t* f = (uint8_t*)&(frame->DATA);
    /* uint8_t* f = (uint8_t*)&(d); */
    for (int i = 0; i < frame->LENGTH; i++) {
        /* if (get_kth_bit(*f, 1) && check_parity_bit(*f) && */
        /*     !check_parity_bit(remove_parity_bit(*f))) { */
        /*     str[i] = bin2char(remove_parity_bit(*f)); */
        /* } */
        str[i] = bin2char(*f);
        f++;
    }
    /* str[frame->LENGTH] = '\0'; */
    strcpy(msg->str, str);
    return true;
}

/*
 *
 */
uint8_t clear_bit(uint8_t n, int i) { return n & (~(1 << i)); }
uint8_t set_bit(uint8_t n, int i) { return n | (1 << i); }

/*
 *
 */
unsigned int count_set_bits(uint8_t n) {
    unsigned int count = 0;
    while (n) {
        n &= (n - 1);
        count++;
    }
    return count;
}

/*
 *
 */
uint8_t get_byte_from(uint32_t value, int start_bit) {
    unsigned mask;
    mask = ((1 << 8) - 1) << start_bit;
    return value & mask;
}

/*
 *
 */
uint8_t* DecodeFrame(uint8_t* inBuf) {
    printf("%s  ---> %d\n", inBuf, inBuf);
    MessageFrame* mf = (MessageFrame*)inBuf;
    MessageFrame* nf = message_frame_create();
    printf("Frame LENGTH: %d SYN: %d DATA: %d\n", mf->LENGTH, mf->SYN,
           mf->DATA);
    nf->SYN = ntohs(mf->SYN);
    nf->LENGTH = ntohs(mf->LENGTH);
    nf->DATA = ntohs(mf->DATA);
    printf("Frame LENGTH: %d SYN: %d DATA: %d\n", nf->LENGTH, nf->SYN,
           nf->DATA);
    MessageInfo* mi = message_info_create();
    DeFrame(nf, mi);
    return (uint8_t*)mi->str;
}

/*
 *
 */
MessageFrameConverter mf_converter;
uint8_t* EncodeFrame(MessageFrame* frameBuf) {
    /* struct MessageFrame frame = { */
    /*     .SYN = (frameBuf->SYN), */
    /*     .LENGTH = (frameBuf->LENGTH), */
    /*     .DATA = (frameBuf->DATA), */
    /* }; */
    mf_converter.frame = *frameBuf;
    return mf_converter.bytes;
}
