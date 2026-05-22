#!/bin/sh
export TARGET_TRIPLE=armv7a-linux-androideabi29
export MESON_CPU_FAMILY=arm
export MESON_CPU=armv7
export EXTRA_ARGS="-Dgallium-drivers=zink -Dvulkan-drivers="
exec ./build.sh
