# ISA-projekt
Projekt do předmětu ISA na VUT FIT. Jedná se o implementaci LDAP serveru, který umožňuje pouze vyhledávání záznamů v databázi.

## Příklady spuštění
> sudo ./isa-ldapserver -p 4200 -f název_souboru   
> sudo ./isa-ldapserver -f název_souboru

* **-p** nepovinný parametr (výchozí hodnota portu je 389)
* **-f** povinný parametr

## Popis programu
Program nejdříve zpracuje argumenty příkazové řádky a ověří jejich platnost. Následně načte data ze souboru a spustí samotný LDAP server.  
Nejdříve očekává zprávu **Bind request**, po úspěšném příjetí této zprávy odešle klientovi **Bind response** a čeká na **Search request** (ostatní příkazy ignoruje) dokud nepřijme **Unbind request**, po které ukončí komunikaci.

## Omezení
* Při načítání dat ze souboru neprobíhá žádná kontrola validity dat
* Server nevyvolá chybu v případě, že by obdržel nevalidní filtr například ve tvaru "(cn=\*Test\*)(uid=test)"

## Odevzdané soubory
* doc/
  * img/
    * BER_long_form.png
    * BER_short_form.png
* obj/
* src/
  * headers/
    * argumentParser.hpp
    * ber.hpp
    * database.hpp
    * error.hpp
    * filter.hpp
    * LDAPserver.hpp
  * argumentParser.cpp
  * ber.cpp
  * database.cpp
  * error.cpp
  * LDAPserver.cpp
  * main.cpp
* testFiles/
  * ldapDB.csv
* Makefile
* README.md
* manual.md
* manual.pdf

* Jméno a příjmení: Michal Žatečka
* Datum: 20.11.2023
