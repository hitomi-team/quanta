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

class MakeDependencyParser():
    dependencies : list = []

    def __init__(self, blob : str):
        separator = blob.index(':') + 2
        newline = blob.index('\n')

        fileListStr = blob[separator:newline]
        dependencies = fileListStr.split()

        for i in dependencies:
            self.dependencies.append(Path(i))

class ShaderCompiler():
    target: str
    compilerPath: Path

    def __init__(self, compilerPath: str, target: str):
        self.target = target
        self.compilerPath = Path(which(compilerPath))
        self.compilerPath.resolve()

    def getDependencies(self, sourceFilePath: Path, shaderStage: str):
        return []

    def compile(self, sourceFilePath: Path, outputFilePath: Path, shaderStage: str):
        return False

class glslcCompiler(ShaderCompiler):
    def getDependencies(self, sourceFilePath: Path, shaderStage: str):
        try:
            stdout = subprocess.check_output([self.compilerPath,
                '-fshader-stage=' + shaderStage,
                '-M', sourceFilePath,
                '--target-env=vulkan1.2',
                '-D_VULKAN'
            ], stderr=subprocess.STDOUT, universal_newlines=True)
        except subprocess.CalledProcessError as exc:
            print(exc.output, file=sys.stderr)
            print(' ** Failed with error code:', exc.returncode, file=sys.stderr)
            return []

        return MakeDependencyParser(stdout).dependencies

    def compile(self, sourceFilePath: Path, outputFilePath: Path, shaderStage: str):
        try:
            stdout = subprocess.check_output([self.compilerPath,
                '-fshader-stage=' + shaderStage,
                '-c', sourceFilePath,
                '-o', outputFilePath,
                '-D_VULKAN',
                '--target-env=vulkan1.2',
                '-O'
            ], stderr=subprocess.STDOUT, universal_newlines=True)
            return True
        except subprocess.CalledProcessError as exc:
            print(exc.output, file=sys.stderr)
            print(' ** Failed with error code:', exc.returncode, file=sys.stderr)
            return False
        else:
            return False

        print(stdout)

class dxcCompiler(ShaderCompiler):
    shaderStages = ['vert', 'frag', 'geom', 'tess', 'comp']
    profiles = ['vs_6_2', 'ps_6_2', 'gs_6_2', 'ts_6_2', 'cs_6_2']

    # make a fake makedeps
    # this is ugly, but it will have to do for now
    def getDependencies(self, sourceFilePath: Path, shaderStage: str):
        try:
            profile = self.profiles[self.shaderStages.index(shaderStage)]
            spirvFlag = '-spirv' if self.target == 'spirv' else ''
            spirvEnvFlag = '-fspv-target-env=vulkan1.2' if self.target == 'spirv' else ''
            apiMacro = '-D_VULKAN' if self.target == 'spirv' else '-D_D3D12'
            stdout = subprocess.check_output([self.compilerPath,
                '-nologo',
                sourceFilePath, spirvFlag, spirvEnvFlag,
                '-Vi', '-Fo', '/dev/null' if os.name == 'posix' else 'nul',
                apiMacro,
                '-T', profile
            ], stderr=subprocess.STDOUT, universal_newlines=True)
        except subprocess.CalledProcessError as exc:
            print(exc.output, file=sys.stderr)
            print(' ** Failed with error code:', exc.returncode, file=sys.stderr)
            return []

        strs = stdout.split('\n')
        makedeps = ': ' + str(sourceFilePath) + ' '

        for i in strs:
            if '.hlsl' in i and '[' in i and ']' in i:
                makedeps += i[(i.index('[') + 1):i.index(']')] + ' '

        makedeps += '\n'
        return MakeDependencyParser(makedeps).dependencies

    def compile(self, sourceFilePath: Path, outputFilePath: Path, shaderStage: str):
        try:
            profile = self.profiles[self.shaderStages.index(shaderStage)]
            spirvFlag = '-spirv' if self.target == 'spirv' else ''
            spirvEnvFlag = '-fspv-target-env=vulkan1.2' if self.target == 'spirv' else ''
            apiMacro = '-D_VULKAN' if self.target == 'spirv' else '-D_D3D12'
            stdout = subprocess.check_output([self.compilerPath,
                '-nologo',
                sourceFilePath, spirvFlag, spirvEnvFlag,
                '-Fo', outputFilePath,
                apiMacro,
                '-T', profile
            ], stderr=subprocess.STDOUT, universal_newlines=True)
            return True
        except subprocess.CalledProcessError as exc:
            print(exc.output, file=sys.stderr)
            print(' ** Failed with error code:', exc.returncode, file=sys.stderr)
            return False
        else:
            return False

        print(stdout)

