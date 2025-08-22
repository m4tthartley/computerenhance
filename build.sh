##
##  Created by Matt Hartley on 21/08/2025.
##  Copyright 2025 GiantJelly. All rights reserved.
##

set -e

outdir="./build"
mkdir -p $outdir

files="8086/emu.c"
flags="-I/Users/matt/code/core -framework CoreFoundation"

clang $files $flags -o $outdir/8086 


input="./data/listing_0039_more_movs"
input2="./data/listing_0040_challenge_movs"

$outdir/8086 $input 
$outdir/8086 $input > $outdir/out.asm
nasm $outdir/out.asm
diff $input $outdir/out

$outdir/8086 $input2 
$outdir/8086 $input2 > $outdir/out.asm
nasm $outdir/out.asm
diff $input2 $outdir/out
