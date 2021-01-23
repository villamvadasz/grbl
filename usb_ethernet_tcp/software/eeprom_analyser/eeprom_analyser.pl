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
			#print "\r\n";
			#print "Data ";
			#print $2;
			#print "\r\n";
			$address = hex($1);
			$data = hex($2);
			#print "Address $address Data $data\r\n";
			$eeprom_memory_journaling[$eeprom_memory_journaling_element] = [$address,$data];
			#print "$eeprom_memory_journaling[$eeprom_memory_journaling_element][0] $eeprom_memory_journaling[$eeprom_memory_journaling_element][1]\r\n";
			$eeprom_memory_journaling_element ++;
		}	
		#print $line;
	}
	
	if ($line =~ "eeprom dump:") {
		if ($foundDumptStart != 0) {
			die "Wrong dump, more than one eeprom dump string was found in the file\r\n";
		} else {
			$foundDumptStart = 1;
		}
	}	
	if ($line =~ "eeprom dump finished!")	{
		if ($foundDumptStart != 1) {
			die "Wrong dump, eeprom dump finished string found before start\r\n";
		} else {
			$foundDumptEnd = 1;
		}
	}	
}

if (($foundDumptEnd != 1) && ($foundDumptStart != 1)) {
	die "Unable to find eeprom dump start and/or end\r\n";
}

my $i;
my $elements;
for ($i  = 0; $i < scalar( @eeprom_memory_journaling ); $i++ )
{
	#print "$eeprom_memory_journaling[$i][0] $eeprom_memory_journaling[$i][1]\r\n";
}
$elements = $i;
$elementsstring = sprintf("0x%X", $elements);
print "Elements found: $elementsstring \r\n";
print "Elements expected: 0x2400\r\n";

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
print "Base Address: $baseaddressstring Size: $sizestring\r\n";
$baseaddressstring_map = sprintf("0x%X", ($baseaddress & 0xDFFFFFFF));
print "Base Address: $baseaddressstring_map Size: $sizestring <<= Look in Map File\r\n";


my $DEE_EEPROM_SIZE = 4096;
my $DEE_PAGE_CNT = 3;
my $DEE_PAGE_PAGE_SIZE = 3;
my $EEPROM_FLASH_ADDR = hex("0x9D030000");

if ($EEPROM_FLASH_ADDR != ($baseaddress & 0xDFFFFFFF)) {
	die "Configuration mismatch\r\n";
}

#typedef struct _Dee_Type 
#	Dee_Page_Status status;
#	uint32 data[DEE_NUMBER_OF_DATA_ELEMENTS];
#	uint32 crc[DEE_NUMBER_OF_DATA_ELEMENTS];
#	uint32 address[DEE_NUMBER_OF_DATA_ELEMENTS];
# Dee_Type;
#

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
	print "Page $i status: $statusstring Address: $statusaddressstring\r\n";
	if ($status & 0x01) {
		print "\t page_expired: not expired\r\n";
	} else {
		print "\t page_expired: expired\r\n";
		$statusProblemPresent |= 0x1;
	}
	if ($status & 0x02) {
		print "\t page_current: current\r\n";
	} else {
		print "\t page_current: not current\r\n";
	}
	if ($status & 0x04) {
		print "\t page_available: available\r\n";
	} else {
		print "\t page_available: not available\r\n";
		$statusProblemPresent |= 0x2;
	}
	if ($status & 0x08) {
		print "\t page_active: not active\r\n";
	} else {
		print "\t page_active: active\r\n";
	}
	$erasecount = ($status & 0xFFFFFFF0) >> 4;
	$erasecountstring =  sprintf("0x%X", $erasecount);
	print "\t page_erase_count: $erasecount $erasecountstring\r\n";
	
	if (($status & 0x02) && (!($status & 0x08))) {
		$current_active_page = $i;
		$current_active_count ++;
	}
	
	if ($current_active_count > 1) {
		$statusProblemPresent |= 0x4;
	}
	
	if ($statusProblemPresent != 0) {
		print "Some kind of a problem is present in the status!\r\n";
		if ($statusProblemPresent & 0x1) {
			print "\t page_expired was set to expired, but it should never reach this case!\r\n";
		}
		if ($statusProblemPresent & 0x2) {
			print "\t page_available was set to not available, but it must be always available!\r\n";
		}
		if ($statusProblemPresent & 0x4) {
			print "\t More than one page ic current and active!\r\n";
		}
	}
}

my $NUMBER_OF_BYTES_IN_PAGE = 4096;
my $NUMBER_OF_INSTRUCTIONS_IN_PAGE = ($NUMBER_OF_BYTES_IN_PAGE / 4); #// number of 32-bit word instructions per page, 4096 bytes
my $DEE_NUMBER_OF_DATA_ELEMENTS =  int((($NUMBER_OF_INSTRUCTIONS_IN_PAGE * $DEE_PAGE_PAGE_SIZE) - 1) / 3);

print "Data elements in one page: $DEE_NUMBER_OF_DATA_ELEMENTS\r\n";

for ($x  = 0; $x < $DEE_PAGE_CNT; $x++ ) {
	$pagebaseaddress = $baseaddress + ($x * $DEE_EEPROM_SIZE * $DEE_PAGE_PAGE_SIZE);
	$pagebaseaddressstring = sprintf("0x%X", $pagebaseaddress);
	print "Processing page $x $pagebaseaddressstring\r\n";
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
		#print "$data_adrstring\r\n";
		#print "$crc_adrstring\r\n";
		#print "$address_adrstring\r\n";
		
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

		#print "$addressstring: $datastring $crcstring\r\n";
		if (($address == 0xFFFFFFFF) && ($crc == 0xFFFFFFFF) && ($data == 0xFFFFFFFF)) {
		} else {
			if (($data & 0xFFFFFFFF) == ((~$crc) & 0xFFFFFFFF)) {
				if ($address >= 0x100) {
					print "$addressstring: $datastring $crcstring\r\n";
					die "Invalid address\r\n";
				}
				$eeprom_memory_linear[$address] = $data;
			} else {
				print "$addressstring: $datastring $crcstring\r\n";
				die "CRC mismatch\r\n";
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
	
	#$byte3 = (($data >> 0) & 0xFF);
	#$byte2 = (($data >> 8) & 0xFF);
	#$byte1 = (($data >> 16) & 0xFF);
	#$byte0 = (($data >> 24) & 0xFF);

	#my $word = ($byte0 << 24) + ($byte1 << 16) + ($byte2 << 8) + $byte3;
	#my $sign = ($word & 0x80000000) ? -1 : 1;
	#my $expo = (($word & 0x7F800000) >> 23) - 127;
	#my $mant = ($word & 0x007FFFFF | 0x00800000);
	#my $num = $sign * (2 ** $expo) * ( $mant / (1 << 23));
	#print "$num ";
	
	#$mych = chr(($data >> 0) & 0xFF);
	#print "$mych ";
	#$mych = chr(($data >> 8) & 0xFF);
	#print "$mych ";
	#$mych = chr(($data >> 16) & 0xFF);
	#print "$mych ";
	#$mych = chr(($data >> 24) & 0xFF);
	#print "$mych ";

	print "$dstring: $datastring\r\n";

}
