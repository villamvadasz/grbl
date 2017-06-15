#!/usr/bin/perl


$num_args = $#ARGV + 1;
if ($num_args != 1) {
    print "\nUsage: eeprom_analyser.pl input_file\n";
    exit;
}

$input_file=$ARGV[0];

open(DATA, "<$input_file") or die "Couldn't open file $input_file, $!";

my @eeprom_memory_journaling;
my $eeprom_memory_journaling_element = 0;

my @eeprom_memory_linear;
my $eeprom_memory_linear_element = 0;

while(<DATA>) {
	$line = $_;
	
	if ($foundDumptStart) {
		if ($line =~ "Address: ([0123456789ABCDEF]{8,8}) Data: ([0123456789ABCDEF]{8,8})")	{
			#print "Address ";
			#print $1;
			#print "\n";
			#print "Data ";
			#print $2;
			#print "\n";
			$address = hex($1);
			$data = hex($2);
			#print "Address $address Data $data\n";
			$eeprom_memory_journaling[$eeprom_memory_journaling_element] = [$address,$data];
			#print "$eeprom_memory_journaling[$eeprom_memory_journaling_element][0] $eeprom_memory_journaling[$eeprom_memory_journaling_element][1]\n";
			$eeprom_memory_journaling_element ++;
		}	
		#print $line;
	}
	
	if ($line =~ "eeprom dump:") {
		if ($foundDumptStart != 0) {
			die "Wrong dump, more than one eeprom dump string was found in the file\n";
		} else {
			$foundDumptStart = 1;
		}
	}	
	if ($line =~ "eeprom dump finished!")	{
		if ($foundDumptStart != 1) {
			die "Wrong dump, eeprom dump finished string found before start\n";
		} else {
			$foundDumptEnd = 1;
		}
	}	
}

if (($foundDumptEnd != 1) && ($foundDumptStart != 1)) {
	die "Unable to find eeprom dump start and/or end\n";
}

my $i;
my $elements;
for ($i  = 0; $i < scalar( @eeprom_memory_journaling ); $i++ )
{
	#print "$eeprom_memory_journaling[$i][0] $eeprom_memory_journaling[$i][1]\n";
}
$elements = $i;
$elementsstring = sprintf("0x%X", $elements);
print "Elements found: $elementsstring \n";
print "Elements expected: 0x2400\n";

if ($i != 0x2400) {
	die "Wrong number of elements found";
}
my $size = $elements * 4;
$sizestring = sprintf("0x%X", $size);

my $baseaddress = 0xFFFFFFFF;
for ($i  = 0; $i < scalar( @eeprom_memory_journaling ); $i++ )
{
	if ($baseaddress > $eeprom_memory_journaling[$i][0]) {
		$baseaddress = $eeprom_memory_journaling[$i][0];
	}
}
$baseaddressstring = sprintf("0x%X", $baseaddress);
print "Base Address: $baseaddressstring Size: $sizestring\n";
$baseaddressstring_map = sprintf("0x%X", ($baseaddress & 0xDFFFFFFF));
print "Base Address: $baseaddressstring_map Size: $sizestring <<= Look in Map File\n";


my $DEE_EEPROM_SIZE = 4096;
my $DEE_PAGE_CNT = 3;
my $DEE_PAGE_PAGE_SIZE = 3;
my $EEPROM_FLASH_ADDR = hex("0x9D030000");

if ($EEPROM_FLASH_ADDR != ($baseaddress & 0xDFFFFFFF)) {
	die "Configuration mismatch\n";
}

#typedef struct _Dee_Type 
#	Dee_Page_Status status;
#	uint32 data[DEE_NUMBER_OF_DATA_ELEMENTS];
#	uint32 crc[DEE_NUMBER_OF_DATA_ELEMENTS];
#	uint32 address[DEE_NUMBER_OF_DATA_ELEMENTS];
# Dee_Type;
#

my $firstActivePage = -1;

