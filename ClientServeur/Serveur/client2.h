#ifndef CLIENT_H
#define CLIENT_H

#include "server2.h"

typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   int etat;
   char bio[800];

}Client;

#endif /* guard */
