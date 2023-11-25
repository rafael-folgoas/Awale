#ifndef CLIENT_H
#define CLIENT_H

#include "server2.h"

typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   int etat;
   char bio[800];
   struct Client *adversaire;
   // int inscrit;
   struct Client *amis[50];
   bool confidentialitePublique;
   int sauvegarde;

} Client;

#endif /* guard */
