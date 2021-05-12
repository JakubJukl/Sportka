#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

#define POCET_CISEL_V_TICKETU 6
#define MAX_SLOUPECKU_V_TICKETU 10
#define POCET_TAHU 10
#define MAX_POCET_CISLIC_VSAZENEHO_CISLA  2
#define CENA_SLOUPECKU 20
#define MAX_VSAZENE_CISLO 49
#define SOUHLAS 'a'

long KolikJsemUhodl(int sloupecky, int cislaNaTiketu[][POCET_CISEL_V_TICKETU], int vylosovanaCisla[][POCET_CISEL_V_TICKETU], int dodatkoveCislo[], char chciPorovnat);
int JeCislo(char cisloJakoText[]);
char NactiZnak(void);
void ClearBuffer(void);
void NovyRadek(void);
void VypisCislaVPoli(int cislaVPoli[], int kolikVypsat);
int Sloupce(long *penize);
int VybranaCisla(int vsazenaCisla[], int cislo, int sloup);
void Konec (FILE **uloz, const char *JMENO_SOUBORU_NA_ULOZENI, long penize, long pocatecniCastka);

int main()
{
  const long PENIZE_DO_ZACATKU = 10000;
  const char *JMENO_SOUBORU_NA_ULOZENI = "save.txt";
  FILE *uloz;

  int j;    //univerzalni promenna pro for cykly
  int cislo;
  int sloup;
  int tah;
  char posledniNapsanyZnak;
  int vsazenaCisla[POCET_CISEL_V_TICKETU];
  int sloupecky;
  int cislaNaTiketu[MAX_SLOUPECKU_V_TICKETU][POCET_CISEL_V_TICKETU];
  int vylosovanaCisla[POCET_TAHU][POCET_CISEL_V_TICKETU];
  int dodatkoveCislo[POCET_TAHU];
  long penize;
  long vyhra;
  long pocatecniCastka;
  int opakovat;


  srand(time(0));   //randomizace pro vypocet pseudo nahodnych cisel
  printf("Dobry den,\nvitejte u me varianty hry Sportka.\n");
  if ((uloz = fopen(JMENO_SOUBORU_NA_ULOZENI, "r+")) != NULL) {     //pasaz pro nacteni souboru s ulozenym vysledkem, pokud existuje
    printf("Chcete nacist posledni save? (%c/n) : ", SOUHLAS);
    if (NactiZnak() == SOUHLAS) {
      fscanf(uloz, "%li",&penize);
      if (fclose(uloz) == EOF) {
        printf("Nemohl se uzavrit soubor se savem: %s\n", JMENO_SOUBORU_NA_ULOZENI);
      }
    } else {
      penize = PENIZE_DO_ZACATKU;
    }
  } else {
    penize = PENIZE_DO_ZACATKU;
  }
  pocatecniCastka = penize;     //pouzijeme pro vypocet bilance pri ukonceni programu
  printf("Zacinate s %li Kc na vasem uctu.\n", penize);
  printf("Chcete si vsadit? (%c/n) : ",SOUHLAS);
  if (NactiZnak() != SOUHLAS) {
    Konec(&uloz, JMENO_SOUBORU_NA_ULOZENI, penize, pocatecniCastka);    //podprogram pro ulozeni a ukonceni programu
    return 0;
  }
  do {
    if (penize < CENA_SLOUPECKU) {      //ma uzivatel dost penez na alespon jeden sloupecek?
      printf("Nemate dost penez na pokracovani ve hre. Chcete ji ukoncit? (%c/n) : ",SOUHLAS);
      if (NactiZnak() == SOUHLAS) {
        Konec(&uloz, JMENO_SOUBORU_NA_ULOZENI, penize, pocatecniCastka);
        return 0;
      } else {
        printf("Chcete si pujcit %li Kc? (%c/n) : ", PENIZE_DO_ZACATKU, SOUHLAS); //poskytneme bezurocnou pujcku
        if (NactiZnak() == SOUHLAS) {
          penize += PENIZE_DO_ZACATKU;
          pocatecniCastka += PENIZE_DO_ZACATKU;     //pujcene penize nesmi mit vliv na finalni bilanci
          printf("Bylo vam pripsano %li Kc. \nAktualni stav konta je : %li Kc.\n\n", PENIZE_DO_ZACATKU, penize);
        }
      }
    }
    do {
      sloupecky = Sloupce(&penize);     //funkce pro zjisteni poctu sloupecku a veci s tim spojenych
    } while (sloupecky == 0);           //vrati-li funkce 0, pak se to musi provest znovu, kvuli spatnemu inputu uzivatele

    for (sloup = 0; sloup < sloupecky; sloup++) {   //tolikrat, kolik je sloupecku musime vsadit cisla do tiketu
      printf("Chcete nahodne vygenerovat cisla do %i. sloupecku? (%c/n) : ",sloup+1,SOUHLAS);    //moznost vsazeni nahodnych cisel
      if (NactiZnak() == SOUHLAS) {
        printf("Vygenerovana cisla jsou : ");
        for (cislo = 0; cislo < POCET_CISEL_V_TICKETU; cislo++) {
          do {
            cislaNaTiketu[sloup][cislo] = (rand() % MAX_VSAZENE_CISLO) + 1;
            for (j = 0; j < cislo; j++) {
              if (cislaNaTiketu[sloup][j] == cislaNaTiketu[sloup][cislo]) {
                opakovat = 1;
                break;
              } else
                opakovat = 0;
            }
          } while (opakovat == 1);

          if(cislo == 0)
            printf(" %i ", cislaNaTiketu[sloup][cislo]);
          else {
            printf(", %i ", cislaNaTiketu[sloup][cislo]);
          }
        }
        NovyRadek();    //konec algoritmu pro generaci nahodnych cisel do sloupecku
      } else {
        for (cislo = 0; cislo < POCET_CISEL_V_TICKETU; cislo++) {
          do {
            cislaNaTiketu[sloup][cislo] = VybranaCisla(vsazenaCisla, cislo, sloup); //funkce pro vybrani cisel do tiketu uzivatelem
          } while (cislaNaTiketu[sloup][cislo] == 0);   //vrati-li funkce 0, pak uzivatel zadal neco zle, musi se to provest znovu
        }
      }
    }
    for (tah = 0; tah < POCET_TAHU; tah++) {
      printf("\nDnes v %i. tahu byla tazena tato cisla: ", tah+1);
      for (cislo = 0; cislo < POCET_CISEL_V_TICKETU; cislo++) {
        do {
          vylosovanaCisla[tah][cislo] = (rand() % MAX_VSAZENE_CISLO) + 1;
          for (j = 0; j < cislo; j++) {         //kontrola, jestli program nevylosoval 2x stejne cislo
            if (vylosovanaCisla[tah][cislo] == vylosovanaCisla[tah][j]) {
              opakovat = 1;
              break;
            } else {
              opakovat = 0;
            }
          }
        } while (opakovat == 1);    //pokud vylosoval stejne cislo, tak musi losovat znovu, dokud nevylosuje jine
        if(cislo == 0)
          printf(" %i ", vylosovanaCisla[tah][cislo]);  //jednoducha podminka pro hezke vypsani cisel, prvni bude bez carky pred nim
        else
          printf(", %i ", vylosovanaCisla[tah][cislo]);
      }
      do {
        dodatkoveCislo[tah] = (rand() % MAX_VSAZENE_CISLO) + 1; //losovani dodatkoveho cisla
        for (j = 0; j < POCET_CISEL_V_TICKETU; j++) {
          if (dodatkoveCislo[tah] == vylosovanaCisla[tah][j]) {     //stejna podminka, jako u losovani tahu
            opakovat = 1;
            break;
          } else {
            opakovat = 0;
          }
        }
      } while (opakovat == 1);
      printf("\nA dodatkove cislo %i. tahu je: %i\n", tah+1, dodatkoveCislo[tah]);
    }
    printf("\nChcete, abych za Vas porovnal vsazena cisla s vyhernimi? (%c/n) : ",SOUHLAS);
    posledniNapsanyZnak = NactiZnak();
    if  (posledniNapsanyZnak == SOUHLAS) {
      vyhra = KolikJsemUhodl(sloupecky, cislaNaTiketu, vylosovanaCisla, dodatkoveCislo, posledniNapsanyZnak);   //funkce pro vypocet vyhry
      penize += vyhra;
    } else {
      printf("Chcete, abych vypsal vsazena cisla? (%c/n) : ",SOUHLAS);
      if (NactiZnak() == SOUHLAS) {
        for (sloup = 0; sloup < sloupecky; sloup++) {       //jen vypise vsechny cleny v poli
          printf("%i. sloupecek: ", sloup+1);
          for (cislo = 0; cislo < POCET_CISEL_V_TICKETU; cislo++) {
            if (cislo == 0)
              printf("%i ", cislaNaTiketu[sloup][cislo]);
            else
              printf(", %i ", cislaNaTiketu[sloup][cislo]);
          }
          NovyRadek();
        }
      }
      do {
        printf("\nMohu Vam uz prozradit vyhru? (%c/n) : ",SOUHLAS);      //dokud nebude chtit znat uzivatel vyhru, tak se ho budeme stale ptat
      } while (NactiZnak() != SOUHLAS);
      vyhra = KolikJsemUhodl(sloupecky, cislaNaTiketu, vylosovanaCisla, dodatkoveCislo, 'n');
      penize += vyhra;
    }
    NovyRadek();
    printf("Aktualni stav konta = %li Kc\n", penize);
    printf("Chcete si opet vsadit? (%c/n) : ",SOUHLAS);
  } while (NactiZnak() == SOUHLAS);
  Konec(&uloz, JMENO_SOUBORU_NA_ULOZENI, penize, pocatecniCastka);
  return 0;
}

