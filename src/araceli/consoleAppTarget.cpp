#include "../cmn/ast.hpp"
#include "../cmn/out.hpp"
#include "../cmn/pathUtil.hpp"
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
   // there are generally two classes of magical prims in shtanky
   // (1) oscall, which is a hookable syscall, along with various
   //     wrappers for its different functions, and
   // (2) prims, which are core syslib routines implemented in assembly,
   //     and aren't oscall callbacks
   //
   // examples
   // - strings
   // - arrays
   // - oscall: memory alloc
   // - oscall: print, getkey, exit, version(?), file I/O

   cmn::treeWriter(root)
      .append<cmn::intrinsicNode>([](auto& i){ i.name = "._osCall"; })
         .append<cmn::argNode>([](auto& a){ a.name = "code"; })
            .append<cmn::intTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::argNode>([](auto& a){ a.name = "payload"; })
            .append<cmn::ptrTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::ptrTypeNode>()
            .backTo<cmn::araceliProjectNode>()
      .append<cmn::intrinsicNode>([](auto& i){ i.name = "._print"; })
         .append<cmn::argNode>([](auto& a){ a.name = "text"; })
            .append<cmn::strTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::voidTypeNode>()
            .backTo<cmn::araceliProjectNode>()

      .append<cmn::intrinsicNode>([](auto& i){ i.name = "._getflg"; })
         .append<cmn::argNode>([](auto& a){ a.name = "offset"; })
            .append<cmn::intTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::intTypeNode>()
            .backTo<cmn::araceliProjectNode>()

      .append<cmn::intrinsicNode>([](auto& i){ i.name = "._strld"; })
         .append<cmn::argNode>([](auto& a){ a.name = "litoff"; })
            .append<cmn::ptrTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::ptrTypeNode>()
            .backTo<cmn::araceliProjectNode>()
      .append<cmn::intrinsicNode>([](auto& i){ i.name = "._strlen"; })
         .append<cmn::argNode>([](auto& a){ a.name = "s"; })
            .append<cmn::ptrTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::intTypeNode>()
            .backTo<cmn::araceliProjectNode>()
      .append<cmn::intrinsicNode>([](auto& i){ i.name = "._strgidx"; })
         .append<cmn::argNode>([](auto& a){ a.name = "s"; })
            .append<cmn::ptrTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::argNode>([](auto& a){ a.name = "i"; })
            .append<cmn::intTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::intTypeNode>()
            .backTo<cmn::araceliProjectNode>()
      .append<cmn::intrinsicNode>([](auto& i){ i.name = "._strsidx"; })
         .append<cmn::argNode>([](auto& a){ a.name = "s"; })
            .append<cmn::ptrTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::argNode>([](auto& a){ a.name = "i"; })
            .append<cmn::intTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::argNode>([](auto& a){ a.name = "c"; })
            .append<cmn::intTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::voidTypeNode>()
            .backTo<cmn::araceliProjectNode>()

      .append<cmn::intrinsicNode>([](auto& i){ i.name = "._arrresize"; })
         .append<cmn::argNode>([](auto& a){ a.name = "arr"; })
            .append<cmn::ptrTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::argNode>([](auto& a){ a.name = "oldSize"; })
            .append<cmn::intTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::argNode>([](auto& a){ a.name = "newSize"; })
            .append<cmn::intTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::ptrTypeNode>()
            .backTo<cmn::araceliProjectNode>()
      .append<cmn::intrinsicNode>([](auto& i){ i.name = "._arrgidx"; })
         .append<cmn::argNode>([](auto& a){ a.name = "arr"; })
            .append<cmn::ptrTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::argNode>([](auto& a){ a.name = "offset"; })
            .append<cmn::intTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::ptrTypeNode>()
            .backTo<cmn::araceliProjectNode>()
      .append<cmn::intrinsicNode>([](auto& i){ i.name = "._arrsidx"; })
         .append<cmn::argNode>([](auto& a){ a.name = "arr"; })
            .append<cmn::ptrTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::argNode>([](auto& a){ a.name = "offset"; })
            .append<cmn::intTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::argNode>([](auto& a){ a.name = "value"; })
            .append<cmn::ptrTypeNode>()
               .backTo<cmn::intrinsicNode>()
         .append<cmn::voidTypeNode>()
            .backTo<cmn::araceliProjectNode>()

   ;
}

