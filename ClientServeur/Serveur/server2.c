#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "server2.h"
#include "jeu.h"
#include <sys/select.h>



static void init(void)
{
#ifdef WIN32messenger
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if (err < 0)
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
   int *nbJeux = (int *)malloc(sizeof(int));
   *nbJeux = 0;
   /* an array for all clients */
   Client clients[MAX_CLIENTS];
   Jeu **listeJeux=malloc(30*sizeof(Jeu*));

   for (int i = 0; i < 30; i++)
   {
      listeJeux[i] = malloc(sizeof(Jeu));
   }

   fd_set rdfs;

   while (1)
   {
      int i = 0;
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the connection socket */
      FD_SET(sock, &rdfs);

      /* add socket of each client */
      for (i = 0; i < actual; i++)
      {
         FD_SET(clients[i].sock, &rdfs);
      }

      if (select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if (FD_ISSET(STDIN_FILENO, &rdfs))
      {
         /* stop process when type on keyboard */
         break;
      }
      else if (FD_ISSET(sock, &rdfs))
      {
         /* new client */
         SOCKADDR_IN csin = {0};
         size_t sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if (csock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }
         /* after connecting the client sends its name */
         if (read_client(csock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }

         char pseudo[BUF_SIZE];
         char mdpC[BUF_SIZE];

         // Extraction du pseudo et du mot de passe de la chaîne lue
         if (sscanf(buffer, "pseudo:%s mdp:%s", pseudo, mdpC) != 2)
         {
            printf("probleeme");
            closesocket(sock);
 
         }

         // vérifier le pseudo et le mot de passe
         int status = verifier_identite(pseudo, mdpC);

         // envoyer le statut de connexion au client
         //write_client(sock, "", status);

         if (status == 1)
         {
            // pseudo et mot de passe corrects, procéder avec le reste du traitement
            // ...
            write_client(sock, "Connexion réussie");
         }
         else if (status == 2)
         {
            // pseudo correct, mot de passe incorrect
            write_client(sock, "Mot de passe incorrect. Veuillez réessayer.");
            closesocket(sock);
         }
         else
         {
            // pseudo incorrect ou ajouté, traitement en fonction des besoins
            // Si vous souhaitez effectuer des actions spécifiques lors de l'inscription,
            // vous pouvez les ajouter ici.
            // Par exemple, envoyer un message de bienvenue au nouveau client.
            write_client(sock, "Bienvenue ! Vous êtes maintenant inscrit.");
            // ... (autres traitements)
         }
      
         // verifier dispo pseudo
         // bool dispo = true;
         // for (int i = 0; i < actual; i++)
         // {
         //    if (strcmp(clients[i].name, buffer) == 0)
         //    {
         //       dispo = false;
         //    }
         // }
         // if (!dispo)
         // {
         //    write_client(csock, "Pseudo non disponible, essayez avec un autre");
         //    closesocket(csock);
         //    continue;
         // }

         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);

         Client c;
         strncpy(c.name, buffer, BUF_SIZE - 1);
         strncpy(c.mdp, buffer, BUF_SIZE - 1);
         c.adversaire = NULL;
         c.etat = ETAT_MENU;
         c.confidentialitePublique = true;
         c.sauvegardeMode = true;
         strncpy(c.bio,"", 800);
         c.sock = csock;
         creerFiles(c.name);
         c.jeuEnCours=NULL;
         c.nbAmis=0;
         //remplirFileJoueur(c.name, c.mdp,c.bio);
         clients[actual] = c;
         menu(clients[actual]);
         actual++;
      }
      else
      {
         int i = 0;
         for (i = 0; i < actual; i++)
         {
            /* a client is talking */
            if (FD_ISSET(clients[i].sock, &rdfs))
            {
               Client *client = &clients[i];
               int c = read_client(clients[i].sock, buffer);
               /* client disconnected */
               if (c == 0)
               {
                  closesocket(clients[i].sock);
                  remove_client(clients, i, &actual);
                  strncpy(buffer, client->name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  send_message_to_all_clients(clients, *client, actual, buffer, 1);
               }
               else
               {
                  // send_message_to_all_clients(clients, client, actual, buffer, 0);
                  gestionEtat(client, buffer, clients, actual,nbJeux,listeJeux);
               }

               break;
            }
         }
      }
   }

   clear_clients(clients, actual);
   end_connection(sock);
}

//Inscription/Connexion
int verifier_identite(const char *pseudo, const char *mdp)
{
   FILE *file = fopen("joueurs.txt", "a+");

   if (file == NULL)
   {
      perror("fopen()");
      exit(errno);
   }

   char buffer[BUF_SIZE];
   char current_pseudo[BUF_SIZE];
   char current_mdp[BUF_SIZE];
   int status_pseudo = 0; // 0: pseudo incorrect, 1: pseudo correct
   int status_mdp = 0;   // 0: mot de passe incorrect, 1: mot de passe correct

   while (fgets(buffer, sizeof(buffer), file) != NULL)
   {
      if (sscanf(buffer, "pseudo:%s mdp:%s", current_pseudo, current_mdp) == 2)
      {
         if (strcmp(current_pseudo, pseudo) == 0)
         {
            status_pseudo = 1;
            status_mdp = (strcmp(current_mdp, mdp) == 0) ? 1 : 0;
            break; 
         }
      }
   }

   if (status_pseudo == 0)
   {
      fprintf(file, "pseudo:%s mdp:%s\n", pseudo, mdp);
      status_pseudo = 1; 
   }

   fclose(file);

   if (status_pseudo == 1 && status_mdp == 1) //connexion réussie
   {
      return 1; 
   }
   else if (status_pseudo == 1 && status_mdp == 0) //mdp incorrect
   {
      return 2; 
   }
   else //inscription client
   {
      return 0; 
   }
}


// Fonction pour gérer l'état du client
void gestionEtat(Client *client, char *buffer, Client *clients, int actual,int *nbJeux,Jeu** listeJeux)
{
   
   switch (client->etat)
   {
   case ETAT_MENU:

      if (strcmp(buffer, "1") == 0)
      {
         afficherListeJoueursEnLigne(clients, *client, actual);
         menu(*client);
      }
      else if (strcmp(buffer, "2") == 0)
      {
         write_client(client->sock, "Entrer le nom du joueur que vous voulez inviter : \n");
         client->etat = ETAT_INVITATION_PARTIE;
      }
      else if (strcmp(buffer, "3") == 0)
      {
         write_client(client->sock, "Vous etes en mode observateur \n"); //test
         client->etat = ETAT_OBSERVATEUR_DEMANDE;
         afficherListPartiesDisponnibles(listeJeux,nbJeux,client);
      
      }
      else if (strcmp(buffer, "4") == 0)
      {
         changementConfidentialite(client);
      }
      else if (strcmp(buffer, "5") == 0)
      {
         write_client(client->sock, "Entrez les pseudos de vos amis un par un (exemple rafael [ENTRER] laziza [ENTRER] puis appuyer sur y [ENTRER] terminer\n");
         client->etat = ETAT_AJOUTER_AMI;
      }
      else if (strcmp(buffer, "6") == 0)
      {
         changementSauvegardeMode(client);
      }
      else if (strcmp(buffer,"7")==0){
         afficherHistoriqueParties(client);
      }
      else if (strcmp(buffer, "8") == 0)
      {
         client->etat = ETAT_ECRIRE_BIO;
         write_client(client->sock, "Ecrivez votre bio : \n");
      }
      else if (strcmp(buffer, "9") == 0)
      {
         write_client(client->sock, "Entrez le nom du joueur dont vous voulez voir la bio : \n");
         client->etat = ETAT_VOIR_BIO;
      }
      else if (client->adversaire != NULL && strcmp(buffer, "0") == 0)
      {
         write_client(client->sock, "1-Accepter la demande : \n");
         write_client(client->sock, "2-Refuser la demande \n");
         client->etat = 6;
      }
      else if (strcmp(buffer, "10") == 0)
      {
         afficherListeAmis(client);
         menu(*client);
      }
      else if (strcmp(buffer, "11") == 0)
      {
         write_client(client->sock, "Chat : \nPour envoyer un message écrivez le nom de l'utilisateur puis le message sous cette forme : \n nom-destinataire:message \n");
         write_client(client->sock, "(pour quiter tappez q) \n");
         client->etat = ETAT_MESSAGE;
      }
      else if (strcmp(buffer, "100") == 0) {   
         write_client(client->sock, "TEST : \r\n");
      }
      else
      {
         write_client(client->sock, "Commande non reconnue. \n");
         client->etat = ETAT_MENU;
         menu(*client);
      }
      break;

   case ETAT_ECRIRE_BIO:
      strncpy(client->bio, buffer, 799);
      write_client(client->sock,"Votre bio a bien été modifiée !\r\n"); 
      client->etat = ETAT_MENU;
      menu(*client);
      break;

   case ETAT_AJOUTER_AMI:
      ajouterAmi(client, clients, actual, buffer);
      break;

   case ETAT_VOIR_BIO:
      if (strcmp(buffer, "q") == 0)
      {
         client->etat = 0;
         menu(*client);
      }
      else
      {
         voirBioJoueur(clients, client, actual, buffer);
      }
      break;

   case ETAT_INVITATION_PARTIE:
      
      if(strcmp(buffer, "q") == 0)
      {
         client->etat = ETAT_MENU;
         menu(*client);
      }
      else
      {
         invitationPartie(clients, client, actual, buffer);
      }
      break;

   case ETAT_ATTENTE_REPONSE_INVITATION:
      if (strcmp(buffer, "q") == 0)
      {
         client->etat = 0;
         menu(*client);
      }
      break;
   case ETAT_DOIT_REPONDRE_INVITATION:
   
      if (strcmp(buffer, "1") == 0)
      {
         write_client(client->sock, "Vous avez accepte invitation ! \n");
         write_client(client->adversaire->sock, "Votre invitation a ete acceptee ! \n");    
         createJeu(client, client->adversaire,nbJeux,listeJeux);

      }
      else if (strcmp(buffer, "2") == 0)
      {
         write_client(client->sock, "Vous avez refuse invitation \n");
         write_client(client->adversaire->sock, "Votre invitation a ete refusee ! \n");
         client->etat = ETAT_MENU;
         client->adversaire->etat = ETAT_MENU;
         menu(*client);
         menu(*(client->adversaire)); 
      }
      break;

   case ETAT_JEU_EN_COURS:
      
      if (strcmp(buffer,"chat")==0){
         write_client(client->sock, "Chat : Envoyer un message sous cette forme : \n<message> \n");
         write_client(client->sock, "pour quiter tappez quit \n");
         client->etat = ETAT_MESSAGE_PARTIE;
         break;
      }
      // char *endptr;
      // long caseChoisie = strtol(buffer, &endptr, 10);

      // if (*endptr != '\0') {
      //     write_client(client->sock, "Veuillez entrer un entier\n");
      //     break;
      // } 
      // bool estSonTour=estTourClient(client);
      // if(!estSonTour){
      //    write_client(client->sock,"Ce n'est pas votre tour\n");
      //    break;
      // }else {
      //    if (caseChoisie>=0 && caseChoisie<12){
      //       jouerUnCoup(client, buffer);
      //    }else {
      //       write_client(client->sock,"Case invalide, veuillez entrer un entier entre 0 et 11\n");
      //    }
      // }
      break;
   case ETAT_MESSAGE_PARTIE:
      if (strcmp(buffer, "quit") == 0)
      {
         client->etat = ETAT_JEU_EN_COURS;
         write_client(client->sock, "Retour a la partie \n");
         break;
      }
      envoyerMessagePartie(client->adversaire, client, buffer);
      break;
   case ETAT_OBSERVATEUR_DEMANDE:
      // char *endptrr;
      // int choix = (int)strtol(buffer, &endptrr, 10);

      // if (*endptrr != '\0') {
      //    write_client(client->sock, "Veuillez entrer un entier\n");
      //    break;
      // } 

      // if(choix>=0 && choix<*nbJeux){
      //    if(!listeJeux[choix]->estFini){
      //       bool trouve =addObserver(listeJeux,client,choix);
      //       if(trouve){
      //          write_client(client->sock,"Vous etes en mode observateur sur la partie en cours : \n");
      //          write_client(client->sock, listeJeux[choix]->joueur1->name);
      //          write_client(client->sock,"VS");
      //          write_client(client->sock, listeJeux[choix]->joueur2->name);
      //          write_client(client->sock, "\n");
      //          write_client(client->sock,"Ecrivez quit pour quitter l observation. \n");
      //          client->etat=ETAT_OBSERVATEUR_JEU;
      //       }else{
      //          write_client(client->sock,"Plus de place dans la partie \n");
      //          client->etat=ETAT_MENU;
      //       } 
      //    }else {
      //       write_client(client->sock,"Rentrez un numero de partie possible : \n");

      //    }
      // }else{
      //    write_client(client->sock,"La partie n'est pas valide\n");
      //    client->etat=ETAT_OBSERVATEUR_DEMANDE;
      // }
      break;
      
   case ETAT_OBSERVATEUR_JEU:
      if (strcmp(buffer, "quit") == 0)
      {
         removeObserver(client->jeuEnCours,client);
         client->etat = ETAT_MENU;
         client->jeuEnCours=NULL;
         menu(*client);
         write_client(client->sock, "Retour au menu \n");
      }
      break;
   case ETAT_MESSAGE:
      envoyerMessage(clients, client, actual, buffer);
      break;
   default://etat 0
      client->etat = ETAT_MENU;
      menu(*client);
      break;
   }
}
static void menu(Client c)
{

   write_client(c.sock, "Menu : \n");
   write_client(c.sock, "1. Afficher liste des joueurs en ligne. \n"); 
   write_client(c.sock, "2. Inviter un joueur pour une partie. \n");    
   write_client(c.sock, "3. Regarder en tant que spectateur une partie. \n"); //a faire mon reuf 
   write_client(c.sock, "4. Passer en mode partie privee/public. \n"); 
   write_client(c.sock, "5. Definir une liste d'ami pouvant visionner ma partie privee. \n"); 
   write_client(c.sock, "6. Activer le mode sauvegarde de parties. \n");  
   write_client(c.sock, "7. Revisionner les parties sauvegardees. \n");
   write_client(c.sock, "8. Ecrire sa bio. \n");                 
   write_client(c.sock, "9. Voir la bio d'un autre joueur. \n"); 
   write_client(c.sock, "10. Afficher liste d'amis. \n");        
   write_client(c.sock, "11. Envoyer un message au destinataire desire, tapez exit pour sortir de ce chat. \n");
   //login et register 
}
static void envoyerMessage(Client *clients, Client *sender, int actual, const char *buffer)
{
   char destinataire[BUF_SIZE];
   char message[BUF_SIZE];
   if (strcmp(buffer, "q") == 0)
   {
      sender->etat = ETAT_MENU;
      menu(*sender);
      return;
   }
   sscanf(buffer, "%[^:]:%[^\n]", destinataire, message);
   if (strcmp(message, "") == 0)
   {
      write_client(sender->sock, "Format incorrect : nom-destinataire:message \n");
      return;
   }
   bool destinataireTrouve = false;
   for (int i = 0; i < actual; i++)
   {
      if (strcmp(clients[i].name, destinataire) == 0)
      {
         destinataireTrouve = true;

         write_client(clients[i].sock, "[Message de ");
         write_client(clients[i].sock, sender->name);
         write_client(clients[i].sock, "] ");
         write_client(clients[i].sock, message);
         write_client(clients[i].sock, "\n");

         write_client(sender->sock, "[Message envoye a ");
         write_client(sender->sock, destinataire);
         write_client(sender->sock, "] ");
         write_client(sender->sock, message);
         write_client(sender->sock, "\n");
         break;
      }
   }

   // Si le destinataire n'est pas trouvé, envoyer un message d'erreur à l'expéditeur
   if (!destinataireTrouve)
   {
      write_client(sender->sock, "Le destinataire n'existe pas ou n'est pas en ligne.\n");
   }
}

static bool addObserver(Jeu** listeJeux,Client* observer,int choix){

   bool trouve=false;
   for(int i=0;i<MAX_OBSERVERS;i++){
      if(listeJeux[choix]->observers[i]==NULL){
         listeJeux[choix]->observers[i]=observer;
         trouve=true;
         break;
      }
   }
   if(trouve){
      observer->jeuEnCours=listeJeux[choix];
   }
   return trouve;
}
static void envoyerMessagePartie(Client *receiver, Client *sender,const char *buffer)
{
   if (strcmp(buffer, "quit") == 0)
   {
      sender->etat = ETAT_JEU_EN_COURS;
      return;
   }
   write_client(receiver->sock, "[Message de ");
   write_client(receiver->sock, sender->name);
   write_client(receiver->sock, "] ");
   write_client(receiver->sock, buffer);
   write_client(receiver->sock, "\n");
   
}
static void removeObserver(Jeu* jeu,Client* observer){
   for(int i=0;i<MAX_OBSERVERS;i++){
      if(jeu->observers[i]!=NULL){
         if(strcmp(jeu->observers[i]->name,observer->name)==0){
            jeu->observers[i]=NULL;
            break;
         }
      }
   }
}
static void afficherHistoriqueParties(Client *client){
   char* historique=lireHistoriqueFile(client->name);
   if (strlen(historique) > 1)
   {
      write_client(client->sock, "Voici la liste des parties sauvegardees : \r\n");
      write_client(client->sock, historique);
      
      client->etat =ETAT_MENU;
   }
   else
   {
      write_client(client->sock, "Aucune partie sauvegardee\r\n");
      client->etat = ETAT_MENU;
   }
   menu(*client);
}
static void afficherListPartiesDisponnibles(Jeu** listeJeux,int *nbJeux,Client* client){
  
   if (*nbJeux > 0)
      {
         write_client(client->sock, "Voici la liste des jeux en cours : \r\n");
         char pos[20];
         for(int i=0 ; i<*nbJeux ; i++){
            if(!listeJeux[i]->estFini){
               sprintf(pos, "%d", i);
               write_client(client->sock, pos);
               write_client(client->sock, " -");
               write_client(client->sock, listeJeux[i]->joueur1->name);
               write_client(client->sock," VS ");
               write_client(client->sock, listeJeux[i]->joueur2->name);
               write_client(client->sock, "- ");
            }
         }
         write_client(client->sock, "\nVeuillez entrer le numero du jeu que vous voulez observer :");
         client->etat = ETAT_OBSERVATEUR_DEMANDE;
      }
      else
      {
         write_client(client->sock, "Aucun jeu en cours\r\n");
         client->etat = ETAT_MENU;
      }
}
bool estTourClient(Client *joueur){
   Jeu* jeu=joueur->jeuEnCours;
   if(strcmp(joueur->name,jeu->joueur1->name)==0){
      if(jeu->tour==0){
         return true;
      }else{
         return false;
      }
   }else if(strcmp(joueur->name,jeu->joueur2->name)==0){
      if(jeu->tour==1){
         return true;
      }else{
         return false;
      }
   }else{
      return false;
   }
   return false;
}

static void jouerUnCoup(Client *joueur, char* caseChoisie){
   Jeu* jeu=joueur->jeuEnCours;


   bool coupValide=jouerCoup(jeu,atoi(caseChoisie),joueur);

   if(!coupValide){
      write_client(joueur->sock,"Coup invalide\n");
   }else{
      char* sJ1=(char*)malloc(20*sizeof(char));
      char* sJ2=(char*)malloc(20*sizeof(char));
      sprintf(sJ1, "%d", jeu->scoreJ1);
      sprintf(sJ2, "%d", jeu->scoreJ2);
      //printf('jeu obsvers : %s',joueur->jeuEnCours->observers[0]->name);
      for(int i=0;i<MAX_OBSERVERS;i++){
         if(jeu->observers[i]!=NULL){
            printf("observer RENTRER %d\n",i);
            write_client(jeu->observers[i]->sock,"Coup joue : ");
            write_client(jeu->observers[i]->sock,caseChoisie);
            write_client(jeu->observers[i]->sock,afficherTableau(jeu));
            write_client(jeu->observers[i]->sock,"Scores actuels :\n");
            write_client(jeu->observers[i]->sock,jeu->joueur1->name);
            write_client(jeu->observers[i]->sock," :");
            write_client(jeu->observers[i]->sock,sJ1);
            write_client(jeu->observers[i]->sock,"\n");
            write_client(jeu->observers[i]->sock,jeu->joueur2->name);
            write_client(jeu->observers[i]->sock," :");
            write_client(jeu->observers[i]->sock,sJ2);
            write_client(jeu->observers[i]->sock,"\n");
         }
      }
      write_client(joueur->adversaire->sock,"Coup joue : ");
      write_client(joueur->adversaire->sock,caseChoisie);
      write_client(joueur->sock,afficherTableau(jeu));
      write_client(joueur->adversaire->sock,afficherTableau(jeu));
      write_client(joueur->sock,"Scores actuels :\n");
      write_client(joueur->sock,jeu->joueur1->name);
      write_client(joueur->sock," :");
      write_client(joueur->sock,sJ1);
      write_client(joueur->sock,"\n");
      write_client(joueur->sock,jeu->joueur2->name);
      write_client(joueur->sock," :");
      write_client(joueur->sock,sJ2);
      write_client(joueur->sock,"\n");
      write_client(joueur->adversaire->sock,"Scores actuels :\n");
      write_client(joueur->adversaire->sock,jeu->joueur1->name);
      write_client(joueur->adversaire->sock," :");
      write_client(joueur->adversaire->sock,sJ1);
      write_client(joueur->adversaire->sock,"\n");
      write_client(joueur->adversaire->sock,jeu->joueur2->name);
      write_client(joueur->adversaire->sock," :");
      write_client(joueur->adversaire->sock,sJ2);
      write_client(joueur->adversaire->sock,"\n");
      if(finPartie(jeu)==0){
         write_client(jeu->joueur1->sock,jeu->joueur1->name);
         write_client(jeu->joueur1->sock," a gagne !\n");
         write_client(jeu->joueur2->sock,jeu->joueur1->name);
         write_client(jeu->joueur2->sock," a gagne !\n"); 
         jeu->joueur1->etat = ETAT_MENU;
         jeu->joueur2->etat = ETAT_MENU;
      }else if(finPartie(jeu)==1){
         write_client(jeu->joueur1->sock,jeu->joueur2->name);
         write_client(jeu->joueur1->sock," a gagne !\n");
         write_client(jeu->joueur2->sock,jeu->joueur2->name);
         write_client(jeu->joueur2->sock," a gagne !\n");
         jeu->joueur1->etat = ETAT_MENU;
         jeu->joueur2->etat = ETAT_MENU;
      }
      else{
         if(jeu->tour==0){
            write_client(jeu->joueur1->sock,"C'est votre tour\n");
            write_client(jeu->joueur2->sock,"C'est le tour de votre adversaire\n");
         }else{
            write_client(jeu->joueur2->sock,"C'est votre tour\n");
            write_client(jeu->joueur1->sock,"C'est le tour de votre adversaire\n");
         }
      }
      
   }
}
static void createJeu(Client* j1,Client* j2,int *nbJeux,Jeu** listeJeux){
   write_client(j1->sock,"La partie va commencer !\n");
   write_client(j2->sock,"La partie va commencer !\n");
   write_client(j1->sock,"Pour chatter appuyez écrivez : chat\n");
   write_client(j2->sock,"Pour chatter appuyez écrivez : chat\n");
   Jeu *jeu=initJeu(j1,j2);

   write_client(j1->sock,afficherTableau(jeu));
   write_client(j2->sock,afficherTableau(jeu));
   j2->adversaire=j1;
   j1->adversaire=j2;
   if(jeu->tour==0){
      write_client(jeu->joueur1->sock,"C'est votre tour\n");
      write_client(jeu->joueur2->sock,"C'est le tour de votre adversaire\n");
   }else{
      write_client(jeu->joueur2->sock,"C'est votre tour\n");
      write_client(jeu->joueur1->sock,"C'est le tour de votre adversaire\n");
   }
   jeu->joueur1->etat = ETAT_JEU_EN_COURS; 
   jeu->joueur2->etat = ETAT_JEU_EN_COURS; 
   listeJeux[*nbJeux]=jeu;
   

   (*nbJeux)++;
   for(int i=0;i<MAX_OBSERVERS;i++){
      jeu->observers[i]=NULL;
   }
   jeu->estFini=false;
   j1->jeuEnCours=jeu;
   j2->jeuEnCours=jeu;
}

static void changementConfidentialite(Client *client)
{
   if (client->confidentialitePublique)
   {
      client->confidentialitePublique = false;
      write_client(client->sock, "Vous etes en mode privee. \n");
   }
   else
   {
      client->confidentialitePublique = true;
      write_client(client->sock, "Vous etes en mode public. \n");
   }
   client->etat = ETAT_MENU;
   menu(*client);
}
static void changementSauvegardeMode(Client *client)
{
   if (client->sauvegardeMode)
   {
      client->sauvegardeMode = false;
      write_client(client->sock, "Vous etes en mode non sauvegarde. \n");
   }
   else
   {
      client->sauvegardeMode = true;
      write_client(client->sock, "Vous etes en mode sauvegarde. \n");
   }
   client->etat = ETAT_MENU;
   menu(*client);
}

static void afficherListeAmis(Client *client)
{
   write_client(client->sock, "Liste de vos amis : \n");

   for (int i = 0; i < 50; i++)
   {
      Client *ami = client->amis[i];
      if (ami != NULL)
      {
         write_client(client->sock, "-");
         write_client(client->sock, ami->name);
         write_client(client->sock, "\n");
      }
   }
}
static void voirBioJoueur(Client *clients, Client *client, int actual, char *nom)
{
   bool trouve = false;
   for (int i = 0; i < actual; i++)
   {
      if (strcmp(clients[i].name, nom) == 0)
      {
         if(strcmp(clients[i].bio,"")==0){
            write_client(client->sock, "Ce joueur n'a pas de bio\n");
            trouve = true;
            break;
         }else{       
            write_client(client->sock, "Bio de ");
            write_client(client->sock, clients[i].name);
            write_client(client->sock, " : \n");
            write_client(client->sock, clients[i].bio);
            write_client(client->sock, "\n\n");
            trouve = true;
            break;
         }
      }
   }
   if (!trouve)
   {
      write_client(client->sock, "Le joueur n'existe pas ou n'est pas en ligne. Reessayez ou appuyez sur q pour quitter \n");
   }
   else
   {
      client->etat = ETAT_MENU;
      menu(*client);
   }
}
// Fonction pour ajouter un ami
static void ajouterAmi(Client *client, Client *clients, int actual, const char *buffer)
{
   if (strcmp(buffer, "y") == 0 || strcmp(buffer, "q") == 0) // retour a l'etat menu
   {
      client->etat = ETAT_MENU;
      menu(*client);
   }
   else
   {
      bool trouve = false;
      bool plusDePlace = true;
      for (int i = 0; i < actual; i++)
      {
         if (strcmp(clients[i].name, buffer) == 0)
         {
            trouve = true;
            for (int j = 0; j < 50; j++)
            {
               if (client->amis[j] == NULL)
               {
                  client->amis[j] = &clients[i];
                  plusDePlace = false;
                  break; // Ajout d'un break pour éviter de continuer la recherche après avoir trouvé une place
               }
            }
            client->nbAmis++;
         }
      }

      if (!trouve)
      {
         write_client(client->sock, "Le joueur n'existe pas ou n'est pas en ligne. \n");
      }
      else if (plusDePlace)
      {
         write_client(client->sock, "Vous avez atteint le nombre maximum d'amis. \n");
      }
      else
      {
         write_client(client->sock, "Ami ajoute avec succes. \n");
      }
   }
}

static void afficherListeJoueursEnLigne(Client *clients, Client client, int actual)
{
   if (actual == 1)
   {
      write_client(client.sock, "Vous etes le seul joueur en ligne. \n");
      return;
   }
   else
   {
      write_client(client.sock, "Liste des joueurs en ligne : \n");
      for (int i = 0; i < actual; i++)
      {
         if (client.sock != clients[i].sock)
         {
            write_client(client.sock, "-");
            write_client(client.sock, clients[i].name);
            write_client(client.sock, "\n");
         }
      }
   }
}

static void invitationPartie(Client *clients, Client *client, int actual, char *nom)
{

   for (int i = 0; i < actual; i++)
   {
      if ((strcmp(clients[i].name, nom) == 0) && strcmp(clients[i].name, client->name) != 0)
      {
         write_client(clients[i].sock, client->name);
         write_client(clients[i].sock, " vous invite pour une partie. \nTaper 1 pour accepter - 2 pour refuser\n");
         clients[i].adversaire = client;
         //client->adversaire=&clients[i];
         write_client(client->sock, "Invitation envoyée à ");
         write_client(client->sock, nom);
         write_client(client->sock, ". Attente de réponse...\n");
         client->etat = ETAT_ATTENTE_REPONSE_INVITATION;
         clients[i].etat= ETAT_DOIT_REPONDRE_INVITATION;
         break;
      }
   }
   if (client->etat != ETAT_ATTENTE_REPONSE_INVITATION)
   {
      write_client(client->sock, "Le joueur n'existe pas ou n'est pas en ligne. \n");
   }
}

static void clear_clients(Client *clients, int actual)
{
   int i = 0;
   for (i = 0; i < actual; i++)
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
   for (i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if (sender.sock != clients[i].sock)
      {
         if (from_server == 0)
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
   SOCKADDR_IN sin = {0};

   if (sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if (bind(sock, (SOCKADDR *)&sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   if (listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
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

   if ((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   char pseudo[BUF_SIZE];
   char mdp[BUF_SIZE];

   if (sscanf(buffer, "pseudo:%s mdp:%s", pseudo, mdp) != 2)
   {

      return -1;
   }

   return n;
}

static void write_client(SOCKET sock, const char *buffer)
{
   if (send(sock, buffer, strlen(buffer), 0) < 0)
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
