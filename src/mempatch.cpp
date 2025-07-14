#include <napi.h>
#include <vector>
#include <cstring>
#include <iostream>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <sys/mman.h>
  #include <fcntl.h>
  #include <link.h>
#endif

bool protectMemory(void* addr, size_t len) {
  #ifdef _WIN32
    DWORD old;
    return VirtualProtect(addr, len, PAGE_EXECUTE_READWRITE, &old);
  #else
    size_t pageSize = sysconf(_SC_PAGE_SIZE);
    uintptr_t pageStart = (uintptr_t)addr & ~(pageSize - 1);
    return mprotect((void*)pageStart, len, PROT_READ | PROT_WRITE | PROT_EXEC) == 0;
  #endif
}

#ifdef _WIN32
  std::vector<MEMORY_BASIC_INFORMATION> getMemoryRegions() {
    std::vector<MEMORY_BASIC_INFORMATION> regions;
    MEMORY_BASIC_INFORMATION mbi;
    uintptr_t addr = 0;
    while (VirtualQuery((LPCVOID)addr, &mbi, sizeof(mbi))) {
      if (mbi.State == MEM_COMMIT &&
          (mbi.Protect & (PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY))) {
        regions.push_back(mbi);
      }
      addr += mbi.RegionSize;
    }
    return regions;
  }
  #else
  std::vector<std::pair<uint8_t*, size_t>> getMemoryRegions() {
    std::vector<std::pair<uint8_t*, size_t>> regions;
    FILE* f = fopen("/proc/self/maps", "r");
    if (!f) return regions;

    char line[1024];
    while (fgets(line, sizeof(line), f)) {
      uintptr_t start, end;
      char perm[5];
      if (sscanf(line, "%lx-%lx %4s", &start, &end, perm) == 3) {
        if (perm[0] == 'r' && perm[2] == 'x') {
          regions.emplace_back((uint8_t*)start, end - start);
        }
      }
    }
    fclose(f);
    return regions;
  }
#endif

Napi::Value patch(const Napi::CallbackInfo& info) {
  auto env = info.Env();
  if (info.Length() < 4)
    Napi::TypeError::New(env, "Need pattern, patMask, replacement, repMask").ThrowAsJavaScriptException();

  auto patBuf  = info[0].As<Napi::Buffer<uint8_t>>();
  auto patMask = info[1].As<Napi::Buffer<uint8_t>>();
  auto repBuf  = info[2].As<Napi::Buffer<uint8_t>>();
  auto repMask = info[3].As<Napi::Buffer<uint8_t>>();

  size_t len = patBuf.Length();
  if (len != patMask.Length() || len != repBuf.Length() || len != repMask.Length()) {
    Napi::TypeError::New(env, "All buffers must be same length").ThrowAsJavaScriptException();
    return env.Null();
  }

  const uint8_t* pat = patBuf.Data();
  const uint8_t* pms = patMask.Data();
  const uint8_t* rep = repBuf.Data();
  const uint8_t* rms = repMask.Data();

#ifdef _WIN32
  auto regions = getMemoryRegions();
  for (const auto& mbi : regions) {
    uint8_t* base = (uint8_t*)mbi.BaseAddress;
    for (size_t off = 0; off < mbi.RegionSize - len; ++off) {
#else
  auto regions = getMemoryRegions();
  for (const auto& [base, size] : regions) {
    for (size_t off = 0; off < size - len; ++off) {
#endif
      uint8_t* addr = base + off;

      bool match = true;
      for (size_t i = 0; i < len; ++i) {
        if (pms[i] && addr[i] != pat[i]) {
          match = false;
          break;
        }
      }
      if (!match)
        continue;

      if (!protectMemory(addr, len)) continue;

      for (size_t i = 0; i < len; ++i){
        if (rms[i]) addr[i] = rep[i];
      }
      return Napi::Number::New(env, (uintptr_t)addr);
    }
  }
  return env.Null();
}


Napi::Object Initialize(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "patch"), Napi::Function::New(env, patch));
  return exports;
}

NODE_API_MODULE(MemPatch, Initialize);