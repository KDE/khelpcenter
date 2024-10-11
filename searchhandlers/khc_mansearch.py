#!/bin/env python3
# Script for searching man pages. The result is generated as HTML.
#
# Converted from Perl code which was:
# SPDX-FileCopyrightText: 2002 SuSE Linux AG Nuernberg
# SPDX-FileContributor: Cornelius Schumacher <cschum@suse.de>
#
# SPDX-License-Identifier: GPL-2.0-or-later

import argparse
import subprocess
import re
import sys

def main():
    parser = argparse.ArgumentParser(description='Search for man pages.')
    parser.add_argument('--maxcount', type=int, help='Maximum number of results to return')
    parser.add_argument('--words', type=str, required=True, help='Search words')
    parser.add_argument('--lang', type=str, default='en', help='Language code')
    parser.add_argument('--method', type=str, choices=['and', 'or'], required=True, help='Search method')
    args = parser.parse_args()

    if args.lang == 'C':
        args.lang = 'en'

    # Store command results only when they succeed
    command_results = ""

    # Strip leading and trailing whitespace from search query
    words = args.words.strip()

    # If the query is a single word, perhaps it is the literal name of the
    # command, or a prefix, or a substring
    if not ' ' in words:
        results_prefix = subprocess.run(['whatis', '-w', words + '*'], capture_output=True, text=True)
        if results_prefix.returncode == 0:
            command_results += results_prefix.stdout
        results_substring = subprocess.run(['whatis', '-w', '*' + words + '*'], capture_output=True, text=True)
        if results_substring.returncode == 0:
            command_results += results_substring.stdout

    # Build the apropos command line
    apropos = ['apropos', '-L', args.lang]
    if args.method == 'and':
        apropos.append('--and')
    apropos.extend(words.split(' '))

    # Perform search
    results_apropos = subprocess.run(apropos, capture_output=True, text=True)
    if results_apropos.returncode != 0:
        command_results += results_apropos.stdout

    # Results from the previous commands are formatted as:
    # assert_perror (3)    - test errnum and abort
    pattern = re.compile(r'^([a-zA-Z0-9_:-]+)\s+\((.*)\)\s+-\s+(.*)$')

    results = []
    seen = set()
    for line in command_results.splitlines():
        match = pattern.match(line)
        if match:
            page, section, description = match.groups()
            if page and (page + section) not in seen:
                seen.add(page + section)
                results.append((page, section, description))

    if results:
        print("<ul>")
        count = 0
        for page, section, description in results:
            url = f"man:{page}({section})"
            print(f"<li><a href=\"{url}\">{page}({section}) - {description}</a></li>")
            count += 1
            if count == args.maxcount:
                break
        print("</ul>")

if __name__ == "__main__":
    main()
