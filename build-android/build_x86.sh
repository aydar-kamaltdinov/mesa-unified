#!/bin/sh
export TARGET_TRIPLE=i686-linux-android26
export MESON_CPU_FAMILY=x86
export MESON_CPU=i686
export EXTRA_ARGS="-Dgallium-drivers=zink -Dvulkan-drivers= "
exec ./build.sh
