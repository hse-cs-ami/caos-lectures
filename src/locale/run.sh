#!/bin/sh -ex
make setlocale
LC_NUMERIC=C ./setlocale
LC_NUMERIC=de_DE.UTF-8 ./setlocale
LC_NUMERIC=en_US.UTF-8 ./setlocale
LC_NUMERIC=ru_RU.UTF-8 ./setlocale

LC_CTYPE=C ./setlocale Ы
LC_CTYPE=en_US.UTF-8 ./setlocale Ы
LC_CTYPE=ru_RU.UTF-8 ./setlocale Ы

LC_COLLATE=C ./setlocale hrnec chert
LC_COLLATE=cs_CZ.UTF-8 ./setlocale hrnec chert

