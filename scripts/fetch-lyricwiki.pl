#!/usr/bin/perl -w
#
# Perl script to fetch lyrics using the LyricWiki SOAP web service.
# Based on David Precious's Lyrics::Fetcher::LyricWiki module.
#
use strict;
use SOAP::Lite;

my ($artist, $title) = @ARGV;
die "usage: fetch-lyricwiki.pl artist title" unless $title;

my $soap = new SOAP::Lite->service('http://lyricwiki.org/server.php?wsdl');
my $result = $soap->getSong($artist, $title);

print $result->{artist} . " - " . $result->{song} . "\n\n";
print $result->{lyrics};
