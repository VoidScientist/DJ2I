#
# Makefile des programmes expérimentaux du projet DJ2I
#
# Auteurs: 	- ARCELON Louis
# 			- MARTEL Mathieu
#

EXPERIMENTAL_SRC := $(wildcard $(SRC_PATH)/experimental/*.c)

EXPERIMENTAL_FOLDER := $(BUILD_PATH)

EXPERIMENTAL_TARGET := $(patsubst $(SRC_PATH)/experimental/%.c, $(BUILD_PATH)/%, $(EXPERIMENTAL_SRC))


experimental-build: $(EXPERIMENTAL_TARGET)

$(BUILD_PATH)/%: $(SRC_PATH)/experimental/%.c
	$Q $(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ -ldrivers $(OPTWIRINGPI)
	$Q echo -e "Compilé: $@"