#!/usr/bin/perl

use strict;

use Encode;
use Getopt::Long;

use open IO => ':utf8';
use open ':std';

my $htsearchpath="/srv/www/cgi-bin/htsearch";
my $config;
my $format;
my $method;
my $words;
my $lang;
my $docbook;
my $indexdir;
my $maxnum;

GetOptions (
  'binary=s' => \$htsearchpath,
  'config=s' => \$config,
  'format=s' => \$format,
  'method=s' => \$method,
  'words=s' => \$words,
  'lang=s' => \$lang,
  'docbook' => \$docbook,
  'indexdir=s' => \$indexdir,
  'maxnum=s' => \$maxnum
);

if ( !$indexdir ) {
  print STDERR "No index dir given.\n";
  exit 1;
}

if ( !$lang ) { $lang = "en"; }

my $charset = langCharset( $lang );

$words = encode( $charset, $words );

if ( !open( HTSEARCH, "-|", "$htsearchpath", "-c", "$indexdir/$config.conf",
            "format=$format&method=$method&words=$words" ) )
{
  print "Can't execute htsearch at '$htsearchpath'.\n";
  exit 1;
}

my ($body,$liststart,$ref,$link,$error,$errorOut);

while( <HTSEARCH> ) {
  if ( !$body ) {
    print;
    if ( /^<body/ ) { $body = 1; }
  }
  if ( /^<h3>/ ) {
    print;
    print "<ul>\n";
    $liststart = 1;
  }
  if ( /^<img src.*<a href="(.*)">(.*)<\/a>/ ) {
    $ref = $1;
    $link = $2;

    print STDERR "REF: $ref  LINK: $link\n";

    $ref =~ s/file:\/\/localhost//;
    
    $ref =~ s/http:\/\/localhost\//file:\//;

    if ( $docbook ) {
      $ref =~ /help:\/\/(.*)\/index.docbook/;
      my $app = $1;
      $ref = "help:$app";

      $link =~ s/apptitle/$app/;
    }
    
    print "  <li><a href=\"$ref\">$link</a></li>\n";
  }
  if ( /^<h1>ht:\/\/Dig error/ ) {
    $error = 1;
    print "Htdig error:\n";
  }
  if ( $error && /^<pre>/ ) {
    $errorOut = 1;
  }
  if ( $errorOut ) {
    print;
    if ( /^<\/pre>/ ) { $errorOut = 0; }
  }
}

close HTSEARCH;

if ( $liststart ) { print "</ul>\n"; }

print "</body></html>\n";

if ( $? != 0 ) { exit $?; }

1;

# Return charset used for given language
sub langCharset( $ )
{
  my $lang = shift;
  if ( $lang eq "cz" || $lang eq "hu" ) {
    return "latin2";
  } elsif ( $lang eq "kr" ) {
    return "utf8";
  } else {
    return "latin1";
  }
}
