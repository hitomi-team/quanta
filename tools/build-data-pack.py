#!/usr/bin/env python3
# build the data pack for the game
# this must be ran in the root dir of the project

import os
import zipfile
import argparse

parser = argparse.ArgumentParser(description='build data pack')
parser.add_argument('-o', type=str, help='output directory', required=True)
args = parser.parse_args()

dir_path = os.getcwd()
open_path = args.o

z = zipfile.ZipFile(os.path.join(open_path, "data.zip"), "w", compression=zipfile.ZIP_STORED)

# this is done so the zip shows the inlined content of data
data_path = os.path.normpath(os.path.join(dir_path, 'data'))
print('chdir into:', data_path)
os.chdir(data_path)

print('writing data.zip to:', os.path.join(open_path, "data.zip"))
for root, dirs, files in os.walk("."):
    for file in files:
        path = os.path.normpath(os.path.join(root, file))
        print("adding: " + path)
        z.write(path)

z.close()