void ClearBuffer()
{
  while ((getchar()) != '\n'); //mozna zbytecna samostatna funkce, ale pro moje zjednoduseni existuje
}
char NactiZnak()        //fce pro nacteni uzivatelova inputu
{
  char prectenyZnak;
  prectenyZnak = getchar();
  ClearBuffer();

  return tolower(prectenyZnak);     //uzivatel muze zadat jak velke 'A', tak male 'a'
}
int JeCislo(char cisloJakoText[])   //overeni, jestli je zadany znak ciselneho charakteru
{
  int delkaCisla;
  int i;

  delkaCisla = strlen(cisloJakoText);
  for (i = 0; i < delkaCisla; i++) {
    if (!isdigit(cisloJakoText[i])) {
      return 0;
    }
  }
  return 1;
}

void VypisCislaVPoli(int cislaVPoli[], int kolikVypsat)     //vypise zadany pocet prvku z pole
{
  int i;

  for (i = 0; i < kolikVypsat; i++) {
    if (i == 0)
      printf(" %i ",cislaVPoli[i]);
    else
      printf(", %i ",cislaVPoli[i]);
  }
}

void NovyRadek()    //jednoducha funkce pro novy radek bez nutnosti psat znak '\'
{
  printf("\n");
}

void Initializuj2DIntPole(int prvni_rozmer, int druhy_rozmer, int pole[][druhy_rozmer])   //vvycisteni adres od predchozich cisel
{
  int i;
  int j;

  for (i = 0; i < prvni_rozmer; i++) {
    for (j = 0; j < druhy_rozmer; j++) {
      pole[i][j] = 0;
    }
  }
}

