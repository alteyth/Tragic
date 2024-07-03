#include "gamelib.h"
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <string.h>
#include <time.h>

//Dichiarazioni delle funzioni
void title();
void main_menu();
void ost();
void clear();
static void reset(struct Mago*);
void imposta_gioco();
static void scelta_classe();
void combatti();
void termina_gioco();
static void scelta_classe(struct Mago*);
static void scelta_numero_carte(unsigned short*);
static void genera_mazzo(struct Mago*);
static void mazzo_tenebre(struct Mago*);
static void mazzo_vita(struct Mago*);
static void mazzo_luce(struct Mago*);
static void pesca_carta(struct Mago*);
static void stampa_mano(struct Mago*);
static void stampa_campo(struct Mago*);
static void turno(struct Mago*, struct Mago*);
static void gioca_carta(struct Mago*, struct Mago*);
static void gioca_creatura(struct Mago*, int);
static void gioca_rimuovi_creatura(struct Mago*, struct Mago*, int);
static void gioca_infliggi_danno(struct Mago*, struct Mago*, int);
static void gioca_guarisci_danno(struct Mago*, int);
static void attacca(struct Mago*, struct Mago*);
static void lotta(struct Mago*, struct Mago*, int);
static void controllo_vittoria(struct Mago*, struct Mago*);

struct Mago giocatore1;
struct Mago giocatore2;
unsigned short n_carte_mazzo;
unsigned short gameset;
int azione, turn;

//Funzioni di utility
void clear(){
	while(getchar()!='\n');
}

//La funzione "reset" dealloca le carte rimanenti nei due mazzi prima di reimpostare il gioco
void reset(struct Mago *player) {

	struct Carta *pTemp;

	while(player->inizio_mazzo!=NULL){
		pTemp=player->inizio_mazzo;
		player->inizio_mazzo=player->inizio_mazzo->pNext;
		free(pTemp);
	}
	
	for(int i=0;i<6;i++)
		player->mano[i]=NULL;
	for(int i=0;i<4;i++)
		player->campo[i]=NULL;
}

//Funzioni prettamente estetiche e di output
void title() {

	system("clear");

	printf("\t_________ _______  _______  _______ _________ _______\n");
	printf("\t\\__   __/(  ____ )(  ___  )(  ____ \\__   __/(  ____\\\n");
	printf("\t   ) (   | (    )|| (   ) || (    \\/   ) (   | (    \\/\n");
	printf("\t   | |   | (____)|| (___) || |         | |   | |      \n");
	printf("\t   | |   |     __)|  ___  || | ____    | |   | |      \n");
	printf("\t   | |   | (\\ (   | (   ) || | \\_  )   | |   | |      \n");
	printf("\t   | |   | ) \\ \\__| )   ( || (___) |___) (___| (____/\\\n");
	printf("\t   )_(   |/   \\__/|/     \\|(_______)\\_______/(_______/\n");
	printf("\t                  -Un gioco di carte-\n\n\n");
}

void main_menu() {
	printf("\tBenvenuto su Tragic, giovane Mago\n\n");
	printf("\t1) Imposta Gioco\n");
	printf("\t2) Combatti\n");
	printf("\t3) Colonna Sonora\n");
	printf("\t4) Esci dal gioco\n");
}

void ost() {
	system("clear");
	title();
	printf("\n\n\tQuesto è il link della OST che ho composto per Tragic!\n\tAscoltala durante il gioco!\n\n\twww.linkmio.it/123456abcdefg\n\n\n\n\n\tPremi INVIO per tornare al Menu Principale...");
	clear();
}

//Funzioni di gioco
void termina_gioco() {
	title();
	printf("\tGrazie per aver giocato!\n");
	sleep(1);
	system("clear");
}

//La funzione "Imposta Gioco" gestisce tutta la fase di preparazione (creazione dei giocatori e dei loro mazzi corrispondenti)
void imposta_gioco() {
	if(gameset==1){
		reset(&giocatore1);
		reset(&giocatore2);
	}
	else
		gameset=1;

	system("clear");
	title();

	printf("\tInserisci il nome del Giocatore 1:\n");
	fgets(giocatore1.nome_giocatore, 256, stdin);
	giocatore1.nome_giocatore[strlen(giocatore1.nome_giocatore)-1]='\0';

	printf("\tInserisci il nome del Giocatore 2:\n");
	fgets(giocatore2.nome_giocatore, 256, stdin);
	giocatore2.nome_giocatore[strlen(giocatore2.nome_giocatore)-1]='\0';

	scelta_classe(&giocatore1);
	scelta_classe(&giocatore2);

	scelta_numero_carte(&n_carte_mazzo);
	if(n_carte_mazzo<10||n_carte_mazzo>60) {
		printf("\tInserisci un numero di carte valido\n");
		sleep(2);
		scelta_numero_carte(&n_carte_mazzo);
	}

	srand(time(NULL));

	for(int i=0;i<n_carte_mazzo;i++)
		genera_mazzo(&giocatore1);
	for(int i=0;i<n_carte_mazzo;i++)
		genera_mazzo(&giocatore2);

	for(int i=0;i<5;i++)
		pesca_carta(&giocatore1);

	for(int i=0;i<5;i++)
		pesca_carta(&giocatore2);

	title();
	printf("\t%s, ecco la tua mano\n\n", giocatore1.nome_giocatore);
	stampa_mano(&giocatore1);
	printf("\n\tPremi INVIO per continuare...\n");
	clear();
	system("clear");
	title();
	printf("\t%s, ecco la tua mano\n\n", giocatore2.nome_giocatore);
	stampa_mano(&giocatore2);
	printf("\n\tPremi INVIO per continuare...\n");
	clear();
}

