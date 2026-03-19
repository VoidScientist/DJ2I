#
# Makefile principal du projet DJ2I
#
# Auteurs: 	- ARCELON Louis
# 			- MARTEL Mathieu
#

# VARIABLES PROJET
VERSION = 0.1

# VARIABLES COMPILATIONS
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
CC = $(realpath ./tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-gcc)
else
$(error "Architecture invalide: $(TARGET)")
endif

# VARIABLES LOGGING
# |
# | MODE QUIET
Q = @
ifeq ($(V), 1)
Q =
endif

all: build

build:
	$Q echo Compilation pour $(TARGET)...
	

.PHONY: all, build, clean, install	
