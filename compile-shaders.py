#!/usr/bin/env python3
# compile HLSL shaders for D3D11 and Vulkan

import argparse
import os
import subprocess
import sys

parser = argparse.ArgumentParser(description='compile all hlsl files. this must be only ran in the root dir of the project! filenames must have the format: "[name].[stage].hlsl". example: test.vert.hlsl')
parser.add_argument('--fxc', type=str, help='path to fxc executable (Windows only)')
parser.add_argument('--glslc', type=str, help='path to glslc executable (all platforms)')
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

fxc_path = None
if windows_mode:
    fxc_path = find_program("fxc", args.fxc)

glslc_path = find_program("glslc", args.glslc)

dir_path = os.path.dirname(os.path.realpath(__file__))
shaders_path = dir_path + "/src/shaders/"

def compile_shaders_for_api(api, ext, compiler):
    subnames = [".vert.hlsl", ".frag.hlsl", ".comp.hlsl"]
    stage_to_profile_d3d11 = [["vert", "vs_5_0"], ["frag", "ps_5_0"], ["comp", "cs_5_0"]]

    output_path = dir_path + "/data/materials/shaders" + api
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
            output_file = output_path + "/" + output_name
            print("[%s] %s -> %s" % (api, file, output_name))

            if api == "d3d11":
                for list in stage_to_profile_d3d11:
                    if list[0] == parts_name[1]:
                        profile = list[1]
                        break

                subprocess.check_output([
                    compiler,
                    "-nologo",
                    "-T", profile,
                    hlsl_file,
                    "-Fo", output_file
                ])
            elif api == "vulkan":
                subprocess.check_output([
                    compiler,
                    "-fshader-stage=%s" % (parts_name[1]),
                    "-c", hlsl_file,
                    "-o", output_file,
                    "-D_VULKAN"
                ])

if windows_mode:
    compile_shaders_for_api("d3d11", ".dxbc", fxc_path)

compile_shaders_for_api("vulkan", ".spv", glslc_path)
