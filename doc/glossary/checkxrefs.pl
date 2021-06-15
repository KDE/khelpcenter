#!/usr/bin/env perl

# SPDX-FileCopyrightText: 2016 Pino Toscano <pino@kde.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

use warnings;
use strict;

use File::Find;

# Collect the *.docbook files in the current directory.
my @docbook_files = ();
sub collect {
  push @docbook_files, $File::Find::name if $_ =~ /\.docbook$/;
}
find(\&collect, '.');

# Collect the list of entries, defined and referenced.
my %defined_entries;
my %referenced_entries;

for my $file (@docbook_files) {
  open(DOCBOOK, "<", $file) || die "$0: can't open $file: $!";
  while(<DOCBOOK>) {
    if (/<glossentry id="(.*)">/) {
      $defined_entries{$1} = 1;
    } elsif (/<glossseealso otherterm="(.*)">/) {
      $referenced_entries{$1} = 1;
    }
  }
  close DOCBOOK;
}

# Check for entries which are referenced but not defined.
foreach my $entry (keys %referenced_entries) {
  die "'$entry' referenced but not defined"
    unless defined($defined_entries{$entry});
}