void Initializuj1DIntPole(int rozmer, int pole[rozmer])   //vvycisteni adres od predchozich cisel
{
  int i;

  for (i = 0; i < rozmer; i++) {
    pole[i] = 0;
  }
}

long VypocitejVyhru(int sloupecky, int uhodlDodatkove[], int vyherniCisla[][sloupecky])
{
  //fce pro urceni vyhry uzivatele za dane kolo
  const long JACKPOT = 15382198;    //vypis moznych vyher
  const long DRUHA = 815723;
  const long TRETI = 24971;
  const long CTVRTA = 619;
  const long PATA = 113;
  const int PATA_SPRAVNYCH_CISEL = 3;
  const int CTVRTA_SPRAVNYCH_CISEL = 4;
  const int TRETI_SPRAVNYCH_CISEL = 5;
  const int DRUHA_SPRAVNYCH_CISEL = 5;
  const int JACKPOT_SPRAVNYCH_CISEL = 6;
  long vyhra = 0;
  int sloup;
  int tah;

  for (tah = 0; tah < POCET_TAHU; tah++) {
    for (sloup = 0; sloup < sloupecky; sloup++) { //zkontroluj kazdy sloupecek
      if  (vyherniCisla[tah][sloup] > PATA_SPRAVNYCH_CISEL - 1) {      //hezci implementace by mohla byt pres CASE, ale tvrzeni neni tolik,
        if (vyherniCisla[tah][sloup] == PATA_SPRAVNYCH_CISEL)          //aby mi to vadilo
          vyhra += PATA;
        else if (vyherniCisla[tah][sloup] == CTVRTA_SPRAVNYCH_CISEL)
          vyhra += CTVRTA;
        else if (vyherniCisla[tah][sloup] == TRETI_SPRAVNYCH_CISEL && uhodlDodatkove[tah] == 0)
          vyhra += TRETI;
        else if (vyherniCisla[tah][sloup] == DRUHA_SPRAVNYCH_CISEL && uhodlDodatkove[tah] == 1)
          vyhra += DRUHA;
        else if (vyherniCisla[tah][sloup] == JACKPOT_SPRAVNYCH_CISEL)
          vyhra += JACKPOT;
      }
    }
  }
  return vyhra;
}

