#!/usr/bin/perl
#
#  Wrapper script for creating search indices for htdig.
#
#  This file is part of the SuSE help system.
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

my $htdigdata = "/srv/www/htdig/common/";
my $htdigbin = "/usr/bin";
my $kdeprefix = `kde-config --prefix`;
chomp $kdeprefix;

my $dbg = 1;

my ($indexdir, $docpath, $identifier, $lang, $help );

GetOptions (
  'indexdir=s' => \$indexdir,
  'docpath=s' => \$docpath,
  'identifier=s' => \$identifier,
  'lang=s' => \$lang,
  'help' => \$help,
);

if ( $help ) {
  usage();
}

if ( !$indexdir || !$docpath || !$identifier ) {
  print STDERR "Missing arguments.\n";
  usage();
}

if ( !$lang ) { $lang = "en"; }

&dbg( "INDEXDIR: $indexdir" );

print "Creating index for <b>'$identifier'</b>\n";

my $htdigconf = $indexdir;
my $htdigdb = $indexdir;

my $conffile = "$htdigconf/$identifier.conf";

if ( !open( CONF, ">$conffile" ) ) {
  print STDERR "Unable to open '$conffile' for writing.\n";
  exit 1;
}

my $commondir = "$htdigdata/$lang";
if ( !$lang || !-e $commondir ) {
  $commondir = "$htdigdata/en";
}
if ( !-e $commondir ) { $commondir = $htdigdata; }

my $locale;
if ( $lang eq "de" ) { $locale = "de_DE"; }
else { $locale = $lang; }

print CONF << "EOT";
# htdig configuration for doc '$identifier'
#
# This file has been automatically created by KHelpcenter

common_dir:		$commondir
locale:                 $locale
database_dir:           $htdigdb
local_urls:		http://localhost=
local_urls_only:	true
limit_urls_to:          http://localhost
ignore_noindex:		true
max_hop_count:		4
robotstxt_name:         kdedig
compression_level:	6
template_map:           Long long $kdeprefix/share/apps/khelpcenter/searchhandlers/htdig/htdig_long.html \\
                        Short short $htdigdata/short.html
search_algorithm:       exact:1 prefix:0.8
maximum_pages:          1
matches_per_page:       10
database_base:		\${database_dir}/$identifier
start_url:		http://localhost/$docpath
# for pdf-files
max_doc_size:           5000000
external_parsers: application/pdf /usr/share/doc/packages/htdig/contrib/parse_doc.pl      application/postscript /usr/share/doc/packages/htdig/contrib/parse_doc.pl
#external_parsers: text/docbook /build/htdig/parser
EOT

close CONF;

$ENV{ PATH } = '';

my $ret = system( "$htdigbin/htdig -s -i -c $conffile" );
if ( $ret != 0 ) {
  print STDERR "htdig failed\n";
} else {
  $ret = system( "$htdigbin/htmerge -c $conffile" );
  if ( $ret != 0 ) { print STDERR "htmerge failed\n"; }
}

if ( $ret == 0 ) {
  my $existsfile = "$indexdir/$identifier.exists";

  if ( !open( EXISTS, ">$existsfile" ) ) {
    print STDERR "Unable to open '$existsfile' for writing.\n";
    exit 1;
  }
  print EXISTS "$identifier\n";
  close EXISTS;

  print "Finished successfully.\n";
}

exit $ret;

sub dbg($)
{
  $dbg && print STDERR shift, "\n";
}

sub usage()
{
  print "Usage: khc_htdig.pl --indexdir <indexdir> --docpath <path> ";
  print "--identifier <identifier>\n";
  exit 1;
}
