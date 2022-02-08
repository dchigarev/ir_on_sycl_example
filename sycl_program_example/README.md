## Wrap L0/OpenCL module into sycl::program and submit it to the SYCL device queue.

Steps to execute:
1.	Compile LLVM IR into SPIR-V module.
2.	Read SPIR-V binary and build L0 module/OpenCL program.
3.	Wrap the module into `sycl::program` with `sycl::backend_name(level_zero/opencl)::make_program(sycl::context, binary_module)`.
4.	Extract required kernel from the `sycl::program` (`sycl::program::get_kernel(string)`).
5.	Operate with the extracted kernel as a regular SYCL kernel.
