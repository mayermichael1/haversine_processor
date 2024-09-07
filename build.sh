#!/bin/bash

WARNING="-Wall -Werror -Wextra -Wno-unused-variable"
BUILD_DIR="./output"

bear -- g++ -O0 -g gen_json.cpp -o $BUILD_DIR/generate_json.out


