
#build arm64-v8a
cp Lib/arm64-v8a/libAisound60.a Lib
cp Application.mk-arm64-v8a Application.mk
ndk-build

#build armeabi-v7a
cp Lib/armeabi-v7a/libAisound60.a Lib
cp Application.mk-armeabi-v7a Application.mk
ndk-build
