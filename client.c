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

#define MAXDATASIZE 1024 //število prebranih zlogov

int main(int argc, char **argv){

	int sockfd, numbytes = 1024;  // socekt file descriptor, new file descriptor
	char buf[MAXDATASIZE]; // array holding the string to be received via TCP
	struct hostent *he;    // pointer to the structure hostent (returned by gethostbyname) 
	struct sockaddr_in their_addr; // server address
	int fd, velikostVsebineDatoteke, dolzinaImenaDatoteke, preveriCeJePrispelo;
	//char *tstr;
	
	if (argc != 4) {
		write(0,"Nepravilno št. argumentov", 29);
		exit(1);
	}
	// get the server IP address
	if ((he = gethostbyname(argv[2])) == NULL) { 
		herror("gethostbyname");  // prints string + value of h_error variable [HOST_NOT_FOUND || NO_ADDRESS or NO_DATA || NO_RECOVERY || TRY_AGAIN]
		exit(1);		  
	}
	
	// create socket
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	
	their_addr.sin_family = AF_INET; // family to Address Family InterNET
	their_addr.sin_port = htons(atoi(argv[3])); // get server's port number - should be checked for input errors (not a number, etc.)
 	their_addr.sin_addr = *((struct in_addr *)he->h_addr); // server's IP address from hostent structure pointed to by he variable...
	memset(&(their_addr.sin_zero), '\0', 8); // for conversion between structures only, a trick to ensure pointer casting...
	
	fd = open(argv[1], O_RDONLY);
	if(fd == -1){
		perror("Neuspešno odpiranje datoteke!");
		exit(1);
	}
	
	// as long there is soething to read...
	while(numbytes != 0) {
		preveriCeJePrispelo = 0;
		

		//
		velikostVsebineDatoteke = read(fd, buf, sizeof(buf));
		dolzinaImenaDatoteke = strlen(argv[1]);

		// connect to the server... (no bind necessary as we are connecting to remote host... Kernel will find a free port for us and will bind it to sockfd)
		if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
			perror("connect");
			exit(1);
		}

		//pošljemo dolžino imena datoteke
		numbytes = send(sockfd, &dolzinaImenaDatoteke, sizeof(dolzinaImenaDatoteke), 0);
		// printf("dolzinaImenaDatoteke: %i, numbytes: %li", dolzinaImenaDatoteke, sizeof(dolzinaImenaDatoteke));
		
		//pošljemo ime datoteke
		numbytes = send(sockfd, argv[1], dolzinaImenaDatoteke, 0);
		/*
		// preverjanje če je prispelo ime 
		if((recv(sockfd, &preveriCeJePrispelo, sizeof(preveriCeJePrispelo), 0)) == -1){
			perror("recv");
		}
		if(preveriCeJePrispelo == -1){
			printf("Nismo uspešno prebrali imena. Ponovni poskus!");
			continue;
		}*/

		//pošljemo dolžino vsebine datoteke
		numbytes = send(sockfd, &velikostVsebineDatoteke, sizeof(velikostVsebineDatoteke), 0);
		
		//pošljemo vsebino datoteke
		numbytes = send(sockfd, buf, velikostVsebineDatoteke, 0);
		
		//preverjanje če je prispela datoteka v celoti
		if((recv(sockfd, &preveriCeJePrispelo, 4, 0)) == -1){
			perror("recv");
		}
		if(preveriCeJePrispelo == -2){
			printf("Nismo uspešno prebrali imena. Ponovni poskus!");
			continue;
		}else{
			printf("prispelo: %i", preveriCeJePrispelo);
		}
		
		
		break;
	}
	
	// close socket
	close(sockfd);
	
	return 0;
}
