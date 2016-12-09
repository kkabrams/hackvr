#!/usr/bin/perl

use strict;

die "usage: dat2hackvr.pl name file\n" unless $ARGV[0];

my $line;
my $prev;
my $first;
open(FILE,$ARGV[1]) if $ARGV[1];
while($line=<FILE>) {
 chomp $line;
 if(!($line =~ m/^#/)) {
  if($line eq "") {
#   print $ARGV[0];
#   print " addtriangle ";
#   print $prev . " 0 " . $first . " 0 " . $first . " 0";
#   print "\n";
   $first="";
  } else {
   if($prev ne "") {
    print $ARGV[0];
    print " addshape 2 ";
    print $line . " 0 " . $prev . " 0";
    print "\n";
   }
   $prev="";
  }
  if($first eq "") {
   $first=$line;
  }
  $prev=$line;
 }
}
