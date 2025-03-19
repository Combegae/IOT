# IOT

Step1:

Pour lancer: ouvrir un premier terminal (dans step1) et tapez "make debug"
Puis ouvrir un deuxième terminal, tapez "gdb-multiarch build/kernel.elf"
Dans le 2è terminal dans gdb: tapez: "tar rem :1234"
On peut par exemple mettre "br _start"