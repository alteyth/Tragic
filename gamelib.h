//Definizioni degli enum
enum Classe_mago {tenebre, vita, luce};
enum Tipo_carta {creatura, rimuovi_creatura, infliggi_danno, guarisci_danno};

//Definizioni delle strutture
struct Carta {
	char nome_carta[21];
	enum Tipo_carta tipo;
	unsigned short danni;
	struct Carta *pNext;
};

struct Mago {
	char nome_giocatore[256];
	unsigned short punti_vita;
	enum Classe_mago classe;
	struct Carta *inizio_mazzo;
	unsigned short dimensione_mazzo;
	struct Carta *mano[6];
	struct Carta *campo[4];
};

//Dichiarazioni delle funzioni
void title();
void main_menu();
void ost();
void clear();
void imposta_gioco();
void combatti();
void termina_gioco();
