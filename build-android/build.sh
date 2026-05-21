#!/bin/sh
rm build-crossfile

install_ndk() {
	# install ndk
	echo "Installing NDK"
	curl https://dl.google.com/android/repository/android-ndk-r29-linux.zip --output android-ndk-r29-linux.zip
	unzip android-ndk-r29-linux.zip
	export ANDROID_NDK_HOME="$(pwd)/android-ndk-r29"
}

if [ ! -d "${ANDROID_NDK_HOME}" ]; then
	echo "NDK not found. Set ANDROID_NDK_HOME if you want to use a preinstalled NDK"
	install_ndk
fi

echo "Will use NDK at ${ANDROID_NDK_HOME}"
echo "Arch (matrix): ${MATRIX_ARCH}"

echo "Building libdrm dependency"
envsubst <crossfile >build-crossfile
pushd drm
mkdir build-android
meson setup . "build-android" \
	--prefix=/tmp/drm-static \
	--cross-file "../build-crossfile"
ninja -C "build-android" install
popd
echo "Building Mesa"
meson setup .. "build-android" \
        --prefix=/tmp/zink-${MATRIX_ARCH} \
        --cross-file "build-crossfile" \
            -Dplatforms=android \
	    -Dbuildtype=release \
	    -Dstrip=true \
            -Dplatform-sdk-version=29 \
            -Dandroid-stub=true \
            -Dxlib-lease=disabled \
            -Degl=enabled \
            -Dgbm=disabled \
            -Dglx=disabled \
            -Dgles1=disabled \
            -Dgles2=enabled \
            -Dopengl=true \
            -Degl-lib-suffix=_mesa \
            -Dandroid-libbacktrace=disabled ${EXTRA_ARGS}
ninja -C "build-android" install
echo "Installing DRM dependencies"
cp -rv /tmp/drm-static/lib/*.so /tmp/zink-${MATRIX_ARCH}/lib/
echo "Removing temporary stuff"
rm -rf /tmp/drm-static
