CC     = gcc
CFLAGS = -Wall -Wno-unused-variable -Wno-unused-function -I..
LDFLAGS = -lm

# ── executables ──────────────────────────────────────────────────────────────
TEST_USER         = test_user
TEST_TRANSACTION  = test_transaction
TEST_COIN         = test_coin
TEST_COIN_PRICE   = test_coin_price
TEST_BANK         = test_bank
TEST_ADMIN        = test_admin
TEST_HASH         = test_hash
TEST_EXCHANGE     = test_exchange
TEST_COINS_LAW    = test_coins_law
TEST_BLOCKS       = test_blocks

ALL_TESTS = \
	$(TEST_USER) \
	$(TEST_TRANSACTION) \
	$(TEST_COIN) \
	$(TEST_COIN_PRICE) \
	$(TEST_BANK) \
	$(TEST_ADMIN) \
	$(TEST_HASH) \
	$(TEST_EXCHANGE) \
	$(TEST_COINS_LAW) \
	$(TEST_BLOCKS)

# ── build rules ──────────────────────────────────────────────────────────────
all: $(ALL_TESTS)

$(TEST_USER): test_user_actions.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_TRANSACTION): test_transaction_actions.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_COIN): test_coinprice_actions.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_COIN_PRICE): test_coin_price.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lcurl

$(TEST_BANK): test_bank_actions.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_ADMIN): test_admin_actions.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_HASH): test_hash.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_EXCHANGE): test_exchange.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_COINS_LAW): test_coins_law.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_BLOCKS): test_blocks.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# ── run all tests ─────────────────────────────────────────────────────────────
run: $(ALL_TESTS)
	@echo ""; echo "====== test_user ======";          ./$(TEST_USER)
	@echo ""; echo "====== test_transaction ======";   ./$(TEST_TRANSACTION)
	@echo ""; echo "====== test_coin ======";          ./$(TEST_COIN)
	@echo ""; echo "====== test_coin_price ======";    ./$(TEST_COIN_PRICE)
	@echo ""; echo "====== test_bank ======";          ./$(TEST_BANK)
	@echo ""; echo "====== test_admin ======";         ./$(TEST_ADMIN)
	@echo ""; echo "====== test_hash ======";          ./$(TEST_HASH)
	@echo ""; echo "====== test_exchange ======";      ./$(TEST_EXCHANGE)
	@echo ""; echo "====== test_coins_law ======";     ./$(TEST_COINS_LAW)
	@echo ""; echo "====== test_blocks ======";        ./$(TEST_BLOCKS)

# ── individual run shortcuts ─────────────────────────────────────────────────
run_user:        $(TEST_USER)        ; ./$(TEST_USER)
run_transaction: $(TEST_TRANSACTION) ; ./$(TEST_TRANSACTION)
run_coin:        $(TEST_COIN)        ; ./$(TEST_COIN)
run_bank:        $(TEST_BANK)        ; ./$(TEST_BANK)
run_admin:       $(TEST_ADMIN)       ; ./$(TEST_ADMIN)
run_hash:        $(TEST_HASH)        ; ./$(TEST_HASH)
run_exchange:    $(TEST_EXCHANGE)    ; ./$(TEST_EXCHANGE)
run_coins_law:   $(TEST_COINS_LAW)  ; ./$(TEST_COINS_LAW)
run_blocks:      $(TEST_BLOCKS)     ; ./$(TEST_BLOCKS)

clean:
	rm -f $(ALL_TESTS) *.o

.PHONY: all run clean \
	run_user run_transaction run_coin run_bank run_admin \
	run_hash run_exchange run_coins_law run_blocks
