#include "../cmn/ast.hpp"
#include "../cmn/out.hpp"
#include "../cmn/throw.hpp"
#include "consoleAppTarget.hpp"
#include "loader.hpp"
#include "metadata.hpp"
#include "projectBuilder.hpp"

namespace araceli {

void consoleAppTarget::addAraceliStandardLibrary(cmn::araceliProjectNode& root)
{
   projectBuilder::addScope(root,".\\testdata\\sht",/*inProject*/false);
}

void consoleAppTarget::populateIntrinsics(cmn::araceliProjectNode& root)
{
   cmn::treeWriter(root)
      .append<cmn::intrinsicNode>([](auto& i){ i.name = "._osCall"; })
         .append<cmn::argNode>([](auto& a){ a.name = "code"; })
            .append<cmn::intTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::argNode>([](auto& a){ a.name = "payload"; })
            .append<cmn::ptrTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::voidTypeNode>()
   ;
}

void consoleAppTarget::araceliCodegen(cmn::araceliProjectNode& root, metadata& md)
{
   std::list<cmn::node*> topLevels;
   md.demandMulti("program",topLevels);
   if(topLevels.size() == 0)
      cdwTHROW("programming is meaningless without [program]");

   auto& scope = findProjectScope(root);
   const std::string fullPath = scope.path + "\\.target.ara";

   cmn::outBundle out;
   auto& stream = out.get<cmn::outStream>(fullPath);

   stream.stream()
      << "class consoleTarget {" << std::endl
      << std::endl
      << "   [entrypoint]" << std::endl
      << "   static main(args : str[]) : void" << std::endl
      << "   {" << std::endl
      << "      var cout : .sht.cons.stdout;" << std::endl
      << std::endl
   ;

   size_t i=0;
   for(auto it=topLevels.begin();it!=topLevels.end();++it,i++)
   {
      auto pClass = dynamic_cast<cmn::classNode*>(*it);
      if(!pClass)
         cdwTHROW("everything marked with [program] must be a class");

      stream.stream()
         << "      var obj" << i << " : "
         << cmn::fullyQualifiedName::build(*pClass) << ";" << std::endl
      ;
      if(wantsStream(*pClass))
         stream.stream()
            << "      obj" << i << ":_out = cout;" << std::endl
         ;

      stream.stream() << std::endl;
   }

   i=0;
   for(auto it=topLevels.begin();it!=topLevels.end();++it,i++)
      stream.stream() << "      obj" << i << "->run(args);" << std::endl;

   stream.stream()
      << "   }" << std::endl
      << std::endl
      << "}" << std::endl
   ;

   cmn::unconditionalWriter wr;
   out.updateDisk(wr);

   loader::loadFile(scope,fullPath);
}

void consoleAppTarget::liamCodegen(cmn::outStream& sourceStream)
{
   sourceStream.stream() << std::endl;
   sourceStream.stream() << "func ._osCall(code : str, payload : str) : void;" << std::endl;
}

void consoleAppTarget::adjustBatchFileFiles(phase p, std::list<std::string>& files)
{
   if(p == iTarget::kShtasmPhase)
      files.push_back("testdata\\sht\\oscall.asm");
   else if(p == iTarget::kShlinkPhase)
      files.push_back("testdata\\sht\\oscall.asm.o");
}

cmn::scopeNode& consoleAppTarget::findProjectScope(cmn::araceliProjectNode& root)
{
   auto scopes = root.getChildrenOf<cmn::scopeNode>();
   for(auto it=scopes.begin();it!=scopes.end();++it)
      if((*it)->inProject)
         return **it;

   cdwTHROW("ISE");
}

bool consoleAppTarget::wantsStream(cmn::classNode& n)
{
   auto ancestors = n.computeLineage();
   for(auto it=ancestors.begin();it!=ancestors.end();++it)
      if(cmn::fullyQualifiedName::build(**it) == ".sht.cons.program")
         return true;
   return false;
}

} // namespace araceli
