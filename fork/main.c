// C program to illustrate 
// pipe system call in C 
// shared by Parent and Child 
#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <sys/wait.h>

#define MSGSIZE 16 

char* msg1 = "hello, world #1"; 
char* msg2 = "hello, world #2"; 
char* msg3 = "hello, world #3"; 
  
int main() 
{ 
    char inbuf[MSGSIZE]; 
    int p[2], pid, nbytes; 
  
    if (pipe(p) < 0) 
        exit(1); 

    int process = 2;
   pid_t children[process];

   for(int i = 0; i < process; i++)
   { 
	   int pid = fork(); 
	    if (pid > 0) { 
	
		children[i] = pid;
		for(int i = 0; i < process; i++)
		{
			pid_t get_child = children[i];
			write(p[1], msg1, MSGSIZE); 
			write(p[1], msg2, MSGSIZE); 
			write(p[1], msg3, MSGSIZE);
			close(p[1]);	
			pipe(p);
			//waitpid(get_child, NULL,0);
			//sleep(1);
		}
		printf("pid child: %d\n",pid);
	    } 
	  
	    else { 
		printf("création du fils %d\n", i);
		close(p[1]);
		while ((nbytes = read(p[0], inbuf, MSGSIZE)) > 0) 
		    printf("% s\n", inbuf); 
		if (nbytes != 0) 
		    exit(2); 
		
		printf("Finished reading\n");
		exit(EXIT_SUCCESS);
	    }
   }

    return 0; 
} 

