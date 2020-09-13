#! /bin/bash

sudo umount /dev/loop16

sudo losetup -d /dev/loop16

rm journal

rm disk

sudo rmmod dadfs

make clean
