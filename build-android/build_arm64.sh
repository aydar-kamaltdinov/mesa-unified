#!/bin/sh
export TARGET_TRIPLE=aarch64-linux-android29
export MESON_CPU_FAMILY=aarch64
export MESON_CPU=armv8
export EXTRA_ARGS="-Dgallium-drivers=zink -Dvulkan-drivers="
exec ./build.sh
