#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>



int main(int argc, char **argv){
	int sockfd, newfd, filefd; // socekt file descriptor, new file descriptor
	socklen_t length;  // socket length (length of clinet address)
	struct sockaddr_in saddr, caddr; // server address, client address
	//char *tstr; // var holding the string to be send via TCP
	int recieve, dolzinaImenaDatoteke, dolzinaVsebineDatoteke, stevilkaNapake;// velikostImenaDatoteke; //number of bytes recieved
	//char *vsebinaDatoteke;
	char buf[1024];
	char message[1024];

	if(argc < 2 ){
		printf("Napačno število argumentov!\n");
		exit(1);
	}
	// create socket
	if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
	}

	saddr.sin_family = AF_INET; // IPv4 
	saddr.sin_addr.s_addr = INADDR_ANY; // localhost
	saddr.sin_port = htons(atoi(argv[1])); // port converted from ascii to integer

    // binds the socket file description to the actual port (similar to open)
	if (bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
		perror("bind");
	}

	// start listening on the given port
	if(listen(sockfd, 0) < 0) {
		perror("listen");
	}

	length = sizeof(caddr); // length of client address

	while(1) {

		// accept new client (wait for him!)
		//perror("start waiting...");
		if((newfd = accept(sockfd, (struct sockaddr *)&caddr, &length)) < 0) {
			perror("accept");
		}

		//prejemanje dolzine datoteke
		if((recieve = recv(newfd, &dolzinaImenaDatoteke, sizeof(dolzinaImenaDatoteke), 0)) == -1){
			perror("recv");
		}

		printf("Prejemam ime datoteke... \n");
		//prejemanje imenmkdira datoteke
		if((recieve = recv(newfd, buf, dolzinaImenaDatoteke, 0)) == -1){
			strcpy(message, "Napaka pri prejemanju imena datoteke!");
			write(1, message, strlen(message));
			perror("recv");
		}
		
		/* buggy preverjanje prejetja imena sporočila
		if(recieve != dolzinaImenaDatoteke){
			stevilkaNapake = -1;
			strcpy(message, "Ime prejeto kao...\n");
			write(1, message, strlen(message));
			send(sockfd, &stevilkaNapake, sizeof(stevilkaNapake), 0);
			close(newfd);
			continue;
		}else{
			
			stevilkaNapake = 1;
			send(sockfd, &stevilkaNapake, 4, 0);
			printf("dolzinaImenaDatoteke: %i, recieve: %i\n", dolzinaImenaDatoteke, recieve);
			strcpy(message, "Ime prejeto kao...\n");
			write(1, message, strlen(message));
		}*/
		
		filefd = open(buf, O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IRGRP|S_IRUSR);

		//prejemanje dolzine vsebine datoteke
		if((recieve = recv(newfd, &dolzinaVsebineDatoteke, sizeof(dolzinaVsebineDatoteke), 0)) == -1){
			perror("recv");
		}

		//prejemanje vsebine datoteke
		if((recieve = recv(newfd, buf, dolzinaVsebineDatoteke, 0)) == -1){
			perror("recv");
		}

		// printf("dolzinaImenaDatoteke: %i, recieve: %i\n", dolzinaImenaDatoteke, recieve);
		if(recieve != dolzinaVsebineDatoteke){
			printf("Napaka pri pošiljanju vsebine datoteke\n");
		}
		
		/* buggy preverjanje prejetega sporočila
		if(recieve != dolzinaImenaDatoteke){
			stevilkaNapake = -2;
			send(sockfd, &stevilkaNapake, 4, 0);
			printf("Datoteka neuspešno prejeta. Ponovni poskus!\n");
			close(newfd);
			close(filefd);
			continue;
		}else{
			stevilkaNapake = 2;
			printf("Datoteka prejeta\n");
			send(sockfd, &stevilkaNapake, 4, 0);
		}
		*/

		write(filefd, buf, dolzinaVsebineDatoteke);


		close(newfd); // close socket
		close(filefd);
		
		break;
	}

	close(sockfd);
	
	return 0;

}