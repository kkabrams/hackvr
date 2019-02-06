#!/usr/bin/perl

use strict;

die "usage: obj2dat.pl file\n" unless $ARGV[0];

my $i;
my $j;
my $com;
my @vertices;
my @faces;
my @linepart;
my @parts;
my @points;
my @tmp;
my $tmp;
my $color=2;
open(FILE,$ARGV[0]) if $ARGV[0];
while(<FILE>) {
 chomp;
 $_ =~ s/[\r\n]//g;
 @linepart=split(/ /,$_,2);
 $com=$linepart[0];
 if($com eq "v") {
  push(@vertices,$linepart[1]);
 }
 if($com eq "f") {
  push(@faces,$linepart[1]);
 }
}

foreach $tmp (@faces) {
 @points=split(/ /,$tmp);
 @points = map { $_ =~ s/\/.+$//g; $_; } @points;
 for($i=0;$i<(@points);$i++) {
  print $vertices[$points[$i]-1];
  print "\n";
 }
 print "\n";
 print "\n";
}
