sequence_test : sequence_test.cpp ../include/sequence.h Makefile
	g++ --std=c++1y -g -ggdb -Wextra -Werror sequence_test.cpp -MMD -MP -MF"$@.d" -MT"$@" -pthread -lgtest -L/usr/lib/x86_64-linux-gnu -lboost_coroutine -lboost_context -o sequence_test

test : sequence_test
	./sequence_test

clean:
	rm -f sequence_test callgrind.out.*

.PHONY: test clean

-include sequence_test.d
