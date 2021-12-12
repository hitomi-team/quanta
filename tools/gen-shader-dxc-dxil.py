#!/usr/bin/env python3
# this needs to be ran from the root directory of the tree!

from importlib import import_module
import errno
import os

class BuildSystemOptions:
    sourceDir = os.getcwd() + '/src/shaders'
    outputDir = os.getcwd() + '/data/shadersd3d12'
    tool = 'dxc'
    useDXIL = True
    useSPIRV = False
    recompile = False
    dryRun = False

if __name__ == '__main__':
    opts = BuildSystemOptions()

    try:
        os.mkdir(opts.outputDir)
    except OSError as exc:
        if exc.errno != errno.EEXIST:
            raise
        pass

    compileShaderScript = import_module('compile-shaders-v2')
    buildSystem = compileShaderScript.ShaderBuildSystem(opts)
    buildSystem.run()
