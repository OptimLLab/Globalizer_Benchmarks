set START_DIR=%cd%
set ROOT_DIR=%~dp0\..

git submodule init
git submodule update

cd %ROOT_DIR%
if not exist build_64_19 mkdir build_64_19
cd build_64_19
cmake -G "Visual Studio 16 2019" -DGLOBALIZER_BENCHMARKS_BUILD_APPLICATION=ON -DGLOBALIZER_BENCHMARKS_USE_CUDA=OFF -DGLOBALIZER_BENCHMARKS_BUILD_TESTS=ON -Drastrigin_build=ON -DrastriginInt_build=ON -DX2_build=ON -Dpython_objective_build=ON -DiOptProblem_build=ON -Dstronginc3_build=ON ..

Globalizer_Benchmarks.sln

cd %START_DIR%

pause
