#include"dysk.h"
#include"dysk_private.h"



//public

Dysk* stworzDysk(unsigned int rozmiar)
{
    Dysk* tworzony = malloc(sizeof(Dysk));
    if(tworzony == NULL)
        return NULL;
    //wyliczamy ilość bloków
    rozmiar = rozmiar<<10;//zamieniamy KB na B
    size_t ilosc_blokow = rozmiar/ROZMIAR_BLOKU;
    if(ilosc_blokow*ROZMIAR_BLOKU < rozmiar) //jeśli dalej nie starcza miejsca
        ++ilosc_blokow;

    tworzony->ilosc_blokow = ilosc_blokow;
    if(allokujPamiecDyskowi(tworzony, ilosc_blokow) == BAD)
    {//jeśli wystąpiły błędy przy allokacji - zwalniamy pamięć dysku
        free(tworzony);
        return NULL;
    }
    //zapisujemy, na razie pusty, dysk w pliku
    if(zapiszDyskDoPliku(tworzony, NAZWA_DYSKU) == BAD)
    {
        deallokujPamiecDyskowi(tworzony);
        free(tworzony);
        return NULL;
    }
    return tworzony;
}
Dysk* otworzDysk()
{
    Dysk* otwierany = malloc(sizeof(Dysk));
    if(otwierany == NULL)
        return NULL;
    //wczytujemy dysk z pliku
    if(otworzDyskZPliku(otwierany, NAZWA_DYSKU) != GOOD)
    {
        free(otwierany);
        return NULL;
    }
    //wszystko poszło dobrze
    return otwierany;
}
int zamknijDysk(Dysk* otwarty_dysk)
{
    int wynik=zapiszDyskDoPliku(otwarty_dysk, NAZWA_DYSKU);
    //zapisujemy dysk do pliku o podanej nazwie
    if(wynik != GOOD)
    {
        return wynik;
    }
    //usuwamy pamiec deskryptorom i blokom
    deallokujPamiecDyskowi(otwarty_dysk);
    //usuwamy pamięć Dysku
    free(otwarty_dysk);
    return GOOD;

}

int usunDysk(Dysk *dysk)
{
    //konwersja nazwy pliku z dyskiem na polecenie shella
    char polecenie_kasujace[128];
    polecenie_kasujace[0] = 'r';
    polecenie_kasujace[1] = 'm';
    polecenie_kasujace[2] = ' ';
    strncpy(polecenie_kasujace+3, NAZWA_DYSKU, 125);

    system(polecenie_kasujace);
    deallokujPamiecDyskowi(dysk);
    free(dysk);
    return GOOD;
}

unsigned int iloscWolnejPamieci(Dysk* dysk)
{
    return iloscWolnychBlokow(dysk)*ROZMIAR_BLOKU;
}

int zapiszNaWirtualnymDysku(Dysk *dysk, const char *nazwa_pliku)
{
    //sprawdz czy plik juz istnieje na dysku
    int indeks_z = znajdzPlikNaDysku(dysk, nazwa_pliku);
    //printf("Indeks znalezionego: %d\n", indeks_z);
    if(indeks_z >= 0)
        return FILE_EXIST;

    fpos_t rozmiar_pliku;
    int wynik =rozmiarPliku(nazwa_pliku, &rozmiar_pliku);
    if(wynik != GOOD)
    {
        return wynik;
    }
    //sprawdzamy czy sie zmiesci
    if(rozmiar_pliku.__pos > iloscWolnejPamieci(dysk) )
        return NO_MEMORY;
    //zmieści się - to teraz otwieramy plik na linuxie i kopiujemy porcjami na dysk
    FILE *plik_na_linuxie = fopen(nazwa_pliku, "rb");
    if(plik_na_linuxie == NULL)
        return FILE_ERROR;

    ladujPlikDoDysku(dysk, plik_na_linuxie, nazwa_pliku, rozmiar_pliku.__pos);
    fclose(plik_na_linuxie);
    //zapisujemy stan dysku po wczytaniu pliku
    zapiszDyskDoPliku(dysk, NAZWA_DYSKU);
    return GOOD;
}

