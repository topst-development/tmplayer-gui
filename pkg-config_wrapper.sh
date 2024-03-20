#!/bin/sh
PKG_CONFIG_SYSROOT_DIR=/home/B180224/dev_server/TCC897x_Linux_IVI_K414/build/tcc8971-lcn-3.0/tmp/work/cortexa7hf-neon-telechips-linux-gnueabi/tmplayer-gui/1.0.0-r0/recipe-sysroot
export PKG_CONFIG_SYSROOT_DIR
PKG_CONFIG_LIBDIR=/home/B180224/dev_server/TCC897x_Linux_IVI_K414/build/tcc8971-lcn-3.0/tmp/work/cortexa7hf-neon-telechips-linux-gnueabi/tmplayer-gui/1.0.0-r0/recipe-sysroot/usr/lib/pkgconfig
export PKG_CONFIG_LIBDIR
exec pkg-config "$@"
