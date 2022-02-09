1. Kompilacja
Aby skompilować program wystarczy wpisać komendę "make" w terminalu.

2. Uruchamianie
./client.exe IPC_KEY
./server.exe IPC_KEY

3. Opis plików .c
a) channel.c - implementacja funkcji zawartych w pliku "channel.h"
b) list.c - implementacja funkcji zawartych w pliku "list.h"
c) messages.c - implementacja funkcji zawartych w pliku "messages.h"
d) string_helper.c - implementacja funkcji zawartych w pliku "string_helper.h"
e) user.c - implementacja funkcji zawartych w pliku "user.h"
f) client.c - plik obsługi klienta. Zawiera obsługę wszystkich funkcji realizowanych przez klienta - wysyłanie, odbieranie wiadomości, menu, obsługa komend.
f) server.c - plik obsługi serwera. Zawiera obsługę wszystkich funkcji realizowanych przez serwera - wysyłanie, odbieranie wiadomości, mechanizm heartbeat.

4. Opis plików .h
a) channel.h - struktura kanału i pomocnicze funkcje do jej obsługi
b) list.h - struktura listy podwójnej pomocna przy przechowywaniu 10 ostatnich wiadomości
c) messages.h - jeden z najważniejszych plików. Znajdują się tutaj wszystkie struktury do obsługi komunikacji pomiędzy klientem i serwerem oraz dodatkowe funkcje pomocnicze.
d) string_helper.h - pomocnicze funkcje do obsługi napisów
e) user.h - instruktura użytkownika i pomocnicze funkcje do jej obsługi
f) servers_config.h - ustawienia serwera takie jak maksymalna długość nazwy użytkownika albo maksymalna liczba użytkowników

5. Komendy użytkownika
* użytkownik może wysyłać wiadomości lub wpisywać komendy, każda komenda rozpoczyna się od znaku "/"
* /wyloguj -> wylogowywuje użytkownika z serwera (użytkownik dalej istnieje w bazie użytkowników i może się ponownie zalogować)
* /lista uzytkownikow -> wyświetla listę zarejestrowanych użytkowników
* /lista kanalow -> wyświetla listę stworzonych kanałów
* /lista kanalow+ -> wyświetla listę kanałów i użytkowników, które do nich należą
* /dolacz NAZWA_KANAŁU -> dodaje użytkownika na kanał (jeżeli kanał nie istnieje to jest tworzony) i wybiera go jako aktywny
* /wyjdz NAZWA_KANAŁU -> usuwa użytkownika z kanału (kanał istnieje nawet jeżeli jest pusty) i wybiera aktywny kanał "Publiczny"
* /wybierz NAZWA_KANAŁU -> ustawia kanał jako aktywny (tzn. można na nim wysyłać wiadomości)
* /wyslij UŻYTKOWNIK NAZWA_KANAŁU -> wysyła wiadomość prywatną do użytkownika
* /zyje -> komenda, która pozwala na przekazanie informacji o życiu serwerowi (heartbeat). Aby potwierdzić serwerowi, że się żyje można też wysłać dowolną inną komendę lub wiadomość

6. Zrealizowane funkcje
Zrealizowane zostały wszystkie funkcje poza komunikacją z wieloma serwerami.

7. Uwagi
* Każdy użytkownik należy do kanału "Publiczny" po wejściu na serwer. Nie może z niego wyjść. Dlatego też każdy użytkownik może wysłać wiadomość prywatną do każdego użytkownika.
* Liczba użytkowników, które obsługuje serwer zdefiniowana jest w pliku "servers_config.h" i wynosi domyślnie 50
* Jeżeli użytkownik nie odpowie na wezwanie heartbeat to zostaje wylogowany, a program zostaje wyłączony
