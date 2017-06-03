

#ifndef DYSK_PRIVATE_H
#define DYSK_PRIVATE_H
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"dysk.h"

//private
int rozmiarPliku(const char* nazwa, fpos_t *zwracany_rozmiar);

int czytajDeskryptory(Dysk *dysk, FILE *deskryptor_pliku);//dysk musi mieć zaallokowaną pamięć
int czytajBloki(Dysk *dysk, FILE *deskryptor_pliku);//dysk musi mieć zaallokowana pamięć

int zapiszDeskryptory(Dysk *dysk, FILE *deskryptor_pliku);
int zapiszBloki(Dysk *dysk, FILE *deskryptor_pliku);

int allokujPamiecDyskowi(Dysk *dysk, unsigned int ilosc_blokow);
void deallokujPamiecDyskowi(Dysk *dysk);

int zapiszDyskDoPliku(Dysk* dysk, const char* nazwa_dysku);

unsigned int iloscWolnychBlokow(Dysk *dysk);//zakłada się, że dysk istnieje
int znajdzNastepnyWolnyBlok(Dysk *dysk);//zwraca indeks nastepnego wolnego bloku
void ustawParametryBloku(Dysk *dysk, unsigned int index, char czyZajety, char czyPoczatekPliku,unsigned int zajeta_pamiec, int nast, const char*nazwa);

int otworzDyskZPliku(Dysk *dysk, const char* nazwa_dysku);

int ladujPlikDoDysku(Dysk *dysk, FILE *plik,const char *nazwa_pliku, size_t rozmiar);//ładuje plik na wirtualny dysk


int znajdzPlikNaDysku(Dysk *dysk, const char* nazwa_pliku);



#endif // DYSK_PRIVATE_H