//La funzione "Scelta Numero Carte" fa scegliere all'utente con quante carte nel mazzo vuole giocare
void scelta_numero_carte(unsigned short *n) {
	title();
	printf("\tInserisci il numero delle carte del mazzo(da 10 a 60):\n");
		scanf("%hu", n);
	clear();
	if(*n<10||*n>60) {
		printf("\tInserisci un numero di carte valido\n");
		sleep(2);
		scelta_numero_carte(n);
	}
}

//La funzione "Scelta Classe" viene chiamata una volta per Giocatore e fa selezionare la propria classe ai due utenti
static void scelta_classe(struct Mago *player) {
	int selection1;
	player->punti_vita=20;
	player->inizio_mazzo=NULL;
	player->dimensione_mazzo=0;
	do {
		selection1=0;
		title();
		printf("\t%s, scegli la tua classe:\n\n\t1) Tenebre  (Le tue carte Infliggi Danno sono più potenti)\n\t2) Vita     (Le tue Creature hanno più vita)\n\t3) Luce     (Le tue carte Guarisci Danno curano di più)\n", player->nome_giocatore);
		scanf("%d", &selection1);
		clear();
		switch(selection1) {

			case 1:
				player->classe=tenebre;
				break;

			case 2:
				player->classe=vita;
				break;

			case 3:
				player->classe=luce;
				break;

			default:
				printf("\tErrore, inserisci un comando valido\n");
				sleep(2);
		}
	}while(selection1!=1&&selection1!=2&&selection1!=3);
}

//La funzione "Genera Mazzo" è il bivio principale su cui si basa il programma per poter capire che tipi di mazzi generare (in base alle classi scelte)
static void genera_mazzo(struct Mago *player) {

	switch(player->classe) {

		case 0:
			mazzo_tenebre(player);
			break;

		case 1:
			mazzo_vita(player);
			break;

		case 2:
			mazzo_luce(player);
			break;
	}
}


//In seguito ci sono le tre funzioni che si occupano di generare i tre tipi diversi di mazzi in base alle classi scelte dai due giocatori e
//sono scritte volutamente in modo separato così da rendere più semplici e intuitivi eventuali bilanciamenti delle statistiche

static void mazzo_tenebre(struct Mago *player) {

	struct Carta *pNew=malloc(sizeof(struct Carta));
	pNew->pNext=NULL;

	if(player->inizio_mazzo==NULL)
		player->inizio_mazzo=pNew;
	else {
		pNew->pNext=player->inizio_mazzo;
		player->inizio_mazzo=pNew;
	}

	int x=rand()%100+1;

	if(x>=1&&x<=50){
		pNew->tipo=creatura;

		if(player->dimensione_mazzo<=20)
			pNew->danni=rand()%2+5;
		if(player->dimensione_mazzo<=40&&player->dimensione_mazzo>20)
			pNew->danni=rand()%2+3;
		if(player->dimensione_mazzo<=60&&player->dimensione_mazzo>40)
			pNew->danni=rand()%2+1;

		if(pNew->danni<=2)
			strcpy(pNew->nome_carta, "Imp");
		if(pNew->danni==3||pNew->danni==4)
			strcpy(pNew->nome_carta, "Dannato");
		if(pNew->danni==5||pNew->danni==6)
			strcpy(pNew->nome_carta, "Demone Minore");
	}

	if(x>=51&&x<=65){
		pNew->tipo=rimuovi_creatura;

		strcpy(pNew->nome_carta, "Dannazione");
	}

	if(x>=66&&x<=85){
		pNew->tipo=infliggi_danno;

		if(player->dimensione_mazzo<=30)
			pNew->danni=6;
		if(player->dimensione_mazzo>30)
			pNew->danni=2;

		if(pNew->danni==2)
			strcpy(pNew->nome_carta, "Punizione");
		if(pNew->danni==6)
			strcpy(pNew->nome_carta, "Maledizione");
	}

	if(x>=86&&x<=99){
		pNew->tipo=guarisci_danno;

		if(player->dimensione_mazzo<=30)
			pNew->danni=4;
		if(player->dimensione_mazzo>30)
			pNew->danni=2;

		if(pNew->danni==2)
			strcpy(pNew->nome_carta, "Paura");
		if(pNew->danni==4)
			strcpy(pNew->nome_carta, "Terrore");
	}

	if(x==100){
		pNew->tipo=creatura;

		pNew->danni=15;
		strcpy(pNew->nome_carta, "Signore dei Dannati");
	}

	player->dimensione_mazzo++;
}

