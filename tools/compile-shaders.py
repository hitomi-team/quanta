#!/usr/bin/env python3
# compile HLSL shaders for D3D11 and Vulkan

import argparse
import os
import subprocess
import sys

parser = argparse.ArgumentParser(description='compile all hlsl files. this must be only ran in the root dir of the project! filenames must have the format: "[name].[stage].hlsl". example: test.vert.hlsl')
parser.add_argument('--glslc', type=str, help='path to glslc executable')
parser.add_argument('--dxc', type=str, help='path to dxc executable')
parser.add_argument('--dxil', type=bool, help='compile DXIL')
parser.add_argument('--spirv', type=bool, help='compile SPIR-V')
args = parser.parse_args()

path_env = os.environ["PATH"]

windows_mode = False
if os.name == 'nt':
    windows_mode = True

def has_execute(path):
    return os.path.isfile(path) and os.access(path, os.X_OK)

def find_program(exe_name, prepath):
    if prepath != None and has_execute(prepath):
        return prepath

    if windows_mode:
        exe_name += '.exe'

    for exe_dir in path_env.split(os.pathsep):
        full_path = os.path.join(exe_dir, exe_name)
        if has_execute(full_path):
            return full_path

    sys.exit("could not find an executable \"%s\"." % (exe_name))

glslc_path = find_program("glslc", args.glslc)

# dir_path = os.path.dirname(os.path.realpath(__file__))
dir_path = os.getcwd()
shaders_path = dir_path + "/src/shaders/"

def compile_shaders_for_api(api, ext, compiler):
    subnames = [".vert.hlsl", ".frag.hlsl", ".comp.hlsl"]
    stage_to_profile_d3d11 = [["vert", "vs_6_2"], ["frag", "ps_6_2"], ["comp", "cs_6_2"]]

    output_path = dir_path + "/data/shaders" + api + "/"
    try:
        os.mkdir(output_path)
    except:
        print(output_path + " cannot be made. continuing...")

    for root, dirs, files in os.walk(shaders_path):
        for file in files:
            valid = False
            for subname in subnames:
                if file.endswith(subname):
                    valid = True
                    break

            if not valid:
                continue

            # [0] - basename, [1] - stage
            parts_name = file.split('.')
            output_name = parts_name[0] + '.' + parts_name[1] + ext

            hlsl_file = os.path.join(root, file)
            output_file = output_path + output_name
            print("[%s] %s -> %s" % (api, hlsl_file, output_file))

            if api == "vulkan":
                subprocess.check_output([
                    compiler,
                    "-fshader-stage=%s" % (parts_name[1]),
                    "-c", hlsl_file,
                    "-o", output_file,
                    "-D_VULKAN"
                ])

compile_shaders_for_api("vulkan", ".spv", glslc_path)
