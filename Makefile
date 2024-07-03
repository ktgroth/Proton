
CC=gcc
CFLAGS=-Wall

SRC=src
OBJ=obj
BUILD=build

SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

all: $(BUILD)/proton


$(BUILD)/proton: $(BUILD) $(OBJ) $(OBJS)
	$(CC) $(OBJS) -o $(BUILD)/proton

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o: $(SRC)/%.c $(SRC)/%.h
	$(CC) $(CFLAGS) -c $< -o $@


$(OBJ):
	mkdir $@

$(BUILD):
	mkdir $@


empty: clear
clear: clean
clean:
	clear
	rm -rf $(OBJ)
	rm -rf $(BUILD)
