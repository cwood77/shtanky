#pragma once
#include <cstddef>
#include <map>
#include <string>
#include <vector>

namespace cmn {
namespace tgt {

enum argTypes {
   kArgTypeNone = 0,

   kR8          = 1 <<  1,
   kR16         = 1 <<  2,
   kR32         = 1 <<  3,
   kR64         = 1 <<  4,

   kI8          = 1 <<  5,
   kI16         = 1 <<  6,
   kI32         = 1 <<  7,
   kI64         = 1 <<  8,

   kM8          = 1 <<  9,
   kM16         = 1 << 10,
   kM32         = 1 << 11,
   kM64         = 1 << 12,
};

enum instrIds {
   kEnterFunc,
   kExitFunc,

   kPush,
   kPop,

   kSub,
   kAdd,

   kMov,

   kPreCallStackAlloc,
   kCall,
   kPostCallStackAlloc,

   kRet,

   kSyscall,
};

class instrFmt {
public:
   instrFmt(const char *g, size_t a1, size_t a2, size_t a3, size_t a4)
   : guid(g), a1(a1), a2(a2), a3(a3), a4(a4) {}

   const char *guid;

   size_t a1;
   size_t a2;
   size_t a3;
   size_t a4;
};

enum genericStorageClasses {
   kStorageUnassigned,
   kStorageStackPtr,
   kStorageStackFramePtr,
   //kStoragePatch,
   kStorageImmediate,
   _kStorageFirstReg  = 23,
   _kStackFlag        = 0x10000, // this leaves 16-bits for disp
   _kVirtStackFlag    = 0x20000,
};

inline bool isStackStorage(size_t s) { return s & _kStackFlag; }
inline int getStackDisp(size_t s) { return (short)(s & 0xFFFF); }
inline size_t makeStackStorage(int disp) { return ((size_t)disp) | _kStackFlag; }

inline bool isVStack(size_t s) { return s & _kVirtStackFlag; }
inline int getVStackInt(size_t s) { return (short)(s & 0xFFFF); }
inline size_t makeVStack(int disp) { return ((size_t)disp) | _kVirtStackFlag; }

class instrInfo {
public:
   instrInfo(const char *name, const instrFmt *f) : name(name), fmts(f) {}

   const char *name;
   const instrFmt *fmts;
};

class iCallingConvention {
public:
   virtual bool stackArgsPushRToL() const = 0;

   virtual size_t getShadowSpace() const = 0;
   //virtual size_t getRegisterHome(size_t r) const = 0;

   virtual size_t getArgumentStackSpace(std::vector<size_t>& v) const = 0;
   virtual void getRValAndArgBank(std::vector<size_t>& v) const = 0;

   virtual bool requiresPrologEpilogSave(size_t r) const = 0;
   virtual bool requiresSubCallSave(size_t r) const = 0;

   virtual void createRegisterBankInPreferredOrder(std::vector<size_t>& v) const = 0;
};

class iSyscallConvention : public iCallingConvention {
public:
   virtual instrIds getOpCode(std::string& thunk) const = 0;
};

class iProcessorInfo {
public:
   virtual void createRegisterMap(std::map<size_t,size_t>& m) const = 0;
   virtual const char *getRegName(size_t r) const = 0;
   virtual const instrInfo *getInstr(instrIds i) const = 0;
};

class iTargetInfo {
public:
   virtual size_t getRealSize(size_t s) const = 0;
   virtual const iProcessorInfo& getProc() const = 0;
   virtual const iCallingConvention& getCallConvention() const = 0;
   virtual const iSyscallConvention& getSyscallConvention() const = 0;
};

} // namespace tgt

} // namespace cmn
