#!/usr/bin/perl

use strict;

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
while(<>) {
 chomp;
 @linepart=split(/ /,$_,2);
 $com=$linepart[0];
 if($com eq "v") {
  push(@vertices,$linepart[1]);
 }
 if($com eq "f") {
  push(@faces,$linepart[1]);
 }
}
#foreach $tmp (@vertices) {
# foreach(@$tmp) {
# print $tmp . "\n";
# }
#}

foreach $tmp (@faces) {
 @points=split(/ /,$tmp);
 @points = map { $_ =~ s/\/.+$//g; $_; } @points;
# print @points;
 for($i=2;$i<(@points);$i++) {
  print "deer addtriangle ";
  print $vertices[$points[0]-1];
  print " ";
  print $vertices[$points[$i-1]-1];
  print " ";
  print $vertices[$points[$i]-1];
  print "\n";
 }
}