static void mazzo_vita(struct Mago *player) {

	struct Carta *pNew=malloc(sizeof(struct Carta));
	pNew->pNext=NULL;

	if(player->inizio_mazzo==NULL)
		player->inizio_mazzo=pNew;
	else {
		pNew->pNext=player->inizio_mazzo;
		player->inizio_mazzo=pNew;
	}

	int x=rand()%100+1;

	if(x>=1&&x<=50){
		pNew->tipo=creatura;

		if(player->dimensione_mazzo<=20)
			pNew->danni=rand()%2+7;
		if(player->dimensione_mazzo<=40&&player->dimensione_mazzo>20)
			pNew->danni=rand()%2+4;
		if(player->dimensione_mazzo<=60&&player->dimensione_mazzo>40)
			pNew->danni=rand()%2+3;

		if(pNew->danni<=3)
			strcpy(pNew->nome_carta, "Soldato Sacro");
		if(pNew->danni==4||pNew->danni==5)
			strcpy(pNew->nome_carta, "Sacerdote");
		if(pNew->danni==7||pNew->danni==8)
			strcpy(pNew->nome_carta, "Legionario Sacro");
	}

	if(x>=51&&x<=65){
		pNew->tipo=rimuovi_creatura;

		strcpy(pNew->nome_carta, "Cessazione");
	}

	if(x>=66&&x<=85){
		pNew->tipo=infliggi_danno;

		if(player->dimensione_mazzo<=30)
			pNew->danni=3;
		if(player->dimensione_mazzo>30)
			pNew->danni=1;

		if(pNew->danni==1)
			strcpy(pNew->nome_carta, "Espiazione");
		if(pNew->danni==3)
			strcpy(pNew->nome_carta, "Blasfemia");
	}

	if(x>=86&&x<=99){
		pNew->tipo=guarisci_danno;

		if(player->dimensione_mazzo<=30)
			pNew->danni=4;
		if(player->dimensione_mazzo>30)
			pNew->danni=2;

		if(pNew->danni==2)
			strcpy(pNew->nome_carta, "Benedizione");
		if(pNew->danni==4)
			strcpy(pNew->nome_carta, "Divina Guarigione");
	}

	if(x==100){
		pNew->tipo=creatura;

		pNew->danni=15;
		strcpy(pNew->nome_carta, "Custode della Vita");
	}

	player->dimensione_mazzo++;

}

static void mazzo_luce(struct Mago *player) {

	struct Carta *pNew=malloc(sizeof(struct Carta));
	pNew->pNext=NULL;

	if(player->inizio_mazzo==NULL)
		player->inizio_mazzo=pNew;
	else {
		pNew->pNext=player->inizio_mazzo;
		player->inizio_mazzo=pNew;
	}

	int x=rand()%100+1;

	if(x>=1&&x<=50){
		pNew->tipo=creatura;

		if(player->dimensione_mazzo<=20)
			pNew->danni=rand()%2+5;
		if(player->dimensione_mazzo<=40&&player->dimensione_mazzo>20)
			pNew->danni=rand()%2+3;
		if(player->dimensione_mazzo<=60&&player->dimensione_mazzo>40)
			pNew->danni=rand()%2+1;

		if(pNew->danni<=2)
			strcpy(pNew->nome_carta, "Predicatore");
		if(pNew->danni==4||pNew->danni==3)
			strcpy(pNew->nome_carta, "Araldo della Dea");
		if(pNew->danni==6||pNew->danni==5)
			strcpy(pNew->nome_carta, "Guerriero Asceso");
	}

	if(x>=51&&x<=65){
		pNew->tipo=rimuovi_creatura;

		strcpy(pNew->nome_carta, "Incenerimento");
	}

	if(x>=66&&x<=85){
		pNew->tipo=infliggi_danno;

		if(player->dimensione_mazzo<=30)
			pNew->danni=3;
		if(player->dimensione_mazzo>30)
			pNew->danni=1;

		if(pNew->danni==1)
			strcpy(pNew->nome_carta, "Luce Splendente");
		if(pNew->danni==3)
			strcpy(pNew->nome_carta, "Luce Abbagliante");
	}

	if(x>=86&&x<=99){
		pNew->tipo=guarisci_danno;

		if(player->dimensione_mazzo<=30)
			pNew->danni=10;
		if(player->dimensione_mazzo>30)
			pNew->danni=7;

		if(pNew->danni==7)
			strcpy(pNew->nome_carta, "Luce Benevola");
		if(pNew->danni==10)
			strcpy(pNew->nome_carta, "Ascensione");
	}

	if(x==100){
		pNew->tipo=creatura;

		pNew->danni=15;
		strcpy(pNew->nome_carta, "Setaka, Dea Guerriera");
	}

	player->dimensione_mazzo++;
}

