##
##  Created by Matt Hartley on 21/08/2025.
##  Copyright 2025 GiantJelly. All rights reserved.
##

outdir="./build"
mkdir -p ./build

files="emu8086.c"
flags="-I../core"

clang $files $flags -o $outdir/8086 
