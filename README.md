# ISA-projekt

* Login: xzatec02
* Jméno a příjmení: Michal Žatečka
* Datum: 20.11.2023

## Příklady spuštění
> sudo ./isa-ldapserver -p 4200 -f název_souboru   
> sudo ./isa-ldapserver -f název_souboru

* **-p** nepovinný parametr (výchozí hodnota portu je 389)
* **-f** povinný parametr

## Popis programu
Program nejdříve zpracuje argumenty příkazové řádky a ověří jejich platnost. Následně načte data ze souboru a spustí samotný LDAP server.  
Nejdříve očekává zprávu **Bind request**, po úspěšném příjetí této zprávy odešle klientovi **Bind response** a čeká na **Search request** (ostatní příkazy ignoruje) dokud nepřijme **Unbind request**, po které ukončí komunikaci.

## Omezení

## Odevzdané soubory
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
* Makefile