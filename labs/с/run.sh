#!/bin/zsh

CXXFLAGS=(
  -std=c++20
  -O3 -DNDEBUG
  -Wall -Wextra -Wpedantic -Werror
  -g
)

/opt/homebrew/bin/g++-15 "${CXXFLAGS[@]}" ./Driver.cpp -o b.out
