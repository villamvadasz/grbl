#!/usr/bin/perl


$num_args = $#ARGV + 1;
if ($num_args != 1) {
    print "\nUsage: eeprom_analyser.pl input_file\n";
    exit;
}

$input_file=$ARGV[0];

open(DATA, "<$input_file") or die "Couldn't open file $input_file, $!";

my $commandcounter = 0;
my $okcounter = 0;
my $diff = 0;

while(<DATA>) {
	$line = $_;
	
	if ($line =~ ">>>")	{
		if ($line =~ ">>> 0x91") {
		} elsif ($line =~ ">>> [\$]{1,1}") {
			$commandcounter ++;
		} elsif ($line =~ ">>> T1") {
			$commandcounter ++;
		} elsif ($line =~ ">>> S") {
			$commandcounter ++;
		} elsif ($line =~ ">>> M") {
			$commandcounter ++;
		} elsif ($line =~ ">>> G92") {
			$commandcounter ++;
		} elsif ($line =~ ">>> G0Z") {
			$commandcounter ++;
		} elsif ($line =~ ">>> G54") {
			$commandcounter ++;
		} elsif ($line =~ ">>> G10") {
			$commandcounter ++;
		} elsif ($line =~ ">>> G21") {
			$commandcounter ++;
		} elsif ($line =~ ">>> G90") {
			$commandcounter ++;
		} elsif ($line =~ ">>> G19") {
			$commandcounter ++;
		} elsif ($line =~ ">>> G17") {
			$commandcounter ++;
		} elsif ($line =~ ">>> G1X") {
			$commandcounter ++;
		} elsif ($line =~ ">>> G0X") {
			$commandcounter ++;
		} elsif ($line =~ ">>> F") {
			$commandcounter ++;
		} else {
			$commandcounter ++;
			print "$line\r\n";
		}
	}
	if ($line =~ "ok")	{
		$okcounter ++;
	}
	$currentdiff = $commandcounter - $okcounter;
	if ($currentdiff > $diff) {
		$diff = $currentdiff;
	}
}

print "Command: $commandcounter\r\n";
print "Ok:      $okcounter\r\n";
print "Diff:    $diff\r\n";
