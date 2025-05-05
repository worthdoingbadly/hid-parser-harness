clang -g -Os -o harness-hid -fsanitize=undefined,address,fuzzer -no-pie harness-hid.c hid-core.o
