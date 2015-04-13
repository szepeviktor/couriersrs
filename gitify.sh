#!/bin/bash

URLF="https://couriersrs.com/download/couriersrs-0.%s.tar.gz"
VERSION_CMD='echo 1.0;echo 1.1;echo 1.2;'
EXTRACT_CMD='tar -xz --strip-components=1'

Die() {
    local RET="$1"
    shift
    echo -e $@ >&2
    exit "$RET"
}

git status || exit 1

eval "$VERSION_CMD" \
    | while read VER; do
        git rm -rf *
        wget -nv --no-check-certificate -O- "$(printf "$URLF" "$VER")" | ${EXTRACT_CMD} || Die 1 "Download/extraction failure. ($?)"
        git add --all || Die 2 "git-add failure. ($?)"
        git commit -m "version ${VER}" || Die 3 "git-commit failure. ($?)"
    done
