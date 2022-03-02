## Declare kernel in host.cpp as SYCL_EXTERNAL and link the implementation at the fat-binary linking stage.

Steps to execute:
1.	Declare kernel function in a host.cpp according to the kernel’s signature in IR as following:
```c++
#include <CL/sycl/detail/defines_elementary.hpp> // SYCL_EXTERNAL macros
extern "C" SYCL_EXTERNAL void external_kernel(float*, float*);
// SYCL_EXTERNAL is to declare 'external_kernel' as a spir_func
// extern "C" is to disable mangling for the function name
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
3.	dpcpp compiler requires both versions of the kernel function to be defined: the device and the host one, otherwise it won't link the fat binary. This requirements comes from the ability of SYCL to execute kernels both at host and device, which forces us to define dummy host functions that accord to the original kernel’s signatures, just to satisfy the linker (we can define dummy host function as we won't call it) # TODO: is there a way to not compile host functions when we don't need it?
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
4.	Compile LLVM IR into .o file using clang++ from oneAPI samples.
5.	Compile host.cpp and link it with the kernel’s object file.
6.	Call these kernel function inside SYCL kernels.

