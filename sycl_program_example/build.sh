llvm-as sample_kernel.ll -o sample_kernel.bc
llvm-spirv sample_kernel.bc -o sample_kernel.spv

dpcpp sycl_host.cpp -lze_loader -o host
