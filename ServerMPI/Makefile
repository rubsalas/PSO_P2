# Compilador y flags
CC = mpicc
CFLAGS = -Wall -Iinclude
LDFLAGS = -lssl -lcrypto -lm

# Directorios
SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Ejecutable objetivo
TARGET = $(BIN_DIR)/server

# Archivos fuente
SRCS = $(SRC_DIR)/server_main.c $(SRC_DIR)/word_counter.c $(SRC_DIR)/mpi_utils.c $(SRC_DIR)/server.c $(SRC_DIR)/crypto_utils.c

# Archivos objeto (reemplaza .c por .o y almacena en OBJ_DIR)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Variables de entorno para OpenMPI
MPI_ENV = env PATH=/usr/bin:$$PATH LD_LIBRARY_PATH=/usr/lib:$$LD_LIBRARY_PATH

# Regla para construir el ejecutable objetivo
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(MPI_ENV) $(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Regla para construir archivos objeto
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(MPI_ENV) $(CC) $(CFLAGS) -c $< -o $@

# Limpiar el proyecto (elimina archivos objeto y binario)
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Ejecutar el proyecto con MPI
run: $(TARGET)
	$(MPI_ENV) mpirun -np $(NP) $(TARGET)

# Ejecutar el proyecto con MPI y un hostfile especificado
run_mpi: $(TARGET)
	$(MPI_ENV) mpirun -np $(NP) --hostfile ../.mpi_hostfile_pso $(TARGET)

# Regla para compilar con soporte de hardware opcional
hardware: CFLAGS += -DHARDWARE_SUPPORT
hardware: $(TARGET)

.PHONY: clean run run_mpi hardware
