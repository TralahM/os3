/*
 * =====================================================================================
 *
 *       Filename:  test_message_framing.c
 *
 *    Description: Test Message Framing and Deframing
 *
 *        Version:  1.0
 *
 *        Created:  11/18/2022 01:21:01 PM
 *
 *       Revision:  none
 *
 *       Compiler:  gcc
 *
 *         Author:  Tralah M Brian (https://github.com/TralahM),
 * briantralah@gmail.com
 *
 *         Email:   briantralah@gmail.com
 *
 *   Organization:  Research and Development
 *
 *   Company:       TralahTek LLC
 *
 * =====================================================================================
 */
#include "common.h"
#define CHUNK_SIZE 4

int main(int argc, char* argv[]) {
    FILE* fp;
    char buffer[CHUNK_SIZE];
    MessageInfo* m_info = message_info_create();
    MessageFrame* m_frame = message_frame_create();
    fp = fopen(argv[1], "r");
    printf("char2bin(e) : %d\n", char2bin('e'));
    printf("bin2char(e) : %d\n", bin2char(char2bin('e')));
    int C = 0;
    while (!feof(fp)) {
        C++;
        fgets(buffer, CHUNK_SIZE, fp);
        strcpy(m_info->str, buffer);
        printf("Orignal message:\033[33m %s\033[0m\n", m_info->str);
        Frame(m_info, m_frame);
        /* printf("Frame LENGTH: %d SYN: %d DATA: %d\n", m_frame->LENGTH, */
        /*        m_frame->SYN, m_frame->DATA); */
        free(m_info);
        m_info = message_info_create();
        DeFrame(m_frame, m_info);
        printf("Decoded message:\033[32m %s\033[0m\n", m_info->str);
        printf("EncodedFrame: %s\n", EncodeFrame(m_frame));
        printf("DecodedFrame: %s\n", DecodeFrame(EncodeFrame(m_frame)));
        free(m_info);
        /* free(m_frame); */
        m_frame = message_frame_create();
        m_info = message_info_create();
    }
    fclose(fp);
    // Testing parity checks
    for (int i = 0; i < 4; i++) {
        int x = (rand() % 255) - i;
        char pbuf[9];
        char qbuf[9];
        char rbuf[9];
        /* char sbuf[9]; */
        pbuf[8] = '\0';
        qbuf[8] = '\0';
        rbuf[8] = '\0';
        int p = check_parity_bit(x);
        /* sbuf[8] = '\0'; */
        printf("\n");
        int2bin(add_parity_bit(x), qbuf, 8);
        int2bin(remove_parity_bit(x), rbuf, 8);
        int2bin(x, pbuf, 8);
        printf(
            "x: %d check_parity_bit: %d  add_parity_bit: %d remove_parity_bit: "
            "%d\n",
            x, check_parity_bit(x), add_parity_bit(x), remove_parity_bit(x));
        printf(
            "x: %s check_parity_bit: %d  add_parity_bit: %s remove_parity_bit: "
            "%s\n",
            pbuf, p, qbuf, rbuf);
        printf("add_parity_bit(remove_parity_bit(%d))=%d\n", x,
               add_parity_bit(remove_parity_bit(x)));
        printf("remove_parity_bit(add_parity_bit(%d))=%d\n", x,
               remove_parity_bit(add_parity_bit(x)));
    }
    return 0;
}
