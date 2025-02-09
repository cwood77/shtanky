#include "intel64.hpp"
#include "throw.hpp"

namespace cmn {
namespace tgt {

static const instrFmt pushFmts[] = {
   { "PUSH{FF /6}",
      kR64 | kM64,
      kArgTypeNone, kArgTypeNone, kArgTypeNone, "r", true },
   { NULL,  kArgTypeNone, kArgTypeNone, kArgTypeNone, kArgTypeNone, NULL, 0 },
};

static const instrFmt popFmts[] = {
   { "POP{8F /0}",
      kR64 | kM64,
      kArgTypeNone, kArgTypeNone, kArgTypeNone, "w", true },
   { NULL,  kArgTypeNone, kArgTypeNone, kArgTypeNone, kArgTypeNone, NULL, 0 },
};

static const instrFmt subFmts[] = {
   { "SUB{REX.W + 83 /5 ib}",
      kR64 | kM64,
      kI8,
      kArgTypeNone, kArgTypeNone, "br", false },
   { NULL,  kArgTypeNone, kArgTypeNone, kArgTypeNone, kArgTypeNone, NULL, 0 },
};

static const instrFmt addFmts[] = {
   { "ADD{REX.W + 81 /0 id}",
      kR64 | kM64,
      kI32,
      kArgTypeNone, kArgTypeNone, "br", false },
   { "ADD{REX.W + 83 /0 ib}",
      kR64 | kM64,
      kI8,
      kArgTypeNone, kArgTypeNone, "br", false },
   { "ADD{REX.W + 01 /r}",
      kR64 | kM64,
      kR64,
      kArgTypeNone, kArgTypeNone, "br", false },
   { NULL,  kArgTypeNone, kArgTypeNone, kArgTypeNone, kArgTypeNone, NULL, 0 },
};

static const instrFmt xorFmts[] = {
   { "XOR{REX.W + 31 /r}",
      kR64 | kM64,
      kR64,
      kArgTypeNone, kArgTypeNone, "br", false },
   { NULL,  kArgTypeNone, kArgTypeNone, kArgTypeNone, kArgTypeNone, NULL, 0 },
};

static const instrFmt movFmts[] = {
   { "MOV{REX.W + 89 /r}",
      kR64 | kM64,
      kR64,
      kArgTypeNone, kArgTypeNone, "wr", false },
   { "MOV{REX.W + 8B /r}",
      kR64,
      kR64 | kM64,
      kArgTypeNone, kArgTypeNone, "wr", false },
   { "MOV{REX.W + B8+ rd io}",
      kR64,
      kI64,
      kArgTypeNone, kArgTypeNone, "wr", false },
   { "MOV{REX.W + C7 /0 id}",
      kR64 | kM64,
      kI32,
      kArgTypeNone, kArgTypeNone, "wr", false },
   { NULL,  kArgTypeNone, kArgTypeNone, kArgTypeNone, kArgTypeNone, NULL, 0 },
};

static const instrFmt leaFmts[] = {
   { "LEA{REX.W + 8D /r}",
      kR64,
      kM64,
      kArgTypeNone, kArgTypeNone, "wr", false },
   { NULL,  kArgTypeNone, kArgTypeNone, kArgTypeNone, kArgTypeNone, NULL, 0 },
};

static const instrFmt callFmts[] = {
   { "CALL{E8 cd}",
      kI32,
      kArgTypeNone, kArgTypeNone, kArgTypeNone, "r", false },
   { "CALL{FF /2}",
      kR64 | kM64,
      kArgTypeNone, kArgTypeNone, kArgTypeNone, "r", false },
   { NULL,  kArgTypeNone, kArgTypeNone, kArgTypeNone, kArgTypeNone, NULL, 0 },
};

static const instrFmt retFmts[] = {
   { "RET{C3}",
      kArgTypeNone, kArgTypeNone, kArgTypeNone, kArgTypeNone, NULL, 0 },
   { NULL,  kArgTypeNone, kArgTypeNone, kArgTypeNone, kArgTypeNone, NULL, 0 },
};

static const instrFmt cmpFmts[] = {
   { "CMP{REX.W + 81 /7 id}",
      kR64 | kM64,
      kI32,
      kArgTypeNone, kArgTypeNone, NULL, 0 },
   { "CMP{REX.W + 3B /r}",
      kR64,
      kR64 | kM64,
      kArgTypeNone, kArgTypeNone, NULL, 0 },
   { NULL,  kArgTypeNone, kArgTypeNone, kArgTypeNone, kArgTypeNone, NULL, 0 },
};

static const instrFmt sltsFmts[] = {
   { "SETL{REX + 0F 9C}",
      kR64,
      kArgTypeNone, kArgTypeNone, kArgTypeNone, NULL, 0 },
   { NULL,  kArgTypeNone, kArgTypeNone, kArgTypeNone, kArgTypeNone, NULL, 0 },
};

static const instrFmt setFmts[] = {
   { "SETE{REX + 0F 94}",
      kR64,
      kArgTypeNone, kArgTypeNone, kArgTypeNone, NULL, 0 },
   { NULL,  kArgTypeNone, kArgTypeNone, kArgTypeNone, kArgTypeNone, NULL, 0 },
};

static const instrFmt jumpEqualFmts[] = {
   { "JE{0F 84 cd}",
      kI32, kArgTypeNone, kArgTypeNone, kArgTypeNone, NULL, 0 },
   { NULL,  kArgTypeNone, kArgTypeNone, kArgTypeNone, kArgTypeNone, NULL, 0 },
};

static const instrFmt gotoFmts[] = {
   { "JMP{E9 cd}",
      kI32, kArgTypeNone, kArgTypeNone, kArgTypeNone, NULL, 0 },
   { NULL,  kArgTypeNone, kArgTypeNone, kArgTypeNone, kArgTypeNone, NULL, 0 },
};

static const instrInfo kInstrs[] = {
   { "<selectSegment>",   NULL, false, NULL     },
   { "<enterFunc>",       NULL, false, NULL     },
   { "<exitFunc>",        NULL, false, NULL     },
   { "<label>",           NULL, false, NULL     },

   { "<reserveLocal>",    NULL, false, NULL     },
   { "<unreserveLocal>",  NULL, false, NULL     },

   { "<globalConstData>", NULL, false, NULL     },

   { "push",              "r",  true,  (const instrFmt*)&pushFmts },
   { "pop",               "w",  true,  (const instrFmt*)&popFmts },

   { "sub",               "br", false, (const instrFmt*)&subFmts },
   { "add",               "br", false, (const instrFmt*)&addFmts },
   { "xor",               "br", false, (const instrFmt*)&xorFmts },

   { "mov",               "wr", false, (const instrFmt*)&movFmts },
   { "lea",               "wr", false, (const instrFmt*)&leaFmts },

   { "<precall>",         NULL, false, NULL     },
   { "call",              "x",  true,  (const instrFmt*)&callFmts },
   { "<postcall>",        NULL, false, NULL     },

   { "ret",               "r",  true, (const instrFmt*)&retFmts },

   { "<split>",           NULL, false, NULL     },

   { "<isLessThan?>",     NULL, false, NULL     },
   { "<isEqualTo?>",      NULL, false, NULL     },
   { "cmp",               "rr", false, (const instrFmt*)&cmpFmts },
   { "setlts",            "w",  false, (const instrFmt*)&sltsFmts },
   { "setet",             "w",  false, (const instrFmt*)&setFmts },

   { "<ifFalse>",         NULL, false, NULL     },
   { "je",                "rx", false, (const instrFmt*)&jumpEqualFmts },
   { "goto",              "x",  false, (const instrFmt*)&gotoFmts },

   { "system",            "x",  true,  NULL     },
};

void x8664Processor::createRegisterMap(std::map<size_t,size_t>& m) const
{
   m[i64::kRegA];
   m[i64::kRegB];
   m[i64::kRegC];
   m[i64::kRegD];

   m[i64::kRegBP];
   m[i64::kRegSP];

   m[i64::kRegSI];
   m[i64::kRegDI];

   m[i64::kReg8];
   m[i64::kReg9];
   m[i64::kReg10];
   m[i64::kReg11];
   m[i64::kReg12];
   m[i64::kReg13];
   m[i64::kReg14];
   m[i64::kReg15];
}

const char *x8664Processor::getRegName(size_t r) const
{
   switch(r)
   {
      case i64::kRegA:
         return "rax";
         break;
      case i64::kRegB:
         return "rbx";
         break;
      case i64::kRegC:
         return "rcx";
         break;
      case i64::kRegD:
         return "rdx";
         break;
      case kStorageStackFramePtr:
      case i64::kRegBP:
         return "rbp";
         break;
      case kStorageStackPtr:
      case i64::kRegSP:
         return "rsp";
         break;
      case i64::kRegSI:
         return "rsi";
         break;
      case i64::kRegDI:
         return "rdi";
         break;
      case i64::kReg8:
         return "r8";
         break;
      case i64::kReg9:
         return "r9";
         break;
      case i64::kReg10:
         return "r10";
         break;
      case i64::kReg11:
         return "r11";
         break;
      case i64::kReg12:
         return "r12";
         break;
      case i64::kReg13:
         return "r13";
         break;
      case i64::kReg14:
         return "r14";
         break;
      case i64::kReg15:
         return "r15";
         break;
      default:
         return iProcessorInfo::getRegName(r);
   }
}

const instrInfo *x8664Processor::getInstr(instrIds i) const
{
   return kInstrs+i;
}

size_t w64CallingConvention::getArgumentStackSpace(std::vector<size_t>& v) const
{
   size_t rVal = 0;
   for(size_t i=0;i<v.size();i++)
   {
      if(i<4)
         continue; // first 4 args are regs
      rVal += v[i];
   }
   return rVal;
}

void w64CallingConvention::getRValAndArgBank(std::vector<size_t>& v) const
{
   v.reserve(5);
   v.push_back(i64::kRegA);
   v.push_back(i64::kRegC);
   v.push_back(i64::kRegD);
   v.push_back(i64::kReg8);
   v.push_back(i64::kReg9);
}

bool w64CallingConvention::requiresPrologEpilogSave(size_t r) const
{
   if(r == i64::kRegB) return true;
   else if(r == i64::kRegBP) return true;
   else if(r == i64::kRegDI) return true;
   else if(r == i64::kRegSI) return true;
   else if(r == i64::kRegSP) return true;
   else if(r == i64::kReg12) return true;
   else if(r == i64::kReg13) return true;
   else if(r == i64::kReg14) return true;
   else if(r == i64::kReg15) return true;
   return false;
}

bool w64CallingConvention::requiresSubCallSave(size_t r) const
{
   if(r == i64::kRegA) return true;
   else if(r == i64::kRegC) return true;
   else if(r == i64::kRegD) return true;
   else if(r == i64::kReg8) return true;
   else if(r == i64::kReg9) return true;
   else if(r == i64::kReg10) return true;
   else if(r == i64::kReg11) return true;
   return false;
}

// registers RBX, RBP, RDI, RSI, RSP, R12, R13, R14, R15, are nonvolatile. They must be saved and restored by a function that uses them.

// registers RAX, RCX, RDX, R8, R9, R10, R11, are volatile. Consider volatile registers destroyed on function calls unless otherwise safety-provable by analysis such as whole program optimization.

void w64CallingConvention::createRegisterBankInPreferredOrder(std::vector<size_t>& v) const
{
   // nonvolatile - saved in pro/epilog
   v.push_back(i64::kRegB);
   // RBP
   v.push_back(i64::kRegDI);
   v.push_back(i64::kRegSI);
   // RSP
   v.push_back(i64::kReg12);
   v.push_back(i64::kReg13);
   v.push_back(i64::kReg14);
   v.push_back(i64::kReg15);

   // volatile - saved around subcalls
   // cc cconvention
   v.push_back(i64::kRegA);
   v.push_back(i64::kRegC);
   v.push_back(i64::kRegD);
   v.push_back(i64::kReg8);
   v.push_back(i64::kReg9);
   // scratch
   v.push_back(i64::kReg10);
   v.push_back(i64::kReg11);
}

void w64CallingConvention::createScratchRegisterBank(std::vector<size_t>& v) const
{
   // volatile - saved around subcalls and not in the calling conventions
   v.push_back(i64::kReg10);
   v.push_back(i64::kReg11);
}

const iSyscallConvention& w64EmuTargetInfo::getSyscallConvention() const
{
   cdwTHROW("unimpled");
}

} // namespace tgt
} // namespace cmn
