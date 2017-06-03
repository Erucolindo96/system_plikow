#include <stdio.h>
#include"dysk.h"

int main(void)
{
    printf("Hello World!\n");
    int wynik;
    Dysk* testowy_dysk;

    testowy_dysk = stworzDysk("testowy3.vdc", 5 );//powinien miec 5KB
    printf("Adres Dysku: %d\n", testowy_dysk);
    printf("Wolna pamięć na dysku: %d\n", iloscWolnejPamieci(testowy_dysk));

    //pobieramy plik z linuxa
    wynik = zapiszNaWirtualnymDysku(testowy_dysk, "nowy.txt");
    printf("Pobrano plik z linuxa. Wynik: %d\n", wynik);

    wynik = zapiszNaRzeczywistymDysku(testowy_dysk, "nowy.txt");
    printf("Zapisano plik na linuxa. Wynik: %d\n", wynik);

    wynik = zamknijDysk(testowy_dysk, "testowy2.vdc");
    printf("Wynik usuwania: %d\n", wynik);

    return 0;
}

