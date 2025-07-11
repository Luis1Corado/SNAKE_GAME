#!/usr/bin/fish

echo "LibraryPath: $LIBRARY_PATH"
echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH"

gcc main.c -o snake -lraylib -lm -lpthread -ldl
