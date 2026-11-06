.PHONY: clean format

%: %.c
	gcc -g -Wall $< line_input.c vec.c -o $@ -lm

clean:
	rm -f $(filter-out %.c %.py,$(wildcard day*))

format:
	clang-format -i *.c
	black *.py
