#include <CL/sycl.hpp>
#include <iostream>


template<typename T, size_t N>
struct alignas(4096) AlignedArray {
  T data[N];
};

extern "C" SYCL_EXTERNAL void kernel_name(float*, float*); 

int main(int argc, char* argv[]) {
  // Initialize default sycl context
  cl::sycl::context ctx;

  std::cout << "Available devices:\n";
  for (auto& device : ctx.get_devices()) {
    std::cout << "\t" << device.get_info<cl::sycl::info::device::name>() << "\n";
  }

  // hello world??
  constexpr int a_size = 32;
  AlignedArray<float, a_size> a, b;
  for (auto i = 0; i < a_size; ++i) {
    a.data[i] = a_size - i;
    b.data[i] = i;
  }

  std::cout << "%dst before running the kernel:\n";
  for (int i = 0; i < a_size; ++i) {
    std::cout << b.data[i] << " ";
  }
  std::cout << std::endl;

  cl::sycl::default_selector device_selector;

  cl::sycl::queue queue(device_selector);
  std::cout << "Running on " << queue.get_device().get_info<cl::sycl::info::device::name>() << "\n";
  {
    cl::sycl::buffer a_sycl(a.data, cl::sycl::range(a_size));
    cl::sycl::buffer b_sycl(b.data, cl::sycl::range(a_size));

    queue.submit([&] (cl::sycl::handler& cgh) {
      auto a_acc = a_sycl.get_access<cl::sycl::access::mode::read>(cgh);
      auto b_acc = b_sycl.get_access<cl::sycl::access::mode::write>(cgh);

      cgh.parallel_for(cl::sycl::range<1>(a_size), ([=](cl::sycl::item<1> i) {
         kernel_name(a_acc.get_pointer(), b_acc.get_pointer());
      }));
    });
  }

  std::cout << "%dst after running the kernel:\n";
  for (int i = 0; i < a_size; ++i) {
    std::cout << b.data[i] << " ";
  }
  std::cout << std::endl;

  return 0;
}
