#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "server2.h"
#include "client2.h"

static void init(void)
{
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if(err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }
#endif
}

static void end(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}

static void app(void)
{
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   /* the index for the array */
   int actual = 0;
   int max = sock;
   /* an array for all clients */
   Client clients[MAX_CLIENTS];

   fd_set rdfs;

   while(1)
   {
      int i = 0;
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the connection socket */
      FD_SET(sock, &rdfs);

      /* add socket of each client */
      for(i = 0; i < actual; i++)
      {
         FD_SET(clients[i].sock, &rdfs);
      }

      if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if(FD_ISSET(STDIN_FILENO, &rdfs))
      {
         /* stop process when type on keyboard */
         break;
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         /* new client */
         SOCKADDR_IN csin = { 0 };
         size_t sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if(csock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }
         /* after connecting the client sends its name */
         if(read_client(csock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }

         
         //verifier dispo pseudo
         bool dispo=true;
         for(int i=0;i<actual;i++){
            if(strcmp(clients[i].name,buffer)==0){
               dispo=false;
            }
         }
         if(!dispo){
            write_client(csock,"Pseudo non disponible, essayez avec un autre");
            closesocket(csock);
            continue;
         }
         


         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);

         Client c ;
         c.sock= csock;
         strncpy(c.name, buffer, BUF_SIZE - 1);
         c.etat=0;
         strncpy(c.bio,"",500);
         clients[actual] = c;
         menu(clients[actual]);
         actual++;
      }
      else
      {
         int i = 0;
         for(i = 0; i < actual; i++)
         {
            /* a client is talking */
            if(FD_ISSET(clients[i].sock, &rdfs))
            {
               Client *client = &clients[i];
               int c = read_client(clients[i].sock, buffer);
               /* client disconnected */
               if(c == 0)
               {
                  closesocket(clients[i].sock);
                  remove_client(clients, i, &actual);
                  strncpy(buffer, client->name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  send_message_to_all_clients(clients, *client, actual, buffer, 1);
               }
               else
               {
                  //send_message_to_all_clients(clients, client, actual, buffer, 0);
                  if(client->etat==0){
                     if (strcmp(buffer,"1")==0){ //afficher liste joueurs online
                        afficherListeJoueursEnLigne(clients,*client,actual);
                        menu(*client);
                     }
                     else if (strcmp(buffer,"2")==0){ //défier joueur
                        //client->etat=5;
                     }
                     else if (strcmp(buffer,"8")==0){ //écrire sa bio
                        client->etat=8;
                        write_client(client->sock, "Ecrivez votre bio : \n");
                     }
                     
                  }else if (client->etat==8){
                     strncpy(client->bio,buffer,BUF_SIZE);
                     client->etat=0;
                     menu(*client);
                  }else{
                     client->etat=0;
                     menu(*client);
                  }
               }
               
               break;
            }
         }
      }
   }

   clear_clients(clients, actual);
   end_connection(sock);
}
static void menu(Client c){

   write_client(c.sock, "Menu : \n");
   write_client(c.sock, "1. Afficher liste des joueurs en ligne. \n");
   write_client(c.sock, "2. Defier un joueur. \n");
   write_client(c.sock, "3. Regarder en tant que spectateur une partie. \n");
   write_client(c.sock, "4. Definir une liste d'ami pouvant visionner ma partie privee. \n");
   write_client(c.sock, "5. Passer en mode partie privee/public. \n");
   write_client(c.sock, "6. Sauvegarder la prochaine partie. \n");
   write_client(c.sock, "7. Revisionner la partie sauvegardee. \n");
   write_client(c.sock, "8. Ecrire sa bio. \n");
   write_client(c.sock, "9. Voir la bio d'un autre joueur. \n");
   write_client(c.sock, "p. Envoyer un message au destinataire desire, tapez exit pour sortir de ce chat. \n");

}

static void afficherListeJoueursEnLigne(Client* clients, Client client, int actual){
   write_client(client.sock, "Liste des joueurs en ligne : ");
   for(int i = 0; i < actual; i++){
      if(client.sock != clients[i].sock){
         write_client(client.sock, "-");
         write_client(client.sock, clients[i].name);
         write_client(client.sock, "\n");
      }
   }
   
   
}

static void clear_clients(Client *clients, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      closesocket(clients[i].sock);
   }
}

static void remove_client(Client *clients, int to_remove, int *actual)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*actual)--;
}

static void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if(sender.sock != clients[i].sock)
      {
         if(from_server == 0)
         {
            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(clients[i].sock, message);
      }
   }
}

static int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_client(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   return n;
}

static void write_client(SOCKET sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

int main(int argc, char **argv)
{
   init();

   app();

   end();

   return EXIT_SUCCESS;
}