void consoleAppTarget::populateInstantiates(cmn::araceliProjectNode& root)
{
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
      << "   static main(args : .sht.core.string[]) : void" << std::endl
      << "   {" << std::endl
   ;

   bool overallWantsStream = false;
   for(auto *pNode : topLevels)
   {
      auto pClass = dynamic_cast<cmn::classNode*>(pNode);
      if(!pClass)
         cdwTHROW("everything marked with [program] must be a class");

      if(wantsStream(*pClass))
         overallWantsStream = true;
   }
   if(overallWantsStream)
   {
      stream.stream()
         << "      var cout : .sht.cons.stdout;" << std::endl
         << std::endl
      ;
   }

   size_t i=0;
   for(auto it=topLevels.begin();it!=topLevels.end();++it,i++)
   {
      auto& rClass = *dynamic_cast<cmn::classNode*>(*it);

      stream.stream()
         << "      var obj" << i << " : "
         << cmn::fullyQualifiedName::build(rClass) << ";" << std::endl
      ;
      if(wantsStream(rClass))
         stream.stream()
            << "      obj" << i << ":_out = cout;" << std::endl
         ;

      stream.stream() << std::endl;
   }

   i=0;
   for(auto it=topLevels.begin();it!=topLevels.end();++it,i++)
      callRunMethod(**it,i,stream);

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
   sourceStream.stream()
      << "ref \""
      << cmn::pathUtil::computeRefPath(
         sourceStream.getFullPath(),
         ".\\testdata\\sht\\prims.lh")
      << "\";" << std::endl;
}

void consoleAppTarget::adjustBatchFileFiles(phase p, std::list<std::string>& files)
{
   if(p == iTarget::kShtasmPhase)
   {
      files.push_back("testdata\\sht\\oscall.asm");
      files.push_back("testdata\\sht\\flags.asm");
      files.push_back("testdata\\sht\\string.asm");
      files.push_back("testdata\\sht\\array.asm");
   }
   else if(p == iTarget::kShlinkPhase)
   {
      files.push_back("testdata\\sht\\oscall.asm.o");
      files.push_back("testdata\\sht\\flags.asm.o");
      files.push_back("testdata\\sht\\string.asm.o");
      files.push_back("testdata\\sht\\array.asm.o");
   }
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

void consoleAppTarget::callRunMethod(cmn::node& obj, size_t instNum, cmn::outStream& s)
{
   cmn::methodNode *pMethod = NULL;
   auto ancestors = dynamic_cast<cmn::classNode&>(obj).computeLineage();
   for(auto it=ancestors.rbegin();it!=ancestors.rend();++it)
   {
      auto hits = (*it)->filterChildren<cmn::methodNode>(
         [](auto& m){ return m.name == "run"; });
      if(hits.size() > 0)
      {
         pMethod = hits[0];
         break;
      }
   }
   if(!pMethod)
      return; // don't generate any call if no 'run' method found

   auto args = pMethod->getChildrenOf<cmn::argNode>();
   if(args.size() == 1 && args[0]->name == "args")
   {
      auto& ty = args[0]->demandSoleChild<cmn::typeNode>();
      auto *pUdTy = dynamic_cast<cmn::userTypeNode*>(&ty);
      if(pUdTy && pUdTy->pDef.ref == ".sht.core.array<.sht.core.string>")
      {
         // it has the right number of args; arg has right name and type, so pass it
         s.stream() << "      obj" << instNum << "->run(args);" << std::endl;
         return;
      }
   }

   if(args.size() == 0)
      // zero-paramed run methods are easy to call
      s.stream() << "      obj" << instNum << "->run();" << std::endl;

   // otherwise it doesn't have a run or I don't know how to call it
}

} // namespace araceli
