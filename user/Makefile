run_server_central:
	./p2p 192.168.0.25 6666 localhost 6666

run_client:
	./p2p localhost 6666 192.168.0.25 6666
	
compile: main.o listener.o talker.o parser.o server_ftp.o client_ftp.o
	gcc -g -o p2p main.o listener.o talker.o parser.o server_ftp.o client_ftp.o

main.o: 
	gcc -c main.c

listener.o: 
	gcc -c listener.c

talker.o:
	gcc -c talker.c	

parser.o:
	gcc -c parser.c

server_ftp.o:
	gcc -c server_ftp.c

client_ftp.o:
	gcc -c client_ftp.c

clean:
	rm *.o p2p

