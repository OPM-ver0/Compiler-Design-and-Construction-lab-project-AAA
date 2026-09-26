CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic

TARGET = sadhu_bangla_compiler
FLEX_SRC = sadhu_bangla_lexer.l
FLEX_GEN = lex.yy.cpp
STANDALONE = sadhu_bangla_standalone

.PHONY: all flex clean test test-flex test-standalone standalone rebuild

all: $(TARGET)

$(FLEX_GEN): $(FLEX_SRC) part01_token.hpp
	flex -o $@ $<

$(TARGET): $(FLEX_GEN) sadhu_bangla_flex_main.cpp part01_token.hpp part03_ast_types.cpp part04_parser.cpp part05_type_checker.cpp part06_python_code_generator.cpp part07_utils.cpp part09_debug_printer.cpp part10_ir.cpp
	$(CXX) $(CXXFLAGS) $(FLEX_GEN) sadhu_bangla_flex_main.cpp -o $@

flex: $(TARGET)
	@echo "Flex lexer build complete: ./$(TARGET)"

rebuild: clean
	$(MAKE) all

standalone: $(STANDALONE)

$(STANDALONE): sadhu_bangla_all_parts.cpp
	$(CXX) $(CXXFLAGS) sadhu_bangla_all_parts.cpp -o $(STANDALONE)

test: test-flex

test-flex: $(TARGET)
	bash tests/run_tests.sh ./$(TARGET)

test-standalone: $(STANDALONE)
	bash tests/run_tests.sh ./$(STANDALONE)

clean:
	rm -f $(TARGET) $(FLEX_GEN) $(STANDALONE) generated.py tests/*.pyc
	rm -rf tests/output
