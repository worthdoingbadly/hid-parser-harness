#!/usr/bin/env python3
import sys
import os
#print(sys.argv)
newargs = [
    "clang", "-fsanitize=undefined,address,fuzzer",
    "-enable-trivial-auto-var-init-zero-knowing-it-will-be-removed-from-clang",
    "-Os"
]
toremove = [
    "-fzero-call-used-regs=used-gpr", "-fconserve-stack",
    "-Wno-alloc-size-larger-than", "-Wimplicit-fallthrough=5",
    "-Wno-maybe-uninitialized"
]
for i in sys.argv[1:]:
    if i in toremove:
        continue
    if i.startswith("-fsanitize=") or i.startswith("-mstack-protector-guard"):
        continue
    newargs.append(i)
#print(newargs)
os.execvp("clang", newargs)
