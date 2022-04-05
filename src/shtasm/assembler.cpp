#include "assembler.hpp"
#include "writer.hpp"

namespace shtasm {

void assembler::assemble(const cmn::tgt::i64::genInfo& gi)
{
   m_pGenInfo = &gi;

   if(m_pGenInfo->rex)
      m_pW->write("rex",&m_pGenInfo->rex,1);
   //m_pW->write("opcode",m_pGenInfo->opcode,m_pGenInfo->numOpcodes());
}

} // namespace shtasm
