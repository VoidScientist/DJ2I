#
# Makefile du module UI du projet DJ2I
#
# Auteurs: - ARCELON Louis
#          - MARTEL Mathieu
#
# Construit tous les fichiers source du dossier src/ui/ en une librairie
# statique libui.a placée dans $(LIB_PATH).
#

UI_SRC  := $(wildcard $(SRC_PATH)/ui/*.c)
UI_OBJ  := $(patsubst $(SRC_PATH)/ui/%.c, $(OBJ_PATH)/%.o, $(UI_SRC))
UI_LIB  := $(LIB_PATH)/libui.a

ui-build: $(UI_LIB)

$(OBJ_PATH)/%.o: $(SRC_PATH)/ui/%.c
	$Q $(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@
	$Q echo -e "Compilé: $@"

$(UI_LIB): $(UI_OBJ)
	$Q ar rcs $@ $^
	$Q echo -e "[AR] Librairie $@ créée."

ui-clean:
	$Q echo -e "Suppression des fichiers relatifs à l'UI..."
	$Q rm -f $(UI_OBJ) $(UI_LIB)
