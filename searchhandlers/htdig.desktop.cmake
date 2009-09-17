[Desktop Entry]
DocumentTypes=text/html

SearchCommand=${LIBEXEC_INSTALL_DIR}/khc_htsearch.pl --indexdir=%d --config=%i --words=%w --method=%o --maxnum=%m --lang=%l
IndexCommand=${LIBEXEC_INSTALL_DIR}/khc_htdig.pl --indexdir=%d --docpath=%p --identifier=%i --lang=%l
TryExec=/usr/bin/htdig