long KolikJsemUhodl(int sloupecky, int cislaNaTiketu[][POCET_CISEL_V_TICKETU], int vylosovanaCisla[][POCET_CISEL_V_TICKETU], int dodatkoveCislo[], char chciPorovnat)
{
  //fce pro urceni, kolik cisel uzivatel uhodl a nasledne vypocitani vyhry s fce VypocitejVyhru
  int j;
  int sloup;
  int cislo;
  int vyherniCisla[POCET_TAHU][sloupecky];
  int uhodlDodatkove[POCET_TAHU];
  long penize = 0;
  int tah;

  Initializuj2DIntPole(POCET_TAHU, sloupecky, vyherniCisla);    //vycisteni poli
  Initializuj1DIntPole(POCET_TAHU, uhodlDodatkove);
  for (sloup = 0; sloup < sloupecky; sloup++) {
    for (cislo = 0; cislo < POCET_CISEL_V_TICKETU; cislo++) {
      for (j = 0; j < POCET_CISEL_V_TICKETU; j++) {     //porovnani vyhernich cisel se vsazenymi
        for (tah = 0; tah < POCET_TAHU; tah++) {
          if (cislaNaTiketu[sloup][cislo] == vylosovanaCisla[tah][j])
            vyherniCisla[tah][sloup]++;
          else if (cislaNaTiketu[sloup][cislo] == dodatkoveCislo[tah])
            uhodlDodatkove[tah] = 1;
        }
      }
    }
    if (chciPorovnat == SOUHLAS) {
      for (tah = 0; tah < POCET_TAHU; tah++) {
        if (vyherniCisla[tah][sloup] != 0)
          printf("Cisel uhodnuto v %i. sloupecku v %i. tahu: %i\n",sloup+1,tah+1,vyherniCisla[tah][sloup]);
      }
    }
  }
  penize = VypocitejVyhru(sloupecky, uhodlDodatkove, vyherniCisla);
  printf("Vase vyhra = %li Kc\n", penize);
  return penize;
}

