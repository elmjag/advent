.PHONY: clean format

%: %.c
	gcc -g -Wall $< -o $@ -lm

clean:
	rm -f $(filter-out %.c %.py,$(wildcard day*))

format:
	clang-format -i *.c
	black *.py
