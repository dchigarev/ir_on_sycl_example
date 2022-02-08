## Declare kernel in host.cpp as SYCL_EXTERNAL and link the implementation at the fat-binary linking stage.

Steps to execute:
1.	Declare kernel function in a host.cpp according to the kernel’s signature in IR as following:
```c++
#include <CL/sycl/detail/defines_elementary.hpp> // SYCL_EXTERNAL macros
SYCL_EXTERNAL void external_kernel(float*, float*);
```
2.	Define two offload bundles in LLVM IR: the one for device (target=spir64) and the one for the host (target=x86_64-…)
```llvm
; __CLANG_OFFLOAD_BUNDLE____START__ sycl-spir64-unknown-unknown
target triple = "spir64-unknown-unknown"

; declarations of device kernels
; ...

; __CLANG_OFFLOAD_BUNDLE____END__ sycl-spir64-unknown-unknown

; __CLANG_OFFLOAD_BUNDLE____START__ host-x86_64-unknown-linux-gnu
target triple = "x86_64-unknown-linux-gnu"

; __CLANG_OFFLOAD_BUNDLE____END__ host-x86_64-unknown-linux-gnu
```
3.	For some reason, dpcpp compiler requires both versions of the kernels to be defined: the device and the host one, otherwise, it won't link the fat-binary. This requirement forces us to define dummy host functions that accord to the original kernel’s signatures, just to satisfy the linker. #TODO: find out why it's required to have both versions and find ways to get rid of it.
```llvm
; __CLANG_OFFLOAD_BUNDLE____START__ host-x86_64-unknown-linux-gnu
target triple = "x86_64-unknown-linux-gnu"

; definitions of dummy host kernels
define void kernel1(float* %1, float* %2) {
entry:
    ret void
}

; ...
; definitions of dummy host kernels
define void kernelN(float* %1, float* %2) {
entry:
    ret void
}

; __CLANG_OFFLOAD_BUNDLE____END__ host-x86_64-unknown-linux-gnu
```
4.	Mangle function names so the linker be able to find them.
5.	Compile LLVM IR into .o file using clang++ from oneAPI samples.
6.	Compile host.cpp and link it with the kernel’s object file.
7.	Call these kernel function inside SYCL kernels.

