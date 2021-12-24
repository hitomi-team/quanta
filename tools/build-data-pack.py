#!/usr/bin/env python3
# build the data archive for the game

from pathlib import Path
from shutil import which

import argparse
import os
import zipfile

class DataArchiver():
    dryRun: bool
    compressed: bool
    rebuild: bool

    cwd: Path
    sourceDir: Path
    outputDir: Path

    filesUnarchivable = [
        '.gitignore'
    ]

    def log(self, prefix, msg):
        print('DataArchiver ** {}: {}'.format(prefix, msg))

    def logError(self, prefix, msg):
        print('DataArchiver ** {}: {}'.format(prefix, msg), file=sys.stderr)

    def __init__(self, args):
        self.dryRun = args.dryRun
        self.compressed = args.compressed
        self.rebuild = args.rebuild

        self.cwd = Path(os.getcwd())
        self.sourceDir = self.cwd / Path(args.sourceDir)
        self.outputDir = self.cwd / Path(args.outputDir)

        if not self.sourceDir.exists():
            self.logError('Error', 'sourceDir not found: ' + str(self.sourceDir))
            sys.exit(1)

        if not self.outputDir.exists():
            self.logError('Error', 'outputDir not found: ' + str(self.outputDir))
            sys.exit(1)

        os.chdir(str(self.sourceDir))

    def __del__(self):
        if not self.dryRun and self.rebuild:
            self.zipFile.close()

    def run(self):
        fileList = sorted(Path('.').glob('**/*'))
        popIndices = []

        # preprocess fileList
        for path in self.filesUnarchivable:
            try:
                fileList.pop(fileList.index(Path(path)))
            except ValueError as exc:
                pass

        outputPath = self.outputDir / Path('data.zip')

        # check for new files
        self.rebuild = not outputPath.exists()
        if not self.rebuild:
            for path in fileList:
                if os.path.getmtime(path) > os.path.getmtime(outputPath):
                    self.rebuild = True
                    break

        if not self.rebuild:
            self.log('Info', 'Data archive up to date.')
            return

        if not self.dryRun:
            self.zipFile = zipfile.ZipFile(self.outputDir / 'data.zip', 'w', compression=zipfile.ZIP_STORED if not self.compressed else zipfile.ZIP_DEFLATED)

        # start archiving
        for path in fileList:
            self.log('Info', 'Archiving: {}'.format(str(path)))

            if not self.dryRun:
                self.zipFile.write(str(path))

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Data Archiver')
    parser.add_argument('-c', type=str, dest='sourceDir', metavar='DIRECTORY', help='Path to directory of data', required=True)
    parser.add_argument('-o', type=str, dest='outputDir', metavar='DIRECTORY', help='Path to directory for output', required=True)
    parser.add_argument('--dry_run', dest='dryRun', help='Do not modify any files on disk', action='store_true')
    parser.add_argument('--compressed', dest='compressed', help='Compress data (use for testing or no delta patching capability)', action='store_true')
    parser.add_argument('--rebuild', dest='rebuild', help='Rebuild archive', action='store_true')
    args = parser.parse_args()

    buildSystem = DataArchiver(args)
    buildSystem.run()