int Sloupce(long *penize)       //fce pro zjisteni poctu sloupecku, ktere uzivatel vsadil
{
  char sloupeckyText[MAX_POCET_CISLIC_VSAZENEHO_CISLA+1];
  int sloupecky;
  long cena;

  printf("Kolik sloupecku (cena za sloupecek = %i Kc) chcete vsadit? (1 - %i) : ", CENA_SLOUPECKU, MAX_SLOUPECKU_V_TICKETU);
  scanf("%2s", sloupeckyText);  //skenuji jen pro 2 znaky, vetsi cisla, stejne neakceptuji
  ClearBuffer();
  if (JeCislo(sloupeckyText) == 0) {
    printf("Pocet sloupecku musi byt cislo!\n\n");
    return 0;
  }
  sscanf( sloupeckyText, "%i", &sloupecky );    //prevod textu na cislo
  if (sloupecky > MAX_SLOUPECKU_V_TICKETU || sloupecky < 1) {
    printf("Pocet sloupecku musi byt v intervalu <1;%i>!\n\n",MAX_SLOUPECKU_V_TICKETU);
    return 0;
  }
  cena = sloupecky * CENA_SLOUPECKU;
  if (cena > *penize) {
    printf("Nemate dost penez na vsazeni tolika sloupecku.\n\n");
    return 0;
  } else
    *penize = *penize - cena;
  return sloupecky;
}

int VybranaCisla(int vsazenaCisla[], int cislo, int sloup)  //fce pro ziskani cisel, ktera vsazi uzivatel do sloupecku
{
  char vsazenaCislaText[POCET_CISEL_V_TICKETU][MAX_POCET_CISLIC_VSAZENEHO_CISLA+1];
  int j;

  printf("Vase %i. cislo (1 - %i) %i. sloupecku",cislo+1, MAX_VSAZENE_CISLO,sloup+1);
  if (cislo > 0) {
    printf(" krome uz zadanych:");
    VypisCislaVPoli(vsazenaCisla, cislo);
    printf(" = ");
  } else
    printf(" = ");
  scanf("%2s", vsazenaCislaText[cislo]);
  ClearBuffer();
  if (JeCislo(vsazenaCislaText[cislo]) == 0) {
    printf("Nenapsal jste cislo!\n\n");
    return 0;
  }
  sscanf( vsazenaCislaText[cislo], "%i", &vsazenaCisla[cislo] );
  if (vsazenaCisla[cislo] > MAX_VSAZENE_CISLO || vsazenaCisla[cislo] < 1) {
    printf("Vsazene cislo musi byt v intervalu <1;%i>!\n\n",MAX_VSAZENE_CISLO);
    return 0;
  }
  for (j = 0; j < cislo; j++) {
    if (vsazenaCisla[cislo] == vsazenaCisla[j]) {
      printf("Nemuzes vsadit jedno cislo dvakrat v jednom sloupecku!\n\n");
      return 0;
    }
  }
  return vsazenaCisla[cislo];
}

void Konec (FILE **uloz, const char *JMENO_SOUBORU_NA_ULOZENI, long penize, long pocatecniCastka) //fce pro ulozeni
{
  printf("Mam hru ulozit? (%c/n) : ",SOUHLAS);
  if (NactiZnak() == SOUHLAS) {
    *uloz = fopen(JMENO_SOUBORU_NA_ULOZENI, "w");
    if (*uloz == NULL) {
      printf("Nemohl se vytvorit soubor pro ulozeni: %s\n", JMENO_SOUBORU_NA_ULOZENI);
    } else {
      fprintf(*uloz, "%li",penize);
      if (fclose(*uloz) == EOF) {
        printf("Nemohl se uzavrit soubor pro ulozeni: %s\n", JMENO_SOUBORU_NA_ULOZENI);
      } else {
        printf("Hra byla uspesne ulozena.");
      }
    }
  }
  printf("\nVase bilance za toto sezeni = %li Kc\n",penize - pocatecniCastka);
}


