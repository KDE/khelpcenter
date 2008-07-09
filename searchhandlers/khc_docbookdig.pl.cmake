#!/usr/bin/perl
#
#  Wrapper script for creating search indices for htdig.
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
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

use strict;

use Getopt::Long;

my $htdigdata = "/srv/www/htdig/common/";
my $htdigbin = "/usr/bin";
my $kdeprefix = "@CMAKE_INSTALL_PREFIX@";
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

&dbg( "INDEXDIR: $indexdir" );

if ( !$lang ) { $lang = "en"; }

my $tmpdir = "$indexdir/$identifier.tmp";
if ( ! -e $tmpdir ) {
  mkdir $tmpdir;
}

print "Creating index for <b>'$identifier'</b>\n";

my $htdigconf = $indexdir;
my $htdigdb = $indexdir;

my $conffile = "$htdigconf/$identifier.conf";

my $commondir = "$htdigdata/$lang";
if ( !$lang || !-e $commondir ) {
  $commondir = "$htdigdata/en";
}
if ( !-e $commondir ) { $commondir = $htdigdata; }

my $locale;
if ( $lang eq "de" ) { $locale = "de_DE"; }
else { $locale = $lang; }

my $startfile = "$tmpdir/index.html";

if ( !open( START, ">$startfile" ) ) {
  print STDERR "Unable to open '$startfile' for writing.\n";
  exit 1;
}

$ENV{ PATH } = '/bin:/usr/bin';
$ENV{ CDPATH } = '';
$ENV{ ENV } = '';

my $findpath = "@HTML_INSTALL_DIR@/$lang/";
my $findcmd = "find $findpath -name index.docbook";

print STDERR "FINDCMD: $findcmd\n";

if ( !open FIND, "$findcmd|" ) {
  print STDERR "Unable to find docs.\n";
  exit 1;
}
while ( <FIND> ) {
  chomp;
  my $path = $_;
  $path =~ /$findpath(.*)\/index.docbook$/;
  my $app = $1;
  print START "<a href=\"help://$app/index.docbook\">$path</a>\n";
}
close START;

my $mimetypefile = "$tmpdir/htdig_mime";
if ( !open( MIME, ">$mimetypefile" ) ) {
  print STDERR "Unable to open '$mimetypefile' for writing.\n";
  exit 1;
}
print MIME << "EOT";
text/html       html
text/docbook    docbook
EOT
close MIME;

my $parserfile = "$tmpdir/docbookparser";
if ( !open( PARSER, ">$parserfile" ) ) {
  print STDERR "Unable to open '$parserfile' for writing.\n";
  exit 1;
}
print PARSER << "EOT";
#! /bin/sh

file=\$1
shift
mime=\$1
shift

if test "\$#" -gt 0; then
  orig=\${1/file:\\//}
  shift
fi

case "\$orig" in
  help:/*)
	orig=\${orig/help:\\//}
	orig=\${orig/\/index.docbook/}
	cd @HTML_INSTALL_DIR@/en/\$orig
	file=index.docbook
	;;
  *)	
	file=\$orig
	cd `dirname \$orig`
	;;
esac

echo "t	apptitle"
$kdeprefix/bin/meinproc --htdig "\$file"
EOT
close PARSER;
chmod 0755, $parserfile;

if ( !open( CONF, ">$conffile" ) ) {
  print STDERR "Unable to open '$conffile' for writing.\n";
  exit 1;
}
print CONF << "EOT";
# htdig configuration for doc '$identifier'
#
# This file has been automatically created by KHelpcenter
common_dir:		$commondir
locale:                 $locale
database_dir:           $htdigdb
database_base:		\${database_dir}/$identifier
local_urls:             help://=@HTML_INSTALL_DIR@/en/ file://=/
local_urls_only:        true
limit_urls_to:          file:// help:/
ignore_noindex:         true
max_hop_count:          4
robotstxt_name:         kdedig
compression_level:      6
template_map:           Long long $kdeprefix/share/apps/khelpcenter/searchhandlers/htdig/htdig_long.html
search_algorithm:       exact:1 prefix:0.8
maximum_pages:          1
matches_per_page:       10
start_url:              file://$tmpdir/index.html
external_parsers:       text/docbook $parserfile
valid_extensions:       .docbook .html
mime_types:             $mimetypefile
EOT
close CONF;

my $ret = system( "$htdigbin/htdig", "-v", "-s", "-i", "-c", $conffile );
if ( $ret != 0 ) {
  print STDERR "htdig failed\n";
} else {
  $ret = system( "$htdigbin/htmerge", "-c", $conffile );
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
  print "Usage: khc_docbookdig.pl --indexdir <indexdir> --docpath <path> ";
  print "--identifier <identifier>\n";
  exit 1;
}
