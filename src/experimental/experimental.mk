#
# Makefile des programmes expérimentaux du projet DJ2I
#
# Auteurs: 	- ARCELON Louis
#

EXPERIMENTAL_FOLDER := $(BUILD_PATH)
EXPERIMENTAL_SRC_PATH := $(SRC_PATH)/experimental

EXPERIMENTAL_SRC = $(wildcard $(EXPERIMENTAL_SRC_PATH)/*.c)
EXPERIMENTAL_TARGET = $(patsubst $(EXPERIMENTAL_SRC_PATH)/%.c, $(EXPERIMENTAL_FOLDER)/%, $(EXPERIMENTAL_SRC))



experimental-build: $(EXPERIMENTAL_FOLDER) $(EXPERIMENTAL_TARGET)


$(EXPERIMENTAL_FOLDER)/button_matrix_test: $(EXPERIMENTAL_SRC_PATH)/button_matrix_test.c
	$Q $(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ -ldrivers $(OPTWIRINGPI) -laudio -lSDL2 -lSDL2_mixer -lm
	$Q echo -e "Compilé: $@"


$(EXPERIMENTAL_FOLDER)/audio_test: $(EXPERIMENTAL_SRC_PATH)/audio_test.c
	$Q $(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ -ldrivers $(OPTWIRINGPI) -laudio -lSDL2 -lSDL2_mixer -lm
	$Q echo -e "Compilé: $@"

$(EXPERIMENTAL_FOLDER)/exp_seven_seg: $(EXPERIMENTAL_SRC_PATH)/exp_seven_seg.c
	$Q $(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ -ldrivers $(OPTWIRINGPI) 
	$Q echo -e "Compilé: $@"

$(EXPERIMENTAL_FOLDER)/serveur_pc: $(EXPERIMENTAL_SRC_PATH)/serveur_pc.c
	$Q $(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ -lapp -linet -llogging -laudio -lSDL2 -lSDL2_mixer -lm -lpthread
	$Q echo -e "Compilé: $@"

$(EXPERIMENTAL_FOLDER)/client_joypi: $(EXPERIMENTAL_SRC_PATH)/client_joypi.c
	$Q $(CC) $(CFLAGS) $(LDFLAGS) $< -o $@  -ldrivers $(OPTWIRINGPI) -lapp -linet -llogging -laudio -lSDL2 -lSDL2_mixer -lm -lpthread
	$Q echo -e "Compilé: $@"




$(EXPERIMENTAL_FOLDER):
	mkdir -p $(EXPERIMENTAL_FOLDER)