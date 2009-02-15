#!/usr/bin/perl

my $SOURCE_PATH = "/usr/src/linux-2.6.11.12-hwk2";
my $REPO_PATH = "/mnt/hgfs/kernel_homework";
open MANIFEST, "<$REPO_PATH/MANIFEST";

for (<MANIFEST>) {
	chomp;
	if ( ! -e "$SOURCE_PATH/$_"
		or -M "$SOURCE_PATH/$_" > -M "$REPO_PATH/$_") {
		print "$_ is not up to date\n";
		system "cp $REPO_PATH/$_ $SOURCE_PATH/$_";
	}
}