my $statusProblemPresent = 0;
my $current_active_count = 0;
my $current_active_page = 0;
for ($i  = 0; $i < $DEE_PAGE_CNT; $i++ ) {
	$statusaddress = $baseaddress + ($i * $DEE_EEPROM_SIZE * $DEE_PAGE_PAGE_SIZE);
	$statusaddressstring = sprintf("0x%X", $statusaddress);

#	typedef struct _Dee_Page_Status 
#		uint32 page_expired : 1;    //1 - not expired 0 - expired
#		uint32 page_current : 1;    //1 - current     0 - not current
#		uint32 page_available : 1;  //1 - available   0 - not available
#		uint32 page_active : 1;     //1 - not active  0 - active
#		uint32 page_erase_count :28;
#	 Dee_Page_Status;
	$status = -1;
	for ($x  = 0; $x < scalar( @eeprom_memory_journaling ); $x++ ) {
		if ($eeprom_memory_journaling[$x][0] == $statusaddress) {
			$status = $eeprom_memory_journaling[$x][1];
		}
	}
	$statusstring =  sprintf("0x%X", $status);
	print "Page $i status: $statusstring Address: $statusaddressstring\n";
	if ($status & 0x01) {
		print "\t page_expired: not expired\n";
	} else {
		print "\t page_expired: expired\n";
		$statusProblemPresent |= 0x1;
	}
	if ($status & 0x02) {
		print "\t page_current: current\n";
	} else {
		print "\t page_current: not current\n";
	}
	if ($status & 0x04) {
		print "\t page_available: available\n";
	} else {
		print "\t page_available: not available\n";
		$statusProblemPresent |= 0x2;
	}
	if ($status & 0x08) {
		print "\t page_active: not active\n";
	} else {
		if ($firstActivePage eq -1) {
			$firstActivePage = $i;
			print "\t page_active: active\n";
		} else {
			print "\t page_active: active MORE THEN ONE PAGE IS ACTIVE\n";
		}
	}
	$erasecount = ($status & 0xFFFFFFF0) >> 4;
	$erasecountstring =  sprintf("0x%X", $erasecount);
	print "\t page_erase_count: $erasecount $erasecountstring\n";
	
	if (($status & 0x02) && (!($status & 0x08))) {
		$current_active_page = $i;
		$current_active_count ++;
	}
	
	if ($current_active_count > 1) {
		$statusProblemPresent |= 0x4;
	}
	
	if ($statusProblemPresent != 0) {
		print "Some kind of a problem is present in the status!\n";
		if ($statusProblemPresent & 0x1) {
			print "\t page_expired was set to expired, but it should never reach this case!\n";
		}
		if ($statusProblemPresent & 0x2) {
			print "\t page_available was set to not available, but it must be always available!\n";
		}
		if ($statusProblemPresent & 0x4) {
			print "\t More than one page ic current and active!\n";
		}
	}
}

my $NUMBER_OF_BYTES_IN_PAGE = 4096;
my $NUMBER_OF_INSTRUCTIONS_IN_PAGE = ($NUMBER_OF_BYTES_IN_PAGE / 4); #// number of 32-bit word instructions per page, 4096 bytes
my $DEE_NUMBER_OF_DATA_ELEMENTS =  int((($NUMBER_OF_INSTRUCTIONS_IN_PAGE * $DEE_PAGE_PAGE_SIZE) - 1) / 3);

print "Data elements in one page: $DEE_NUMBER_OF_DATA_ELEMENTS\n";

my $processingPage = -1;
my $xt = 0;

