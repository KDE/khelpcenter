[Desktop Entry]
DocumentTypes=application/docbook+xml

SearchBinary=htsearch
SearchBinaryPaths=/srv/www/cgi-bin,/usr/lib/cgi-bin,/opt/www/htdig/bin
SearchCommand=${LIBEXEC_INSTALL_DIR}/khc_htsearch.pl --binary=%b --docbook --indexdir=%d --config=%i --words=%w --method=%o --maxnum=%m --lang=en
IndexCommand=${LIBEXEC_INSTALL_DIR}/khc_docbookdig.pl --indexdir=%d --docpath=%p --identifier=%i
TryExec=/usr/bin/htdig
