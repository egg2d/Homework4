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
	char portch[20];
	int numfread;
	int sockid, retcode, addrlen;
	int nread = 0;
	struct hostent *hostp;
	struct sockaddr_in my_addr, server_addr, client_addr;
	char msg[1024];
	char input[50];
	double percent;
	int currentSize = 0;
	int second = 0;
	int progressbar;
	int ID = 0;
	int sendrate = 10000;
	int recvrate = 10000;
	int speed;
	int connectflag = 0;
	struct timeval time1, time2;


	for (;;)
	{
		fflush(stdin);
		printf(">client: ");
		//  gets_s(msg,sizeof(msg));
		getLine(input, 40);
		i = 0;
		fflush(stdin);
		temp = strtok(input, " ");
		while (temp != NULL)
		{
			if (i == 0)
			{
				strcpy(input, temp); temp = strtok(NULL, " ");
			}
			else if (i == 1)
			{
				strcpy(fileName, temp); temp = strtok(NULL, " ");

			}
			else if (i == 2)
			{

				strcpy(portch, temp); temp = strtok(NULL, " ");

			}
			i++;
		}
		i = 0;

		if (strcmp(input, "connect") == 0)
		{

			portNum = 0;
			strcpy(ip, fileName);
			portNum = atoi(portch);


			printf("Client: creating socket\n");
			if ((sockid = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
				printf("Client: socket failed: %d\n", errno); 			exit(0);
			}

			//printf("Client: binding my local socket\n");


			if ((hostp = gethostbyname(ip)) == 0) {
				fprintf(stderr, "%d: unknown host\n", portNum);
				exit(1);
			}


			memset((void *)&server_addr, 0, sizeof(server_addr));
			server_addr.sin_family = AF_INET;
			server_addr.sin_addr.s_addr = inet_addr(ip);
			memcpy((void *)&server_addr.sin_addr, hostp->h_addr, hostp->h_length);

			server_addr.sin_port = htons((u_short)portNum);

			if (connect(sockid, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
				(void)close(sockid);
				fprintf(stderr, "connect failed.\n");
				exit(1);
			}
			connectflag = 1;
		}



		if (connectflag == 1)
		{
			// input or get
			strcpy(msg, input);
			retcode = send(sockid, msg, 12, 0);
			if (retcode <= -1) {
				printf("client: sendto failed: %d\n", errno); exit(0);
			}

			//credit
			if (strcmp(input, "credit") == 0)
			{

				if (strcmp(fileName, "20133267") == 0)
				{
					printf("20133267 jinwon designed get part, credit part and participated in writing the report.");
				}

				else if (strcmp(fileName, "20123374") == 0)
				{
					printf("20123374 yeonjin designed credit part and some part of changing tcp connection.");
				}

				else if (strcmp(fileName, "20090399") == 0)
				{
					printf("20090399 kyungbin designed server and many part of client.");
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
					numfread = fread(msg, 1, 1024, stream);
					gettimeofday(&time1, NULL);
					retcode = send(sockid, msg, numfread, 0);
					gettimeofday(&time2, NULL);
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
					else{
						printf("[**********]\r");
					}
					printf("\n");

					if ((1000000 / sendrate) - abs(time2.tv_usec - time1.tv_usec) >= 0)
					{
						speed = ((1000000 / sendrate) - abs(time2.tv_usec - time1.tv_usec));
						usleep(abs((speed * 71) / 86));
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
				nread = recv(sockid, msg, 12, 0);
				msg[nread] = '\0';

				fileSize = atoi(msg);

				// file size receive


				// 3) file receive

				currentSize = 0;


				printf("%s size: (%.2lfMB) is being received!\n", fileName, fileSize / (1024 * 1024.0));
				while (1)
				{
					msg[0] = '\0';
					nread = read(sockid, msg, 1024);
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
					else if (percent / 10 == 10)
					{
						printf("[**********]\r");
					}
					fwrite(msg, 1, nread, stream2);

					if (currentSize >= fileSize)
					{
						fclose(stream2);
						break;
					}

				}
				msg[nread] = '\0';
				printf("\nClient: Successful\n");
			}// end of strcmp



			if (strcmp(input, "sendrate") == 0)
			{
				sendrate = atoi(fileName);
				printf("sendrate ok!!\n");
			}
			if (strcmp(input, "recvrate") == 0)
			{
				strcpy(msg, input);
				retcode = send(sockid, msg, 12, 0); // msg recvrate

				recvrate = atoi(fileName);
				printf("recvrate ok!!\n");
				sprintf(msg, "%d", recvrate);
				retcode = send(sockid, msg, 12, 0);
				// how much?	

			}
			if (strcmp(input, "ratecurr") == 0)
			{
				printf("sendrate : %d , recvrate : %d\n", sendrate, recvrate);
			}
			if (strcmp(input, "close") == 0)
			{
				printf("Disconnected.\n");
				close(sockid);

				connectflag = 0;

			}//}

		} // connect //							
		/* close socket */

		// quit 
		if (strcmp(input, "quit") == 0)
		{
			break;
		}
	}// end of loop
}
