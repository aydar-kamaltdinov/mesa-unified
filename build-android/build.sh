#!/bin/sh
rm build-crossfile

install_ndk() {
	# install ndk
	echo "Installing NDK"
	curl https://dl.google.com/android/repository/android-ndk-r29-linux.zip --output android-ndk-r29-linux.zip
	unzip android-ndk-r29-linux.zip
	export ANDROID_NDK_HOME="$(pwd)/android-ndk-r29"
}

if [ -d "${ANDROID_NDK_HOME}" ]; then
	echo "NDK not found. Set ANDROID_NDK_HOME if you want to use a preinstalled NDK"
	install_ndk
fi

echo "Will use NDK at ${ANDROID_NDK_HOME}"

echo "Begin building Mesa"
envsubst <crossfile >build-crossfile
meson setup "build-android" \
        --prefix=/tmp/zink-$MESON_CPU_FAMILY \
        --cross-file "build-crossfile" \
            -Dplatforms=android \
            -Dplatform-sdk-version=29 \
            -Dandroid-stub=true \
            -Dxlib-lease=disabled \
            -Degl=enabled \
            -Dgbm=disabled \
            -Dglx=disabled \
            -Dgles1=disabled \
            -Dgles2=enabled \
            -Dopengl=true \
	    -Dfreedreno-kmds=kgsl \
            -Degl-lib-suffix=_mesa \
            -Dandroid-libbacktrace=disabled \
            -Dgallium-drivers=freedreno,zink \
	    -Dvulkan-drivers=freedreno \
	    -Dallow-fallback-for=libdrm \
       ..
ninja -C "build-android" install
