#!/usr/bin/env python3
# Compile HLSL shaders, now with build system like behavior!

from pathlib import Path
from shutil import which

import argparse
import os
import subprocess
import sys

def checkForProgram(programName: str):
    return which(programName) is not None

class ShaderCompiler():
    target: str
    compilerPath: Path

    def __init__(self, compilerPath: str, target: str):
        self.target = target
        self.compilerPath = Path(which(compilerPath))
        self.compilerPath.resolve()

    def compile(self, sourceFilePath: Path, outputFilePath: Path, shaderStage: str):
        return False

class glslcCompiler(ShaderCompiler):
    def compile(self, sourceFilePath: Path, outputFilePath: Path, shaderStage: str):
        try:
            subprocess.check_output([self.compilerPath,
                '-fshader-stage=' + shaderStage,
                '-c', sourceFilePath,
                '-o', outputFilePath,
                '-D_VULKAN',
                '-O'
            ], stderr=subprocess.STDOUT, universal_newlines=True)
            return True
        except subprocess.CalledProcessError as exc:
            print(exc.output, file=sys.stderr)
            print(' ** Failed with error code:', exc.returncode, file=sys.stderr)
            return False
        else:
            print(stdout)
            return False

class dxcCompiler(ShaderCompiler):
    shaderStages = ['vert', 'frag', 'geom', 'tess', 'comp']
    profiles = ['vs_6_0', 'ps_6_0', 'gs_6_0', 'ts_6_0', 'cs_6_0']
    def compile(self, sourceFilePath: Path, outputFilePath: Path, shaderStage: str):
        try:
            profile = profiles[shaderStages.index(shaderStage)]
            subprocess.check_output([self.compilerPath,
                '-nologo',
                sourceFilePath,
                '-Fo', outputFilePath,
                '-D_D3D12',
                '-T', profile
            ], stderr=subprocess.STDOUT, universal_newlines=True)
            return True
        except subprocess.CalledProcessError as exc:
            print(exc.output, file=sys.stderr)
            print(' ** Failed with error code:', exc.returncode, file=sys.stderr)
            return False
        else:
            print(stdout)
            return False

class ShaderBuildSystem():
    # Supported tools and targets
    tools = ['glslc', 'dxc']
    shaderStages = ['vert', 'frag', 'geom', 'tess', 'comp']

    recompileEverything: bool

    cwd: Path
    sourceDir: Path
    outputDir: Path

    tool: Path
    target: str
    fileExt: str
    shaderCompiler: ShaderCompiler

    def log(self, prefix, msg):
        print('ShaderBuildSystem ** {}: {}'.format(prefix, msg))

    def logError(self, prefix, msg):
        print('ShaderBuildSystem ** {}: {}'.format(prefix, msg), file=sys.stderr)

    def __init__(self, args):
        self.recompileEverything = args.recompile

        self.cwd = Path(os.getcwd())
        self.sourceDir = self.cwd / Path(args.sourceDir)
        self.outputDir = self.cwd / Path(args.outputDir)

        if not self.sourceDir.exists():
            self.logError('Error', 'sourceDir not found: ' + self.sourceDir)
            sys.exit(1)

        if not self.outputDir.exists():
            self.logError('Error', 'outputDir not found: ' + self.sourceDir)
            sys.exit(1)

        if not args.tool in self.tools:
            self.logError('Error', 'Unsupported tool: ' + args.tool)
            sys.exit(1)

        if not checkForProgram(args.tool):
            self.logError('Error', 'Shader compiler tool not found: ' + args.tool)
            sys.exit(1)

        # Always SPIR-V
        if args.tool == 'glslc':
            self.target = 'spirv'
            self.fileExt = '.spv'
            self.shaderCompiler = glslcCompiler(Path(args.tool), self.target)
        elif args.tool == 'dxc':
            if args.useDXIL:
                self.target = 'dxil'
                self.fileExt = '.dxo'
            elif args.useSPIRV:
                self.target = 'spirv'
                self.fileExt = '.spv'
            else:
                self.logError('Error', 'Choose a backend with dxc Compiler.')

            self.shaderCompiler = dxcCompiler(Path(args.tool), self.target)

    def run(self):
        self.log('Info', 'Source Directory: ' + str(self.sourceDir))
        self.log('Info', 'Output Directory: ' + str(self.outputDir))
        self.log('Info', 'Compiler: ' + str(self.shaderCompiler.compilerPath))
        self.log('Info', 'Target: ' + self.target)

        try:
            # scrub to only find the files with shader stages
            shaderPaths = []
            for path in list(self.sourceDir.glob('*.hlsl')):
                parts = path.name.split('.')
                if parts[1] == 'hlsl':
                    continue
                shaderPaths.append(path)

            # then scrub again to only find modified files or recompile
            shaderPathsToCompile = []
            for shaderPath in shaderPaths:
                parts = shaderPath.name.split('.')
                shaderName = parts[0]
                shaderStage = parts[1]
                outputPath = self.outputDir / Path(shaderName + "." + shaderStage + self.fileExt)

                if outputPath.exists() and not self.recompileEverything:
                    if os.path.getmtime(outputPath) < os.path.getmtime(shaderPath):
                        shaderPathsToCompile.append(shaderPath)
                else:
                    shaderPathsToCompile.append(shaderPath)

            numShaderPaths = len(shaderPathsToCompile)
            x = 1

            if numShaderPaths == 0:
                self.log('Info', 'Everything up-to-date.')

            # then start compiling
            for shaderPath in shaderPathsToCompile:
                # Grab various info from filename
                parts = shaderPath.name.split('.')
                shaderName = parts[0]
                shaderStage = parts[1]

                outputPath = self.outputDir / Path(shaderName + "." + shaderStage + self.fileExt)

                # Display counter
                print('[{}/{}] {} -> {}'.format(x, numShaderPaths, str(shaderPath.relative_to(self.sourceDir)), str(outputPath.relative_to(self.sourceDir))))
                x += 1

                if not self.shaderCompiler.compile(shaderPath, outputPath, shaderStage):
                    self.logError('Error', 'Could not compile shader: ' + shaderPath.name)
                    raise Exception()
        except:
            self.logError('Error', 'Failure occurred while compiling shaders!')

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='HLSL Shader Build System. Required tools must be in PATH!')
    parser.add_argument('-c', type=str, dest='sourceDir', metavar='DIRECTORY', help='Path to directory with shader sources', required=True)
    parser.add_argument('-o', type=str, dest='outputDir', metavar='DIRECTORY', help='Path to directory for compiler output', required=True)
    parser.add_argument('-t', type=str, dest='tool', metavar='PROGRAM', help='Compiler tool to use', required=True)
    parser.add_argument('--dxil', dest='useDXIL', help='Output DXIL for D3D12 (dxc only)', action='store_true')
    parser.add_argument('--spirv', dest='useSPIRV', help='Output SPIR-V for Vulkan (dxc only)', action='store_true')
    parser.add_argument('--recompile', dest='recompile', help='Recompile everything', action='store_true')
    args = parser.parse_args()

    buildSystem = ShaderBuildSystem(args)
    buildSystem.run()
