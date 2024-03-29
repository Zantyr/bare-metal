#!/bin/bash

set -euo pipefail

#init directory
mkdir -p builds/iso/boot
mkdir -p builds/obj

#preprocessing macra
mkdir -p src/built
MACROS=`ls src/macros/*.macro | sort`
MACROABLE=`find src | grep \\.meta\\.c || echo ""`
if [ ! -z "$MACROABLE" ]; then 
for fname in $MACROABLE; do
	python tools/preprocess.py ${fname} $MACROS
done
fi

function cleanup {
	# META=`find src | grep \\.meta.c`
	# rm $META
	echo ""
}
trap cleanup EXIT

#assemble
nasm src/boot.asm -o builds/obj/boot.o -f elf32
nasm src/init_tools.asm -o builds/obj/init_tools.o -f elf32

if [ ! -z "${1:-}" ]; then
	MALLOC="$1"
else
	MALLOC="naive"
fi

echo Using malloc "${MALLOC^^}"

#compile C
gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -Wno-unused-function -D DESIREDMALLOCIS"${MALLOC^^}" -c src/kernel.c -o builds/obj/kernel.o

#link things
ld -T src/linker.ld -melf_i386 -o builds/iso/boot/shitos.bin builds/obj/boot.o builds/obj/kernel.o builds/obj/init_tools.o

# grub thingz
if grub-file --is-x86-multiboot builds/iso/boot/shitos.bin; then
  echo multiboot confirmed
else
  echo the file is not multiboot
  exit 1
fi

# build ISO
mkdir -p builds/iso/boot/grub
cp src/grub.cfg builds/iso/boot/grub/grub.cfg
grub-mkrescue -o builds/shitos.iso builds/iso
