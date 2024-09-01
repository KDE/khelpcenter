#!/usr/bin/perl
#
#  Script for searching man pages. The result is generated as HTML.
#
#  This file is part of KHelpcenter.
#
#  SPDX-FileCopyrightText: 2002 SuSE Linux AG Nuernberg
#  SPDX-FileContributor: Cornelius Schumacher <cschum@suse.de>
#
#  SPDX-License-Identifier: GPL-2.0-or-later

use strict;

use Getopt::Long;

my ( $words, $maxcount, $lang, $method, $help );

GetOptions (
  'maxcount=i' => \$maxcount,
  'words=s' => \$words,
  'lang=s' => \$lang,
  'method=s' => \$method,
  'help' => \$help
);

if ( $help ) {
  print STDERR "Usage: khc_mansearch.pl --maxcount=n --words=<string> " .
    "--lang=<languagecode> --method=and|or\n";
  exit 1;
}

if ( $method ne 'and' and $method ne 'or' ) {
  print STDERR "Unrecognized method: $method.\n";
  exit 1;
}

if ( !$words ) {
  print STDERR "No search words given.\n";
  exit;
}

if ( !$lang or $lang eq 'C' ) {
  $lang = 'en';
}

if ( $words =~ /[^+]$/ ) {
  # If the query was a single word, perhaps it is the literal name of the command (or a prefix)
  if ( !open( PREFIX, "-|", ( 'whatis', '-w', $words . '*' ) ) ) {
    print "Can't open whatis.\n";
    exit 1;
  }

  # ... or a substring
  if ( !open( NAME, "-|", ( 'whatis', '-w', '*' . $words . '*' ) ) ) {
    print "Can't open whatis.\n";
    exit 1;
  }
}

# Build the apropos command line
my @apropos;
push @apropos, 'apropos';
push @apropos, '-L', $lang;
if ( $method eq 'and' ) {
  push @apropos, '--and';
}
push @apropos, split( '\+', $words );

# Perform search
if ( !open( DESCRIPTION, "-|", @apropos ) ) {
  print "Can't open apropos.\n";
  exit 1;
}
my @results;
my %seen;
while( $_ = (<PREFIX> // <NAME> // <DESCRIPTION>) ) {
#  print "RAW:$_";
  chop;
  /^([^\s]+)\s+\((.*)\)\s+-\s+(.*)$/;
  my $page = $1;
  my $section = $2;
  my $description = $3;

  if ( $page && ! $seen{ $page . $section }++ ) { push @results, [ $page, $section, $description ]; }
}
close PREFIX;
close NAME;
close DESCRIPTION;

my $nummatches = @results;

if ( $nummatches > 0 ) {
  print "<ul>\n";

  my $count = 0;
  for my $result ( @results ) {
    my ( $page, $section, $description ) = @$result;
    my $url = "man:" . $page . "(" . $section . ")";
    print "<li><a href=\"$url\">";
    print "$page($section) - $description</a></li>\n";
    if ( ++$count == $maxcount ) { last; }
  }

  print "</ul>\n";
}

1;
