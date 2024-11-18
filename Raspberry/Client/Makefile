# Compilador y banderas
CC = gcc
CFLAGS = -Wall -Iinclude -Iinclude/lcd_buzzer
LDFLAGS = -lssl -lcrypto -lm

# Directorios
SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin

# Objetivo principal
TARGET = $(BIN_DIR)/client
LIBRARY = $(LIB_DIR)/liblcd_buzzer.a

# Fuentes
SRCS = $(SRC_DIR)/client_main.c $(SRC_DIR)/socket_utils.c \
       $(SRC_DIR)/file_utils.c $(SRC_DIR)/crypto_utils.c \
       

# Objetos
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Regla para compilar el ejecutable (enlazando con la biblioteca)
$(TARGET): $(OBJS) $(LIBRARY)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -L$(LIB_DIR) -llcd_buzzer $(LDFLAGS)

# Regla para compilar la biblioteca estática
$(LIBRARY): $(OBJ_DIR)/lcd_buzzer.o
	@mkdir -p $(LIB_DIR)
	ar rcs $(LIBRARY) $<

# Regla para compilar archivos objeto
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR)

# Ejecutar el programa
run: $(TARGET)
	$(TARGET)

.PHONY: clean run