for ($xt  = 0; $xt < $DEE_PAGE_CNT; $xt++ ) {
	$processingPage = $firstActivePage + 1 + $xt;
	$processingPage %= $DEE_PAGE_CNT;

	$pagebaseaddress = $baseaddress + ($processingPage * $DEE_EEPROM_SIZE * $DEE_PAGE_PAGE_SIZE);
	$pagebaseaddressstring = sprintf("0x%X", $pagebaseaddress);
	print "Processing page $processingPage $pagebaseaddressstring\n";
	for ($i  = 0; $i < $DEE_NUMBER_OF_DATA_ELEMENTS; $i++ ) {
		$data = 0;
		$crc = 0;
		$address = 0;
		
		$data_adr = ($pagebaseaddress + 4 + ($i * 4) + ($DEE_NUMBER_OF_DATA_ELEMENTS * 4 * 0) );
		$crc_adr = ($pagebaseaddress + 4 + ($i * 4) + ($DEE_NUMBER_OF_DATA_ELEMENTS * 4 * 1) );
		$address_adr = ($pagebaseaddress + 4 + ((($DEE_NUMBER_OF_DATA_ELEMENTS - 1 ) * 4) - ($i * 4)) + ($DEE_NUMBER_OF_DATA_ELEMENTS * 4 * 2) );
		$data_adrstring = sprintf("0x%X", $data_adr);
		$crc_adrstring = sprintf("0x%X", $crc_adr);
		$address_adrstring = sprintf("0x%X", $address_adr);
		#print "$data_adrstring\n";
		#print "$crc_adrstring\n";
		#print "$address_adrstring\n";
		
		for ($d  = 0; $d < scalar( @eeprom_memory_journaling ); $d++ ) {
			if ($eeprom_memory_journaling[$d][0] == $data_adr) {
				$data = $eeprom_memory_journaling[$d][1];
			}
		}
		for ($d  = 0; $d < scalar( @eeprom_memory_journaling ); $d++ ) {
			if ($eeprom_memory_journaling[$d][0] == $crc_adr) {
				$crc = $eeprom_memory_journaling[$d][1];
			}
		}
		for ($d  = 0; $d < scalar( @eeprom_memory_journaling ); $d++ ) {
			if ($eeprom_memory_journaling[$d][0] == $address_adr) {
				$address = $eeprom_memory_journaling[$d][1];
			}
		}
		
		
		$datastring = sprintf("0x%X", $data);
		$crcstring = sprintf("0x%X", $crc);
		$addressstring = sprintf("0x%X", $address);

		#print "$addressstring: $datastring $crcstring\n";
		if (($address == 0xFFFFFFFF) && ($crc == 0xFFFFFFFF) && ($data == 0xFFFFFFFF)) {
		} else {
			if (($data & 0xFFFFFFFF) == ((~$crc) & 0xFFFFFFFF)) {
				if ($address >= 0x100) {
					print "$addressstring: $datastring $crcstring\n";
					die "Invalid address\n";
				}
				$eeprom_memory_linear[$address] = $data;
			} else {
				print "CRC mismatch: $addressstring: $datastring $crcstring\n";
				#die "CRC mismatch\n";
			}
		}
		if ($i == 4) {
			#last;
		}
	}
}

