[Desktop Entry]
DocumentTypes=text/html

SearchBinary=htsearch
SearchBinaryPaths=/srv/www/cgi-bin,/usr/lib/cgi-bin,/opt/www/htdig/bin
SearchCommand=${LIBEXEC_INSTALL_DIR}/khc_htsearch.pl --binary=%b --indexdir=%d --config=%i --words=%w --method=%o --maxnum=%m --lang=%l
IndexCommand=${LIBEXEC_INSTALL_DIR}/khc_htdig.pl --indexdir=%d --docpath=%p --identifier=%i --lang=%l
TryExec=/usr/bin/htdig
