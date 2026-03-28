#
# Makefile de la lib inet du projet DJ2I
#
# Auteurs: 	- ARCELON Louis
#

INET_SRC := $(wildcard $(SRC_PATH)/inet/*.c)

INET_OBJ := $(patsubst $(SRC_PATH)/inet/%.c, $(OBJ_PATH)/%.o, $(INET_SRC))

INET_LIB := $(LIB_PATH)/libinet.a

inet-build: $(INET_LIB)

$(OBJ_PATH)/%.o: $(SRC_PATH)/inet/%.c
	$Q $(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@
	$Q echo -e "Compilé: $@"


$(INET_LIB): $(INET_OBJ)
	$Q ar rcs $@ $^
	$Q echo -e "[AR] Librairie $@ créée."

inet-clean:
	$Q echo -e "Suppression des fichiers relatifs à la librairie inet..."
	$Q rm -f $(INET_OBJ) $(INET_LIB)