//La funzione "Pesca Carta" prende la prima carta del mazzo e la mette nella mano del giocatore
static void pesca_carta(struct Mago *player){

	struct Carta *pTemp;

	if(player->inizio_mazzo!=NULL){
		for(int j=0;j<6;){
			if(player->mano[j]==NULL){
				pTemp=player->inizio_mazzo;
				player->mano[j]=pTemp;
				player->inizio_mazzo=player->inizio_mazzo->pNext;
				player->dimensione_mazzo--;
				break;
			}else{
				++j;
				if(j==6){
					pTemp=player->inizio_mazzo;
					player->inizio_mazzo=player->inizio_mazzo->pNext;
					free(pTemp);
				}
			}
		}
	}
	else{
		printf("\t%s, il tuo mazzo è esaurito\n", player->nome_giocatore);
		sleep(3);
	}
}


//La funzione "Stampa Mano" stampa le carte presenti nella mano del giocatore corrispondente
static void stampa_mano(struct Mago *player){
	for(int i=0;i<6;i++){
		if(player->mano[i]!=NULL&&player->mano[i]->tipo==rimuovi_creatura)
			printf("\t%d) %s\n", i+1, player->mano[i]->nome_carta);
		else{
			if(player->mano[i]!=NULL)
				printf("\t%d) %s [%hu]\n", i+1, player->mano[i]->nome_carta, player->mano[i]->danni);
			else
				printf("\t%d) [x]\n", i+1);
		}
	}
}

//La funzione "Combatti" fa iniziare la partita vera e propria
void combatti(){
	if(gameset==0){
		printf("\tErrore, prima di combattere devi impostare il gioco\n");
		sleep(2);
		title();
		return;
	}
	title();

	int turn=rand()%2+1;

	if(turn==1){
		while(1){
			azione=0;
			turn=1;
			turno(&giocatore1, &giocatore2);
			controllo_vittoria(&giocatore1, &giocatore2);

			azione=0;
			turn=2;
			turno(&giocatore2, &giocatore1);
			controllo_vittoria(&giocatore2, &giocatore1);
		}
	}
	if(turn==2){
		while(1){
			azione=0;
			turn=2;
			turno(&giocatore2, &giocatore1);
			controllo_vittoria(&giocatore2, &giocatore1);

			azione=0;
			turn=1;
			turno(&giocatore1, &giocatore2);
			controllo_vittoria(&giocatore1, &giocatore2);
		}
	}
}

//La funzione "Turno" è quella che gestisce i vari turni e le fasi della partita
static void turno(struct Mago *player1, struct Mago *player2){
	
	pesca_carta(player1);

	int selection2;

	do{
		selection2=0;
		title();
		printf("\t%s, è il tuo turno\n\n\t1) Mostra la mano\n\t2) Mostra il campo\n\t3) Gioca una carta\n\t4) Attacca\n\t5) Passa il turno\n", player1->nome_giocatore);
		scanf("%d", &selection2);
		clear();

		switch(selection2) {

			case 1:
				title();
				stampa_mano(player1);
				printf("\n\tPremi INVIO per continuare...\n");
				clear();
				break;

			case 2:
				title();
				stampa_campo(player1);
				printf("\n\tPremi INVIO per continuare...\n");
				clear();
				break;

			case 3:
				if(azione==0)
					gioca_carta(player1, player2);
				else{
					printf("\tHai già fatto la tua mossa, passa il turno\n");
					sleep(2);
					title();
				}
				break;

			case 4:
				if(azione==0)
					attacca(player1, player2);
				else{
					printf("\tHai già fatto la tua mossa, passa il turno\n");
					sleep(2);
					title();
				}
				break;

			case 5:
				azione=1;
				break;

			default:
				printf("\tErrore, inserisci un comando valido\n");
				sleep(2);
		}


	}while(selection2!=5);


}

//La funzione "Stampa Campo" stampa le carte presenti nel campo del giocatore corrispondente
static void stampa_campo(struct Mago *player){
	for(int i=0;i<4;i++){
		if(player->campo[i]!=NULL)
			printf("\t%d) %s [%hu]\n", i+1, player->campo[i]->nome_carta, player->campo[i]->danni);
		else
			printf("\t%d) [x]\n", i+1);
	}
}

