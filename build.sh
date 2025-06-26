#!/usr/bin/bash

CC="gcc"
LD="gcc"

CFLAGS="-O2 -Wall -Wextra -g"
LFLAGS=""

SRC=src
LIBC=libc
OBJ=obj
BUILD=build

SRCS=($(find "$SRC" -name "*.c" -o -path "$SRC" -name "*.c") $(find "$LIBC" -name "*.c" -o -path "$LIBC" -name "*.c"))
OBJS=()

for src in "${SRCS[@]}"; do
    obj="${OBJ}/$(basename "$src" .c).o"
    OBJS+=("$obj")
done

OUTPUT=$BUILD/proton

function build {
    mkdir -p $OBJ
    mkdir -p $BUILD

    for i in "${!SRCS[@]}"; do
        src="${SRCS[$i]}"
        obj="${OBJS[$i]}"
        comp $src $obj
    done

    $LD $LFLAGS "${OBJS[@]}" -o $OUTPUT
}

function comp {
    $CC $CFLAGS -c $1 -o $2
}

function clean {
    clear

    rm -rf $OBJ
    rm -rf $BUILD
}

for arg in "$@"; do
    if [ "$arg" = "build" ]; then
        build
    elif [ "$arg" = "build" ]; then
        build
    elif [ "$arg" = "clean" ]; then
        clean
    fi
done
