# Compilador y flags
CC = mpicc
CFLAGS = -Wall -Iinclude
LDFLAGS = -lssl -lcrypto

# Directorios
SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Ejecutable objetivo
TARGET = $(BIN_DIR)/word_counter_mpi

# Archivos fuente
SRCS = $(SRC_DIR)/main_mpi.c $(SRC_DIR)/word_counter.c $(SRC_DIR)/crypto_utils.c

# Archivos objeto (reemplaza .c por .o y almacena en OBJ_DIR)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Regla para construir el ejecutable objetivo
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Regla para construir archivos objeto
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar el proyecto (elimina archivos objeto y binario)
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Ejecutar el proyecto con MPI
run: $(TARGET)
	mpirun -np 4 $(TARGET)

# Regla para compilar con soporte de hardware opcional
hardware: CFLAGS += -DHARDWARE_SUPPORT
hardware: $(TARGET)

.PHONY: clean run hardware