//La funzione "Gioca Carta" mostra la propria mano e fa scegliere una carta da giocare
static void gioca_carta(struct Mago *player1, struct Mago *player2){

	int selection3;

	do{
		selection3=0;
		title();
		printf("\t%s, scegli la carta da giocare\n\n", player1->nome_giocatore);
		stampa_mano(player1);
		printf("\n\t7) Annulla\n");
		scanf("%d", &selection3);
		clear();

		switch(selection3){

			case 1:

				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==creatura){
					gioca_creatura(player1, selection3);
					return;
				}
				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==rimuovi_creatura){
					gioca_rimuovi_creatura(player1, player2, selection3);
					return;
				}
				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==infliggi_danno){
					gioca_infliggi_danno(player1, player2, selection3);
					return;
				}
				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==guarisci_danno){
					gioca_guarisci_danno(player1, selection3);
					return;
				}
				break;

			case 2:

				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==creatura){
					gioca_creatura(player1, selection3);
					return;
				}
				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==rimuovi_creatura){
					gioca_rimuovi_creatura(player1, player2, selection3);
					return;
				}
				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==infliggi_danno){
					gioca_infliggi_danno(player1, player2, selection3);
					return;
				}
				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==guarisci_danno){
					gioca_guarisci_danno(player1, selection3);
					return;
				}
				break;

			case 3:

				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==creatura){
					gioca_creatura(player1, selection3);
					return;
				}
				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==rimuovi_creatura){
					gioca_rimuovi_creatura(player1, player2, selection3);
					return;
				}
				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==infliggi_danno){
					gioca_infliggi_danno(player1, player2, selection3);
					return;
				}
				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==guarisci_danno){
					gioca_guarisci_danno(player1, selection3);
					return;
				}
				break;

			case 4:

				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==creatura){
					gioca_creatura(player1, selection3);
					return;
				}
				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==rimuovi_creatura){
					gioca_rimuovi_creatura(player1, player2, selection3);
					return;
				}
				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==infliggi_danno){
					gioca_infliggi_danno(player1, player2, selection3);
					return;
				}
				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==guarisci_danno){
					gioca_guarisci_danno(player1, selection3);
					return;
				}
				break;

			case 5:

				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==creatura){
					gioca_creatura(player1, selection3);
					return;
				}
				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==rimuovi_creatura){
					gioca_rimuovi_creatura(player1, player2, selection3);
					return;
				}
				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==infliggi_danno){
					gioca_infliggi_danno(player1, player2, selection3);
					return;
				}
				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==guarisci_danno){
					gioca_guarisci_danno(player1, selection3);
					return;
				}
				break;

			case 6:

				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==creatura){
					gioca_creatura(player1, selection3);
					return;
				}
				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==rimuovi_creatura){
					gioca_rimuovi_creatura(player1, player2, selection3);
					return;
				}
				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==infliggi_danno){
					gioca_infliggi_danno(player1, player2, selection3);
					return;
				}
				if(player1->mano[selection3-1]!=NULL&&player1->mano[selection3-1]->tipo==guarisci_danno){
					gioca_guarisci_danno(player1, selection3);
					return;
				}
				break;

			case 7:

				break;

			default:
				printf("\tErrore, inserisci un comando valido\n");
				sleep(2);
		}
	}while(selection3!=7);
}

//La funzione "Gioca Creatura" viene chiamata se la carta scelta è una creatura e la schiera in campo
static void gioca_creatura(struct Mago *player, int s){
	for(int i=0;i<4;){
		if(player->campo[i]==NULL){
			player->campo[i]=player->mano[s-1];
			player->mano[s-1]=NULL;
			azione=1;
			break;
		}
		else
			++i;
		if(i==4){
			printf("\tIl tuo campo è pieno, non puoi piazzare altrea carte\n");
			sleep(2);
			break;
		}
	}
}

//La funzione "Gioca Rimuovi Creatura" viene chiamata se la carta selezionata è di tipo rimuovi creatura e la gioca
static void gioca_rimuovi_creatura(struct Mago *player1, struct Mago *player2, int s){

	int selection4;

	do{
		selection4=0;
		title();
		printf("\t%s,scegli il tuo bersaglio:\n\n", player1->nome_giocatore);
		stampa_campo(player2);
		printf("\n\t7) Annulla\n");
		scanf("%d", &selection4);
		clear();

		switch(selection4){

			case 1:
				if(player2->campo[selection4-1]!=NULL&&player2->campo[selection4-1]->tipo==creatura&&player1->mano[s-1]!=NULL){
					printf("\t%s, hai distrutto la carta %s di %s\n", player1->nome_giocatore, player2->campo[selection4-1]->nome_carta, player2->nome_giocatore);
					sleep(3);
					free(player2->campo[selection4-1]);
					free(player1->mano[s-1]);
					player2->campo[selection4-1]=NULL;
					player1->mano[s-1]=NULL;
					azione=1;
				}
				return;
				break;

			case 2:
				if(player2->campo[selection4-1]!=NULL&&player2->campo[selection4-1]->tipo==creatura&&player1->mano[s-1]!=NULL){
					printf("\t%s, hai distrutto la carta %s di %s\n", player1->nome_giocatore, player2->campo[selection4-1]->nome_carta, player2->nome_giocatore);
					sleep(3);
					free(player2->campo[selection4-1]);
					free(player1->mano[s-1]);
					player2->campo[selection4-1]=NULL;
					player1->mano[s-1]=NULL;
					azione=1;
				}
				return;
				break;

			case 3:
				if(player2->campo[selection4-1]!=NULL&&player2->campo[selection4-1]->tipo==creatura&&player1->mano[s-1]!=NULL){
					printf("\t%s, hai distrutto la carta %s di %s\n", player1->nome_giocatore, player2->campo[selection4-1]->nome_carta, player2->nome_giocatore);
					sleep(3);
					free(player2->campo[selection4-1]);
					free(player1->mano[s-1]);
					player2->campo[selection4-1]=NULL;
					player1->mano[s-1]=NULL;
					azione=1;
				}
				return;
				break;

			case 4:
				if(player2->campo[selection4-1]!=NULL&&player2->campo[selection4-1]->tipo==creatura&&player1->mano[s-1]!=NULL){
					printf("\t%s, hai distrutto la carta %s di %s\n", player1->nome_giocatore, player2->campo[selection4-1]->nome_carta, player2->nome_giocatore);
					sleep(3);
					free(player2->campo[selection4-1]);
					free(player1->mano[s-1]);
					player2->campo[selection4-1]=NULL;
					player1->mano[s-1]=NULL;
					azione=1;
				}
				return;
				break;

			case 7:
				return;
				break;

			default:
				printf("\tErrore, inserisci un comando valido\n");
				sleep(2);
		}
	}while(selection4!=7);
}

