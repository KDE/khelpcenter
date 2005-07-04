#!/usr/bin/perl
#
#  Script for searching man pages. The result is generated as HTML.
#
#  This file is part of KHelpcenter.
#
#  Copyright (C) 2002  SuSE Linux AG, Nuernberg
#
#  Author: Cornelius Schumacher <cschum@suse.de>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301  USA

use strict;

use Getopt::Long;

my ( $words, $maxcount, $lang, $help );

GetOptions (
  'maxcount=s' => \$maxcount,
  'words=s' => \$words,
  'lang=s' => \$lang,
  'help' => \$help
);

if ( $help ) {
  print STDERR "Usage: khc_mansearch.pl --maxcount=n --words=<string> " .
    "--lang=<languagecode>\n";
  exit 1;
}

if ( !$words ) {
  print STDERR "No search words given.\n";
  exit;
}

# Perform search
if ( !open( MAN, "apropos $words|" ) ) {
  print "Can't open apropos.\n";
  exit 1;
}
my @results;
while( <MAN> ) {
#  print "RAW:$_";
  chop;
  /^([^\s]+)\s+\((.*)\)\s+-\s+(.*)$/;
  my $page = $1;
  my $section = $2;
  my $description = $3;

  if ( $page ) { push @results, [ $page, $section, $description ]; }
}
close MAN;

my $nummatches = @results;

if ( $nummatches > 0 ) {
  print "<ul>\n";

  my $count = 0;
  for my $result ( @results ) {
    my ( $page, $section, $description ) = @$result;
    my $url = "man:" . $page;
    print "<li><a href=\"$url\">";
    print "$page - $description</a></li>\n";
    if ( ++$count == $maxcount ) { last; }
  }

  print "</ul>\n";
}

1;
