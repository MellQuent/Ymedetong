#include "socket.h"
//Dernier test pour vérifier le git
void initialiser_signaux(void){
	struct sigaction sa ;
	
sa.sa_handler = traitement_signal ;
	sigemptyset (& sa . sa_mask );
	sa.sa_flags = SA_RESTART ;
	if(sigaction(SIGCHLD,&sa,NULL) == -1){
		perror("sigaction(SIGCHLD)");
	}

	if(signal(SIGPIPE, SIG_IGN) == SIG_ERR){
		perror("signal");
	}


}

void traitement_signal (int sig){
	printf("Signal %d reçu \n" , sig);
	waitpid(-1, NULL, WNOHANG);
}

int creer_serveur (int port){
	/*Variables*/
	int socket_serveur ;
	int socket_client;
	struct sockaddr_in saddr ;
	const char * message_bienvenue = "Bienvenue sur le serveur Ymédétong ! Blblblblblblblb blblblbblb blblblbblb blblblbblb blblblbblb  blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb blblblblbblbl blblblbblb blblblbblb blblblbblb blblblbblb blblblbblb bonjour\n" ;
	char buffer[512];
	//int length;
	int optval = 1;
	int pid;
	FILE *fp;
	char* requete = malloc(sizeof(char)*512);
	char tailleMessage[4];
	char str[128] = "Content-length: ";

	
	/*Création de socket serveur*/
	socket_serveur = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_serveur == -1){
		perror("socket_serveur");
		exit(-1);
	}

	saddr.sin_family = AF_INET ; /* Socket ipv4 */
	saddr.sin_port = htons(port); /* Port d ’ écoute */
	saddr.sin_addr.s_addr = INADDR_ANY ; /* écoute sur toutes les interfaces */
	
	if(setsockopt(socket_serveur, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1){
		perror(" Can not set SO_REUSEADDR option ");
	}
	
	if(bind(socket_serveur,(struct sockaddr *)& saddr,sizeof(saddr)) == -1){
		perror("bind socker_serveur");
		exit(-1);
	}

	if(listen(socket_serveur, 10) == -1){
		perror("listen socket_serveur");
	}
	

	while(1){
		socket_client = accept(socket_serveur,NULL,NULL);
		if(socket_client == -1){
			perror("accept");
			/* traitement d ’ erreur */
		}
	
		pid = fork();
		if(pid<0){
			printf("Error");
			return -1;
		}

		
		initialiser_signaux();

		if(pid==0){
			/* On peut maintenant dialoguer avec le client */
			fp = fdopen(socket_client, "w+");

			while(1){
				if(fgets(buffer, 512, fp)==NULL){
					fclose(fp);
					exit(0);
				}
				if(strcmp(buffer,"\r\n")!=0){
					strcat(requete, buffer);
					
				}else{

					if(verifierRequete(requete)==1){
						fprintf(fp,"%s","HTTP/1.1 400 Bad Request\r\n");
						fprintf(fp,"%s","Connection: close\r\n");
						fprintf(fp,"%s","Content-Length: 17\r\n");
						fprintf(fp,"%s","\r\n");
						fprintf(fp,"%s","400 Bad request\r\n");
					}else{
						printf(requete);
						if(verifierAddress(requete)==0){
							fprintf(fp,"%s","HTTP/1.1 200 OK\r\n");
							sprintf(tailleMessage, "%u", (int)strlen(message_bienvenue));
							strcat(str, tailleMessage);
							strcat(str,"\r\n");
							fprintf(fp,"%s",str);
							fprintf(fp,"\r\n");
							fprintf(fp,message_bienvenue);
						}else{
							fprintf(fp,"%s","HTTP/1.1 404 Not Found\r\n");
							fprintf(fp,"%s","Connection: close\r\n");
							fprintf(fp,"%s","Content-Length: 15\r\n");
							fprintf(fp,"%s","\r\n");
							fprintf(fp,"%s","404 Not found\r\n");
						}

					}
				}
				if(strcmp(buffer, "exit\r\n")==0){
					printf("Le client se déconnecte \n");
					exit(0);
				}
			
			}
		} else {
			close(socket_client);
		}
	}
	
	return 0;

}

//Fonction vraiment super utile pour voir un peu comment les chaines sont construites hein.
void compterChar(char c[], int length){
	int cpt=0;
	for (cpt=0; cpt<length; cpt++){
		printf("%d : %d\n",cpt, c[cpt]);
	}
}

int verifierAddress(char *chaine){
	char* requete = malloc(sizeof(char)*512);
	strcpy(requete, chaine);
	int nbMot=0;
	char *token;
	token = strtok(requete, " ");
	while(token!=NULL){
		if(nbMot==1 && strcmp(token,"/")==0){
			return 0;
		}
		nbMot++;
		token = strtok(NULL, " ");
	}
	return 1;
}

int verifierRequete(char *chaine){
	char* requete = malloc(sizeof(char)*512);
	strcpy(requete, chaine);
	int nbMot=0;
	char *token;
	char *phrase;
	int get=1;
	int http=1;
	phrase=strtok(requete,"\r\n");
	token = strtok(phrase, " ");
	while(token!=NULL){
		//Verification des 
		if(nbMot==0 && strcmp(token,"GET")==0){
			get=0;
		}
		if(nbMot==2 && (strcmp(token,"HTTP/1.0")==0 || (strcmp(token,"HTTP/1.1")==0))){
			http=0;
		}
		nbMot++;
		token = strtok(NULL, " ");
	}
	if(get==0 && http==0){
		return 0;
	}else{
		return 1;
	}
}
