# Protokół komunikacji między komponentami projektu


## Informacje podstawowe

* Wszystkie procesy użytkowników komunikują się z procesem serwera za pomocą wspólnego klucza _KEY_.
* Wszystkie typy struktur mają swoje własne zastosowanie i nie powinny być używane zamiennie.


## Opis użytkowanych struktur

### Struktury kolejkowe:

* _msg\_test_ - wykorzystywana jest do sprawdzenia komunikacji z serwerem i przesłania mu odpowiedniej wartości, która odpowiadać będzie komendzie wybranej przez użytkownika
* _msg\_text_ - wykorzystywana jest do przesyłania zmiennych o typie tablicy znaków do/z serwera
* _msg\_int_ - wykorzystywana jest do przesyłania zmiennych o typie integer do/z serwera (używana głównie do informowania o ilości następujących sygnałów kolejki o strukturze _msg\_text_)


### Struktury danych:

* _User_ - tworzy obiekt nowego użytkownika (przechowywany w tablicy _users_) i zawiera informacje o jego nazwie, grupach, wiadomościach oraz statusie
* _Group_ - tworzy obiekt nowej grupy (przechowywany w tablict _groups_) i zawiera informacje o jej nazwie, użytkownikach oraz wiadomościach
