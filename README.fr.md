# hshell

> Version anglaise : [README.md](README.md)

Un mini shell Linux en C++ fait comme projet perso pour comprendre comment l'exécution des processus, la redirection et les pipes marchent au niveau système.

## Ce qu'il fait

- Prompt interactif avec le dossier courant (`hshell ~/Projects >`)
- Exécution avec `fork` / `exec`
- Commandes internes : `cd`, `pwd`, `history`, `exit`
- Un seul pipe (`cmd1 | cmd2`)
- Redirection vers un fichier (`cmd > file.txt`)

## Démarrage rapide

Il faut `g++` (standard C++17) sur Linux.

```bash
make
./hshell
```

Quitter avec `exit` ou Ctrl+D. `make clean` supprime les binaires.

## Exemple d'utilisation

```
hshell ~/Projects > ls | grep cpp
main.cpp

hshell ~/Projects > echo "hello world" > output.txt

hshell ~/Projects > cat output.txt
"hello world"
```

## Limites

- Un seul pipe (`|`) ou une seule redirection (`>`) par commande (pas les deux ensemble)
- Pas de guillemets, pas de wildcards (`*`), pas de variables (`$HOME`)
- Pas de processus en arrière-plan (`&`)
- Historique gardé en mémoire seulement, perdu à la fermeture

## Structure du projet

```
src/
├── main.cpp    # boucle REPL, parsing, commandes internes
└── shell.cpp   # processus, pipes, descripteurs de fichiers
include/
└── shell.h     # déclarations
```

~330 lignes de C++ au total.
