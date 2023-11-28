#ifndef SERVER_H
#define SERVER_H

#ifdef WIN32

#include <winsock2.h>

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h>  /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#endif

#define CRLF "\r\n"
#define PORT 1977
#define MAX_CLIENTS 100
#define MAX_OBSERVERS 30
#define MAX_JEUX 100

#define BUF_SIZE 1024

#include "client2.h"

static void init(void);
static void end(void);
static void app(void);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(SOCKET sock, char *buffer);
static void write_client(SOCKET sock, const char *buffer);
static void send_message_to_all_clients(Client *clients, Client client, int actual, const char *buffer, char from_server);
static void remove_client(Client *clients, int to_remove, int *actual);
static void clear_clients(Client *clients, int actual);
static void menu(Client c);
static void afficherListeJoueursEnLigne(Client *clients, Client client, int actual);
static void invitationPartie(Client *clients, Client *client, int actual, char *nom);
static void gestionEtat(Client *client, char *buffer, Client *clients, int actual,int *nbJeux, Jeu **listJeux);
static void afficherListeAmis(Client *client);
static void ajouterAmi(Client *client, Client *clients, int actual, const char *buffer);
static void voirBioJoueur(Client *clients, Client *client, int actual, char *nom);
static void changementConfidentialite(Client *client);
static void changementSauvegardeMode(Client *client);
static void envoyerMessage(Client *clients, Client *sender, int actual, const char *buffer);
static void createJeu(Client* j1,Client* j2,int *nbJeux,Jeu **listJeux);
static void jouerUnCoup(Client *joueur, char *caseChoisie);
bool estTourClient(Client *joueur);
static void afficherHistoriqueParties(Client *client);
static void envoyerMessagePartie(Client *receiver, Client *sender,const char *buffer);
static bool addObserver(Jeu** listeJeux,Client* observer,int choix);
static void removeObserver(Jeu* jeu,Client* observer);
static void afficherListPartiesDisponnibles(Jeu** listeJeux,int *nbJeux,Client* client);
int verifier_identite(const char *pseudo, const char *mdp);


#endif /* guard */
