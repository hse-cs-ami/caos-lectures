#!/bin/sh -ex

CC=gcc
LD=gcc
CFLAGS="-Wall -Werror"

$CC hello.c -c $CFLAGS
$CC main.c -c $CFLAGS
$LD hello.o main.o -o hello
