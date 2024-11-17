# Compilador y banderas
CC = gcc
CFLAGS = -Wall -Iinclude
LDFLAGS = -lm

# Directorios
SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Objetivo
TARGET = $(BIN_DIR)/client

# Fuentes
SRCS = $(SRC_DIR)/main_client.c $(SRC_DIR)/socket_utils.c $(SRC_DIR)/file_utils.c $(SRC_DIR)/hardware_utils.c

# Objetos
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Regla para compilar el ejecutable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Regla para compilar archivos objeto
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Ejecutar el programa
run: $(TARGET)
	$(TARGET)

.PHONY: clean run
