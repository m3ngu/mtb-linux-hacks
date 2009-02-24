#!/usr/bin/perl

my $hwk_no = shift || 3;
my $SOURCE_PATH = "/usr/src/linux-2.6.11.12-hwk$hwk_no";
my $REPO_PATH = "/mnt/hgfs/kernel_homework/HW$hwk_no";
open MANIFEST, "<$REPO_PATH/MANIFEST";

for (<MANIFEST>) {
	chomp;
	my $sp = "$SOURCE_PATH/$_";
	my $rp = "$REPO_PATH/$_";
	if ( ! -e $rp ) {
		print "Initializing $_ in repository\n";
		system "cp $sp $rp";
	} elsif ( ! -e $sp or -M $sp > -M $rp) {
		print "$_ is not up to date\n";
#		system "diff -s $sp $rp";
		system "cp --remove-destination $rp $sp";
	}
}