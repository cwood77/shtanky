#pragma once
#include <memory>

namespace shtemu {

class iImageAllocation {
public:
   virtual ~iImageAllocation() {}
   virtual unsigned char *getBasePtr() = 0;
};

class iImageAllocator {
public:
   virtual iImageAllocation *allocate(size_t s) = 0;
};

class windowsAllocator : public iImageAllocator {
public:
   virtual iImageAllocation *allocate(size_t s);
};

class image {
public:
   typedef void (*entrypoint_t)(size_t);
   typedef void (*osCall_t)(size_t, void*);

   explicit image(iImageAllocation& a) : m_pPtr(&a) {}

   entrypoint_t findEntrypoint();
   void patch(osCall_t hook);

private:
   osCall_t findOsCallImpl();

   std::unique_ptr<iImageAllocation> m_pPtr;
};

class loader {
public:
   image *loadFile(iImageAllocator& a, const std::string& path);
};

} // namespace shtemu
