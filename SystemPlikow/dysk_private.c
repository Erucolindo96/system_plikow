#include"dysk_private.h"


//private
int rozmiarPliku(const char* nazwa, fpos_t *zwracany_rozmiar)
{
    FILE *plik = fopen(nazwa, "rb");
    if(plik == NULL)
        return FILE_ERROR;
    if(fseek(plik, 0, SEEK_END) != 0)//jeśli coś sie popsuło
    {
        fclose(plik);
        return READ_ERROR;
    }
    fgetpos(plik, zwracany_rozmiar);
    fclose(plik);
    return GOOD;
}

int czytajDeskryptory(Dysk *dysk, FILE *deskryptor_pliku)//dysk musi mieć zaallokowaną pamięć
{
    if(fread(dysk->tablica_deskryptorow, sizeof(DeskryptorBloku), dysk->ilosc_blokow, deskryptor_pliku) != dysk->ilosc_blokow)
    {
        return READ_ERROR;
    }
    return GOOD;
}

int czytajBloki(Dysk *dysk, FILE *deskryptor_pliku)//dysk musi mieć zaallokowana pamięć
{
    if(fread(dysk->tablica_blokow, sizeof(Blok), dysk->ilosc_blokow, deskryptor_pliku) != dysk->ilosc_blokow)
    {
        return READ_ERROR;
    }
    return GOOD;
}

int zapiszDeskryptory(Dysk *dysk, FILE *deskryptor_pliku)
{
    if(fwrite(dysk->tablica_deskryptorow, sizeof(DeskryptorBloku), dysk->ilosc_blokow, deskryptor_pliku) != dysk->ilosc_blokow)
    {
       return WRITE_ERROR;
    }
    return GOOD;
}

int zapiszBloki(Dysk *dysk, FILE *deskryptor_pliku)
{
     if(fwrite(dysk->tablica_blokow, sizeof(Blok), dysk->ilosc_blokow, deskryptor_pliku) != dysk->ilosc_blokow)
     {
        return WRITE_ERROR;
     }
     return GOOD;
}

