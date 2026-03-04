mkdir -p build/

cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=/usr/bin/clang \
  -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
  -DCMAKE_C_FLAGS_RELEASE="-O3 -march=native" \
  -DCMAKE_CXX_FLAGS_RELEASE="-O3 -march=native" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DDEBUGGER=OFF

cmake --build build


