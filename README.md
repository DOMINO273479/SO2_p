Wielowątkowy serwer czatu dokumentacja Dominik Filipiak 273479 Kacper Kostrzewa 272855

Projekt realizuje prosty, wielowątkowy system czatu działający w architekturze klient-serwer.

Program składa się z dwóch części:
Serwer czatu — nasłuchuje na połączenia od klientów, obsługuje wielu użytkowników jednocześnie, rozsyła otrzymane wiadomości do wszystkich uczestników.
Użytkownik czatu — łączy się z serwerem, umożliwia wpisywanie wiadomości i odbieranie wiadomości od innych użytkowników.
 Wymagania:
- System operacyjny: Windows
- Kompilator: C++ obsługujący standard C++11 lub nowszy
- Biblioteka: ws2_32.lib 
  Kompilacja:
Serwer
g++ server.cpp -o server.exe -lws2_32
.\server.exe
Użytkownik
g++ user.cpp -o user.exe -lws2_32
.\user.exe
Struktura programu:
Serwer:
-Inicjalizacja Winsock
-Utworzenie gniazda serwera TCP i przypisanie do portu
-Nasłuchiwanie połączeń
-Akceptowanie klientów i tworzenie dla nich osobnych wątków
-Obsługa użytkownika: wprowadzanie nick-u, dodanie użytkownika do listy, odbiór i rozsyłanie wiadomości
-Synchronizacja dostępu do listy klientów realizowana jest za pomocą spinlocka który pozwala wielu wątkom lub procesom na wzajemne wykluczanie się podczas dostępu do współdzielonych zasobów. 
-Usuwanie klientów po rozłączeniu
Klient:
-Inicjalizacja Winsock
-Utworzenie gniazda TCP
-Połączenie z serwerem pod adresem IP i portem
-Wysyłanie nazwy użytkownika
-Uruchomienie wątku odbierającego wiadomości
-Pętla główna do wysyłania wiadomości
-Zamykanie połączenia
Opis działania:
Serwer:
Po uruchomieniu serwer tworzy gniazdo i przypisuje je do portu 8080 po czym rozpoczyna się nieskończona pętla w której czeka na nowe połączenia.
Po zaakceptowaniu klienta, tworzy nowy wątek do obsługi tego klienta.
Wątek klienta odbiera nazwę użytkownika a następnie dodaje klienta do globalnej listy.
Każda wiadomość od klienta jest odbierana i rozsyłana do pozostałych klientów.
Po rozłączeniu klienta, jest on usuwany z listy, a pozostali są o tym informowani.
Użytkownik:
Użytkownik inicjalizuje Winsock i tworzy gniazdo TCP.
Łączy się z serwerem pod adresem 127.0.0.1 i portem 8080.
Po połączeniu wysyła nazwę użytkownika następnie uruchamia wątek, który odbiera i wyświetla wiadomości od serwera.
W głównym wątku użytkownik wpisuje wiadomości, które są wysyłane do serwera.
Po wpisaniu komendy "exit" użytkownik kończy połączenie i zamyka program.
