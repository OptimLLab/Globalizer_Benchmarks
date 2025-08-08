set START_DIR=%cd%
set ROOT_DIR=%~dp0\..

git submodule init
git submodule update

cd %ROOT_DIR%
if not exist build_64 mkdir build_64
cd build_64
cmake -G "Visual Studio 17 2022" -DGLOBALIZER_BENCHMARKS_BUILD_APPLICATION=ON -DGLOBALIZER_BENCHMARKS_USE_CUDA=OFF -DGLOBALIZER_BENCHMARKS_BUILD_TESTS=ON -Drastrigin_build=ON -DX2_build=ON ..

Globalizer_Benchmarks.sln

cd %START_DIR%

pause
