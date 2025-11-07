##################
# CONFIGURATIONS #
##################
srcdir := .
objdir := $(srcdir)/.object
depdir := $(srcdir)/.depend
headir := $(srcdir)/header
datdir := dat
insdir := bin
hookdir := hook

exec := proofread
sources := $(wildcard $(srcdir)/*.c)
objects := $(sources:$(srcdir)/%.c=$(objdir)/%.o)
hook_exec := pre-commit
hook_srcs := $(hookdir)/$(hook_exec).c
hook_objs := $(hook_srcs:%.c=%.o)
hook_deps := $(addprefix $(objdir)/,strutil.o wrapper.o readline.o fatal.o)

SHELL := /bin/sh
CC := gcc
DEV_CFLAGS := -O3 -g -Wall -W -pedantic
CFLAGS := -O3 -g
CPPFLAGS := -I $(headir)

# resets the default suffix list
.SUFFIXES:
.SUFFIXES: .c .o .h

###############
# COMPILATION #
###############
# DEFAULT GOAL
$(exec): $(objects)
	$(CC) $^ $(CFLAGS) -o $@

EXT_LIST :=
SHUTUP := 1
hook: $(hook_deps)
	$(CC) $(hook_srcs) -DEXT_LIST='$(EXT_LIST)' -DSHUTUP=$(SHUTUP) $(CPPFLAGS) $(CFLAGS) -c -o $(hook_objs)
	$(CC) $^ $(hook_objs) $(CPPFLAGS) $(CFLAGS) -o $(hookdir)/$(hook_exec)

# auto-generates dependency files
$(depdir)/%.d: $(srcdir)/%.c
	@set -e; rm -f $@; \
	 $(CC) $< $(CPPFLAGS) -MM > $@.$$$$; \
	 printf "%s%s" "$(objdir)/" "$$(cat $@.$$$$)" > $@; \
	 rm -f $@.$$$$

include $(sources:$(srcdir)/%.c=$(depdir)/%.d)

$(objdir)/%.o:
	$(CC) $< $(CPPFLAGS) $(CFLAGS) -c -o $@

#################
# MISCELLANEOUS #
#################
.PHONY: clean
clean:
	rm -f $(exec) $(depdir)/*.d $(objdir)/*.o
	rm -f $(hookdir)/$(hook_exec) $(hookdir)/$(hook_exec).o

.PHONY: install
bakdir := bak
install: version
	test -d $(insdir) || (mkdir $(insdir) && mkdir $(insdir)/$(bakdir) && mkdir $(insdir)/$(datdir))
	install $(exec) $(insdir)
	install -m 444 --target-directory=$(insdir)/$(datdir) $(datdir)/*

.PHONY: version
version:
	git describe core > $(datdir)/version.txt

.PHONY: help
define clrstr
"\033[$1m$2\033[0m"
endef
Cyellow = $(call clrstr,33,$1)
help:
	@echo $(call Cyellow,make)
	@echo "    Builds the executable."
	@echo ""
	@echo $(call Cyellow,make install insdir=<directory-name>)
	@echo "    Copies the exe with related files under the specified directory."
	@echo "    Defaults to \"bin\"."
	@echo ""
	@echo $(call Cyellow,make hook EXT_LIST=<list> SHUTUP=<value>)
	@echo "    Builds the pre-commit hook. For the variables, please refer to the manual page."

###########
# DEVELOP #
###########
.PHONY: doc
doc:
	man $(datdir)/proofread.1 > $(datdir)/proofread.1.txt

.PHONY: remake
remake: version doc
	make $(exec) --always-make

.PHONY: dev
dev:
	make CFLAGS='$(DEV_CFLAGS)'

.PHONY: hookinst
hookinst:
	cp hook/pre-commit .git/hooks/

tests := nt lt ft at   ntl ltl ftl atl   st kt wt
.PHONY: $(tests)
.ONESHELL: $(tests)

# test: does not test against any condition
nt:
	cd test
	proofread --dry-run + -- ok l f lf

# test: against -l option
lt:
	cd test
	proofread -l --dry-run + -- ok l f lf

# test: against -f option
ft:
	cd test
	proofread -f --dry-run + -- ok l f lf

# test: against -l and -f options
at:
	cd test
	proofread -lf --dry-run + -- ok l f lf

# test: dry-run=line, no condition
ntl:
	cd test
	proofread --dry-run=line + -- ok l f lf

# test: dry-run=line, -l
ltl:
	cd test
	proofread -l --dry-run=line + -- ok l f lf

# test: dry-run=line, -f
ftl:
	cd test
	proofread -f --dry-run=line + -- ok l f lf

# test: dry-run=line, -l -f
atl:
	cd test
	proofread -lf --dry-run=line + -- ok l f lf

# test: stdin-stdout
st:
	cd test
	proofread < lf | proofread --dry-run=line
	proofread -l < lf | proofread -l --dry-run=line
	proofread -f < lf | proofread -f --dry-run=line
	proofread -lf < lf | proofread -lf --dry-run=line

# test: --keep option; compare original against fixed
kt:
	cp test/lf test/lf.copy

	proofread --keep=bak + test/lf
	proofread --dry-run=line + bak/lf
	proofread --dry-run=line + test/lf
	cp test/lf.copy test/lf

	proofread --keep=bak -l + test/lf
	proofread -l --dry-run=line + bak/lf
	proofread -l --dry-run=line + test/lf
	cp test/lf.copy test/lf

	proofread --keep=bak -f + test/lf
	proofread -f --dry-run=line + bak/lf
	proofread -f --dry-run=line + test/lf
	cp test/lf.copy test/lf

	proofread --keep=bak -lf + test/lf
	proofread -lf --dry-run=line + bak/lf
	proofread -lf --dry-run=line + test/lf
	cp test/lf.copy test/lf

	rm bak/lf
	rm test/lf.copy

# test: write to a file
wt:
	cd test
	cp lf lf.copy

	proofread + lf
	proofread --dry-run=line + lf
	cp lf.copy lf

	proofread -l + lf
	proofread -l --dry-run=line + lf
	cp lf.copy lf

	proofread -f + lf
	proofread -f --dry-run=line + lf
	cp lf.copy lf

	proofread -lf + lf
	proofread -lf --dry-run=line + lf
	cp lf.copy lf

	rm lf.copy
