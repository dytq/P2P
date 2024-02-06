#include "talker.h"
#include "listener.h"
#include "ftp.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>

int get_buff_array(char * buf[10], char * cmd)
{
	// ten bloc max
	char bloc_str[64]; // command size
	int i = 0;
	int j = 0;
	int k = 0;
	while(1)
	{
		char c = cmd[k];
		bloc_str[i] = c;
		i++;
		if(c == ' ' || c == '\n')
		{
			buf[j] = malloc(sizeof(char)*i);
			strncpy(buf[j], bloc_str, i);
			memset(bloc_str, '\0', sizeof(char)*64);	
			j++;
			i = 0;
		}
		if(c == '\n')
		{
			break;
		}
		k++;
	}
	return j;
}

int exec_cmd(char buf[1024], server_ftp_t * server_ftp, client_ftp_t * client_ftp)
{
	char * buff_array[10];
	int nb_bloc = get_buff_array(buff_array, buf);
	if(strncmp(buff_array[0],"cmd", 3) == 0)
	{
		if(strncmp(buff_array[1], "auth", 4) == 0)
		{
			if(nb_bloc == 4)
			{
				server_ftp->auth(buff_array[2],buff_array[3]);
			} 
			else
			{
				printf("auth take 2 arguments: login password\n");	
			}
		} 
	}
	if(strncmp(buff_array[0], "lst", 3) == 0)
	{
		if(strncmp(buff_array[1], "auth", 4) == 0)
		{
			if(nb_bloc == 3)
			{
				server_ftp->update_metadata();
			}
		}
	}
	return 0;
}

int command(int p_cmd)
{
	while(1)
	{
		char buffer[32];
    		char *b = buffer;
    		size_t bufsize = 32;
    		size_t characters;
		characters = getline(&b,&bufsize,stdin);
		char buffer2[32] = "cmd ";
		strcat(buffer2, buffer);
		write(p_cmd, buffer2, 32);
	}
}

int main(int argc, char * argv[])
{	
	// Init server_ftp
	server_ftp_t * server_ftp;
        server_ftp = malloc(sizeof(server_ftp_t));
	// si null
	// server_ftp->sockfd = -1;

	// Init client_ftp	
	client_ftp_t * client_ftp;
	client_ftp = malloc(sizeof(client_ftp_t));
	// si null
	
	
	int p[2]; // pipe 2 files descriptor for cmd		
	pipe(p);
	
	int pid_cmd = fork();
	if(pid_cmd == -1)
	{
		fprintf(stderr, "erreur de la création du fork command");
		exit(EXIT_FAILURE);
	}
	if(pid_cmd == 0)
	{
		printf("$> Listening in cmd\n");
		
		command(p[1]);
		
		close(p[1]);
		exit(0);
	}
	if(pid_cmd > 0)
	{
		// void	
	}
	
	//int p_lst[2]; // pipe 2 files descriptor for listener
	// pipe(p_lst);
	
	int pid_lst = fork();
	
	if(pid_lst == -1)
	{
		fprintf(stdout, "erreur fork\n");
		return -1;
	}
	if(pid_lst == 0)
	{
		printf("$> Listening in : %s:%s\n", argv[1],argv[2]); 
		listener(argv[1], argv[2], p[1]);	
		exit(0);
	}
	if(pid_lst > 0)
	{
		char buf[1024];
		while(read(p[0],buf,1024)>0)
		{ 
			exec_cmd(buf, server_ftp, client_ftp);
		}
	}
	return 0;
}