#
# Makefile de la lib protocol applicatif du projet DJ2I
#
# Auteurs: 	- ARCELON Louis
#

APP_SRC := $(wildcard $(SRC_PATH)/app/*.c)

APP_OBJ := $(patsubst $(SRC_PATH)/app/%.c, $(OBJ_PATH)/%.o, $(APP_SRC))

APP_LIB := $(LIB_PATH)/libapp.a

app-build: $(APP_LIB)

$(OBJ_PATH)/%.o: $(SRC_PATH)/app/%.c
	$Q $(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@
	$Q echo -e "Compilé: $@"


$(APP_LIB): $(APP_OBJ)
	$Q ar rcs $@ $^
	$Q echo -e "[AR] Librairie $@ créée."

app-clean:
	$Q echo -e "Suppression des fichiers relatifs au protocol applicatif..."
	$Q rm -f $(APP_OBJ) $(APP_LIB)
