CC = gcc
CFLAGS = -Wall -Wno-unused-variable
LDFLAGS = -lm

# Executáveis
TEST_COIN = test_coin

all: $(TEST_COIN)

$(TEST_COIN): test_coinprice_actions.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run: $(TEST_COIN)
	./$(TEST_COIN)

clean:
	rm -f $(TEST_COIN) *.o

.PHONY: all run clean