class ShaderBuildSystem():
    # Supported tools and targets
    tools = ['glslc', 'dxc']
    shaderStages = ['vert', 'frag', 'geom', 'tess', 'comp']

    dryRun: bool
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
        self.dryRun = args.dryRun

        self.cwd = Path(os.getcwd())
        self.sourceDir = self.cwd / Path(args.sourceDir)
        self.outputDir = self.cwd / Path(args.outputDir)

        if not self.sourceDir.exists():
            self.logError('Error', 'sourceDir not found: ' + str(self.sourceDir))
            sys.exit(1)

        if not self.outputDir.exists():
            self.logError('Error', 'outputDir not found: ' + str(self.sourceDir))
            sys.exit(1)

        if not args.tool in self.tools:
            self.logError('Error', 'Unsupported tool: ' + str(args.tool))
            sys.exit(1)

        if not checkForProgram(args.tool):
            self.logError('Error', 'Shader compiler tool not found: ' + str(args.tool))
            sys.exit(1)

        # Always SPIR-V
        if args.tool == 'glslc':
            self.target = 'spirv'
            self.fileExt = '.spv'
            self.shaderCompiler = glslcCompiler(args.tool, self.target)
        elif args.tool == 'dxc':
            if args.useDXIL:
                self.target = 'dxil'
                self.fileExt = '.dxo'
            elif args.useSPIRV:
                self.target = 'spirv'
                self.fileExt = '.spv'
            else:
                self.logError('Error', 'Choose a backend with dxc Compiler.')

            self.shaderCompiler = dxcCompiler(args.tool, self.target)

    def run(self):
        self.log('Info', 'Source Directory: ' + str(self.sourceDir))
        self.log('Info', 'Output Directory: ' + str(self.outputDir))
        self.log('Info', 'Compiler: ' + str(self.shaderCompiler.compilerPath))
        self.log('Info', 'Target: ' + self.target)

        # scrub to only find the files with shader stages
        shaderPaths = []
        for path in list(self.sourceDir.glob('*.hlsl')):
            parts = path.name.split('.')
            if parts[1] == 'hlsl':
                continue
            shaderPaths.append(path)

        self.log('Info', 'Build dependency list')

        # then scrub again to only find modified files or recompile
        shaderPathsToCompile = []
        for shaderPath in shaderPaths:
            parts = shaderPath.name.split('.')
            shaderName = parts[0]
            shaderStage = parts[1]
            outputPath = self.outputDir / Path(shaderName + '.' + shaderStage + self.fileExt)

            if outputPath.exists() and not self.recompileEverything:
                if os.path.getmtime(shaderPath) > os.path.getmtime(outputPath):
                    shaderPathsToCompile.append(shaderPath)
                    continue

                makeDependencies = self.shaderCompiler.getDependencies(shaderPath, shaderStage)
                for i in makeDependencies:
                    if os.path.getmtime(i) > os.path.getmtime(outputPath) and not shaderPath in shaderPathsToCompile:
                        shaderPathsToCompile.append(shaderPath)
                        continue
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

            outputPath = self.outputDir / Path(shaderName + '.' + shaderStage + self.fileExt)

            # Display counter
            print('[{}/{}] {} -> {}'.format(x, numShaderPaths, str(shaderPath.relative_to(self.sourceDir)), str(outputPath.relative_to(self.outputDir))))
            x += 1

            if not self.dryRun:
                if not self.shaderCompiler.compile(shaderPath, outputPath, shaderStage):
                    self.logError('Error', 'Could not compile shader: ' + shaderPath.name)
                    sys.exit(1)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='HLSL Shader Build System. Required tools must be in PATH!')
    parser.add_argument('-c', type=str, dest='sourceDir', metavar='DIRECTORY', help='Path to directory with shader sources', required=True)
    parser.add_argument('-o', type=str, dest='outputDir', metavar='DIRECTORY', help='Path to directory for compiler output', required=True)
    parser.add_argument('-t', type=str, dest='tool', metavar='PROGRAM', help='Compiler tool to use', required=True)
    parser.add_argument('--dxil', dest='useDXIL', help='Output DXIL for D3D12 (dxc only)', action='store_true')
    parser.add_argument('--spirv', dest='useSPIRV', help='Output SPIR-V for Vulkan (dxc only)', action='store_true')
    parser.add_argument('--recompile', dest='recompile', help='Recompile everything', action='store_true')
    parser.add_argument('--dry_run', dest='dryRun', help='Do not modify any files on disk', action='store_true')
    args = parser.parse_args()

    buildSystem = ShaderBuildSystem(args)
    buildSystem.run()
