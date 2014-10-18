#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
#include <dirent.h>
#include <sys/stat.h>

#define BACKLOG 10

struct addrinfo hints,*serv_info,host_addr;
struct sockaddr_storage their_addr;
FILE* fd;

int dir(char* host,char *path)
{
	struct stat st = {0};
	char* temp=NULL;
	char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
printf("..........\n");
        else
           	perror("getcwd() error");
	mkdir("cache", 0700);
	chdir("cache");
	mkdir(host,0700);
	chdir(host);
	temp=strtok(path,"/");
	char *extn;
	int flag=0;
	while(temp!=NULL)
	{
		if ((stat(temp, &st) == -1)&&(temp!=NULL)) 
		{
		    mkdir(temp, 0700);
		    flag=1;
		}
		chdir(temp);
		extn=temp;
		temp=strtok(NULL,"/");
	}
	if(temp==NULL)
	{
	if (stat("Homepage", &st) == -1) 
		{
			flag=1;
		}


		fd=fopen("Homepage","a+");
	}
	else
	{
	if (stat(extn, &st) == -1) 
		{
			flag=1;
		}


	fd=fopen(extn,"a+");
	}
	return flag;
}

void cache(char* buffer)
{
	char temp[128];
	while(1)
	{
		if((fgets(temp,128,fd))==NULL) break;
		strcat(buffer,temp);
	}
	return;
	

	
}

int send_file(char* host,char* port,char* path,char* v,int newsockfd)
{
	struct addrinfo host_addr,*servinfo;
	struct addrinfo *p;
	int newsockfd1,sockfd1,n; 
   	memset(&host_addr, 0, sizeof(host_addr));
	host_addr.ai_family=AF_UNSPEC;
	host_addr.ai_socktype=SOCK_STREAM;
	char s[INET6_ADDRSTRLEN];
	char buffer[1024];
	int rv;
/*
	if ((rv = getaddrinfo(host,port,&host_addr, &servinfo)) != 0) 
	{
        	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        	return;
    	}

    
    	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
        	if ((sockfd1 = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
		{
            		perror("client: socket");
            		continue;
        	}

        	if (connect(sockfd1, p->ai_addr, p->ai_addrlen) == -1) 
		{
        	    	close(sockfd1);
        	    	perror("client: connect");
        	    	continue;
	        }
	
	        break;
	}

    	if (p == NULL) 
	{
	        fprintf(stderr, "client: failed to connect\n");
	        return;
	}

    	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);

	sprintf(buffer,"\nConnected to %s  IP - %s\n",host,s);
	printf("\n%s\n",buffer);

//	temp=path;

	if(path!=NULL)
		sprintf(buffer,"GET /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",path,v,host);
	else
		sprintf(buffer,"GET / %s\r\nHost: %s\r\nConnection: close\r\n\r\n",v,host);
sending:
	n=send(sockfd1,buffer,strlen(buffer),0);
	printf("\n%s\n",buffer);
	if(n<0)
		error("Error writing to socket");
	else
	{
*/		

			bzero((char*)buffer,1024);
			cache(buffer);	
				send(newsockfd,buffer,strlen(buffer),0);

//	}
}
void error(char* msg)
{
perror(msg);
exit(0);
}

void* get_in_addr(struct sockaddr *sa)
{
	if(sa->sa_family==AF_INET)
	{
	return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void arg_check(int argc)
{
	if (argc<2) 
	{
		fprintf (stderr,"ERROR! Provide A Port!\n");
  		exit(1);
  	}
}

int set_server(const char *Host,const char *service)
{
	struct addrinfo *p;
	

	int yes=1;


	memset(&hints,0,sizeof hints);
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;
	
	int sockfd,i,portno,clilen,n;

	n=getaddrinfo(Host,service,&hints,&serv_info);
	if(n!=0)
	{
	fprintf(stderr,"getaddrinfo %s\n",gai_strerror(n));
	return 1;
	}

	for(p=serv_info;p!=NULL;p=p->ai_next)
	{
		if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1)
		{
			perror("server:socket\n");
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1)
		{
            		perror("setsockopt");
            		exit(1);
        	}	
		
		if(bind(sockfd,p->ai_addr,p->ai_addrlen)==-1)
		{
			close(sockfd);
			perror("server:bind");
			continue;
		}	
		break;
	}

	if(p==NULL)
	{
		fprintf(stderr,"server:failed to bind\n");
		return 2;
	}
	freeaddrinfo(serv_info);

	if(listen(sockfd,BACKLOG)==-1)
	{
	perror("listen");
	exit(1);
	}
	return sockfd;
}


int make_connection(int sockfd)
{
	printf("Server Start Running.........\n");
	char s[INET6_ADDRSTRLEN];
	socklen_t sin_size;
	int newsockfd,pid;	
	while(1)
	{
		sin_size=sizeof their_addr;
		newsockfd=accept(sockfd,(struct sockaddr *)&their_addr,&sin_size);

		if(newsockfd==-1)
		{
			perror("accept");
			continue;
		}


		inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s,sizeof s);

		printf("Connected client:\n");
		printf("server:got connection from %s\n",s);
pid=fork();
if(pid==0)
break;
	}
	return newsockfd;
}



