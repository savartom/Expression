CXX = clang++
CXXFLAGS = -std=c++20 -O2 -Iinclude

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
TESTS_DIR = tests

SRC_FILES = $(SRC_DIR)/Lexer.cpp
MAIN_FILE = main.cpp
TEST_FILE = $(TESTS_DIR)/test.cpp

OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))
MAIN_OBJ = $(BUILD_DIR)/main.o
TEST_OBJ = $(BUILD_DIR)/test.o

TARGET = $(BUILD_DIR)/differentiator
TEST_TARGET = $(BUILD_DIR)/test

all: $(TARGET)

$(TARGET): $(MAIN_OBJ) $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_TARGET): $(TEST_OBJ) $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(TESTS_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

rebuild: clean all

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all test rebuild clean