for ($d  = 0; $d < scalar( @eeprom_memory_linear ); $d++ ) {
	$dstring = sprintf("0x%X", $d);
	$data = $eeprom_memory_linear[$d];
	$datastring = sprintf("0x%X", $data);
	
	$byte3 = (($data >> 0) & 0xFF);
	$byte2 = (($data >> 8) & 0xFF);
	$byte1 = (($data >> 16) & 0xFF);
	$byte0 = (($data >> 24) & 0xFF);

	for ($q  = 0; $q < 4; $q++ ) {
		$byte_add = ($d * 4) + $q;
		$byte_val = (($data >> (8*$q)) & 0xFF);
		$byte_add_hex = sprintf("0x%X", $byte_add);
		$byte_val_hex = sprintf("0x%X", $byte_val);
		
		add_value_holder($byte_val);

		print "$byte_add_hex: $byte_val_hex";
		if ($byte_add eq 0x2) {
			my $string= (get_value_holder() & 0xFF000000) >> 24;
			$string = sprintf("0x%X", $string);
			print " EEPMANAGER_ADDR_VERSION $string";
			
			if (((get_value_holder() & 0xFF000000) >> 24) == 0x0A) {
			} else {
				die "Can not analyse this version\n";
			}

		} elsif ($byte_add eq 0x4) {
			print " EEPROM_PERSISTENT_ID";
		} elsif ($byte_add eq 0x5) {
			print " EEPROM_PERSISTENT_ID";
		} elsif ($byte_add eq 0x6) {
			print " EEPROM_PERSISTENT_ID";
		} elsif ($byte_add eq 0x7) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			print " EEPROM_PERSISTENT_ID $string";
		} elsif ($byte_add eq 0x10) {
			my $string= (get_value_holder() & 0xFF000000) >> 24;
			$string = sprintf("0x%X", $string);
			print " eep_settings_version $string";
		} elsif ($byte_add eq 0x11) {
			my $string= (get_value_holder() & 0xFF000000) >> 24;
			$string = sprintf("0x%X", $string);
			print " CRC $string";
		} elsif ($byte_add eq 0x12) {
			my $string= (get_value_holder() & 0xFF000000) >> 24;
			$string = sprintf("0x%X", $string);
			print " eep_settings_version $string";
		} elsif ($byte_add eq 0x13) {
			my $string= (get_value_holder() & 0xFF000000) >> 24;
			$string = sprintf("0x%X", $string);
			print " CRC $string";

		} elsif ($byte_add eq 0x14) {
			print " X_STEPS_PER_MM";
		} elsif ($byte_add eq 0x15) {
			print " X_STEPS_PER_MM";
		} elsif ($byte_add eq 0x16) {
			print " X_STEPS_PER_MM";
		} elsif ($byte_add eq 0x17) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " X_STEPS_PER_MM: $float $string";

		} elsif ($byte_add eq 0x18) {
			print " Y_STEPS_PER_MM";
		} elsif ($byte_add eq 0x19) {
			print " Y_STEPS_PER_MM";
		} elsif ($byte_add eq 0x1A) {
			print " Y_STEPS_PER_MM";
		} elsif ($byte_add eq 0x1B) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " Y_STEPS_PER_MM: $float $string";

		} elsif ($byte_add eq 0x1C) {
			print " Z_STEPS_PER_MM";
		} elsif ($byte_add eq 0x1D) {
			print " Z_STEPS_PER_MM";
		} elsif ($byte_add eq 0x1E) {
			print " Z_STEPS_PER_MM";
		} elsif ($byte_add eq 0x1F) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " Z_STEPS_PER_MM: $float $string";

		} elsif ($byte_add eq 0x20) {
			print " X_MAX_RATE";
		} elsif ($byte_add eq 0x21) {
			print " X_MAX_RATE";
		} elsif ($byte_add eq 0x22) {
			print " X_MAX_RATE";
		} elsif ($byte_add eq 0x23) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " X_MAX_RATE: $float $string";

		} elsif ($byte_add eq 0x24) {
			print " Y_MAX_RATE";
		} elsif ($byte_add eq 0x25) {
			print " Y_MAX_RATE";
		} elsif ($byte_add eq 0x26) {
			print " Y_MAX_RATE";
		} elsif ($byte_add eq 0x27) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " Y_MAX_RATE: $float $string";

		} elsif ($byte_add eq 0x28) {
			print " Z_MAX_RATE";
		} elsif ($byte_add eq 0x29) {
			print " Z_MAX_RATE";
		} elsif ($byte_add eq 0x2A) {
			print " Z_MAX_RATE";
		} elsif ($byte_add eq 0x2B) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " Z_MAX_RATE: $float $string";

		} elsif ($byte_add eq 0x2C) {
			print " X_ACCELERATION";
		} elsif ($byte_add eq 0x2D) {
			print " X_ACCELERATION";
		} elsif ($byte_add eq 0x2E) {
			print " X_ACCELERATION";
		} elsif ($byte_add eq 0x2F) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			$float /= 60.0;
			$float /= 60.0;
			print " X_ACCELERATION: $float mm/sec^2";

		} elsif ($byte_add eq 0x30) {
			print " Y_ACCELERATION";
		} elsif ($byte_add eq 0x31) {
			print " Y_ACCELERATION";
		} elsif ($byte_add eq 0x32) {
			print " Y_ACCELERATION";
		} elsif ($byte_add eq 0x33) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			$float /= 60.0;
			$float /= 60.0;
			print " Y_ACCELERATION: $float mm/sec^2";

		} elsif ($byte_add eq 0x34) {
			print " Z_ACCELERATION";
		} elsif ($byte_add eq 0x35) {
			print " Z_ACCELERATION";
		} elsif ($byte_add eq 0x36) {
			print " Z_ACCELERATION";
		} elsif ($byte_add eq 0x37) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			$float /= 60.0;
			$float /= 60.0;
			print " Z_ACCELERATION: $float mm/sec^2";

		} elsif ($byte_add eq 0x38) {
			print " X_MAX_TRAVEL";
		} elsif ($byte_add eq 0x39) {
			print " X_MAX_TRAVEL";
		} elsif ($byte_add eq 0x3A) {
			print " X_MAX_TRAVEL";
		} elsif ($byte_add eq 0x3B) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " X_MAX_TRAVEL: $float mm";

		} elsif ($byte_add eq 0x3C) {
			print " Y_MAX_TRAVEL";
		} elsif ($byte_add eq 0x3D) {
			print " Y_MAX_TRAVEL";
		} elsif ($byte_add eq 0x3E) {
			print " Y_MAX_TRAVEL";
		} elsif ($byte_add eq 0x3F) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " Y_MAX_TRAVEL: $float mm";

		} elsif ($byte_add eq 0x40) {
			print " Z_MAX_TRAVEL";
		} elsif ($byte_add eq 0x41) {
			print " Z_MAX_TRAVEL";
		} elsif ($byte_add eq 0x42) {
			print " Z_MAX_TRAVEL";
		} elsif ($byte_add eq 0x43) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " Z_MAX_TRAVEL: $float mm";

		} elsif ($byte_add eq 0x44) {
			print " STEP_PULSE_MICROSECONDS";
		} elsif ($byte_add eq 0x45) {
			print " STEPPING_INVERT_MASK";
		} elsif ($byte_add eq 0x46) {
			print " DIRECTION_INVERT_MASK";
		} elsif ($byte_add eq 0x47) {
			print " STEPPER_IDLE_LOCK_TIME";

		} elsif ($byte_add eq 0x48) {
			print " STATUS_REPORT_MASK";

		} elsif ($byte_add eq 0x4C) {
			print " JUNCTION_DEVIATION";
		} elsif ($byte_add eq 0x4D) {
			print " JUNCTION_DEVIATION";
		} elsif ($byte_add eq 0x4E) {
			print " JUNCTION_DEVIATION";
		} elsif ($byte_add eq 0x4F) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " JUNCTION_DEVIATION: $float mm $string";

		} elsif ($byte_add eq 0x50) {
			print " ARC_TOLERANCE";
		} elsif ($byte_add eq 0x51) {
			print " ARC_TOLERANCE";
		} elsif ($byte_add eq 0x52) {
			print " ARC_TOLERANCE";
		} elsif ($byte_add eq 0x53) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " ARC_TOLERANCE: $float mm";

		} elsif ($byte_add eq 0x54) {
			print " SPINDLE_RPM_MAX";
		} elsif ($byte_add eq 0x55) {
			print " SPINDLE_RPM_MAX";
		} elsif ($byte_add eq 0x56) {
			print " SPINDLE_RPM_MAX";
		} elsif ($byte_add eq 0x57) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " SPINDLE_RPM_MAX: $float mm";

		} elsif ($byte_add eq 0x58) {
			print " SPINDLE_RPM_MIN";
		} elsif ($byte_add eq 0x59) {
			print " SPINDLE_RPM_MIN";
		} elsif ($byte_add eq 0x5A) {
			print " SPINDLE_RPM_MIN";
		} elsif ($byte_add eq 0x5B) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " SPINDLE_RPM_MIN: $float mm";

		} elsif ($byte_add eq 0x5C) {
			print " (INVERT_PROBE_PIN, INVERT_LIMIT_PINS, SOFT_LIMIT_ENABLE, HOMING_ENABLE, HARD_LIMIT_ENABLE, EMPTY, LASER_MODE, REPORT_INCHES)";
		} elsif ($byte_add eq 0x5D) {
			print " HOMING_DIR_MASK";

		} elsif ($byte_add eq 0x60) {
			print " HOMING_FEED_RATE";
		} elsif ($byte_add eq 0x61) {
			print " HOMING_FEED_RATE";
		} elsif ($byte_add eq 0x62) {
			print " HOMING_FEED_RATE";
		} elsif ($byte_add eq 0x63) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " HOMING_FEED_RATE: $float mm/sec";

		} elsif ($byte_add eq 0x64) {
			print " HOMING_SEEK_RATE";
		} elsif ($byte_add eq 0x65) {
			print " HOMING_SEEK_RATE";
		} elsif ($byte_add eq 0x66) {
			print " HOMING_SEEK_RATE";
		} elsif ($byte_add eq 0x67) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " HOMING_SEEK_RATE: $float mm/sec";

		} elsif ($byte_add eq 0x68) {
			print " HOMING_DEBOUNCE_DELAY";

		} elsif ($byte_add eq 0x6C) {
			print " HOMING_PULLOFF";
		} elsif ($byte_add eq 0x6D) {
			print " HOMING_PULLOFF";
		} elsif ($byte_add eq 0x6E) {
			print " HOMING_PULLOFF";
		} elsif ($byte_add eq 0x6F) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " HOMING_PULLOFF: $float mm";

		} elsif ($byte_add eq 0x70) {
			print " AUTOMATIC_TOOL_CHANGE";

		} elsif ($byte_add eq 0x71) {
			my $string= (get_value_holder() & 0xFF000000) >> 24;
			$string = sprintf("0x%X", $string);
			print " SPINDLE_ALGORITHM $string";

		} elsif ($byte_add eq 0x74) {
			print " SPINDLE_A";
		} elsif ($byte_add eq 0x75) {
			print " SPINDLE_A";
		} elsif ($byte_add eq 0x76) {
			print " SPINDLE_A";
		} elsif ($byte_add eq 0x77) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " SPINDLE_A: $float";

		} elsif ($byte_add eq 0x78) {
			print " SPINDLE_B";
		} elsif ($byte_add eq 0x79) {
			print " SPINDLE_B";
		} elsif ($byte_add eq 0x7A) {
			print " SPINDLE_B";
		} elsif ($byte_add eq 0x7B) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " SPINDLE_B: $float";

		} elsif ($byte_add eq 0x7C) {
			print " SPINDLE_C";
		} elsif ($byte_add eq 0x7D) {
			print " SPINDLE_C";
		} elsif ($byte_add eq 0x7E) {
			print " SPINDLE_C";
		} elsif ($byte_add eq 0x7F) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " SPINDLE_C: $float";

		} elsif ($byte_add eq 0x80) {
			print " SPINDLE_D";
		} elsif ($byte_add eq 0x81) {
			print " SPINDLE_D";
		} elsif ($byte_add eq 0x82) {
			print " SPINDLE_D";
		} elsif ($byte_add eq 0x83) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " SPINDLE_D: $float";


			
		} elsif ($byte_add eq 0x84) {
			print " CRC";


		} elsif ($byte_add eq 0xE5) {
			print " Backup SPINDLE_A";
		} elsif ($byte_add eq 0xE6) {
			print " Backup SPINDLE_A";
		} elsif ($byte_add eq 0xE7) {
			print " Backup SPINDLE_A";
		} elsif ($byte_add eq 0xE8) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " Backup SPINDLE_A: $float";

		} elsif ($byte_add eq 0xE9) {
			print " Backup SPINDLE_B";
		} elsif ($byte_add eq 0xEA) {
			print " Backup SPINDLE_B";
		} elsif ($byte_add eq 0xEB) {
			print " Backup SPINDLE_B";
		} elsif ($byte_add eq 0xEC) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " Backup SPINDLE_B: $float";

		} elsif ($byte_add eq 0xED) {
			print " Backup SPINDLE_C";
		} elsif ($byte_add eq 0xEE) {
			print " Backup SPINDLE_C";
		} elsif ($byte_add eq 0xEF) {
			print " Backup SPINDLE_C";
		} elsif ($byte_add eq 0xF0) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " Backup SPINDLE_C: $float";

		} elsif ($byte_add eq 0xF1) {
			print " Backup SPINDLE_D";
		} elsif ($byte_add eq 0xF2) {
			print " Backup SPINDLE_D";
		} elsif ($byte_add eq 0xF3) {
			print " Backup SPINDLE_D";
		} elsif ($byte_add eq 0xF4) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " Backup SPINDLE_D: $float";

		} elsif ($byte_add eq 0xF5) {
			print " CRC";
			
		} elsif ($byte_add eq 0xF9) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G54 eep_coord_data X: $float";
		} elsif ($byte_add eq 0xFD) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G54 eep_coord_data Y: $float";
		} elsif ($byte_add eq 0x101) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G54 eep_coord_data Z: $float";

		} elsif ($byte_add eq 0x105) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G55 eep_coord_data X: $float";
		} elsif ($byte_add eq 0x109) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G55 eep_coord_data Y: $float";
		} elsif ($byte_add eq 0x10D) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G55 eep_coord_data Z: $float";

		} elsif ($byte_add eq 0x101) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G56 eep_coord_data X: $float";
		} elsif ($byte_add eq 0x105) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G56 eep_coord_data Y: $float";
		} elsif ($byte_add eq 0x109) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G56 eep_coord_data Z: $float";

		} elsif ($byte_add eq 0x10D) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G57 eep_coord_data X: $float";
		} elsif ($byte_add eq 0x121) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G57 eep_coord_data Y: $float";
		} elsif ($byte_add eq 0x125) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G57 eep_coord_data Z: $float";

		} elsif ($byte_add eq 0x129) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G58 eep_coord_data X: $float";
		} elsif ($byte_add eq 0x12D) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G58 eep_coord_data Y: $float";
		} elsif ($byte_add eq 0x131) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G58 eep_coord_data Z: $float";

		} elsif ($byte_add eq 0x135) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G59 eep_coord_data X: $float";
		} elsif ($byte_add eq 0x139) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G59 eep_coord_data Y: $float";
		} elsif ($byte_add eq 0x13D) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G59 eep_coord_data Z: $float";

		} elsif ($byte_add eq 0x141) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G28 eep_coord_data X: $float";
		} elsif ($byte_add eq 0x145) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G28 eep_coord_data Y: $float";
		} elsif ($byte_add eq 0x149) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " G28 eep_coord_data Z: $float";

		} elsif ($byte_add eq 0x14D) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " res eep_coord_data X: $float";
		} elsif ($byte_add eq 0x151) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " res eep_coord_data Y: $float";
		} elsif ($byte_add eq 0x155) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " res eep_coord_data Z: $float";

		} elsif ($byte_add eq 0x159) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " res eep_coord_data X: $float";
		} elsif ($byte_add eq 0x15D) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " res eep_coord_data Y: $float";
		} elsif ($byte_add eq 0x161) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " res eep_coord_data Z: $float";

		} elsif ($byte_add eq 0x165) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " eep_coord_data X: $float";
		} elsif ($byte_add eq 0x169) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " eep_coord_data Y: $float";
		} elsif ($byte_add eq 0x16D) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " eep_coord_data Z: $float";
		} elsif ($byte_add eq 0x16E) {
			print " CRC";
		} elsif ($byte_add eq 0x1E7) {
			print " CRC";



		} elsif ($byte_add eq 0x1EB) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " sys_position X: $string";
		} elsif ($byte_add eq 0x1EF) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " sys_position Z: $string";
		} elsif ($byte_add eq 0x1F3) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " sys_position Z: $string";
		} elsif ($byte_add eq 0x1F4) {
			print " CRC";
		} elsif ($byte_add eq 0x201) {
			print " CRC";


		} elsif ($byte_add eq 0x205) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " coord_system_eep X: $float";
		} elsif ($byte_add eq 0x209) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " coord_system_eep Z: $float";
		} elsif ($byte_add eq 0x20D) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " coord_system_eep Z: $float";
		} elsif ($byte_add eq 0x20E) {
			print " CRC";
		} elsif ($byte_add eq 0x21B) {
			print " CRC";


		} elsif ($byte_add eq 0x21F) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " coord_offset_eep X: $float";
		} elsif ($byte_add eq 0x223) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " coord_offset_eep Z: $float";
		} elsif ($byte_add eq 0x227) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " coord_offset_eep Z: $float";
		} elsif ($byte_add eq 0x228) {
			print " CRC";
		} elsif ($byte_add eq 0x235) {
			print " CRC";

		} elsif ($byte_add eq 0x239) {
			my $string= get_value_holder();
			#$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " grbl_running_running_minutes X: $string";
		} elsif ($byte_add eq 0x23A) {
			print " CRC";
		} elsif ($byte_add eq 0x23F) {
			print " CRC";

		} elsif ($byte_add eq 0x243) {
			my $string= get_value_holder();
			#$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " grbl_running_running_hours X: $string";
		} elsif ($byte_add eq 0x244) {
			print " CRC";
		} elsif ($byte_add eq 0x249) {
			print " CRC";

		} elsif ($byte_add eq 0x24D) {
			my $string= get_value_holder();
			#$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " grbl_running_startup_cnt X: $string";
		} elsif ($byte_add eq 0x24E) {
			print " CRC";
		} elsif ($byte_add eq 0x253) {
			print " CRC";

		} elsif ($byte_add eq 0x257) {
			my $string= get_value_holder();
			#$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " grbl_running_spindle_running_minutes X: $string";
		} elsif ($byte_add eq 0x258) {
			print " CRC";
		} elsif ($byte_add eq 0x25D) {
			print " CRC";

		} elsif ($byte_add eq 0x25E) {
			print " MAC";
		} elsif ($byte_add eq 0x25F) {
			print " MAC";
		} elsif ($byte_add eq 0x260) {
			print " MAC";
		} elsif ($byte_add eq 0x261) {
			print " MAC";
		} elsif ($byte_add eq 0x262) {
			print " MAC";
		} elsif ($byte_add eq 0x263) {
			print " MAC";
		} elsif ($byte_add eq 0x264) {
			print " CRC";
		} elsif ($byte_add eq 0x26B) {
			print " CRC";

		} elsif ($byte_add eq 0x26C) {
			print " grbl_Tool_Length_1";
		} elsif ($byte_add eq 0x26D) {
			print " grbl_Tool_Length_1";
		} elsif ($byte_add eq 0x26E) {
			print " grbl_Tool_Length_1";
		} elsif ($byte_add eq 0x26F) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " grbl_Tool_Length_1: $float mm";
		} elsif ($byte_add eq 0x270) {
			print " CRC";
		} elsif ($byte_add eq 0x275) {
			print " CRC";


		} elsif ($byte_add eq 0x276) {
			print " grbl_Tool_Length_2";
		} elsif ($byte_add eq 0x277) {
			print " grbl_Tool_Length_2";
		} elsif ($byte_add eq 0x278) {
			print " grbl_Tool_Length_2";
		} elsif ($byte_add eq 0x279) {
			my $string= get_value_holder();
			$string = sprintf("0x%X", $string);
			my $float= unpack "f", pack "L", hex $string;
			print " grbl_Tool_Length_2: $float mm";
		} elsif ($byte_add eq 0x27A) {
			print " CRC";
		} elsif ($byte_add eq 0x27F) {
			print " CRC";
			

		} elsif ($byte_add eq 0x280) {
			print " eep_coord_teached";
		} elsif ($byte_add eq 0x281) {
			print " eep_coord_teached";
		} elsif ($byte_add eq 0x282) {
			print " eep_coord_teached";
		} elsif ($byte_add eq 0x283) {
			print " eep_coord_teached";
		} elsif ($byte_add eq 0x284) {
			print " eep_coord_teached";
		} elsif ($byte_add eq 0x285) {
			print " eep_coord_teached";
		} elsif ($byte_add eq 0x286) {
			print " eep_coord_teached";
		} elsif ($byte_add eq 0x287) {
			print " eep_coord_teached";
		} elsif ($byte_add eq 0x288) {
			print " eep_coord_teached";
		} elsif ($byte_add eq 0x289) {
			print " eep_coord_teached";
		} elsif ($byte_add eq 0x28A) {
			print " CRC";
		} elsif ($byte_add eq 0x295) {
			print " CRC";

		}
		print "\n";
	}
}

my $value_holder = 0;

sub add_value_holder {
	my ($p1) = @_;
	$value_holder >>= 8;
	$value_holder &= 0x00FFFFFF;
	$p1 <<= 24;
	#$value_holder <<= 8;
	$value_holder += $p1;
	$value_holder &= 0xFFFFFFFF;
}

sub get_value_holder {
	return $value_holder;
}

sub reset_value_holder {
	$value_holder = 0;
}