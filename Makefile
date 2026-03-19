#
# Makefile principal du projet DJ2I
#
# Auteurs: 	- ARCELON Louis
# 			- MARTEL Mathieu
#


# == VARIABLES PROJET ==
# |
# | Version du projet
# |
VERSION = 0.1
# |
# | Nom du projet
# |
PROJECT_NAME = DJ2I

# == VARIABLES COMPILATIONS ==
# |
# | Architecture visée
# |
TARGET = x86_64
# |
# | Compilateur C
# |
ifeq ($(TARGET), x86_64)
CC = gcc
else ifeq ($(TARGET), ARM)
CC = tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-gcc
else
$(error "Architecture invalide: $(TARGET)")
endif
# |
# | Chemin vers dossier builds pour les exécutables
# |
BUILD_PATH = builds/$(TARGET)
# |
# | Chemins vers le dossier contenant les librairies
# |
LIB_PATH = lib/$(TARGET)
INCLUDE_PATH = include/**
# |
# | Flags compilation C
# |
CFLAGS = -Wall 
LDFLAGS = -L $(LIB_PATH) -I $(INCLUDE_PATH)
# |
# | Chemins vers différents sous-dossiers
# |
SRC_PATH = src
SRC_EXP_PATH = $(SRC_PATH)/experimental


# == VARIABLES LOGGING ==
# |
# | MODE QUIET
# |
Q = @
ifeq ($(V), 1)
Q =
endif


# == VARIABLES INSTALLATION ==
# |
# | Informations SSH RPI
# |
RPI_USER = pi
RPI_IP = 10.192.29.100
RPI_TARGET = $(RPI_USER)@$(RPI_IP)
SSH_KEY_PATH = ~/.ssh
RPI_SSH_KEY = $(SSH_KEY_PATH)/id_rpi
RPI_SSH_ID_PARAM = -i $(RPI_SSH_KEY)
# |
# |	Chemins d'installation (librairie puis exécutables)
# |
RPI_LIB_PATH = /lib
RPI_INSTALL_PATH = ~/$(PROJECT_NAME)



all: clean build


build: build-init $(BUILD_PATH)/buzzer
	$Q echo Fini de compiler pour: $(TARGET) !


build-init:
	$Q echo Compilation pour $(TARGET)...
	$Q mkdir -p $(BUILD_PATH)


$(BUILD_PATH)/%: $(SRC_EXP_PATH)/%.c
	$Q $(CC) $(LDFLAGS) $(CFLAGS) -lwiringPi $< -o $@ 
	$Q echo Compilé: $@.


clean:
	$Q echo Suppression de tous les builds.
	$Q rm -rf builds


install:
	$Q echo Envoi des builds ARM à $(RPI_TARGET):$(RPI_INSTALL_PATH).
	$Q ssh $(RPI_TARGET) $(RPI_SSH_ID_PARAM) "mkdir -p $(RPI_INSTALL_PATH)"
	$Q scp $(RPI_SSH_ID_PARAM) builds/ARM/* $(RPI_TARGET):$(RPI_INSTALL_PATH)
	$Q echo Envoi des librairies ARM à $(RPI_TARGET):/lib.
	$Q scp $(RPI_SSH_ID_PARAM) lib/ARM/* $(RPI_TARGET):$(PROJECT_NAME).tmp
	$Q ssh $(RPI_SSH_ID_PARAM) $(RPI_TARGET) "sudo mv $(PROJECT_NAME).tmp/* /lib && rmdir $(PROJECT_NAME).tmp"


setup-keyauth:
	$Q echo -e "Création et installation d'une ssh-key."
	$Q mkdir -p $(SSH_KEY_PATH)
	$Q ssh-keygen -f $(RPI_SSH_KEY)
	$Q scp $(RPI_SSH_KEY).pub $(RPI_TARGET):~/.ssh/authorized_keys
	$Q echo -e "C'est tout bon !" 


.PHONY: all build build-init clean install setup-keyauth