//La funzione "Gioca Infliggi Danno" viene chiamata se la carta selezionata è di tipo infliggi danno e la gioca
static void gioca_infliggi_danno(struct Mago *player1, struct Mago *player2, int s){

	int selection5;

	do{
		selection5=0;
		title();
		printf("\t%s,scegli il tuo bersaglio:\n\n", player1->nome_giocatore);
		stampa_campo(player2);
		printf("\t5) %s [%hu]\n", player2->nome_giocatore, player2->punti_vita);
		printf("\n\t7) Annulla\n");
		scanf("%d", &selection5);
		clear();

		switch(selection5){

			case 1:
				if(player2->campo[selection5-1]!=NULL&&player2->campo[selection5-1]->tipo==creatura){
					if(player1->mano[s-1]->danni>=player2->campo[selection5-1]->danni){
						printf("\t%s, hai distrutto la carta %s di %s\n", player1->nome_giocatore, player2->campo[selection5-1]->nome_carta, player2->nome_giocatore);
						free(player2->campo[selection5-1]);
						free(player1->mano[s-1]);
						player2->campo[selection5-1]=NULL;
						player1->mano[s-1]=NULL;
						sleep(3);
						azione=1;
						return;
					}
					else{
						free(player1->mano[s-1]);
						player1->mano[s-1]=NULL;
						printf("\t%s, danni non sufficienti, la tua carta Infliggi Danno è stata distrutta\n", player1->nome_giocatore);
						sleep(3);
						azione=1;
						return;
					}
				}
				else
					return;
				break;

			case 2:
				if(player2->campo[selection5-1]!=NULL&&player2->campo[selection5-1]->tipo==creatura){
					if(player1->mano[s-1]->danni>=player2->campo[selection5-1]->danni){
						printf("\t%s, hai distrutto la carta %s di %s\n", player1->nome_giocatore, player2->campo[selection5-1]->nome_carta, player2->nome_giocatore);
						free(player2->campo[selection5-1]);
						free(player1->mano[s-1]);
						player2->campo[selection5-1]=NULL;
						player1->mano[s-1]=NULL;
						sleep(3);
						azione=1;
						return;
					}
					else{
						free(player1->mano[s-1]);
						player1->mano[s-1]=NULL;
						printf("\t%s, danni non sufficienti, la tua carta Infliggi Danno è stata distrutta\n", player1->nome_giocatore);
						sleep(3);
						azione=1;
						return;
					}
				}
				else
					return;
				break;

			case 3:
				if(player2->campo[selection5-1]!=NULL&&player2->campo[selection5-1]->tipo==creatura){
					if(player1->mano[s-1]->danni>=player2->campo[selection5-1]->danni){
						printf("\t%s, hai distrutto la carta %s di %s\n", player1->nome_giocatore, player2->campo[selection5-1]->nome_carta, player2->nome_giocatore);
						free(player2->campo[selection5-1]);
						free(player1->mano[s-1]);
						player2->campo[selection5-1]=NULL;
						player1->mano[s-1]=NULL;
						sleep(3);
						azione=1;
						return;
					}
					else{
						free(player1->mano[s-1]);
						player1->mano[s-1]=NULL;
						printf("\t%s, danni non sufficienti, la tua carta Infliggi Danno è stata distrutta\n", player1->nome_giocatore);
						sleep(3);
						azione=1;
						return;
					}
				}
				else
					return;
				break;

			case 4:
				if(player2->campo[selection5-1]!=NULL&&player2->campo[selection5-1]->tipo==creatura){
					if(player1->mano[s-1]->danni>=player2->campo[selection5-1]->danni){
						printf("\t%s, hai distrutto la carta %s di %s\n", player1->nome_giocatore, player2->campo[selection5-1]->nome_carta, player2->nome_giocatore);
						free(player2->campo[selection5-1]);
						free(player1->mano[s-1]);
						player2->campo[selection5-1]=NULL;
						player1->mano[s-1]=NULL;
						sleep(3);
						azione=1;
						return;
					}
					else{
						free(player1->mano[s-1]);
						player1->mano[s-1]=NULL;
						printf("\t%s, danni non sufficienti, la tua carta Infliggi Danno è stata distrutta\n", player1->nome_giocatore);
						sleep(3);
						azione=1;
						return;
					}
				}
				else
					return;
				break;

			case 5:
				player2->punti_vita-=player1->mano[s-1]->danni;
				if(player2->punti_vita>1000)
					player2->punti_vita=0;
				printf("\t%s, hai inflitto [%hu] danni a %s. Nuovi punti vita di %s: [%hu]\n", player1->nome_giocatore, player1->mano[s-1]->danni, player2->nome_giocatore, player2->nome_giocatore, player2->punti_vita);
				free(player1->mano[s-1]);
				player1->mano[s-1]=NULL;
				sleep(3);
				azione=1;
				return;
				break;

			case 7:
				return;
				break;

			default:
				printf("\tErrore, inserisci un comando valido\n");
				sleep(2);
		}
	}while(selection5!=7);
}

