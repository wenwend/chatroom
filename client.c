/*
** Author: wenwendong onid:dongwen
** Date: Feb 8, 2018
** Desc: This progmme serves as the client side for a TCP chatroom. 
**  	 The client must start talking first, then the conversition will set up with server.
** client.c -- the set up codes are cited from http://beej.us/guide/bgnet/html/multi/clientserver.html#simpleclient
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "20002" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//chatroom hostA is server name, host B is client name
void chatroom(int sockfd, char* hostA, char* hostB){
	char input[501],output[501];
	int count=0,flag;
	
	//clear input and output.
	memset(input,0,sizeof(input));
	memset(output,0,sizeof(output));
	fgets(input,500,stdin);
	while(1){
		printf("%s > ",hostB);
		fgets(input,500,stdin);
		if(strcmp(input,"\\quit\n") == 0)
			break;
		count=send(sockfd,input,strlen(input),0);
		if(count ==-1){
			fprintf(stderr,"Error sending message\n");
			exit(1);
		}
		flag=recv(sockfd,output,500,0);
		if(flag==-1){
			fprintf(stderr,"Error receiving message\n");
			exit(1);
		}
		else if(flag ==0){
			printf("Server closing now\n");
			exit(1);
		}
		else
			printf("%s >%s \n",hostA,output);	
		memset(input,0,sizeof(input));
		memset(output,0,sizeof(output));
	}
	close(sockfd);
	
}


int main(int argc, char *argv[])
{
	int sockfd,numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	int sender,receiver;
	char hostA[10],hostB[10];

	if (argc != 3) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}
	
	printf("Please enter a 10 character username:");
	scanf("%s",hostB);
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s:%s\n", s, PORT);
	
	//hostB is client, hostA is server
	sender=send(sockfd,hostB,strlen(hostB),0);
	//initial query "handle" to get server name
	receiver=recv(sockfd,hostA,10,0);
	
	//printf("servername '%s'\n",hostA);
	chatroom(sockfd,hostA,hostB);
	
	
	freeaddrinfo(servinfo); // all done with this structure

	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}
	
	

	
	//buf[numbytes] = '\0';

	//printf("client: received '%s'\n",buf);

	

	return 0;
}
