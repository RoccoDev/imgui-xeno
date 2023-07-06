#include "MemoryBuffer.h"
#include "imgui_impl_nvn.hpp"
#include "logger/Logger.hpp"
#include "helpers/memoryHelper.h"

MemoryBuffer::MemoryBuffer(size_t size) {

  auto *bd = ImguiNvnBackend::getBackendData();

  size_t alignedSize = ALIGN_UP(size, 0x1000);

  memBuffer = Mem::AllocateAlign(0x1000, alignedSize);
  memset(memBuffer, 0, alignedSize);

  bd->memPoolBuilder.SetDefaults()
      .SetDevice(bd->device)
      .SetFlags(nvn::MemoryPoolFlags::CPU_UNCACHED | nvn::MemoryPoolFlags::GPU_CACHED)
      .SetStorage(memBuffer, alignedSize);

  if (!pool.Initialize(&bd->memPoolBuilder)) {
    Logger::log("Failed to Create Memory Pool!\n");
    return;
  }

  bd->bufferBuilder.SetDevice(bd->device).SetDefaults().SetStorage(&pool, 0, alignedSize);

  if (!buffer.Initialize(&bd->bufferBuilder)) {
    Logger::log("Failed to Init Buffer!\n");
    return;
  }

  mIsReady = true;
}

MemoryBuffer::MemoryBuffer(size_t size, nvn::MemoryPoolFlags flags) {

  auto *bd = ImguiNvnBackend::getBackendData();

  size_t alignedSize = ALIGN_UP(size, 0x1000);

  memBuffer = Mem::AllocateAlign(0x1000, alignedSize);
  memset(memBuffer, 0, alignedSize);

  bd->memPoolBuilder.SetDefaults()
      .SetDevice(bd->device)
      .SetFlags(flags)
      .SetStorage(memBuffer, alignedSize);

  if (!pool.Initialize(&bd->memPoolBuilder)) {
    Logger::log("Failed to Create Memory Pool!\n");
    return;
  }

  bd->bufferBuilder.SetDevice(bd->device).SetDefaults().SetStorage(&pool, 0, alignedSize);

  if (!buffer.Initialize(&bd->bufferBuilder)) {
    Logger::log("Failed to Init Buffer!\n");
    return;
  }

  mIsReady = true;
}

MemoryBuffer::MemoryBuffer(size_t size, void *bufferPtr, nvn::MemoryPoolFlags flags) {

  auto *bd = ImguiNvnBackend::getBackendData();

  // Copy to respect alignment for both size and pointer
  memBuffer = Mem::AllocateAlign(0x1000, size);
  memcpy(memBuffer, bufferPtr, size);

  bd->memPoolBuilder.SetDefaults()
      .SetDevice(bd->device)
      .SetFlags(flags)
      .SetStorage(memBuffer, size);

  if (!pool.Initialize(&bd->memPoolBuilder)) {
    Logger::log("Failed to Create Memory Pool!\n");
    return;
  }

  bd->bufferBuilder.SetDevice(bd->device).SetDefaults().SetStorage(&pool, 0, size);

  if (!buffer.Initialize(&bd->bufferBuilder)) {
    Logger::log("Failed to Init Buffer!\n");
    return;
  }

  mIsReady = true;
}

void MemoryBuffer::Finalize() {
  IM_FREE(memBuffer);
  pool.Finalize();
  buffer.Finalize();
}

void MemoryBuffer::ClearBuffer() {
  memset(memBuffer, 0, pool.GetSize());
}