CC=gcc

CFLAGS=-Wall -Wextra -O0

SRC=src/main.c \
    src/image_io.c \
    src/noise.c \
    src/bilateral.c \
    src/metrics.c

OUT=bilateral_cpu

INPUT_IMAGE=images/input.pgm
NOISY_IMAGE=images/noisy.pgm
OUTPUT_IMAGE=images/output.pgm

NUMBER_OF_RUNS ?= 1

build:
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) -lm

run:
	./$(OUT) \
	$(INPUT_IMAGE) \
	$(NOISY_IMAGE) \
	$(OUTPUT_IMAGE) \
	$(NUMBER_OF_RUNS)

clean:
	rm -f $(OUT)