#include <stdio.h>
#include"dysk.h"

int main(void)
{
    printf("Hello World!\n");

    Dysk testowy;
    int wynik = stworzDysk(&testowy, 4 );
    printf("Wynik tworzenia: %d\n", wynik);
    wynik = usunDysk(&testowy);
    printf("Wynik usuwania: %d", wynik);

    return 0;
}

