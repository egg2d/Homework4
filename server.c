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
	//struct sockaddr_in my_addr, client_addr, server_addr;
	struct sockaddr_in server, remote;
	int request_sock, new_sock;
	char msg[1024];
	double percent;
	int currentSize = 0;
	int second = 0;
	char msg2[20];
	int mb;
	int start = clock();
	char input[50];
	int numfwrite;
	int numfread;
	int end = 0;
	int recvrate = 10000;
	int speed;

	struct timeval time1, time2, sec1, sec2;
	if (argc != 2) {
		printf("%s port\n", argv[0]);
		return 0;
	}




	for (;;){

		printf("Server: creating socket\n");
		//socket()
		if ((request_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0){
			perror("socket");
			exit(1);
		}

		//bind()
		memset((void*)&server, 0, sizeof(server));
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_port = htons((u_short)atoi(argv[1]));//os\B0\A1 \C0ڵ\BF\C0\FB\C0\B8\B7\CE IP\C1ּҸ\A6 ã\BEƼ\AD ó\B8\AE\C7\D4.

		if (bind(request_sock, (struct sockaddr*)&server, sizeof(server))<0){
			perror("bind");
			exit(1);
		}

		//listen()
		if (listen(request_sock, SOMAXCONN)<0){
			perror("listen");
			exit(1);

		}	printf("I'm waiting a client.\n");
		//accept()



		addrlen = sizeof(remote);
		new_sock = accept(request_sock, (struct sockaddr*)&remote, &addrlen);
		if (new_sock < 0){
			perror("accpet");
			exit(1);
		}
		printf("New connection from host %s,port %d,socket %d\n",
			inet_ntoa(remote.sin_addr), ntohs(remote.sin_port),
			new_sock);

		while (1)
		{

			// put or get
			//nread = recvfrom(sockid, msg, 12, 0, (struct sockaddr *) &client_addr, (socklen_t*)&addrlen);

			nread = recv(new_sock, msg, 12, 0);
			strcpy(input, msg);


			if (strcmp(input, "recvrate") == 0)
			{
				nread = recv(new_sock, msg2, 12, 0);
				nread = recv(new_sock, msg2, 12, 0);
				recvrate = atoi(msg2);
				printf("recvrate %d setting!\n", recvrate);

			}

			if (strcmp(input, "put") == 0)
			{
				// file name recevie
				//nread = recvfrom(sockid, msg, 12, 0, (struct sockaddr *) &client_addr, (socklen_t*)&addrlen);

				nread = recv(new_sock, msg, 12, 0);
				msg[nread] = '\0';

				printf("Server: return code from read is %d\n", nread);
				if (nread > 0) {
					printf("Server: message is: %.11s\n", msg);
				} // file name
				strcpy(fileName, msg);


				// file size receive
				//nread = recvfrom(sockid, msg, 100, 0, (struct sockaddr *) &client_addr, (socklen_t*)&addrlen);
				nread = recv(new_sock, msg, 12, 0);
				printf("Server: return code from read is %d\n", nread);
				msg[nread] = '\0';
				fileSize = atoi(msg);
				// file size receive

				if ((stream = fopen(fileName, "w")) == 0)
				{
					printf("error");
					exit(1);
				}
				fseek(stream, 0, SEEK_SET);
				currentSize = 0;


				gettimeofday(&sec1, NULL);
				while (1)
				{
					//nread = recvfrom(sockid, msg, 100, 0, (struct sockaddr *) &client_addr, (socklen_t*)&addrlen);
					start = clock();
					nread = recv(new_sock, msg, 12, 0);

					if (nread < 0)
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
					gettimeofday(&sec2, NULL);

					if (sec2.tv_sec - sec1.tv_sec >= 1)
					{
						printf("Transfer status : send[%s]", fileName);
						printf("[%.2lf%%, %.2lfMB/%.2lfMB]\n", percent, (double)currentSize / (1024 * 1024), (double)fileSize / (1024 * 1024));
						sec1.tv_sec = sec2.tv_sec;
					}

					/*if (end >= 240000)// approximately one second.
					{
					//second++;
					end = 0;
					printf("Transfer status : recv[%s]", fileName);
					printf("[%.2lf%%, %.2lfMB/%.2lfMB]\n", percent, (double)currentSize / (1024 * 1024), (double)fileSize / (1024 * 1024));


					//start = (int)clock();
					}*/
					end += clock() - start;
					//second++; 

				}
				// get file//////////////
				start = 0, end = 0;
				msg[nread] = '\0';

				printf("Transfer status : recv[%s]", fileName);
				printf("[%.2lf%%, %.2lfMB/%.2lfMB]\n", percent, (double)currentSize / (1024 * 1024), (double)fileSize / (1024 * 1024));



			} //  end of strcmp


			if (strcmp(input, "get") == 0)
			{
				// 1) receive file name 
				//nread = recvfrom(sockid, msg, 100, 0, (struct sockaddr *) &client_addr, (socklen_t*)&addrlen);
				nread = recv(new_sock, msg, 12, 0);
				msg[nread] = '\0';

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
				//retcode = sendto(sockid, msg, 12, 0, (struct sockaddr *) &client_addr, sizeof(client_addr));
				retcode = send(new_sock, msg, 12, 0);

				if (retcode <= -1)
				{
					printf("client: sendto failed: %d\n", errno); exit(0);
				} // filesize send

				// 3) file reading
				// file transfer
				currentSize = 0;
				second = 0;
				gettimeofday(&sec1, NULL);
				while (!feof(stream))
				{
					start = clock();
					msg[0] = '\0';
					//fgets(msg,sizeof(msg), stream);  // read buffer
					numfread = fread(msg, 1, 1024, stream);
					//retcode = sendto(sockid, msg, numfread, 0, (struct sockaddr *) &client_addr, sizeof(client_addr));
					gettimeofday(&time1, NULL);
					retcode = write(new_sock, msg, numfread);
					gettimeofday(&time2, NULL);

					currentSize += retcode;
					percent = ((double)currentSize / fileSize) * 100;
					gettimeofday(&sec2, NULL);
					if (sec2.tv_sec - sec1.tv_sec >= 1)
					{
						printf("Transfer status : send[%s]", fileName);
						printf("[%.2lf%%, %.2lfMB/%.2lfMB]\n", percent, (double)currentSize / (1024 * 1024), (double)fileSize / (1024 * 1024));
						sec1.tv_sec = sec2.tv_sec;
					}

					/*	if (end >= 240000)// approximately one second.
					{
					//second++;
					end = 0;
					printf("Transfer status : send[%s]", fileName);
					printf("[%.2lf%%, %.2lfMB/%.2lfMB]\n", percent, (double)currentSize / (1024 * 1024), (double)fileSize / (1024 * 1024));
					//start = (int)clock();
					}*/

					if ((1000000 / recvrate) - abs(time2.tv_usec - time1.tv_usec) >= 0)
					{
						speed = ((1000000 / recvrate) - abs(time2.tv_usec - time1.tv_usec));
						usleep(abs((speed * 71) / 86));
					}
					end += (clock() - start);
				}
				fclose(stream);
				// end file
				//strcpy(msg, "end");
				//retcode = sendto(sockid, msg, 100, 0, (struct sockaddr *) &client_addr, sizeof(client_addr));
				//	retcode = send(new_sock, msg, 100, 0);
				//retcode = write(new_sock, msg, 4);
				start = 0, end = 0;
				printf("Transfer status : send[%s]", fileName);
				printf("[%.2lf%%, %.2lfMB/%.2lfMB]\n", percent, (double)currentSize / (1024 * 1024), (double)fileSize / (1024 * 1024));
			} // end of strcmp

			if (strcmp(input, "close") == 0)
			{

				break;
			}


		} // end of loop

		if (strcmp(input, "close") == 0)
		{
			printf("server end!\n");
			break;
		}
	}
	printf("close is oK\n");
}