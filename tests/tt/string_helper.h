#ifndef STRING_HELPER_H
#define STRING_HELPER_H

typedef void( * heartbeatFunction )( void* );

// funkcja odczytuje ze standardowego wejścia jedną linię
// należy pamiętać o zwolnieniu pamięci zwróconej wartości
char* readLine();

// funkcja dzieli @str na tablicę poprzez separator
// należy pamiętać o zwolnieniu pamięci zwróconej wartości
char** split(const char* str, char separator);

// funkcja tworząca nowy krótszy napis na podstawie argumentu @str
// należy pamiętać o zwolnieniu pamięci zwróconej wartości
char* substring(const char* str, int begin, int end);

// funkcja do czyszczenia bufora
// jeżeli ktoś poda za dużo znaków na wejściu, a miał podać mniej to bufor powinien zostać wyczyszczony
// bufor wyczyści się do pierwszego znaku nowej linii
void cleanBuffer();


#endif