# PROOFREAD
**proofread** - removes spaces at EOL (if any) and adds a newline at EOF (if not exist)
```bash
proofread [-l] [-f] [--dry-run[=(full|line)]] [-]
proofread [-l] [-f] [--dry-run[=(full|line)]] [--mute] + [--] <file>...
proofread [-l] [-f] [--keep[=<path>]] [--mute] + [--] <file>...
proofread (-h | --help)
proofread (-v | -vv | -vvv)
```

## Build and Install
```bash
# builds the executable
$ make

# copies the exe with related files under "bin" directory
$ make install

# (optional) appends to PATH
export PATH=$PATH:<path-to-installation-directory>
```
It is possible to specify a directory name when installing; please type `make help` for more information.

The structure of the installation directory is as follows:
```bash
$ tree bin
bin
├── bak
├── dat
│   ├── proofread.1
│   ├── proofread.1.txt
│   └── version.txt
└── proofread

3 directories, 4 files
```

## Usage
### Checking for EOL and EOF Problems
```bash
# Reads one file from stdin and prints to stdout
$ proofread -lf --dry-run=line - < input.txt

# Reads multiple files and prints to stdout
$ proofread -lf --dry-run=line + -- a.txt b.txt c.txt
```

### Fixing EOL and EOF Problems
```bash
# Fixes the problem in the input and overwrites to it
$ proofread -lf + -- input.txt

# Fixes the problem in the input and keeps it under a directory
$ proofread -lf --keep=backup/ + -- input.txt
```

### As a Filter
```bash
WORKING DIRECTORY  -->  CLEAN FILTER  -->  INDEX
 a.txt (original)  -->   "proofread"  -->  a.txt (fixed)
```
It is useful to have this program as a "clean" filter in git so that it "proofreads" text files before they are copied into the staging area. Please consult the manual page for more information.
