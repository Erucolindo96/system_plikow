#include"dysk.h"

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


//private
int czytajDeskryptory(Dysk *dysk, FILE *deskryptor_pliku)//dysk musi mieć zaallokowaną pamięć
{
    if(fread(dysk->tablica_deskryptorow, sizeof(DeskryptorBloku), dysk->ilosc_blokow, deskryptor_pliku) != dysk->ilosc_blokow)
    {
        return BAD;
    }
    return GOOD;
}

int czytajBloki(Dysk *dysk, FILE *deskryptor_pliku)//dysk musi mieć zaallokowana pamięć
{
    if(fread(dysk->tablica_blokow, sizeof(Blok), dysk->ilosc_blokow, deskryptor_pliku) != dysk->ilosc_blokow)
    {
        return BAD;
    }
    return GOOD;
}

int zapiszDeskryptory(Dysk *dysk, FILE *deskryptor_pliku)
{
    if(fwrite(dysk->tablica_deskryptorow, sizeof(DeskryptorBloku), dysk->ilosc_blokow, deskryptor_pliku) != dysk->ilosc_blokow)
    {
       return BAD;
    }
    return GOOD;
}

int zapiszBloki(Dysk *dysk, FILE *deskryptor_pliku)
{
     if(fwrite(dysk->tablica_blokow, sizeof(DeskryptorBloku), dysk->ilosc_blokow, deskryptor_pliku) != dysk->ilosc_blokow)
     {
        return BAD;
     }
     return GOOD;
}

int otworzDyskZPliku(Dysk *dysk)//zmienna dysk musi mieć zaallokowaną pamięć, aby móc wczytać plik z wirtualnym dyskiem
{
    unsigned int ilosc_blokow_plik;
    FILE* deskryptor_pliku = fopen(PLIK_DYSKU, "rb");
    if(deskryptor_pliku == NULL)
        return BAD;
    //czytamy ilość bloków
    //wynik = fread(&ilosc_blokow_plik, sizeof(unsigned int), 1, deskryptor_pliku);
    if(fread(&ilosc_blokow_plik, sizeof(unsigned int), 1, deskryptor_pliku) != 1 || ilosc_blokow_plik != dysk->ilosc_blokow )//jeśli nie wczytano niczego, lub zmienna dysk nie ma zaallokowanej odpowiednio pamięci
    {
        fclose(deskryptor_pliku);
        return BAD;
    }
    //czytamy deskryptory bloków
    if(czytajDeskryptory(dysk, deskryptor_pliku) == BAD)
    {
        fclose(deskryptor_pliku);
        return BAD;
    }
    //czytamy bloki
    if(czytajBloki(dysk, deskryptor_pliku) == BAD)
    {
        fclose(deskryptor_pliku);
        return BAD;
    }
    //wszystko poszło dobrze, zamykamy plik
    fclose(deskryptor_pliku);
    return GOOD;
}

int allokujPamiecDyskowi(Dysk *dysk, unsigned int ilosc_blokow)
{
    dysk->tablica_blokow = malloc(sizeof(Blok) * ilosc_blokow);
    dysk->tablica_deskryptorow = malloc(sizeof(DeskryptorBloku)*ilosc_blokow);
    //inicjalizacja deksryptorów bloków
    if(dysk->tablica_blokow ==NULL || dysk->tablica_deskryptorow == NULL)//jeśli nie można zaallokować pamięci dla którejś z tablic
        return BAD;

    unsigned int i;
    //inicjalizujemy deskryptory
    for(i = 0; i< ilosc_blokow;++i)
    {
        dysk->tablica_deskryptorow[i].jestZajety = 0;
        dysk->tablica_deskryptorow[i].jestPoczatkiemPliku = 0;
        dysk->tablica_deskryptorow[i].nastepny = -1;
        dysk->tablica_deskryptorow[i].zajeta_pamiec =0;
    }
    return GOOD;
}

int zapiszDyskDoPliku(Dysk* dysk)
{
    FILE *deskryptor_pliku;
    size_t zapisane_elementy;
    deskryptor_pliku = fopen(PLIK_DYSKU, "wb");
    if(deskryptor_pliku == NULL)
        return BAD;
    //zapisujemy ilość bloków
    zapisane_elementy = fwrite(&(dysk->ilosc_blokow), sizeof(unsigned int), 1, deskryptor_pliku);//zapisujemy ilośc bloków dysku
    if(zapisane_elementy != 1)
    {
        fclose(deskryptor_pliku);
        return BAD;
    }
    //zapisujemy deksryptory
    if(zapiszDeskryptory(dysk, deskryptor_pliku) == BAD)
    {
        fclose(deskryptor_pliku);
        return BAD;
    }
    //zapisujemy bloki
    if(zapiszBloki(dysk, deskryptor_pliku) == BAD )
    {
        fclose(deskryptor_pliku);
        return BAD;
    }
    //wszystko poszło dobrze, zamykamy plik i kończymy
    fclose(deskryptor_pliku);
    return GOOD;

}


//public
unsigned int stworzDysk(Dysk* dysk, unsigned int rozmiar_dysku)//rozmiar dysku w kilobajtach
{
    rozmiar_dysku = rozmiar_dysku<<10;//zamieniamy kilobajty na bajty
    unsigned int ilosc_blokow = rozmiar_dysku/ROZMIAR_BLOKU;
    ++ilosc_blokow;//teraz dysk będzie miał rozmiar >=rozmiar_dysku
                printf("Ilosc blokow: %d\n", ilosc_blokow);
 //   dysk->ilosc_blokow = ilosc_blokow;
/*
    dysk->tablica_blokow = malloc(sizeof(Blok) * ilosc_blokow);
    dysk->tablica_deskryptorow = malloc(sizeof(DeskryptorBloku)*ilosc_blokow);
    //inicjalizacja deksryptorów bloków
    if(dysk->tablica_blokow==NULL || dysk->tablica_deskryptorow == NULL)//jeśli nie można zaallokować pamięci dla którejś z tablic
        return BAD;

    unsigned int i;
    for(i = 0; i< ilosc_blokow;++i)
    {
        dysk->tablica_deskryptorow[i].jestZajety = 0;
        dysk->tablica_deskryptorow[i].jestPoczatkiemPliku = 0;
        dysk->tablica_deskryptorow[i].nastepny = -1;
        dysk->tablica_deskryptorow[i].zajeta_pamiec =0;
    }*/
    if(allokujPamiecDyskowi(dysk, ilosc_blokow)== BAD)
    {
        fprintf(stderr, "Nie udało się zaallokowac pamieci dla dysku\n") ;
        return BAD;
    }
    return zapiszDyskDoPliku(dysk);
}



int usunDysk(Dysk* dysk)
{
    int wynik;
    wynik = zapiszDyskDoPliku(dysk);
    if(wynik==BAD)
    {
                    fprintf(stderr, "Nie udało się zapisać dysku do pliku.\n Dysk nie został usunięty z pamięci - można go jeszcze poprawnie zapisać\n");
        return BAD;
    }
    free(dysk->tablica_blokow);
    free(dysk->tablica_deskryptorow);
                    printf("Dokonano poprawnego zapisu wirtualnego dysku\n");
    return GOOD;
}





