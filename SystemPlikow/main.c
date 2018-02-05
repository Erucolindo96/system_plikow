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
#define DISC_NOT_FOUND 11
#define DISC_EXIST 10

void printCommands();
void incorrectSyntax();
void tooManyArguments();

void twoArguments(char **argv);
void treeArguments(char **argv);

int create(size_t rozmiar_dysku);

int add(const char* nazwa_pliku);
int remove_(const char * nazwa_pliku);
int get(const char *nazwa_pliku);

int ls();
int memoryMap();
int delete_();




int main(int argc, char**argv)
{
    switch(argc)
    {
    case 1:
        printCommands();
        return 0;
    case 2:
        twoArguments(argv);
        return 0;
    case 3:
        treeArguments(argv);
        return 0;
    default:
        tooManyArguments();
        return 0;
    }


    if(argc == 1)
    {
        printCommands();
        return 0;
    }
    if(argc > 3)
    {
        tooManyArguments();
        return 0;
    }

    return 0;
}

void printCommands()
{
    printf("Możliwe komendy: \n");
    printf("-create Rozmiar_dysku Tworzenie dysku\n");
    printf("-delete               Usuwanie dysku\n");
    printf("-memorymap            Wyświetlanie mapy pamięci dysku\n");
    printf("-ls                   Wyświetlenie katalogu dysku\n");
    printf("-add Nazwa_pliku      Dodawanie pliku do wirtualnego dysku\n");
    printf("-remove Nazwa_pliku   Usuwanie pliku z wirtualnego dysku\n");
    printf("-get Nazwa_pliku       Dodanie pliku z dysku wirtualnego do dysku rzeczywistego \n");

}
void tooManyArguments()
{
    printf("Podano zbyt dużo argumentów\n");
}

void incorrectSyntax()
{
    printf("Niepoprawna składnia komendy\n");
}

void twoArguments(char **argv)
{

    int wynik;
    if(strcmp(argv[1], "-delete") == 0)
    {
        wynik = delete_();
        if(wynik == GOOD)
            printf("Usunięto wirtualny dysk\n");
        else
            printf("Nie udało sie usunąć wirtualnego dysku\nSprawdź czy na pewno istnieje\n");
    }
    else if(strcmp(argv[1], "-memorymap") == 0)
    {
        wynik = memoryMap();
        if(wynik == DISC_NOT_FOUND)
            printf("Nie znaleziono wirtualnego dysku\nAby utworzyć dysk, wpisz -create rozmiar_dysku\n");

    }
    else if(strcmp(argv[1], "-ls") == 0)
    {
        wynik = ls();
        if(wynik ==DISC_NOT_FOUND)
            printf("Nie znaleziono wirtualnego dysku\nAby utworzyć dysk, wpisz -create rozmiar_dysku\n");

    }
    else
    {
        incorrectSyntax();
    }
}

