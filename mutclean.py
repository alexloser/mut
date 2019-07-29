#!/usr/bin/python3
# -*- coding:utf-8 -*-
# Clean output of mut
import re
import sys

def clean_control_charactors(name):
    txt = open(name).read()
    x = re.compile("\\033\\[[0-9]+;[0-9][0-9]m")
    f = open(name, "w")
    f.write(x.sub("", txt))
    f.close()
    print("Cleaned:", name)


if __name__ == '__main__':
    if len(sys.argv) > 1:
        for name in sys.argv[1:]:
            clean_control_charactors(name)
    else:
        print("Usage: python3 mutclean.py mutlogfile")