//La funzione "Gioca Guarisci Danno" viene chiamata quando la carta scelta è ti tipo guarisci danno e la gioca
static void gioca_guarisci_danno(struct Mago* player, int s){

	if(player->punti_vita<20)
		player->punti_vita+=player->mano[s-1]->danni;
	if(player->punti_vita>20)
		player->punti_vita=20;
	free(player->mano[s-1]);
	player->mano[s-1]=NULL;
	azione=1;
	printf("\t%s, ti sei curato. Adesso hai [%hu] Punti Vita\n", player->nome_giocatore, player->punti_vita);
	sleep(3);
	title();
}

//La funzione "Attacca" viene chiamata quando il giocatore decide di attaccare con una creatura durante il suo turno
static void attacca(struct Mago *player1, struct Mago *player2){

	int selection6;

	do{
		selection6=0;
		title();
		printf("\t%s, scegli con che creatura attaccare:\n\n", player1->nome_giocatore);
		stampa_campo(player1);
		printf("\n\t7) Annulla\n");
		scanf("%d", &selection6);
		clear();

		switch(selection6){

			case 1:
				if(player1->campo[selection6-1]!=NULL&&player1->campo[selection6-1]->tipo==creatura){
					lotta(player1, player2, selection6);
					return;
				}
				break;

			case 2:

				if(player1->campo[selection6-1]!=NULL&&player1->campo[selection6-1]->tipo==creatura){
					lotta(player1, player2, selection6);
					return;
				}
				break;

			case 3:

				if(player1->campo[selection6-1]!=NULL&&player1->campo[selection6-1]->tipo==creatura){
					lotta(player1, player2, selection6);
					return;
				}
				break;

			case 4:

				if(player1->campo[selection6-1]!=NULL&&player1->campo[selection6-1]->tipo==creatura){
					lotta(player1, player2, selection6);
					return;
				}
				break;

			case 7:
				return;
				break;

			default:
				printf("\tErrore, inserisci un comando valido\n");
				sleep(2);
		}

	}while(selection6!=7);
}

