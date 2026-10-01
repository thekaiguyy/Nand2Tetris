CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
OBJS     = main.o Parser.o Code.o

assembler: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main.o: main.cpp Parser.h Code.h SymbolTable.h
Parser.o: Parser.cpp Parser.h
Code.o: Code.cpp Code.h

# Assembles each tests/*.asm and diffs against the expected tests/*.cmp
test: assembler
	@for f in tests/*.asm; do \
	  b=$${f%.asm}; \
	  ./assembler $$f /tmp/$$(basename $$b).hack > /dev/null || exit 1; \
	  if diff -q /tmp/$$(basename $$b).hack $$b.cmp > /dev/null; then echo "PASS $$f"; \
	  else echo "FAIL $$f"; exit 1; fi; \
	done

clean:
	rm -f assembler *.o

.PHONY: test clean
