#!/usr/bin/perl

my $SOURCE_PATH = "/usr/src/linux-2.6.11.12-hwk2";
my $REPO_PATH = "/mnt/hgfs/kernel_homework";
open MANIFEST, "<$REPO_PATH/MANIFEST";

for (<MANIFEST>) {
	chomp;
	my $sp = "$SOURCE_PATH/$_";
	my $rp = "$REPO_PATH/$_";
	if ( ! -e $sp or -M $sp > -M $rp) {
		print "$_ is not up to date\n";
#		system "diff -s $sp $rp";
		system "cp $REPO_PATH/$_ $SOURCE_PATH/$_";
	}
}