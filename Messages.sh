#! /usr/bin/env bash
$EXTRACTRC `find . -name "*.kcfg" -o -name "*.rc"` >> rc.cpp || exit 11
$XGETTEXT *.cpp -o $podir/khelpcenter.pot
rm -f rc.cpp
