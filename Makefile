all:
	gcc message_encoder.c common.c queue.c server.c -o server
	gcc message_encoder.c common.c queue.c client.c -o client
	gcc message_encoder.c common.c test_message_framing.c -o testmf
	gcc queue.c test_queue.c -o testq
