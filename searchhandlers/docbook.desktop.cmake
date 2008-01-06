[Desktop Entry]
DocumentTypes=application/docbook+xml

SearchCommand=${LIBEXEC_INSTALL_DIR}/khc_htsearch.pl --docbook --indexdir=%d --config=%i --words=%w --method=%o --maxnum=%m --lang=en
IndexCommand=${LIBEXEC_INSTALL_DIR}/khc_docbookdig.pl --indexdir=%d --docpath=%p --identifier=%i
