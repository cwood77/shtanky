#include "../cmn/i64asm.hpp"
#include "../cmn/trace.hpp"
#include "frontend.hpp"

namespace shtasm {

#if 0
class iObjLineStream {
public:
   void writeBytes(const std::string& reason, void *p, size_t n);
};

class iObjStream {
public:
   iObjLineStream *create(size_t lineNum);
};

class assembler {
public:
   void sink(iObjStream& o);

   void assemble(const cmn::tgt::i64::genInfo& gi);
   void addArg(const std::string& a);
};
#endif

} // namespace shtasm

int main(int argc, const char *argv[])
{
   using namespace shtasm;

   lexor l(".\\testdata\\test\\fake.shtasm");
   parser p(l);
   while(!p.getLexor().isDone())
   {
      std::string l,c;
      std::vector<std::string> a;
      p.parseLine(l,a,c);
      cdwDEBUG("lable=%s]]\n",l.c_str());
      for(auto it=a.begin();it!=a.end();++it)
         cdwDEBUG("   a=%s]]\n",it->c_str());
      cdwDEBUG("comm=%s]]\n",c.c_str());
   }

   return 0;
}
