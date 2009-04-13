#!/usr/local/bin/perl

use strict;

my $ROOT = shift;
my $TMPDIR = "$ROOT.2";
my $TGZ = "$TMPDIR.tar.gz";
open my $readme, "<$ROOT/README" or die "Unable to open $ROOT/README: $!\n";
my @file_list;
while (<$readme>) {
	if (/^Files/ .. /^\s*$/) { # only this paragraph
		if (/^\t(\S+)\s*-/) {  # one tab, then a name, then a dash
			push @file_list, $1;
		}
	}
}

mkdir "$ROOT.2"; # should remove first

system "tar -C $ROOT -cf - @file_list | tar -C $ROOT.2 -xf -";
system "tar -czvf $TGZ $TMPDIR";
