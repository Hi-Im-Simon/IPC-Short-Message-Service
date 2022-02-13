# IPC-Short-Message-Service


## Instrukcja kompilacji

* `cd project_directory` (gdzie _project\_directory_ to główny folder projektu)
* `make` (flaga -Wall jest automatycznie dodana do kompilacji)


## Instrukcja uruchomienia

* należy otworzyć dwa okna terminala i w każdym z nich wykonać polecenie `cd project_directory` (gdzie _project\_directory_ to główny folder projektu)
* na jednym z terminali wykonać polecenie `./server.out`
* a na drugim `./client.out`


## Opis zawartości plików .C

* _inf150192s.c_ - zawiera polecenia serwera:
  
  * jego główną pętlę, która oczekuje na sygnał z informacją o komendzie wybranej przez użytkownika
  * funkcje odpowiadające każdej z dostępnych komend i odpowiadające za ich funkcjonalność
  * struktury danych, które odpowiedzialne są za przechowywanie informacji o użytkownikach oraz grupach

* _inf150192c.c_ - zawiera polecenia użytkownika:

  * pętlę zewnętrzną, która umożliwia logowanie i wylogowywanie użytkownika oraz zakomunikowanie tego serwerowi
  * pętlę główną, która odpowiada za wyświetlanie menu oraz czytanie komend użytkownika
  * funkcje odpowiadające za dalsze zbieranie informacji od użytkownika oraz przesyłanie ich do serwera, a także wyświetlanie informacji zwrotnych przesłanych przez serwer
