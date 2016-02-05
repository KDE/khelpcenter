[Desktop Entry]
DocumentTypes=application/docbook+xml

SearchBinary=htsearch
SearchBinaryPaths=/srv/www/cgi-bin,/usr/lib/cgi-bin,/opt/www/htdig/bin
SearchCommand=${KDE_INSTALL_FULL_LIBEXECDIR}/khc_htsearch.pl --binary=%b --docbook --indexdir=%d --config=%i --words=%w --method=%o --maxnum=%m --lang=en
IndexCommand=${KDE_INSTALL_FULL_LIBEXECDIR}/khc_docbookdig.pl --indexdir=%d --docpath=%p --identifier=%i
TryExec=/usr/bin/htdig
