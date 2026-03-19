# DJ2I

Application développée avec la mallette JoyPI dans le cadre du cours "Compilation Croisée - Objets connectés" à l'IG2I.

## Compilation / Installation

- Le target par défaut du Makefile est (x86\_64) il est aussi possible de choisir ARM. ex: `make TARGET=ARM build`
- Il est possible d'installer sur une machine distante (ici RPI) les builds ARM à travers `make install`
- Il est FORTEMENT conseillé d'utiliser `make setup-keyauth` afin de configurer une clé d'authentification afin de ne plus avoir à saisir le MDP distant.

L'installation copie par défaut les fichiers de `builds/ARM` dans `~/DJ2I` et les librairies de `lib/ARM` dans `lib`.

Pour la compilation, il faut que les librairies se trouvent dans `lib/$(TARGET)`.

Pour la compilation, il faut que les fichiers en-têtes se trouvent dans `include/<LIB_NAME>`

Le compilateur ARM par défaut est celui du sous-module `tools` originaire de [ce dépôt](https://github.com/raspberrypi/tools).

## Structure actuelle du projet:

.
├── builds
│   └── `TARGET`
├── include
│   └── `LIB_NAME`
├── lib
│   └── `TARGET`
├── Makefile
├── README.md
├── src
│   └── experimental
└── tools
    ├── arm-bcm2708
    └── ...

