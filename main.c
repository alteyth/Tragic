#include "gamelib.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>


int main()
{	
	int selection;
	do{
		selection=0;
		title();
		main_menu();
		scanf("%d", &selection);
		clear();
		switch(selection) {

			case 1:
				imposta_gioco();
				break;

			case 2:
				combatti();
				break;

			case 3:
				ost();
				break;

			case 4:
				termina_gioco();
				break;

			default:
				printf("\tErrore, inserisci un comando valido\n");
				sleep(2);
		}
	}while(selection!=4);
}