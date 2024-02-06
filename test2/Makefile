run_server_central:
	./p2p 192.168.0.44 66666 localhost 6666
run_client:
	./p2p localhost 6666 192.168.0.44 6666
compile: main.o listener.o talker.o parser.o
	gcc -o p2p main.o listener.o talker.o parser.o

main.o: 
	gcc -c main.c listener.c talker.c

listener.o: 
	gcc -c listener.c

talker.o:
	gcc -c talker.c	

parser.o:
	gcc -c parser.c
clean:
	rm *.o p2p
