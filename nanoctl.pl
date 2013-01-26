#!/usr/bin/perl

use Device::SerialPort;

if ($#ARGV == 0) 
{

	my $port = Device::SerialPort->new("/dev/nano");
	$port->databits(8);
	$port->baudrate(9600);
	$port->parity("none");
	$port->stopbits(1);
	$port->write("{".$ARGV[0]."}");
	undef $port;
}
else
{
	print("argument needs to be there");
}

