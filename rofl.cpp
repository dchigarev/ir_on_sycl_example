#define __SYCL_INTERNAL_API

#include <CL/cl.h>
#include <CL/sycl.hpp>

#include <CL/sycl/program.hpp>

#include <string>
#include <fstream>

#include <level_zero/zet_api.h>
#include <level_zero/ze_api.h>
#include <CL/sycl/backend/level_zero.hpp>


template<typename T, size_t N>
struct alignas(4096) AlignedArray {
  T data[N];
};

cl::sycl::program read_module(std::string fname, cl::sycl::context& ctx) {
  /// Reads SPIR-V binary module from file `fname` and wraps into a sycl::program
  /// with the passed sycl::context

  // read binary spirv from file
  std::ifstream is;
  std::string fn = fname;

  is.open(fn, std::ios::binary);
  if (!is.good()) {
    fprintf(stderr, "Open %s failed\n", fn.c_str());
    exit(1);
  }

  is.seekg(0, std::ios::end);
  size_t codeSize = is.tellg();
  is.seekg(0, std::ios::beg);

  if (codeSize == 0) {
    exit(2);
  }
  unsigned char *codeBin = new unsigned char[codeSize];
  if (!codeBin) {
    exit(3);
  }

  is.read((char *)codeBin, codeSize);
  is.close();

  // build l0 descriptor
  ze_module_desc_t ZeModuleDesc = {};
  ZeModuleDesc.stype = ZE_STRUCTURE_TYPE_MODULE_DESC;
  ZeModuleDesc.format = ZE_MODULE_FORMAT_IL_SPIRV;
  ZeModuleDesc.inputSize = codeSize;
  ZeModuleDesc.pInputModule = (uint8_t *)codeBin;
  ZeModuleDesc.pBuildFlags = "";
  ZeModuleDesc.pConstants = nullptr;

  // build l0 context
  auto ZeCtx = cl::sycl::get_native<cl::sycl::backend::level_zero>(ctx);

  // build l0 device
  auto ZeDevice = cl::sycl::get_native<cl::sycl::backend::level_zero>(ctx.get_devices()[0]);

  // build l0 module
  ze_module_handle_t ZeModule;
  auto ret = zeModuleCreate(ZeCtx, ZeDevice, &ZeModuleDesc, &ZeModule, nullptr);
  if (ret != ZE_RESULT_SUCCESS) {
    std::cout << "ZeModule creation failed." << std::endl;
    exit(1);
  }

  // build sycl::program
  auto sycl_program = cl::sycl::level_zero::make_program(
    ctx, reinterpret_cast<uintptr_t>(ZeModule)
  );

  return sycl_program;
}

int main(int argc, char* argv[]) {
  // Initialize default sycl context
  cl::sycl::context ctx;

  std::cout << "Available devices:\n";
  for (auto& device : ctx.get_devices()) {
    std::cout << "\t" << device.get_info<cl::sycl::info::device::name>() << "\n";
  }

  // Read SPIR-V module from file
  std::string spvFilename = "test.spv";
  if (argc > 1) {
    spvFilename = std::string(argv[1]);
  }

  auto sycl_program = read_module(spvFilename, ctx);
  cl::sycl::kernel kernel = sycl_program.get_kernel("plus1");

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

      cgh.set_args(a_acc, b_acc);
      cgh.single_task(kernel);
    });
  }

  std::cout << "%dst after running the kernel:\n";
  for (int i = 0; i < a_size; ++i) {
    std::cout << b.data[i] << " ";
  }
  std::cout << std::endl;

  return 0;
}