int zapiszNaRzeczywistymDysku(Dysk* dysk, const char *nazwa_pliku)
{
    int i_pliku = znajdzPlikNaDysku(dysk, nazwa_pliku);
    if(i_pliku < 0)
        return NOT_FOUND;
    //tworzymy plik w linuxie
    FILE *plik_na_linuxie = fopen(nazwa_pliku, "wb");
    if(plik_na_linuxie == NULL)
        return FILE_ERROR;
    size_t zajeta_pamiec;
    do
    {
        //zapisujemy blok
        zajeta_pamiec = dysk->tablica_deskryptorow[i_pliku].zajeta_pamiec;
        if( fwrite(dysk->tablica_blokow[i_pliku].zawartosc, sizeof(char), zajeta_pamiec, plik_na_linuxie) != zajeta_pamiec )
        {
            fclose(plik_na_linuxie);
            return WRITE_ERROR;
        }
            //szukamy następnego
        i_pliku = dysk->tablica_deskryptorow[i_pliku].nastepny;
    }while(i_pliku >= 0);//dopoki plik nie został cały zapisany(czyli pole następny == -1)

    fclose(plik_na_linuxie);
    return GOOD;
}

int usunPlikZWirtualnegoDysku(Dysk *dysk, const char *nazwa_pliku)
{
    int indeks_pliku = znajdzPlikNaDysku(dysk, nazwa_pliku);
    if(indeks_pliku < 0)
        return NOT_FOUND;
    int nastepny_indeks = -1;
    do
    {
        nastepny_indeks = dysk->tablica_deskryptorow[indeks_pliku].nastepny;
        ustawParametryBloku(dysk, indeks_pliku, FALSE, FALSE, 0, -1, NULL);//usuwamy blok
        indeks_pliku = nastepny_indeks;

    }while(indeks_pliku >= 0);
    //plik usunięty - zapisujemy stan dysku do pliku
    int wynik = zapiszDyskDoPliku(dysk, NAZWA_DYSKU);
    return wynik;
}

int wypiszZawartoscDysku(Dysk *dysk)
{
    printf("Pliki dysku wirtualnego:\n");
    size_t i;
    for(i=0; i<dysk->ilosc_blokow; ++i)
    {
        if(dysk->tablica_deskryptorow[i].jestPoczatkiemPliku)
        {
            printf("%s \n", dysk->tablica_deskryptorow[i].nazwa_pliku);
        }
    }
    return GOOD;
}

int wyswietlMapePamieci(Dysk *dysk)
{
    int i, wolne_bloki = 0, zajeta_pamiec = 0;
    printf("Blok pamięci ma długość %d bajtów\n", ROZMIAR_BLOKU);
    for(i=0; i<dysk->ilosc_blokow; ++i)
    {
        printf("Zawartość bloku %d: ", i);
        if(dysk->tablica_deskryptorow[i].jestZajety)
        {
            zajeta_pamiec+=dysk->tablica_deskryptorow[i].zajeta_pamiec;

            printf("Zajęty,zajęta pamięć: %d, ", dysk->tablica_deskryptorow[i].zajeta_pamiec);
            if(dysk->tablica_deskryptorow[i].jestPoczatkiemPliku)
                printf("Początek pliku o nazwie: %s, następny blok pliku: %d\n", dysk->tablica_deskryptorow[i].nazwa_pliku, dysk->tablica_deskryptorow[i].nastepny);
            else
                if(dysk->tablica_deskryptorow[i].nastepny >= 0)
                    printf("część pliku, następny blok pliku: %d\n", dysk->tablica_deskryptorow[i].nastepny);
                else
                    printf("koniec pliku\n");
        }
        else
        {
            ++wolne_bloki;
            printf("Wolny \n");
        }
    }

    printf("Wolna pamięć: %d\n", wolne_bloki*ROZMIAR_BLOKU);
    printf("Zajęta pamięć: %d\n", (dysk->ilosc_blokow-wolne_bloki) * ROZMIAR_BLOKU);
    printf("Zajęta pamięć efektywna: %d\n", zajeta_pamiec);

    double wykorzystanie_d = zajeta_pamiec;
    wykorzystanie_d = wykorzystanie_d/(ROZMIAR_BLOKU*dysk->ilosc_blokow );
    printf("Wykorzystanie dysku: %f procent \n",wykorzystanie_d );
    return GOOD;
}



