#!/usr/bin/env python3
# build the data pack for the game
# this must be ran in the root dir of the project

import os
import py7zr

dir_path = os.path.dirname(os.path.realpath(__file__))

try:
    os.mkdir(os.path.join(dir_path, "build"))
except:
    print("build dir already exists...")

z = py7zr.SevenZipFile(os.path.join(dir_path, "build/data.7z"), "w", filters=[{"id": py7zr.FILTER_LZMA2, "preset": 0}])

print("creating build/data.7z")
os.chdir(os.path.join(dir_path, "data"))
z.writeall('.')
z.close()
