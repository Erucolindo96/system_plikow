#ifndef DYSK_H
#define DYSK_H

#include<stdlib.h>
#include<stdio.h>

#define ROZMIAR_BLOKU 2048
#define MAX_NAZWA 62
#define GOOD 1
#define BAD 0
#define PLIK_DYSKU "WirtualnyDysk.vdc"

/**
  * Operacje na dysku:
  * 1.Tworzenie dysku:
  * -Oblicz ilośc potrzebnych bloków
  * -Zaallokuj odpowiednio dużą ilośc pamięci
  * -Zapisz nowo utworzony dysk do pliku
  *
  * 2.Otwiernia dysku:
  * -Otwórz plik z dyskiem
  * -Pobierz jego wielkość
  * -Zaallokuj pamięć dla dysku
  * -Załaduj do zmiennej Dysk zawartość pliku z dyskiem
  *
  *
  *
  *
  */


typedef struct Blok
{
    char zawartosc[ROZMIAR_BLOKU];
}Blok;

typedef struct DeskryptorBloku
{
    char jestZajety, jestPoczatkiemPliku;
    char nazwa_bloku[MAX_NAZWA];
    unsigned int zajeta_pamiec;
    int nastepny;//indeks następnego deksryptora(jeśli plik jest zawarty w kilku blokach) lub -1, gdy blok jest ostatnim blokiem pliku

}DeskryptorBloku;

typedef struct Dysk
{
    //FILE *plik_dysku;//deskryptor pliku, w którym zapisujemy wirtualny dysk
    unsigned int ilosc_blokow;
    DeskryptorBloku* tablica_deskryptorow;
    Blok* tablica_blokow;



}Dysk;




//public

int stworzDysk(Dysk* dysk, unsigned int rozmiar_dysku);//rozmiar dysku w kilobajtach
int otworzDysk(Dysk* dysk);

int usunDysk(Dysk* dysk);





#endif // DYSK_H

