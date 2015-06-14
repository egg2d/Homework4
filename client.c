#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include<time.h>
#define MAXLINE  511 //최대값 지정
#define BLOCK 255 //BLOCK 단위로 저장


int getLine(char s[], int limit)
{
	int c, i;
	i = 0;
	while ((c = getchar()) != EOF && --limit > 0)  {
		if (c == '\n') {
			s[i++] = '\0';
			return i;
		}
		else s[i++] = c;
	}
	return -1;
}
void credit20123374(){
	printf("yeonjin designed credit part and some part of changing tcp connection \n");
}

int main(int argc, char *argv[])
{

	int i = 0;
	int s; //socket
	int nbyte;
	int fileSize;
	char ip[20];
	//char buf[MAXLINE+1], buf2[MAXLINE+1];
	FILE *stream; //파일 입출력
	FILE *stream2; // get
	char fileName[20];
	int portNum;
	char msg2[20];   // received value
	char msg3[20]; // recevied 3
	char *temp;
	int numfread;
	int sockid, retcode, addrlen;
	int nread=0;
	struct hostent *hostp;
	struct sockaddr_in my_addr, server_addr, client_addr;
	char msg[500];
	char input[50];
	double percent;
	int currentSize = 0;
	int second = 0;
	int progressbar;
	int ID=0;
	for (;;)
	{
		portNum = 0;
		printf("connect : ");
		scanf("%s %d", ip, &portNum);
		if (ip != NULL && portNum != 0)
			break;
		printf("reinsert your ip and port");
	}
	/*printf("Client: creating socket\n");
	if ((sockid = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		printf("Client: socket failed: %d\n", errno); exit(0);
	}
	// bind 
	printf("Client: binding my local socket\n");
	memset((char *)&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_port = htons(portNum + 100);

	if ((bind(sockid, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0)) {
		printf("Client: bind fail: %d\n", errno); exit(0);
	}
	if ((hostp = gethostbyname(ip)) == 0) {
		fprintf(stderr, "%s: unknown host\n", ip);
		exit(1);
	}

	printf("Client: creating addr structure for server\n");
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	memcpy((void *)&server_addr.sin_addr, hostp->h_addr, hostp->h_length);

	server_addr.sin_port = htons((u_short)portNum);
*/
printf("Client: creating socket\n");
	if ((sockid = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		printf("Client: socket failed: %d\n", errno); 			exit(0);
	}

	//printf("Client: binding my local socket\n");


	if ((hostp = gethostbyname(ip)) == 0) {
		fprintf(stderr, "%d: unknown host\n", portNum);
		exit(1);
	}


	memset((void *)&server_addr,0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	memcpy((void *)&server_addr.sin_addr, hostp->h_addr, hostp->h_length);

	server_addr.sin_port = htons((u_short)portNum);

	if(connect(sockid,(struct sockaddr*)&server_addr,sizeof(server_addr))<0){
		(void)close(sockid);
		fprintf(stderr,"connect failed.\n");
		exit(1);
	}

	for (;;)
	{
		fflush(stdin);
		printf("\n>");
		//  gets_s(msg,sizeof(msg));
		getLine(input, 20);

		fflush(stdin);
		temp = strtok(input, " ");
		while (temp != NULL)
		{
			if (i == 0)
			{
				strcpy(fileName, temp); temp = strtok(NULL, " ");
			}
			else
			{
				strcpy(fileName, temp); temp = strtok(NULL, " ");
			}

		}
		i = 0;

		// input or get
		strcpy(msg, input);
		retcode = send(sockid, msg, 12, 0);
		if (retcode <= -1) {
			printf("client: sendto failed: %d\n", errno); exit(0);
		}
		
		//credit
		if(strcmp(input, "credit") == 0)
		{
		
			if(strcmp(fileName, "20133267") == 0)
			{
				printf("20133267 jin won designed get part and credit part.");
			}
			
			ID=atoi(fileName);
			switch(ID){
			case 20123374:credit20123374();break;
			}
		}



		//input (input)
		//fileName 

		if (strcmp(input, "put") == 0)
		{
			//////// put ///////////////////////////////////////////
			strcpy(msg, fileName);
			retcode = send(sockid, msg, 12, 0);
			if (retcode <= -1) {
				printf("client: sendto failed: %d\n", errno); exit(0);
			}
			//file open 
			if ((stream = fopen(fileName, "r")) == NULL) { // argv[3]의 파일을 open
				printf("Error");
				exit(1);
			} //

			// file size 
			fseek(stream, 0, SEEK_END);
			fileSize = ftell(stream);//
			fseek(stream, 0, SEEK_SET);
			// filesize send
			sprintf(msg, "%d", fileSize);
			retcode = send(sockid, msg, 12, 0);
			if (retcode <= -1) {
				printf("client: sendto failed: %d\n", errno); exit(0);
			} // filesize send



			printf("%s size: (%.2lfMB) is being sent!\n", fileName, fileSize / (1024 * 1024.0));

			// file transfer
			while (!feof(stream))
			{

				msg[0] = '\0';
				//fgets(msg,sizeof(msg), stream);  // read buffer
				numfread = fread(msg, 1, 100, stream);

				retcode = send(sockid, msg, numfread, 0);
				currentSize += retcode;
				percent = ((double)currentSize / fileSize) * 100;

				if (percent / 10<1)
				{
					printf("[          ]\r");
				}
				else if (percent / 10<2)
				{
					printf("[*         ]\r");
				}
				else if (percent / 10<3)
				{
					printf("[**        ]\r");
				}
				else if (percent / 10<4)
				{
					printf("[***       ]\r");
				}
				else if (percent / 10<5)
				{
					printf("[****      ]\r");
				}
				else if (percent / 10<6)
				{
					printf("[*****     ]\r");
				}
				else if (percent / 10<7)
				{
					printf("[******    ]\r");
				}
				else if (percent / 10<8)
				{
					printf("[*******   ]\r");
				}
				else if (percent / 10<9)
				{
					printf("[********  ]\r");
				}
				else if (percent / 10<10)
				{
					printf("[********* ]\r");
				}
				else
				{
					printf("[**********]\r");
				}

			}
			fclose(stream);
			// end file
			strcpy(msg, "end");
			retcode = send(sockid, msg, 100, 0);

		}// end of strcmp

		if (strcmp(input, "get") == 0)
		{
			// get file//////////////
			msg[0] = '\0';
			strcpy(msg, fileName);
			// 1) send file name
			retcode = send(sockid, msg, 12, 0);
			// 2) file open
			if ((stream2 = fopen(fileName, "w")) == 0)
			{
				printf("error");
				exit(1);
			}
			// file size receive
			nread = recv(sockid, msg,12, 0);
			printf("Server: return code from read is %d\n", nread);

			if (nread >0) { printf("Server: message is: %.11s\n", msg); }
			msg[nread]='\0';

			fileSize = atoi(msg);

			// file size receive


			// 3) file receive

			currentSize = 0;


			printf("%s size: (%.2lfMB) is being received!\n", fileName, fileSize / (1024 * 1024.0));
			while (1)
			{
				msg[0] = '\0';
				nread = write(sockid,msg,100);
					printf("%s\n",msg);
				printf("%s\n",msg);
				printf("%s\n",msg);
				printf("%s\n",msg);
							
				/*  if(nread<0)
				{
				perror("receive fail");
				exit(1);
				}*/
				currentSize += nread;
				percent = ((double)currentSize / fileSize) * 100;
				if (percent / 10<1)
				{
					printf("[          ]\r");
				}
				else if (percent / 10<2)
				{
					printf("[*         ]\r");
				}
				else if (percent / 10<3)
				{
					printf("[**        ]\r");
				}
				else if (percent / 10<4)
				{
					printf("[***       ]\r");
				}
				else if (percent / 10<5)
				{
					printf("[****      ]\r");
				}
				else if (percent / 10<6)
				{
					printf("[*****     ]\r");
				}
				else if (percent / 10<7)
				{
					printf("[******    ]\r");
				}
				else if (percent / 10<8)
				{
					printf("[*******   ]\r");
				}
				else if (percent / 10<9)
				{
					printf("[********  ]\r");
				}
				else if (percent / 10<10)
				{
					printf("[********* ]\r");
				}
				else if (percent / 10==10)
				{
					printf("[**********]\r");
				}
				if (percent / 10 > 10)
				{
					fclose(stream2);
					break;
				}
				
				//if(!strncmp(msg, "end",12))
				//if(!strcmp(msg, "end"))
				/*if(nread==0)
				{
					fclose(stream2);
					break;
				}*/
				
				else{
					//	fputs(msg, stream2); //file save
					fwrite(msg, 1, nread, stream2);
				}

			}
			msg[nread]='\0';
			printf("\nClient: Successful\n");
		}// end of strcmp



		if (strcmp(input, "close") == 0)
		{
			close(sockid);
			printf("Disconnected.\n");
			break;
		}//}
		/* close socket */

	
	}// end of loop

	// quit 
	for (;;)
	{
		printf(">");
		scanf("%s", input);
		if (strcmp(input, "quit") == 0)
			break;
	}


}