void treeArguments(char **argv)
{
    int wynik;
    if(strcmp(argv[1], "-create") == 0)
    {
        int rozmiar_dysku = atoi(argv[2]);
        if(rozmiar_dysku <= 0)
        {
            printf("Podano zły rozmiar dysku\n");
            return;
        }
        wynik = create( rozmiar_dysku );
        if(wynik == GOOD)
            printf("Utworzono wirtualny dysk\n");
        else
            printf("Plik z dyskiem już istnieje\nWpisz -delete aby usunąć dysk\n");
    }
    else if(strcmp(argv[1], "-add") == 0)
    {
        wynik = add(argv[2]);
        switch(wynik)
        {
        case DISC_NOT_FOUND:
            printf("Nie znaleziono dysku\n Aby utworzyć dysk, wpisz -create rozmiar_dysku\n");
            break;
        case FILE_ERROR:
            printf("Nie znaleziono pliku o nazwie \" %s \"\n", argv[2]);
            break;
        case NO_MEMORY:
            printf("Brak pamięci na dysku, aby dodać plik %s\n", argv[2]);
            break;
        case GOOD:
            printf("Zapisano plik na dysku wirtualnym\n");
            break;
        case FILE_EXIST:
            printf("Plik już istnieje na dysku\n");
            break;
        default:
            printf("Nie udało się zapisać pliku na wirtualny dysk. Spróbuj ponownie\n");
            break;
        }
        return;

    }
    else if(strcmp(argv[1], "-remove") == 0)
    {
        wynik = remove_(argv[2]);
        switch (wynik) {
        case DISC_NOT_FOUND:
            printf("Nie znaleziono dysku\n Aby utworzyć dysk, wpisz -create rozmiar_dysku\n");
            break;
        case FILE_EXIST:
            printf("Plik już istnieje na dysku wirtualnym. Nie można go ponownie zapisać\n");
            break;
        case NOT_FOUND:
            printf("Nie znaleziono pliku o nazwie \" %s \" na dysku wirtualnym\n", argv[2]);
            break;
        case GOOD:
            printf("Usunięto plik o nazwie \" %s \" z dysku wirtualnego\n", argv[2]);
            break;
        default:
            printf("Nie można usunąć pliku\n");
            break;
        }
    }
    else if(strcmp(argv[1], "-get") == 0)
    {
        wynik  = get(argv[2]);
        switch(wynik)
        {
        case GOOD:
            printf("Zapisano plik na dysku rzeczywistym\n");
            break;
        case NOT_FOUND:
            printf("Nie znaleziono pliku na dysku\nWpisz -ls aby wyświetlić pliki istniejące na dysku\n");
            break;
        case FILE_ERROR:
            printf("Nie udało się zapisać pliku na dysku rzeczywistym\n");
            break;
        case WRITE_ERROR:
            printf("Nie udało się zapisać pliku na dysku rzeczywistym\n");
            break;
        default:
            printf("Nie udało się zapisać pliku na dysku rzeczywistym\n");
            break;

        }

    }
    else
    {
        incorrectSyntax();
    }
}

int create(size_t rozmiar_dysku)
{
    FILE *plik_z_dyskiem = fopen(NAZWA_DYSKU, "r");
    if(plik_z_dyskiem != NULL)//jeśli plik z dyskiem już istnieje
    {
        fclose(plik_z_dyskiem);
        return DISC_EXIST;
    }
    //dysk nie istnieje, można go bezpiecznie utworzyć
    Dysk *dysk = stworzDysk(rozmiar_dysku);
    zamknijDysk(dysk);
    return GOOD;
}
int delete_()
{
    Dysk* dysk = otworzDysk();
    if(dysk == NULL)
        return DISC_NOT_FOUND;
//dysk istnieje w pliku, usuwamy go
    int wynik = usunDysk(dysk);
    return wynik;
}
int memoryMap()
{
    Dysk* dysk = otworzDysk();
    if(dysk == NULL)
        return DISC_NOT_FOUND;
    wyswietlMapePamieci(dysk);
    return zamknijDysk(dysk);

}
int ls()
{
    Dysk *dysk = otworzDysk();
    if(dysk == NULL)
        return DISC_NOT_FOUND;
    wypiszZawartoscDysku(dysk);
    return zamknijDysk(dysk);
}
int add(const char *nazwa_pliku)
{
    Dysk *dysk = otworzDysk();
    if(dysk == NULL)
        return DISC_NOT_FOUND;
    int wynik = zapiszNaWirtualnymDysku(dysk, nazwa_pliku);
    zamknijDysk(dysk);
    return wynik;
}
int remove_(const char *nazwa_pliku)
{
    Dysk *dysk = otworzDysk();
    if(dysk == NULL)
        return DISC_NOT_FOUND;
    int wynik = usunPlikZWirtualnegoDysku(dysk, nazwa_pliku);
    zamknijDysk(dysk);
    return wynik;
}

int get(const char *nazwa_pliku)
{
    Dysk *dysk = otworzDysk();
    if(dysk == NULL)
        return DISC_NOT_FOUND;
    int wynik = zapiszNaRzeczywistymDysku(dysk, nazwa_pliku);
    zamknijDysk(dysk);
    return wynik;
}
