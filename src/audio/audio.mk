#
# Makefile de la lib audio du projet DJ2I
#
# Auteurs: 	- ARCELON Louis
# 			- MARTEL Mathieu
#

AUDIO_SRC := $(wildcard $(SRC_PATH)/audio/*.c)

AUDIO_OBJ := $(patsubst $(SRC_PATH)/audio/%.c, $(OBJ_PATH)/%.o, $(AUDIO_SRC))

AUDIO_LIB := $(LIB_PATH)/libaudio.a

audio-build: $(AUDIO_LIB)

$(OBJ_PATH)/%.o: $(SRC_PATH)/audio/%.c
	$Q $(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@ 
	$Q echo -e "Compilé: $@"


$(AUDIO_LIB): $(AUDIO_OBJ)
	$Q ar rcs $@ $^
	$Q echo -e "[AR] Librairie $@ créée."

audio-clean:
	$Q echo -e "Suppression des fichiers relatifs aux audio..."
	$Q rm -f $(AUDIO_OBJ) $(AUDIO_LIB)
