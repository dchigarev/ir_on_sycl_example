# clang version 14.0.0 (https://github.com/intel/llvm.git 90a4dc7b783e066dc3f341b3c8caf07310740640)
clang++ -fsycl -c modified_sample_kernel.ll
dpcpp sycl_host.cpp modified_sample_kernel.o -o host
