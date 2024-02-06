struct server_str {
	int sockfd;
};

int talker(char * addr, char * port, char * text, struct server_str * sockfd);

