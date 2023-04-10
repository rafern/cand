#!/bin/bash
args=""
#wrapper="valgrind"
bdir="bin"
bin="cand"

(cd "./$bdir" && $wrapper "./$bin" $args)
echo Program terminated! Exit code $?.
