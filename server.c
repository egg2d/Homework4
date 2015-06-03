#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <time.h> 
int main(int argc, char *argv[])
{
	int i = 0;
	int sockid, nread, addrlen, retcode;
	int fileSize;
	char fileName[20];
	char buf[12];
	FILE *stream;
	struct sockaddr_in my_addr, client_addr, server_addr;
	char msg[100];
	double percent;
	int currentSize = 0;
	int second = 0;
	int mb;
	int start = 0;
	int end = 0;
	char input[50];
	int numfwrite;
	int numfread;
	if (argc != 2) {
		printf("%s port\n", argv[0]);
		return 0;
	}

	printf("Server: creating socket\n");
	if ((sockid = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		printf("Server: socket error: %d\n", errno); exit(0);
	}

	printf("Server: binding my local socket\n");
	memset((char *)&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = htons(INADDR_ANY);
	my_addr.sin_port = htons(atoi(argv[1]));

	if ((bind(sockid, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0)) {
		printf("Server: bind fail: %d\n", errno); exit(0);
	}

	printf("Server: starting blocking message read\n");




	while (1)
	{

		// put or get
		nread = recvfrom(sockid, msg, 12, 0, (struct sockaddr *) &client_addr, (socklen_t*)&addrlen);
		strcpy(input, msg);



		if (strcmp(input, "put") == 0)
		{
			// file name recevie
			nread = recvfrom(sockid, msg, 12, 0, (struct sockaddr *) &client_addr, (socklen_t*)&addrlen);

			printf("Server: return code from read is %d\n", nread);
			if (nread >0) {
				printf("Server: message is: %.11s\n", msg);
			} // file name
			strcpy(fileName, msg);


			// file size receive
			nread = recvfrom(sockid, msg, 100, 0, (struct sockaddr *) &client_addr, (socklen_t*)&addrlen);
			printf("Server: return code from read is %d\n", nread);
			if (nread >0) {
				printf("Server: message is: %.11s\n", msg);
			}
			fileSize = atoi(msg);
			// file size receive

			if ((stream = fopen(fileName, "w")) == 0)
			{
				printf("error");
				exit(1);
			}
			fseek(stream, 0, SEEK_SET);
			currentSize = 0;

			while (1)
			{
				start = clock();

				nread = recvfrom(sockid, msg, 100, 0, (struct sockaddr *) &client_addr, (socklen_t*)&addrlen);

				if (nread<0)
				{
					perror("receive fail");
					exit(1);
				}


				if (!strncmp(msg, "end", 12))
				{
					fclose(stream);
					break;
				}
				else{
					//fputs(msg, stream); //file save
					numfwrite = fwrite(msg, 1, nread, stream);
				}
				currentSize += numfwrite;
				percent = ((double)currentSize / fileSize) * 100;
				if (end >= 110000)// approximately one second. 240000->110000
				{
					end = 0;
					printf("Transfer status : recv[%s]", fileName);
					printf("[%.2lf%%, %.2lfMB/%.2lfMB]\n", percent, (double)currentSize / (1024 * 1024), (double)fileSize / (1024 * 1024));

				}
				end += clock() - start;
				//second++; 

			}
			// get file//////////////
			start = 0, end = 0;
			printf("Transfer status : recv[%s]", fileName);
			printf("[%.2lf%%, %.2lfMB/%.2lfMB]\n", percent, (double)currentSize / (1024 * 1024), (double)fileSize / (1024 * 1024));



		} //  end of strcmp


		if (strcmp(input, "get") == 0)
		{
			// 1) receive file name 
			nread = recvfrom(sockid, msg, 100, 0, (struct sockaddr *) &client_addr, (socklen_t*)&addrlen);

			strcpy(fileName, msg);
			// 2) file open
			if ((stream = fopen(fileName, "r")) == NULL) { // argv[3]ÀÇ ÆÄÀÏÀ» open
				printf("Error no file!\n");
				exit(1);
			} //



			// file size 
			fseek(stream, 0, SEEK_END);
			fileSize = ftell(stream);//
			fseek(stream, 0, SEEK_SET);
			// filesize send
			sprintf(msg, "%d", fileSize);
			retcode = sendto(sockid, msg, 12, 0, (struct sockaddr *) &client_addr, sizeof(client_addr)); 			if (retcode <= -1)
			{
				printf("client: sendto failed: %d\n", errno); exit(0);
			} // filesize send

			// 3) file reading
			// file transfer
			currentSize = 0;
			while (!feof(stream))
			{
				start = clock();

				msg[0] = '\0';
				//fgets(msg,sizeof(msg), stream);  // read buffer
				numfread = fread(msg, 1, 100, stream);
				retcode = sendto(sockid, msg, numfread, 0, (struct sockaddr *) &client_addr, sizeof(client_addr));

				currentSize += retcode;
				percent = ((double)currentSize / fileSize) * 100;
				if (end >= 110000)// approximately one second. 240000->110000
				{
					end = 0;
					printf("Transfer status : send[%s]", fileName);
					printf("[%.2lf%%, %.2lfMB/%.2lfMB]\n", percent, (double)currentSize / (1024 * 1024), (double)fileSize / (1024 * 1024));

				}
				end += (clock() - start);

			}
			fclose(stream);
			// end file
			strcpy(msg, "end");
			retcode = sendto(sockid, msg, 100, 0, (struct sockaddr *) &client_addr, sizeof(client_addr));
			start = 0, end = 0;
			printf("Transfer status : send[%s]", fileName);
			printf("[%.2lf%%, %.2lfMB/%.2lfMB]\n", percent, (double)currentSize / (1024 * 1024), (double)fileSize / (1024 * 1024));
		} // end of strcmp

		if (strcmp(input, "close") == 0)
		{
			sleep(2);
			close(sockid);
			break;
		}


	} // end of loop


}