int allokujPamiecDyskowi(Dysk *dysk, unsigned int ilosc_blokow)
{
    dysk->tablica_deskryptorow = malloc(sizeof(DeskryptorBloku)*ilosc_blokow);
    if( dysk->tablica_deskryptorow == NULL)
        return ALLOC_ERROR;
    dysk->tablica_blokow = malloc(sizeof(Blok) * ilosc_blokow);
    //inicjalizacja deksryptorów bloków
    if(dysk->tablica_blokow ==NULL) //jeśli nie można zaallokować pamięci dla którejś z tablic
    {
        free(dysk->tablica_deskryptorow);
        return ALLOC_ERROR;
    }

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
void deallokujPamiecDyskowi(Dysk *dysk)
{
    free(dysk->tablica_blokow);
    free(dysk->tablica_deskryptorow);
}


int zapiszDyskDoPliku(Dysk* dysk, const char* nazwa_dysku)
{
    FILE *deskryptor_pliku;
    size_t zapisane_elementy;
    deskryptor_pliku = fopen(nazwa_dysku, "wb");
    if(deskryptor_pliku == NULL)
        return FILE_ERROR;
    //zapisujemy ilość bloków
    zapisane_elementy = fwrite(&(dysk->ilosc_blokow), sizeof(unsigned int), 1, deskryptor_pliku);//zapisujemy ilośc bloków dysku
    if(zapisane_elementy != 1)
    {
        fclose(deskryptor_pliku);
        return WRITE_ERROR;
    }
    //zapisujemy deksryptory
    if(zapiszDeskryptory(dysk, deskryptor_pliku) == WRITE_ERROR)
    {
        fclose(deskryptor_pliku);
        return WRITE_ERROR;
    }
    //zapisujemy bloki
    if(zapiszBloki(dysk, deskryptor_pliku) == WRITE_ERROR )
    {
        fclose(deskryptor_pliku);
        return WRITE_ERROR;
    }
    //wszystko poszło dobrze, zamykamy plik i kończymy
    fclose(deskryptor_pliku);
    return GOOD;

}

unsigned int iloscWolnychBlokow(Dysk *dysk)//zakłada się, że dysk istnieje
{
    unsigned int wolne_bloki = 0;
    unsigned int i;
    for(i = 0;i<dysk->ilosc_blokow; ++i)
    {
        if(dysk->tablica_deskryptorow[i].jestZajety == FALSE)//oznacza, że dany blok jest wolny
            ++wolne_bloki;
    }
    return wolne_bloki;
}

int znajdzNastepnyWolnyBlok(Dysk *dysk)//zwraca indeks nastepnego wolnego bloku
{
    size_t i;
    for(i = 0; i< dysk->ilosc_blokow; ++i)
    {
        if(dysk->tablica_deskryptorow[i].jestZajety == FALSE)//jeśli blok nie jest zajęty
            return i;//zwracamy indeks niezajętego bloku
    }
    return -1;//nie znaleziono wolnego bloku
}

void ustawParametryBloku(Dysk *dysk, unsigned int index, char czyZajety, char czyPoczatekPliku,unsigned int zajeta_pamiec, int nast, const char*nazwa)
{
    DeskryptorBloku *desk_ptr = dysk->tablica_deskryptorow +index;
    desk_ptr->jestZajety = czyZajety;
    desk_ptr->jestPoczatkiemPliku = czyPoczatekPliku;
    desk_ptr->nastepny = nast;
    desk_ptr->zajeta_pamiec = zajeta_pamiec;
    if(nazwa != NULL)
        strncpy(desk_ptr->nazwa_pliku, nazwa, MAX_NAZWA);
}

int otworzDyskZPliku(Dysk *dysk, const char* nazwa_dysku)
{
    //unsigned int ilosc_blokow_plik;
    FILE* deskryptor_pliku = fopen(nazwa_dysku, "rb");
    if(deskryptor_pliku == NULL)
        return FILE_ERROR;
    //czytamy ilość bloków
    if(fread(&(dysk->ilosc_blokow), sizeof(unsigned int), 1, deskryptor_pliku) != 1 )
    {
        fclose(deskryptor_pliku);
        return READ_ERROR;
    }
    //allokujemy pamięć
    if(allokujPamiecDyskowi(dysk, dysk->ilosc_blokow) == ALLOC_ERROR)
    {
        fclose(deskryptor_pliku);
        return ALLOC_ERROR;
    }
    //czytamy deskryptory bloków
    if(czytajDeskryptory(dysk, deskryptor_pliku) == READ_ERROR)
    {
        fclose(deskryptor_pliku);
        return READ_ERROR;
    }
    //czytamy bloki
    if(czytajBloki(dysk, deskryptor_pliku) == READ_ERROR)
    {
        fclose(deskryptor_pliku);
        return READ_ERROR;
    }
    //wszystko poszło dobrze, zamykamy plik
    fclose(deskryptor_pliku);
    return GOOD;
}

int ladujPlikDoDysku(Dysk *dysk, FILE *plik,const char *nazwa_pliku, size_t rozmiar)//ładuje plik na wirtualny dysk
{//zakłada się, że plik zmieści się na dysku
    char bufor[ROZMIAR_BLOKU];
    size_t zaladowane_dane = 0, pobrane ;
    unsigned int nastepny_wolny_blok, poprzedni_blok ;
    int i;
    while(zaladowane_dane < rozmiar)
    {//dopoki nie pobrałeś całego pliku
        //pobieramy znaki do bufora
        pobrane = fread(bufor, sizeof(char), ROZMIAR_BLOKU, plik);
        if(zaladowane_dane == 0)//zapisujemy początek pliku
        {
            nastepny_wolny_blok = znajdzNastepnyWolnyBlok(dysk);
            ustawParametryBloku(dysk, nastepny_wolny_blok, TRUE, TRUE,pobrane,-1, nazwa_pliku );
            poprzedni_blok = nastepny_wolny_blok;
            //kopiujemy zawartość bufora
            for(i=0; i<pobrane; ++i)
            {
                dysk->tablica_blokow[nastepny_wolny_blok].zawartosc[i] = bufor[i];
            }
            zaladowane_dane+=pobrane;
        }
        else
        {   //najpierw poprzedniemu ustawiamy nastepnika
            nastepny_wolny_blok = znajdzNastepnyWolnyBlok(dysk);
            //printf("Następny wolny blok: %d, poprzedni blok:%d\n", nastepny_wolny_blok, poprzedni_blok);
            dysk->tablica_deskryptorow[poprzedni_blok].nastepny = nastepny_wolny_blok;
            //i dopiero teraz ustawiamy parametry nowego
            ustawParametryBloku(dysk, nastepny_wolny_blok, TRUE, FALSE, pobrane,-1, NULL);
            //kopiujemy zawartość bufora
            for(i=0; i<pobrane; ++i)
            {
                dysk->tablica_blokow[nastepny_wolny_blok].zawartosc[i] = bufor[i];
            }
            poprzedni_blok = nastepny_wolny_blok;
            zaladowane_dane+=pobrane;
        }
    }
    return GOOD;
}

int znajdzPlikNaDysku(Dysk *dysk, const char *nazwa_pliku)
{
    int i;
    int cmp = -1;
    for(i=0; i< dysk->ilosc_blokow; ++i)
    {
        if(dysk->tablica_deskryptorow[i].jestPoczatkiemPliku)
        {
            cmp = strcmp(nazwa_pliku, dysk->tablica_deskryptorow[i].nazwa_pliku);
            if(cmp == 0)
            {
                return i;
            }
        }
    }
    return -1;//nie znaleziono pliku
}
