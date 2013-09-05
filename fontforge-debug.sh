#!/bin/sh
version
settings set frame-format "frame #${frame.index}: ${frame.pc}{ ${module.file.basename}`${function.name-with-args}{${function.pc-offset}}}{ at ${line.file.basename}:${line.number}}\n"
target create /usr/local/bin/fontforge
target select 0
run
