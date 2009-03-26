#!/usr/bin/perl


die "You must be root to run this script\n" if $<;
print "Changing Perl script to Real-time\n\n";
system("chrt -r -p 50 $$");

print "Changing Kernel event handler to Real-time\n\n";
system("chrt -r -p 50 4");

print "Switching current bash to Real-time\n\n";
my $bash_pid = getppid();
system("chrt -r -p 50  $bash_pid");

my $spawnfmt = "chrt -o 0 sudo -b -u %s ./inf-loop 30";

system sprintf($spawnfmt, $_) for qw(alice alice carol bob);


chomp(my @pids = `ps -o pid= -u alice -u bob -u carol`);

print "Launching loop as root for setuid test\n";
my $pid = fork();
if ($pid) {
	push @pids, $pid;
	sleep 1;
} else { 
	exec "chrt -o 0 ./inf-loop 30 alice 5";
}

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
exec qw(chrt -r -p 60 top -d 4);
