#!/usr/bin/env perl

# Copyright (C) 2016 Pino Toscano <pino@kde.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

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
  die "'$entry' referenced by not defined"
    unless defined($defined_entries{$entry});
}
