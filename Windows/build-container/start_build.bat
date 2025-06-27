@REM (c) ArangoDB, 2025

echo "ArangoDB build on windows latest started!"

git clone --depth 1 --recurse-submodules --shallow-submodules --jobs 8 --branch feature/windows-clang-build https://github.com/fceller/arangodb src

echo "Cloning done, now starting cmake..."

cmake --fresh -B ./build/ -S ./src/  --preset community-win

echo "Build arangod..."

cmake --build ./build/ --config=Release --target arangod
