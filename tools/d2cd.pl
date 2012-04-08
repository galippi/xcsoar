#!/usr/bin/perl
use strict;
use warnings;

#main
my $filename = shift;
my $filename_out = shift;
local *FIN = *STDIN;
local *FOUT = *STDOUT;
if (defined($filename))
{
  open(FIN, $filename) or die "Unable to open file $filename!\n";
  if (!defined($filename_out))
  {
    $filename_out = $filename . ".dc";
  }
  open(FOUT, ">".$filename_out) or die "Unable to create file $filename_out!\n";
}
while(<FIN>)
{
  chomp;
  my @items = split(" ", $_);
  my $line = "";
  foreach my $item (@items)
  {
    if ($item =~ /^([a-zA-Z])\:(.+)$/)
    {
      $item = "/cygdrive/$1$2";
    }
    if ($item ne "\\")
    {
      $item =~ s/\\/\//g;
    }
    if ($line ne "")
    {
      $line = $line . " ";
    }
    $line = $line . $item;
  }
  print FOUT $line . "\n";
}
if (defined($filename))
{
  close(FOUT);
  close(FIN);
}
