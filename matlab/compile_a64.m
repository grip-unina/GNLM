mex -glnxa64 -largeArrayDims -O -v -D_GLIBCXX_USE_CXX11_ABI=0 -I../include -DGUIDA_NUM_BANDS=4   guidedNLMeans.cpp ../lib_static_a64/libcv.a ../lib_static_a64/libcxcore.a ../lib_static_a64/libopencv_lapack.a -output guidedNLMeans_b04
mex -glnxa64 -largeArrayDims -O -v -D_GLIBCXX_USE_CXX11_ABI=0 -I../include -DGUIDA_NUM_BANDS=8   guidedNLMeans.cpp ../lib_static_a64/libcv.a ../lib_static_a64/libcxcore.a ../lib_static_a64/libopencv_lapack.a -output guidedNLMeans_b08
mex -glnxa64 -largeArrayDims -O -v -D_GLIBCXX_USE_CXX11_ABI=0 -I../include -DGUIDA_NUM_BANDS=16  guidedNLMeans.cpp ../lib_static_a64/libcv.a ../lib_static_a64/libcxcore.a ../lib_static_a64/libopencv_lapack.a -output guidedNLMeans_b16
mex -glnxa64 -largeArrayDims -O -v -D_GLIBCXX_USE_CXX11_ABI=0 -I../include -DGUIDA_NUM_BANDS=32  guidedNLMeans.cpp ../lib_static_a64/libcv.a ../lib_static_a64/libcxcore.a ../lib_static_a64/libopencv_lapack.a -output guidedNLMeans_b32
mex -glnxa64 -largeArrayDims -O -v -D_GLIBCXX_USE_CXX11_ABI=0 -I../include removezeros.cpp ../lib_static_a64/libcv.a ../lib_static_a64/libcxcore.a ../lib_static_a64/libopencv_lapack.a 


