#!/bin/sh
export TARGET_TRIPLE=aarch64-linux-android34
export MESON_CPU_FAMILY=aarch64
export MESON_CPU=armv8
export EXTRA_ARGS="-Dgallium-drivers= -Dvulkan-drivers=amd"
exec ./build.sh
