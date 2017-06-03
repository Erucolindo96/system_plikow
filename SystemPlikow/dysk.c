#include"dysk.h"
#include"dysk_private.h"



/*
//private - deklaracje
int czytajDeskryptory(Dysk *dysk, FILE *deskryptor_pliku);//dysk musi mieć zaallokowaną pamięć
int czytajBloki(Dysk *dysk, FILE *deskryptor_pliku);//dysk musi mieć zaallokowana pamięć

int zapiszDeskryptory(Dysk *dysk, FILE *deskryptor_pliku);
int zapiszBloki(Dysk *dysk, FILE (deskryptor_pliku);

int otworzDyskZPliku(Dysk *dysk);//zmienna dysk musi mieć zaallokowaną pamięć, aby móc wczytać plik z wirtualnym dyskiem
int allokujPamiecDyskowi(Dysk *dysk, unsigned int ilosc_blokow);
int zapiszDyskDoPliku(Dysk* dysk);
*/


//public

Dysk* stworzDysk(const char* nazwa, unsigned int rozmiar)
{
    Dysk* tworzony = malloc(sizeof(Dysk));
    if(tworzony == NULL)
        return NULL;
    //wyliczamy ilość bloków
    rozmiar = rozmiar<<10;//zamieniamy KB na B
    int ilosc_blokow = rozmiar/ROZMIAR_BLOKU;
    tworzony->ilosc_blokow = ilosc_blokow;
    if(allokujPamiecDyskowi(tworzony, ilosc_blokow) == BAD)
    {//jeśli wystąpiły błędy przy allokacji - zwalniamy pamięć dysku
        free(tworzony);
        return NULL;
    }
    //zapisujemy, na razie pusty, dysk w pliku
    if(zapiszDyskDoPliku(tworzony, nazwa) == BAD)
    {
        deallokujPamiecDyskowi(tworzony);
        free(tworzony);
        return NULL;
    }
    return tworzony;
}
Dysk* otworzDysk(const char* nazwa)
{
    Dysk* otwierany = malloc(sizeof(Dysk));
    if(otwierany == NULL)
        return NULL;
    //wczytujemy dysk z pliku
    if(otworzDyskZPliku(otwierany, nazwa) == BAD)
    {
        free(otwierany);
        return NULL;
    }
    //wszystko poszło dobrze
    return otwierany;
}
int zamknijDysk(Dysk* otwarty_dysk, const char* nazwa_dysku)
{
    int wynik=zapiszDyskDoPliku(otwarty_dysk, nazwa_dysku);
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

unsigned int iloscWolnejPamieci(Dysk* dysk)
{
    return iloscWolnychBlokow(dysk)*ROZMIAR_BLOKU;
}

int zapiszNaWirtualnymDysku(Dysk *dysk, const char *nazwa_pliku)
{
    //char bufor[ROZMIAR_BLOKU];
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





