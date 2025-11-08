# PROOFREAD
**proofread** - removes spaces at EOL (if any) and adds a newline at EOF (if not exist)
```bash
proofread [-l] [-f] [--dry-run[=(full|line)]] [-]
proofread [-l] [-f] [--dry-run[=(full|line)]] [--mute] + [--] <file>...
proofread [-l] [-f] [--keep[=<path>]] [--mute] + [--] <file>...
proofread (-h | --help)
proofread (-v | -vv | -vvv)
```

## Installation
```bash
$ make install insdir=<absolute-path>
```
The `make install` call builds `proofread`, the executable, and installs the program under the directory given by `<absolute-path>`. The path must not be a relative path. The program needs this path so as to work properly.

The structure of the installation directory is as follows:
```bash
$ tree <absolute-path>
<absolute-path>
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

### Pre-commit Hook
There is a way to apply **proofread** automatically, when `git commit` is executed, on the files which are staged (by a prior `git add`) and whose state is modified or newly added. The pre-commit hook does this task. Its behavior differs depending on whether the files has local changes or not. If it does, the hook updates the index only. Otherwise, the hook updates both the index and the worktree.

In order to use it, the hook must be compiled first and installed under the `.git/hooks` directory of the desired git repository:
```bash
$ make hook EXT_LIST='".c", ".h"' SHUTUP=1
$ cp hook/pre-commit <git-repo>/.git/hooks
```

Two variables affect the behavior of the hook:
- `EXT_LIST` defines the extensions to apply **proofread**. The value must be enclosed with a single-quote mark and each extension must be enclosed with a double-quote mark. If not specified, all of the files which meet the above conditions are subject to the application.
- `SHUTUP` sets the verbosity of the hook. The value of **1** means **true** and **0**, **false**.

## Verification on Tags and Commits
If one is in need of verifying the tags and commits, please type `git show maintainer-pgp-pub` and follow the instruction written in it.
