#ifndef DYSK_H
#define DYSK_H



#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define ROZMIAR_BLOKU 2048
#define MAX_NAZWA 62
#define NAZWA_DYSKU "virtualDisk.vdc"

//wartości zwracane przez metody



#define GOOD 0
#define BAD 1
#define FILE_ERROR 2
#define ALLOC_ERROR 3
#define READ_ERROR 4
#define WRITE_ERROR 5
#define NO_MEMORY 6
#define NOT_FOUND 7
#define FILE_EXIST 8

#define TRUE 1
#define FALSE 0




/**
  *Sekcja private:
  * 1.Zwroc ilośc wolnego miejsca
  * -ilosc_wolnych_blokow = 0;
  * -Przejdź po tablicy deksryptorów
  * -Jeśli deskryptor wskazuje na blok wolny - ++ilosc_wolnych_blokow
  * -Zwróć ilość_wolnych_blokow * ROZMIAR_BLOKU
  *
  *
  * Operacje na dysku:
  * 1.Tworzenie dysku:
  * -Zaallokuj dysk
  * -Oblicz ilośc potrzebnych bloków
  * -Zaallokuj odpowiednio dużą ilośc pamięci dla dysku
  * -Zapisz nowo utworzony dysk do pliku
  *
  * 2.Otwiernia dysku:
  * -Otwórz plik z dyskiem
  * -Pobierz jego wielkość
  * -Zaallokuj pamięć dla dysku
  * -Załaduj do zmiennej Dysk zawartość pliku z dyskiem
  * -Zamknij plik z dyskiem
  *
  * 3. Zamykanie Dysku:
  * -Zapisz dysk do pliku o podanej nazwie
  * -Deallokuj pamięć dysku
  * -Deallokuj dysk
  *
  * 4.Kopiuj plik z dysku linuxa do wirtualnego
  * -Otwieramy plik o danej nazwie z dysku linuxa do pamięci
  * -Sprawdzamy, czy zmieści się na dysku - jeśli nie zwracamy błąd
  * -Szukamy pierwszego wolnego bloku na dysku i zapisujemy początek pliku, jego nazwę i odpowiednie flagi
  * -Dokopi nie zapiszemy całego pliku - szukamy kolejnego wolnego bloku, łączymy go z poprzednim i ładujemy tak fragment pliku
  * -Zapisz stan wirtualnego dysku do jego pliku
  *
  * 5.Kopiuj plik z wirtualnego dysku do linuxa
  * -Szukamy deskryptora z nazwą naszego pliku
  * -Jeśli go znaleźliśmy, to otwieramy w linuxie taki plik/lub go tworzymy
  * -Kopiujemy do niego po kolei dane z dysku
  *
  * 6.Usuń plik z wirtualnego dysku
  * -Znajdź blok początkowy pliku - jeśli nie ma, to znaczy ze nie ma takiego pliku
  * -Kolejne deskryptory pliku ustaw jako puste
  * -Zapisz stan wirtualnego dysku do jego pliku
  *
  */


typedef struct Blok
{
    char zawartosc[ROZMIAR_BLOKU];
}Blok;

typedef struct DeskryptorBloku
{
    char jestZajety, jestPoczatkiemPliku;
    char nazwa_pliku[MAX_NAZWA];
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
/*
int stworzDysk(Dysk* dysk, unsigned int rozmiar_dysku);//rozmiar dysku w kilobajtach
int otworzDysk(Dysk* dysk);

int usunDysk(Dysk* dysk);
*/

Dysk* stworzDysk(unsigned int rozmiar);
Dysk* otworzDysk();
int zamknijDysk(Dysk* otwarty_dysk);
int usunDysk(Dysk *dysk);

unsigned int iloscWolnejPamieci(Dysk* dysk);

int zapiszNaWirtualnymDysku(Dysk* dysk, const char *nazwa_pliku);
int zapiszNaRzeczywistymDysku(Dysk* dysk, const char *nazwa_pliku);
int usunPlikZWirtualnegoDysku(Dysk *dysk, const char *nazwa_pliku);

int wypiszZawartoscDysku(Dysk *dysk);
int wyswietlMapePamieci(Dysk *dysk);

#endif // DYSK_H

