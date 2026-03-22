#
# Makefile de la lib drivers du projet DJ2I
#
# Auteurs: 	- ARCELON Louis
# 			- MARTEL Mathieu
#

DRIVERS_SRC := $(wildcard $(SRC_PATH)/drivers/*.c)

DRIVERS_OBJ := $(patsubst $(SRC_PATH)/drivers/%.c, $(OBJ_PATH)/%.o, $(DRIVERS_SRC))

DRIVERS_LIB := $(LIB_PATH)/libdrivers.a

drivers-build: $(DRIVERS_LIB)

$(OBJ_PATH)/%.o: $(SRC_PATH)/drivers/%.c
	$Q $(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@ $(OPTWIRINGPI)
	$Q echo -e "Compilé: $@"


$(DRIVERS_LIB): $(DRIVERS_OBJ)
	$Q ar rcs $@ $^
	$Q echo -e "[AR] Librairie $@ créée."

drivers-clean:
	$Q echo -e "Suppression des fichiers relatifs aux drivers..."
	$Q rm -f $(DRIVERS_OBJ) $(DRIVERS_LIB)
