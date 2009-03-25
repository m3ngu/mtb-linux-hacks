#!/usr/bin/perl

system("chrt -r -p 50 $$");

my $spawnfmt = "chrt -o 0 sudo -b -u %s ./inf-loop";

system sprintf($spawnfmt, $_) for qw(alice alice carol bob);

chomp(my @pids = `ps -o pid= -u alice -u bob -u carol`);
print "Got pids @pids for child processes\n";
kill "STOP", @pids;
foreach my $pid (@pids) {
	my $stat = system "./change-scheduler", $pid, 4, 50;
	if ($stat) {
		warn "System failed in some regard for $pid \n";
	} else {
		print "System seems OK for changing $PID scheduler\n";
	}
}
kill "CONT", @pids;
exec qw(chrt -r -p 60 top);