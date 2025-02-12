#pragma once
#include <cstddef>
#include <map>
#include <string>
#include <vector>

namespace cmn {
namespace tgt {

// these are bitfields because instructions can support multiple (e.g. an arg that's R/M32)
// types are widened for matching (e.g. '7' will match I8, I16, I32, ...)
enum argTypes {
   kArgTypeNone = 1 <<  1,

   kR8          = 1 <<  2,
   kR16         = 1 <<  3,
   kR32         = 1 <<  4,
   kR64         = 1 <<  5,

   kI8          = 1 <<  6,
   kI16         = 1 <<  7,
   kI32         = 1 <<  8,
   kI64         = 1 <<  9,

   kM8          = 1 << 10,
   kM16         = 1 << 11,
   kM32         = 1 << 12,
   kM64         = 1 << 13,
};

enum instrIds {
   kFirstInstr,

   kSelectSegment = kFirstInstr,
   kEnterFunc,
   kExitFunc,
   kLabel,

   kReserveLocal,
   kUnreserveLocal, // 5

   kGlobalConstData,

   kPush,
   kPop,

   kSub,
   kAdd, // 10
   kXor,

   kMov,
         // an aside on LEA: I had originally hoped to avoid this instr, but I believe it
         // is unavoidable given my requirements. Consider the string literals.  To deal
         // with a string, I need to know the address of the first character, which is
         // determined by where the loader placed the data segment at runtime.  Since I
         // don't want load-time patching, this is only discoverable with LEA.
   kLea,

   kPreCallStackAlloc,
   kCall,
   kPostCallStackAlloc,

   kRet, // 17

   kSplit,

   kMacroIsLessThan,
   kMacroIsEqualTo, // 20
   kCmp,
   kSetLessThanSigned,
   kSetEqualTo,

   kMacroIfFalse, // 24
   kJumpEqual,
   kGoto,

   kLastInstr,
   kSyscall = kLastInstr,
};

class instrFmt {
public:
   instrFmt(const char *g, size_t a1, size_t a2, size_t a3, size_t a4,
      const char *io, bool ss);

   const char *guid;

   const char *argIo;   // (r)ead, (w)rite, or (b)oth
   bool stackSensitive; // can I make last-minute stack adjustments or will that affect this
                        // instr?

   size_t a1;
   size_t a2;
   size_t a3;
   size_t a4;
};

enum genericStorageClasses {
   kStorageUnassigned,
   kStorageStackPtr,
   kStorageStackFramePtr,
   kStorageImmediate,
   kStorageUndecidedStack,
   _kStorageFirstReg  = 23,
   _kStackFlag        = 0x10000, // this leaves 16-bits for disp
   _kVirtStackFlag    = 0x20000,
};

inline bool isStackStorage(size_t s) { return s & _kStackFlag; }
inline int getStackDisp(size_t s) { return (short)(s & 0xFFFF); }
inline size_t makeStackStorage(int disp) { return (disp & 0xFFFF) | _kStackFlag; }

inline bool isVStack(size_t s) { return s & _kVirtStackFlag; }
inline int getVStackInt(size_t s) { return (short)(s & 0xFFFF); }
inline size_t makeVStack(int disp) { return (disp & 0xFFFF) | _kVirtStackFlag; }

class instrInfo {
public:
   const char *name;

   const char *argIo;   // (r)ead, (w)rite, or (b)oth
   bool stackSensitive; // can I make last-minute stack adjustments or will that affect this

   const instrFmt *fmts;

   const instrFmt& demandFmt(const std::vector<argTypes> a) const;
   const instrFmt *findFmt(const std::vector<argTypes> a) const;
};

// represents everything expressed in a single assembly-level argument.
// this includes all addressing modes [reg+8*reg+23], etc.
class asmArgInfo {
public:
   enum argFlags {
      kLabel    = 1<< 1,
      kImm8     = 1<< 2,
      kImm16    = 1<< 3,
      kImm32    = 1<< 4,
      kImm64    = 1<< 5,
      kReg64    = 1<< 6,
      kMem64    = 1<< 7,
   };

   asmArgInfo();

   size_t flags;

   union {
      struct {
         unsigned char v[8];
      } bytes;
      struct {
         unsigned short v[4];
      } words;
      struct {
         unsigned long v[2];
      } dwords;
      struct {
         unsigned __int64 v[1];
      } qwords;
   } data;

   std::string label;
   //__int64 index;
   //__int64 scale;
   __int64 disp;

   argTypes computeArgType();
};

class iCallingConvention {
public:
   virtual bool stackArgsPushRToL() const = 0;

   virtual size_t getShadowSpace() const = 0;
   //virtual size_t getRegisterHome(size_t r) const = 0;

   // return the total stack size given args sizes in v
   virtual size_t getArgumentStackSpace(std::vector<size_t>& v) const = 0;
   virtual void getRValAndArgBank(std::vector<size_t>& v) const = 0;

   virtual bool requiresPrologEpilogSave(size_t r) const = 0;
   virtual bool requiresSubCallSave(size_t r) const = 0; // TODO HACK - not used... yet

   virtual void createRegisterBankInPreferredOrder(std::vector<size_t>& v) const = 0;
   // registers that don't require prolog/epilog save
   virtual void createScratchRegisterBank(std::vector<size_t>& v) const = 0;

   virtual size_t alignStackForSubcall(const size_t& currentAlignment) const = 0;
};

class iSyscallConvention : public iCallingConvention {
public:
   virtual instrIds getOpCode(std::string& thunk) const = 0;
};

class iProcessorInfo {
public:
   virtual void createRegisterMap(std::map<size_t,size_t>& m) const = 0;
   virtual const char *getRegName(size_t r) const;
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
