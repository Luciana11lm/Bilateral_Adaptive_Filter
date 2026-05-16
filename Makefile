CC=gcc

CFLAGS=-Wall -Wextra -O3 -fopenmp

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
NUM_THREADS ?= 4

SIGMA_NOISE ?= 20
SIGMA_SPATIAL ?= 1
SIGMA_RANGE ?= 25

build:
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) -lm

run:
	OMP_NUM_THREADS=$(NUM_THREADS) ./$(OUT) \
	$(INPUT_IMAGE) \
	$(NOISY_IMAGE) \
	$(OUTPUT_IMAGE) \
	$(NUMBER_OF_RUNS) \
	$(SIGMA_NOISE) \
	$(SIGMA_SPATIAL) \
	$(SIGMA_RANGE)

tune:
	@for range in 10 15 20 25 30 35 40 50 60; do \
		echo "\n===== SIGMA_RANGE=$$range ====="; \
		OMP_NUM_THREADS=$(NUM_THREADS) ./$(OUT) \
		$(INPUT_IMAGE) \
		$(NOISY_IMAGE) \
		$(OUTPUT_IMAGE) \
		$(NUMBER_OF_RUNS) \
		$(SIGMA_NOISE) \
		$(SIGMA_SPATIAL) \
		$$range; \
	done

clean:
	rm -f $(OUT)