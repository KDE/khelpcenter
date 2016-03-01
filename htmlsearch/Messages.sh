#! /usr/bin/env bash
exit 0  # htmlsearch is not built and deprecated
### TODO: why do we need 2 POT files for one directory?
$XGETTEXT index.cpp htmlsearch.cpp progressdialog.cpp rc.cpp -o $podir/htmlsearch.pot
$XGETTEXT kcmhtmlsearch.cpp klangcombo.cpp ktagcombobox.cpp rc.cpp -o $podir/kcmhtmlsearch.pot
