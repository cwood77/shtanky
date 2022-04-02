#include "../cmn/i64asm.hpp"
#include "../cmn/trace.hpp"
#include "frontend.hpp"

#include "writer.hpp"

namespace shtasm {

#if 0
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

   {
      compositeObjWriter w;
      w.sink(
         *new retailObjWriter(
            *new binFileWriter(".\\testdata\\test\\fake.shtasm.o")));
      w.sink(
         *new listingObjWriter(
            *new binFileWriter(".\\testdata\\test\\fake.shtasm.list")));

      lineWriter l(w);
      l.setLineNumber(0);
      {
         l.write("header","cdwe o fmt",10);
         { unsigned long v = 0; l.write("version",&v,sizeof(v)); }
         l.under().nextPart();
      }
   }

   return 0;
}
