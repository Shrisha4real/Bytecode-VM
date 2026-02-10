mkdir -p build/
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DDEBUGGER=OFF

cmake --build build


./build/bin/BytecodeVM file.txt