int connect_host(char* host,char* port,char* path,char* v,int newsockfd)
{
	struct addrinfo *p;
	int newsockfd1,sockfd1,n; 
   	memset(&host_addr, 0, sizeof(host_addr));
	host_addr.ai_family=AF_UNSPEC;
	host_addr.ai_socktype=SOCK_STREAM;
	char s[INET6_ADDRSTRLEN];
	char buffer[1024];

	int rv;
	if ((rv = getaddrinfo(host,port,&host_addr, &serv_info)) != 0) 
	{printf("erorororororoorororor");	
        	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        	return 1;
    	}
printf("one\n");

    	for(p = serv_info; p != NULL; p = p->ai_next) 
	{
        	if ((sockfd1 = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
		{
            		perror("client: socket");
            		continue;
        	}

        	if (connect(sockfd1, p->ai_addr, p->ai_addrlen) == -1) 
		{
        	    	close(sockfd1);
        	    	perror("client: connect");
        	    	continue;
	        }
	
	        break;
	}
printf("two\n");
    	if (p == NULL) 
	{
	        fprintf(stderr, "client: failed to connect\n");
	        return;
	}
    	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);

	sprintf(buffer,"\nConnected to %s  IP - %s\n",host,s);
	printf("\n%s\n",buffer);

	if(path!=NULL)
		sprintf(buffer,"GET /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",path,v,host);
	else
		sprintf(buffer,"GET / %s\r\nHost: %s\r\nConnection: close\r\n\r\n",v,host);
sending:
	n=send(sockfd1,buffer,strlen(buffer),0);
	printf("\n%s\n",buffer);
	if(n<0)
		error("Error writing to socket");
	else
	{
		do
		{
			bzero((char*)buffer,500);
			n=recv(sockfd1,buffer,500,0);
			fprintf(fd,"%s",buffer);
			if(n<0)
				error("Error reading from socket");
			else
				send(newsockfd,buffer,strlen(buffer),0);
		}
		while(n>0);

	}
}
	









void GET(int newsockfd)
{
	
	char s[INET6_ADDRSTRLEN];
	char buffer[1024];
	
	int flag=0,n,i;
	char t1[512],t2[512],t3[16];
	char* temp=NULL;
	char* port;
	bzero(buffer,1024);
	recv(newsockfd,buffer,1024,0);
	sscanf(buffer,"%s %s %s",t1,t2,t3);
	if(((strncmp(t1,"GET",3)==0))&&((strncmp(t3,"HTTP/1.1",8)==0)||(strncmp(t3,"HTTP/1.0",8)==0))&&(strncmp(t2,"http://",7)==0))
	{
		strcpy(t1,t2);
		flag=0;
		for(i=7;i<strlen(t2);i++)
		{
			if(t2[i]==':')
			{
				flag=1;
				break;
			}
		}
		temp=strtok(t2,"//");
		if(flag==0)
		{
			port="80";
			temp=strtok(NULL,"/");
		}
		else
		{
			temp=strtok(NULL,":");
		}
  		sprintf(t2,"%s",temp);
		printf("host = %s",t2);
 		if(flag==1)
		{
			temp=strtok(NULL,"/");
			port=temp;
		}



		strcat(t1,"^]");
		temp=strtok(t1,"//");
		temp=strtok(NULL,"/");
		if(temp!=NULL)
			temp=strtok(NULL,"^]");
		printf("\npath = %s\nPort = %s\n",temp,port);
		i=dir(t2,temp);
		if(i==1)
		{	printf("From Host\n");
			connect_host(t2,port,temp,t3,newsockfd);
		}
		else
		{	printf("From cache\n");
			send_file(t2,port,temp,t3,newsockfd);
		}
	}	

	else
	{
		send(newsockfd,"400 : BAD REQUEST\nONLY HTTP REQUESTS ALLOWED",44,0);
	}
  
}





int main(int argc,char **argv)
{	
	int sockfd,newsockfd;
	arg_check(argc);	
	sockfd=set_server(NULL,argv[1]);
	newsockfd=make_connection(sockfd);
	GET(newsockfd);
}




