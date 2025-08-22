##
##  Created by Matt Hartley on 21/08/2025.
##  Copyright 2025 GiantJelly. All rights reserved.
##

set -e

outdir="./build"
mkdir -p $outdir

files="emu8086.c"
flags="-I../core -framework CoreFoundation"

clang $files $flags -o $outdir/8086 

$outdir/8086 
