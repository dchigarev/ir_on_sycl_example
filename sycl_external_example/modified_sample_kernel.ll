; Example handmade kernel
; __CLANG_OFFLOAD_BUNDLE____START__ sycl-spir64-unknown-unknown
target triple = "spir64-unknown-unknown"

; Function Attrs: mustprogress nofree norecurse nosync nounwind readnone willreturn
define spir_func void @_Z11kernel_namePfS_(float addrspace(1)* %src, float addrspace(1)* %dst) {
entry:
    %idx = call spir_func i32 @_Z13get_global_idj(i32 0)
    %srcidx = getelementptr inbounds float, float addrspace(1)* %src, i32 %idx
    %arrayidx = getelementptr inbounds float, float addrspace(1)* %dst, i32 %idx
    %ld = load float, float addrspace(1)* %srcidx, align 4
    %add = fadd float %ld, 100.0
    store float %add, float addrspace(1)* %arrayidx, align 4
    ret void
}

declare spir_func i32 @_Z13get_global_idj(i32)


!spirv.Source = !{!0}
!opencl.spir.version = !{!1}
!opencl.ocl.version = !{!2}
!opencl.used.extensions = !{!3}
!opencl.used.optional.core.features = !{!3}
!spirv.Generator = !{!4}

!0 = !{i32 3, i32 100000}
!1 = !{i32 1, i32 2}
!2 = !{i32 1, i32 0}
!3 = !{}
!4 = !{i16 6, i16 14}

; __CLANG_OFFLOAD_BUNDLE____END__ sycl-spir64-unknown-unknown

; __CLANG_OFFLOAD_BUNDLE____START__ host-x86_64-unknown-linux-gnu
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define void @_Z11kernel_namePfS_(float addrspace(1)* %src, float addrspace(1)* %dst) {
entry:
    ret void
}

; __CLANG_OFFLOAD_BUNDLE____END__ host-x86_64-unknown-linux-gnu

