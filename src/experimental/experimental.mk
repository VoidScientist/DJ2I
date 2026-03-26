#
# Makefile des programmes expérimentaux du projet DJ2I
#
# Auteurs: 	- ARCELON Louis
#

EXPERIMENTAL_FOLDER := $(BUILD_PATH)
EXPERIMENTAL_SRC    := $(SRC_PATH)/experimental




experimental-build: $(EXPERIMENTAL_FOLDER) $(EXPERIMENTAL_FOLDER)/audio_test $(EXPERIMENTAL_FOLDER)/button_matrix_test


$(EXPERIMENTAL_FOLDER)/button_matrix_test: $(EXPERIMENTAL_SRC)/button_matrix_test.c
	$Q $(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ -ldrivers $(OPTWIRINGPI)
	$Q echo -e "Compilé: $@"


$(EXPERIMENTAL_FOLDER)/audio_test: $(EXPERIMENTAL_SRC)/audio_test.c
	$Q $(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ -ldrivers $(OPTWIRINGPI) -laudio -lSDL2 -lSDL2_mixer -lm
	$Q echo -e "Compilé: $@"


$(EXPERIMENTAL_FOLDER):
	mkdir -p $(EXPERIMENTAL_FOLDER)