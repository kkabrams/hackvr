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
my $color=2;
open(FILE,$ARGV[1]) if $ARGV[1];
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

#foreach $tmp (@vertices) {
# foreach(@$tmp) {
# print $tmp . "\n";
# }
#}

foreach $tmp (@faces) {
 @points=split(/ /,$tmp);
 @points = map { $_ =~ s/\/.+$//g; $_; } @points;
## print @points;
 print $ARGV[0];
 print " addshape " . (rand()*8%8) . " ";
 print @points+0;
 for($i=0;$i<(@points);$i++) {
  print " ";
  print $vertices[$points[$i]-1];
 }
 print "\n";
}
