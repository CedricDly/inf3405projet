#undef UNICODE

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

// Link avec ws2_32.lib
#pragma comment(lib, "ws2_32.lib")
using namespace std;


//Fonction de test de validite d'une adresse IP
bool testIP(string ipTest) {
	int nbpoints = 0;
	vector<int> indicesPts;
	for(int i = 0; i < ipTest.size(); i++) {
		if (ipTest[i] == '.') {
			nbpoints++;
			indicesPts.push_back(i);
		}
	}
	if (nbpoints != 3)
		return false;

	int octet1 = stoi(ipTest.substr(0, indicesPts[0]));
	int octet2 = stoi(ipTest.substr(indicesPts[0] + 1, indicesPts[1] - indicesPts[0] - 1));
	int octet3 = stoi(ipTest.substr(indicesPts[1] + 1, indicesPts[2] - indicesPts[1] - 1));
	int octet4 = stoi(ipTest.substr(indicesPts[2] + 1, ipTest.size() - indicesPts[2] - 1));

	if (0 <= octet1 && octet1 <= 255 && 0 <= octet2 && octet2 <= 255 && 0 <= octet3 && octet3 <= 255 && 0 <= octet4 && octet4 <= 255)
		return true;
	else
		return false;

}

//Fonction de test de validite d'un port
bool testPort(const string& port) {
	int portBis = stoi(port);
	if (portBis > 0)
		return true;
	else
		return false;
	}

int __cdecl main(int argc, char **argv)
{
    WSADATA wsaData;
    SOCKET leSocket;// = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    char motEnvoye[10];
	char motRecu[10];
    int iResult;

	//--------------------------------------------
    // InitialisATION de Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("Erreur de WSAStartup: %d\n", iResult);
        return 1;
    }
	// On va creer le socket pour communiquer avec le serveur
	leSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (leSocket == INVALID_SOCKET) {
        printf("Erreur de socket(): %ld\n\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
		printf("Appuyez une touche pour finir\n");
		getchar();
        return 1;
	}
	//--------------------------------------------
	// On va chercher l'adresse du serveur en utilisant la fonction getaddrinfo.
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_INET;        // Famille d'adresses
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;  // Protocole utilis� par le serveur

	// On indique le nom et le port du serveur auquel on veut se connecter
	//char *host = "L4708-XX";
	//char *host = "L4708-XX.lerb.polymtl.ca";
	//char *host = "add_IP locale";

	string hostVoulu;
	cout << "Veuillez entrer l'adresse du serveur que vous voulez joindre: ";
	cin >> hostVoulu;

	while(testIP(hostVoulu)==false) {
		cout << "l'adresse entr�e en param�tres est incorrecte" << endl;
		cout << "Veuillez entrer l'adresse du serveur que vous voulez joindre: ";
		cin >> hostVoulu;
	}

	const char *host = hostVoulu.c_str();

	string portVoulu;
	cout << "Veuillez entrer le port du serveur auquel vous voulez vous connecter : ";
	cin >> portVoulu;

	while (testPort(portVoulu) == false) {
		cout << "le port entr� en param�tres est incorrect" << endl;
		cout << "Veuillez entrer le port du serveur auquel vous voulez vous connecter : ";
		cin >> portVoulu;
	}

	const char *port = portVoulu.c_str();
	
	// getaddrinfo obtient l'adresse IP du host donn�
    iResult = getaddrinfo(host, port, &hints, &result);
    if ( iResult != 0 ) {
        printf("Erreur de getaddrinfo: %d\n", iResult);
        WSACleanup();
        return 1;
    }
	//---------------------------------------------------------------------		
	//On parcours les adresses retournees jusqu'a trouver la premiere adresse IPV4
	while((result != NULL) &&(result->ai_family!=AF_INET))   
			 result = result->ai_next; 

//	if ((result != NULL) &&(result->ai_family==AF_INET)) result = result->ai_next;  
	
	//-----------------------------------------
	if (((result == NULL) ||(result->ai_family!=AF_INET))) {
		freeaddrinfo(result);
		printf("Impossible de recuperer la bonne adresse\n\n");
        WSACleanup();
		printf("Appuyez une touche pour finir\n");
		getchar();
        return 1;
	}

	sockaddr_in *adresse;
	adresse=(struct sockaddr_in *) result->ai_addr;
	//----------------------------------------------------
	printf("Adresse trouvee pour le serveur %s : %s\n\n", host,inet_ntoa(adresse->sin_addr));
	printf("Tentative de connexion au serveur %s avec le port %s\n\n", inet_ntoa(adresse->sin_addr),port);
	
	// On va se connecter au serveur en utilisant l'adresse qui se trouve dans
	// la variable result.
	iResult = connect(leSocket, result->ai_addr, (int)(result->ai_addrlen));
	if (iResult == SOCKET_ERROR) {
        printf("Impossible de se connecter au serveur %s sur le port %s\n\n", inet_ntoa(adresse->sin_addr),port);
        freeaddrinfo(result);
        WSACleanup();
		printf("Appuyez une touche pour finir\n");
		getchar();
        return 1;
	}

	printf("Connecte au serveur %s:%s\n\n", host, port);
    freeaddrinfo(result);

	//----------------------------
	// Envoi des messages au serveur
	printf("Saisir un mot de 7 lettres pour envoyer au serveur: ");
	gets_s(motEnvoye);

	//-----------------------------
    // Envoyer le mot au serveur
    iResult = send(leSocket, motEnvoye, 7, 0 );
    if (iResult == SOCKET_ERROR) {
        printf("Erreur du send: %d\n", WSAGetLastError());
        closesocket(leSocket);
        WSACleanup();
		printf("Appuyez une touche pour finir\n");
	getchar();

        return 1;
    }

    printf("Nombre d'octets envoyes : %ld\n", iResult);

	//------------------------------
	// Maintenant, on va recevoir l' information envoy�e par le serveur
	iResult = recv(leSocket, motRecu, 7, 0);
	if (iResult > 0) {
		printf("Nombre d'octets recus: %d\n", iResult);
		motRecu[iResult] = '\0';
		printf("Le mot recu est %*s\n", iResult, motRecu);
	}
    else {
        printf("Erreur de reception : %d\n", WSAGetLastError());
    }

    // cleanup
    closesocket(leSocket);
    WSACleanup();

	printf("Appuyez une touche pour finir\n");
	getchar();
    return 0;
}