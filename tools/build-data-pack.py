#!/usr/bin/env python3
# build the data pack for the game
# this must be ran in the root dir of the project

import os
import zipfile
import argparse

parser = argparse.ArgumentParser(description='build data pack')
parser.add_argument('-o', type=str, help='output directory')
args = parser.parse_args()

dir_path = os.path.dirname(os.path.realpath(__file__))

open_path = os.path.dirname(os.path.realpath(__file__))
if (args.o != None):
    open_path = args.o

print("building data pack")

z = zipfile.ZipFile(os.path.join(open_path, "data.zip"), "w", compression=zipfile.ZIP_STORED) 

# this is done so the zip shows the inlined content of data
os.chdir(os.path.join(dir_path, "data"))

for root, dirs, files in os.walk("."):
    for file in files:
        print("adding: data/" + os.path.join(root, file))
        z.write(os.path.join(root, file))

z.close()
