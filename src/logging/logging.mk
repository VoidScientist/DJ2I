#
# Makefile de la lib logging du projet DJ2I
#
# Auteurs: 	- ARCELON Louis
#

LOGGING_SRC := $(wildcard $(SRC_PATH)/logging/*.c)

LOGGING_OBJ := $(patsubst $(SRC_PATH)/logging/%.c, $(OBJ_PATH)/%.o, $(LOGGING_SRC))

LOGGING_LIB := $(LIB_PATH)/liblogging.a

logging-build: $(LOGGING_LIB)

$(OBJ_PATH)/%.o: $(SRC_PATH)/logging/%.c
	$Q $(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@
	$Q echo -e "Compilé: $@"


$(LOGGING_LIB): $(LOGGING_OBJ)
	$Q ar rcs $@ $^
	$Q echo -e "[AR] Librairie $@ créée."

logging-clean:
	$Q echo -e "Suppression des fichiers relatifs à la librairie logging..."
	$Q rm -f $(LOGGING_OBJ) $(LOGGING_LIB)
