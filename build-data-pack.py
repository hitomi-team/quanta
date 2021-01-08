#!/usr/bin/env python3
# build the data pack for the game
# this must be ran in the root dir of the project

import os
import zipfile

dir_path = os.path.dirname(os.path.realpath(__file__))

try:
    os.mkdir(os.path.join(dir_path, "build"))
except:
    print("build dir already exists...")

print("building data pack")

z = zipfile.ZipFile(os.path.join(dir_path, "build/data.zip"), "w", compression=zipfile.ZIP_STORED) 
os.chdir(os.path.join(dir_path, "data"))

for root, dirs, files in os.walk("."):
    for file in files:
        print("adding: data/" + os.path.join(root, file))
        z.write(os.path.join(root, file))

z.close()
