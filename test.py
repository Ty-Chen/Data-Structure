#!usr/bin/env

import struct
from functools import partial

#struct_fmt = '=5if255s' # int[5], float, byte[255]
struct_fmt = '=iii'
struct_len = struct.calcsize(struct_fmt)
struct_unpack = struct.Struct(struct_fmt).unpack_from

filename = "test.bin"

with open(filename, "rb") as f:
    results = [struct_unpack(chunk) for chunk in iter(partial(f.read, struct_len), b'')]

print(results)
