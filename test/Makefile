sequence_test : sequence_test.cpp ../include/sequence.h Makefile
	g++ --std=c++11 -g -Wextra -Werror sequence_test.cpp -pthread -lgtest -L/usr/lib/x86_64-linux-gnu -lboost_coroutine -lboost_context -o sequence_test

test : sequence_test
	./sequence_test

clean:
	rm -f sequence_test callgrind.out.*

.PHONY: test clean
