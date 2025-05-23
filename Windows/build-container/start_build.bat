@REM (c) ArangoDB, 2025

echo "ArangoDB build on windows latest started!"

git clone --depth 1 --recurse-submodules --shallow-submodules --jobs 8 --branch feature/windows-clang-build https://github.com/fceller/arangodb

cmake --fresh -G "Visual Studio 17 2022" -B ./build-arangodb/ -S ./arangodb/  --preset community -T="ClangCL" -DSKIP_PACKAGING=TRUE -DUSE_JEMALLOC=FALSE -DUSE_LIBUNWIND=FALSE -DUSE_V8=OFF -DUSE_BUILD_ID_READER=OFF

cmake --build ./build-arangodb/ --target arangod.exe

echo "To build arangod target run:"
echo "cmake --build ./build-arangodb/ --target arangod"