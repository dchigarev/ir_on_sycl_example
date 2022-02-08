# Arbitrary LLVM IR via SYCL runtime

This repository contains several examples of how to execute arbitrary spir_kernel/spir_func from LLVM IR via SYCL Runtime:

#### 1. Wrap L0/OpenCL module into sycl::program and submit it to the SYCL device queue.
[sycl_program_example/](https://github.com/dchigarev/ir_on_sycl_example/tree/master/sycl_program_example)

#### 2. Declare kernel in host.cpp as SYCL_EXTERNAL and link the implementation at the fat-binary linking stage.
[sycl_external_example/](https://github.com/dchigarev/ir_on_sycl_example/tree/master/sycl_external_example)

## Requirements

1. [LLVM tools from oneAPI toolkit (including `dpcpp` compiler)](https://github.com/intel/llvm).
