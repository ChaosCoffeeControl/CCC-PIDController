#!/usr/bin/perl -w
use Device::SerialPort; 
Device::SerialPort->new($ARGV[0])->pulse_dtr_on(300);

