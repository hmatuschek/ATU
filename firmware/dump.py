#!/usr/bin/env python3

from re import match
from struct import unpack
from sys import argv 

def readHEX(filename):
  f = open(filename, "r")
  data = bytes()
  for line in f.readlines():
    m = match(r"^:([0-9A-F]{8})([0-9A-F]{64})([0-9A-F]{2})$", line.strip())
    if None == m:
      continue
    addr = int(m.group(1),16)
    data += bytes.fromhex(m.group(2))
    crc = int(m.group(3),16)
  return data

def unpack_log(filename):
  data = readHEX(filename)
  print(" VSWR | L6 | L5 | L4 | L3 | L2 | L1 | IO | C5 | C4 | C3 | C2 | C1 | L (uH) | C (pF) |")
  print("------+----+----+----+----+----+----+----+----+----+----+----+----+--------+--------+")
  for i in range(0, len(data), 4):
    L,C,SWR = unpack("<BbH", data[i:(i+4)])
    if 0xFFFF == SWR:
      return
    IO = int(C<0);C = abs(C)
    C1,C2,C3,C4,C5 = (C>>0)&1,(C>>1)&1,(C>>2)&1,(C>>3)&1,(C>>4)&1
    L1,L2,L3,L4,L5,L6 = (L>>0)&1,(L>>1)&1,(L>>2)&1,(L>>3)&1,(L>>4)&1,(L>>5)&1
    L = 0.1*L1 + 0.22*L2 + 0.47*L3 + 1.0*L4 + 2.2*L5 + 4.7*L6 
    C = 10*C1 + 22*C2 + 47*C3 + 100*C4 + 220*C5
    print("{:>5} | {:>2} | {:>2} | {:>2} | {:>2} | {:>2} | {:>2} | {:>2} | {:>2} | {:>2} | {:>2} | {:>2} | {:>2} | {:1.4f} | {:>6} |".format(
      SWR/100, L6,L5,L4,L3,L2,L1,IO,C5,C4,C3,C2,C1,L,C))

if __name__ == "__main__":
  if len(argv) != 2:
    print("Usage: dump.py HEXFILE")
    exit(-1)
  unpack_log(argv[1])
  exit(0)