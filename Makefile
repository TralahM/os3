all:
	gcc -Og message_encoder.c common.c queue.c server.c -o server
	gcc -Og message_encoder.c common.c queue.c client.c -o client
	gcc -Og message_encoder.c common.c test_message_framing.c -o testmf
	gcc -Og queue.c test_queue.c -o testq
