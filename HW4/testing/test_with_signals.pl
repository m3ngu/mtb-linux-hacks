#!/usr/bin/perl


die "You must be root to run this script\n" if $<;

print "\n\nChanging Perl script (PID:$$) to Real-time (round-robin) prio=50 using chrt.\n\n";
system("chrt -r -p 50 $$");

print "Changing Kernel event handler (PID:4) to Real-time (round-robin) prio=50 using chrt.\n\n";
system("chrt -r -p 50 4");

my $bash_pid = getppid();
print "Switching current bash (PID:$bash_pid) to Real-time (round-robin) prio=50 using chrt.\n\n";
system("chrt -r -p 50  $bash_pid");

print "Hit return to proceed.\n\n";
$user_input = <STDIN>;

print "Launching inf-loop's as normal processes.\n\n";

my $spawnfmt = "chrt -o 0 sudo -b -u %s ./inf-loop 30";
system sprintf($spawnfmt, $_) for qw(alice alice carol bob);
chomp(my @pids = `ps -o pid= -u alice -u bob -u carol`);

print "Got pids (@pids) for child processes.\n";

print "Hit return to proceed.\n\n";
$user_input = <STDIN>;

print "Launching loop as root (will be change to Alice in 10 seconds of runtime).\n\n";
my $pid = fork();
if ($pid) {
	push @pids, $pid;
	print "Added setuid test case (PID: $pid) to child processes (@pids).\n\n";
	sleep 1;
} else { 
	exec "chrt -o 0 ./inf-loop 30 alice 10";
}

print "Suspending child processes to change their scheduler to UWRR.\n\n";
kill "STOP", @pids;

foreach my $pid (@pids) {
	my $stat = system "./change-scheduler", $pid, 4, 50;
	if ($stat) {
		warn "System failed in some regard for $pid \n";
	} else {
		print "System seems OK for changing $PID scheduler\n";
	}
}

print "Waking up child processes (their scheduler should now be UWRR).\n\n";
kill "CONT", @pids;

print "Launching top (update freq=5) to observe UWRR at work.\n\n";

exec qw(chrt -r -p 60 top -d 5);
