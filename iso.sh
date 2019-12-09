#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/khaos.kernel isodir/boot/khaos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "khaos" {
	multiboot /boot/khaos.kernel
}
EOF
grub-mkrescue -o khaos.iso isodir
