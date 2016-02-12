[Desktop Entry]
DocumentTypes=application/docbook+xml

SearchCommand=${KDE_INSTALL_FULL_LIBEXECDIR}/khc_xapiansearch --indexdir=%d --identifier=%i --words=%w --method=%o --maxnum=%m --lang=%l
IndexCommand=${KDE_INSTALL_FULL_LIBEXECDIR}/khc_xapianindexer --indexdir=%d --identifier=%i --lang=%l
