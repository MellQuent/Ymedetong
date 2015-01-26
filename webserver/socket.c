#include "socket.h"


int creer_serveur (int port){
	/*Variables*/
	int socket_serveur ;
	int socket_client;
	struct sockaddr_in saddr ;
	const char * message_bienvenue = "Bonjour , bienvenue sur mon serveur \n" ;

	/*Code*/
	/*Création de socket serveur*/
	socket_serveur = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_serveur == -1){
		perror ("socket_serveur");
		exit(-1);
	}

	saddr.sin_family = AF_INET ; /* Socket ipv4 */
	saddr.sin_port = htons(port); /* Port d ’ écoute */
	saddr.sin_addr.s_addr = INADDR_ANY ; /* écoute sur toutes les interfaces */
	if ( bind ( socket_serveur, (struct sockaddr *)& saddr, sizeof(saddr)) == -1){
		perror ( " bind socker_serveur " );
		exit(-1);
	}

	if (listen(socket_serveur, 10) == -1){
		perror ( " listen socket_serveur " );
	}

	socket_client = accept ( socket_serveur , NULL , NULL );
	if ( socket_client == -1){
		perror ( " accept " );
		/* traitement d ’ erreur */
	}

	/* On peut maintenant dialoguer avec le client */
	write (socket_client, message_bienvenue, strlen(message_bienvenue));
	return 0;
}
	
	