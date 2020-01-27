#!/usr/bin/perl

use strict;

die "usage: obj2hackvr.pl name file\n" unless $ARGV[0];

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
open(FILE,$ARGV[1]) if $ARGV[1];
while(<FILE>) {
 $_ =~ s/[\n\r]//g;
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

#foreach $tmp (@faces) {
# @points=split(/ /,$tmp);
# @points = map { $_ =~ s/\/.+$//g; $_; } @points;
## print @points;
# print $ARGV[0];
# print " addshape ";
# print @points+0;
# for($i=0;$i<(@points+0);$i++) {
#  print " ";
#  print $vertices[$points[$i]];
# }
# print "\n";
#}

#convert to triangles
foreach $tmp (@faces) {
 @points=split(/ /,$tmp);
## this map is to split off the vertex texture and vertex normal parts of the face.
 @points = map { $_ =~ s/\/.+$//g; $_; } @points;
# print @points;
 for($i=2;$i<(@points);$i++) {
  print $ARGV[0];
  print " addshape 2 3 ";
  print $vertices[$points[0]-1];
  print " ";
  print $vertices[$points[$i-1]-1];
  print " ";
  print $vertices[$points[$i]-1];
  print "\n";
 }
}