//La funzione "Lotta" viene chiamata quando il giocatore ha scelto con che creatura attaccare e che creatura nemica attaccare e confronta i Punti Vita per decidere quale creatura distruggere
static void lotta(struct Mago *player1, struct Mago *player2, int s){

	int selection7;

	do{
		selection7=0;
		title();
		printf("\t%s, scegli chi attaccare:\n\n", player1->nome_giocatore);
		stampa_campo(player2);
		printf("\t5) %s [%hu]", player2->nome_giocatore, player2->punti_vita);
		printf("\n\n\t7) Annulla\n");
		scanf("%d", &selection7);
		clear();

		switch(selection7){

			case 1:
				if(player2->campo[selection7-1]!=NULL&&player1->campo[s-1]->danni>player2->campo[selection7-1]->danni){
					printf("\t%s di %s ha distrutto %s di %s\n", player1->campo[s-1]->nome_carta, player1->nome_giocatore, player2->campo[selection7-1]->nome_carta, player2->nome_giocatore);
					sleep(3);
					free(player2->campo[selection7-1]);
					player2->campo[selection7-1]=NULL;
					azione=1;
					return;
				}
				else{
					if(player2->campo[selection7-1]!=NULL){
						printf("\t%s di %s ha distrutto %s di %s\n", player2->campo[selection7-1]->nome_carta, player2->nome_giocatore, player1->campo[s-1]->nome_carta, player1->nome_giocatore);
						sleep(3);
						free(player1->campo[s-1]);
						player1->campo[s-1]=NULL;
						azione=1;
						return;
					}
				}
				break;

			case 2:
				if(player2->campo[selection7-1]!=NULL&&player1->campo[s-1]->danni>player2->campo[selection7-1]->danni){
					printf("\t%s di %s ha distrutto %s di %s\n", player1->campo[s-1]->nome_carta, player1->nome_giocatore, player2->campo[selection7-1]->nome_carta, player2->nome_giocatore);
					sleep(3);
					free(player2->campo[selection7-1]);
					player2->campo[selection7-1]=NULL;
					azione=1;
					return;
				}
				else{
					if(player2->campo[selection7-1]!=NULL){
						printf("\t%s di %s ha distrutto %s di %s\n", player2->campo[selection7-1]->nome_carta, player2->nome_giocatore, player1->campo[s-1]->nome_carta, player1->nome_giocatore);
						sleep(3);
						free(player1->campo[s-1]);
						player1->campo[s-1]=NULL;
						azione=1;
						return;
					}
				}
				break;

			case 3:
				if(player2->campo[selection7-1]!=NULL&&player1->campo[s-1]->danni>player2->campo[selection7-1]->danni){
					printf("\t%s di %s ha distrutto %s di %s\n", player1->campo[s-1]->nome_carta, player1->nome_giocatore, player2->campo[selection7-1]->nome_carta, player2->nome_giocatore);
					sleep(3);
					free(player2->campo[selection7-1]);
					player2->campo[selection7-1]=NULL;
					azione=1;
					return;
				}
				else{
					if(player2->campo[selection7-1]!=NULL){
						printf("\t%s di %s ha distrutto %s di %s\n", player2->campo[selection7-1]->nome_carta, player2->nome_giocatore, player1->campo[s-1]->nome_carta, player1->nome_giocatore);
						sleep(3);
						free(player1->campo[s-1]);
						player1->campo[s-1]=NULL;
						azione=1;
						return;
					}
				}
				break;

			case 4:
				if(player2->campo[selection7-1]!=NULL&&player1->campo[s-1]->danni>player2->campo[selection7-1]->danni){
					printf("\t%s di %s ha distrutto %s di %s\n", player1->campo[s-1]->nome_carta, player1->nome_giocatore, player2->campo[selection7-1]->nome_carta, player2->nome_giocatore);
					sleep(3);
					free(player2->campo[selection7-1]);
					player2->campo[selection7-1]=NULL;
					azione=1;
					return;
				}
				else{
					if(player2->campo[selection7-1]!=NULL){
						printf("\t%s di %s ha distrutto %s di %s\n", player2->campo[selection7-1]->nome_carta, player2->nome_giocatore, player1->campo[s-1]->nome_carta, player1->nome_giocatore);
						sleep(3);
						free(player1->campo[s-1]);
						player1->campo[s-1]=NULL;
						azione=1;
						return;
					}
				}
				break;

			case 5:
				player2->punti_vita-=player1->campo[s-1]->danni;
				if(player2->punti_vita>1000)
					player2->punti_vita=0;
				printf("\t%s viene attaccato direttamente e subisce [%hu] danni. Nuovi Punti Vita di %s: [%hu]\n", player2->nome_giocatore, player1->campo[s-1]->danni, player2->nome_giocatore, player2->punti_vita);
				sleep(3);
				azione=1;
				return;
				break;

			case 7:
				return;
				break;

			default:
				printf("\tErrore, inserisci un comando valido\n");
				sleep(2);
		}
	}while(selection7!=7);
}

//La funzione "Controllo Vittoria" si occupa di controllare se uno dei due giocatori rispetti le condizioni necessarie per terminare la partita
static void controllo_vittoria(struct Mago *player1, struct Mago *player2){
	if(player2->punti_vita==0){
		printf("\t%s ha esaurito i Punti Vita. %s vince la partita!\n", player2->nome_giocatore, player1->nome_giocatore);
		sleep(3);
		system("clear");
		termina_gioco();
		exit(0);
	}
	if(player1->punti_vita==0){
		printf("\t%s ha esaurito i Punti Vita. %s vince la partita!\n", player1->nome_giocatore, player2->nome_giocatore);
		sleep(3);
		system("clear");
		termina_gioco();
		exit(0);
	}
	if(player1->inizio_mazzo==NULL){
		printf("\t%s non ha più carte nel mazzo. %s vince la partita!\n", player2->nome_giocatore, player1->nome_giocatore);
		sleep(3);
		system("clear");
		termina_gioco();
		exit(0);
	}
	if(player2->inizio_mazzo==NULL){
		printf("\t%s non ha più carte nel mazzo. %s vince la partita!\n", player1->nome_giocatore, player2->nome_giocatore);
		sleep(3);
		system("clear");
		termina_gioco();
		exit(0);
	}
}