#include <stdio.h>
#include"dysk.h"
/*
#define GOOD 0
#define BAD 1
#define FILE_ERROR 2
#define ALLOC_ERROR 3
#define READ_ERROR 4
#define WRITE_ERROR 5
#define NO_MEMORY 6
#define NOT_FOUND 7
*/
int main(void)
{
    printf("Hello World!\n");
    int wynik;
    Dysk* dysk;
    dysk = stworzDysk(6);
    printf("Utworzono dysk. Rozmiar: %d\n", iloscWolnejPamieci(dysk));

    wynik = zapiszNaWirtualnymDysku(dysk, "nowy.txt");
    wynik = zapiszNaWirtualnymDysku(dysk, "drugi.txt");

    wynik = wypiszZawartoscDysku(dysk);
    wynik = wyswietlMapePamieci(dysk);

    wynik = usunPlikZWirtualnegoDysku(dysk, "nowy.txt");

    wynik = wypiszZawartoscDysku(dysk);
    wynik = wyswietlMapePamieci(dysk);

    wynik = zapiszNaRzeczywistymDysku(dysk, "nowy.txt");
    wynik = zapiszNaRzeczywistymDysku(dysk, "drugi.txt");

    wynik = wypiszZawartoscDysku(dysk);
    wynik = wyswietlMapePamieci(dysk);
    usunDysk(dysk);
    printf("Koniec\n");

    return 0;
}

