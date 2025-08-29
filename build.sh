##
##  Created by Matt Hartley on 21/08/2025.
##  Copyright 2025 GiantJelly. All rights reserved.
##

set -e

outdir="./build"
mkdir -p $outdir

files="8086/main.c"
flags="-g -I/Users/matt/code/core -framework CoreFoundation"

clang $files $flags -o $outdir/8086 

nasmOptions="-w-prefix-lock-xchg"
nasm ./data/test.asm $nasmOptions

# input="./data/listing_0041_add_sub_cmp_jnz"
# input2="./data/listing_0040_challenge_movs"

# input="./data/listing_0039_more_movs"
# $outdir/8086 $input 
# $outdir/8086 $input > $outdir/out.asm
# nasm $outdir/out.asm
# diff $input $outdir/out

# input="./data/listing_0040_challenge_movs"
# $outdir/8086 $input 
# $outdir/8086 $input > $outdir/out.asm
# nasm $outdir/out.asm
# diff $input $outdir/out

input="./data/test"
$outdir/8086 $input 
$outdir/8086 $input > $outdir/out.asm
nasm $outdir/out.asm $nasmOptions
diff $input $outdir/out

# input="./data/listing_0041_add_sub_cmp_jnz"
# $outdir/8086 $input 
# $outdir/8086 $input > $outdir/out.asm
# nasm $outdir/out.asm $nasmOptions
# diff $input $outdir/out

# input="./data/listing_0042_completionist_decode"
# $outdir/8086 $input 
# $outdir/8086 $input > $outdir/out.asm
# nasm $outdir/out.asm $nasmOptions
# diff $input $outdir/out

# input="./data/listing_0043_immediate_movs"
# $outdir/8086 $input 
# $outdir/8086 $input > $outdir/out.asm
# nasm $outdir/out.asm
# diff $input $outdir/out

# input="./data/listing_0044_register_movs"
# $outdir/8086 $input 
# $outdir/8086 $input > $outdir/out.asm
# nasm $outdir/out.asm
# diff $input $outdir/out

# input="./data/listing_0045_challenge_register_movs"
# $outdir/8086 $input 
# $outdir/8086 $input > $outdir/out.asm
# nasm $outdir/out.asm
# diff $input $outdir/out

# input="./data/listing_0046_add_sub_cmp"
# $outdir/8086 $input 
# $outdir/8086 $input > $outdir/out.asm
# nasm $outdir/out.asm
# diff $input $outdir/out
