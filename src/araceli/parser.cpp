#include "lexor.hpp"
#include "parser.hpp"

namespace cmn {

parserBase::parserBase(lexorBase& l)
: m_nFac(l)
{
}

} // namespace cmn

namespace araceli {

parser::parser(lexor& l)
: parserBase(l)
, m_l(l)
{
}

std::unique_ptr<fileNode> parser::parseFile()
{
   std::unique_ptr<fileNode> pFile(m_nFac.create<fileNode>());
   parseFile(*pFile.get());
   return pFile;
}

void parser::parseFile(fileNode& f)
{
   if(m_l.getToken() == lexor::kEOI)
      return;

   parseClass(f);
}

void parser::parseClass(fileNode& f)
{
   parseAttributes();

   m_l.demandAndEat(lexor::kClass);
   classNode& c = m_nFac.appendNewChild<classNode>(f);

   m_l.demand(lexor::kName);
   c.name = m_l.getLexeme();
   m_l.advance();

   parseClassBases(c);

   m_l.demandAndEat(lexor::kLBrace);

   parseClassMembers(c);
}

void parser::parseClassBases(classNode& c)
{
   // only 1 for now
   if(m_l.getToken() != lexor::kLBrace)
   {
      m_l.demandAndEat(lexor::kColon);

      m_l.demand(lexor::kName);
      c.baseClasses.push_back(link<classNode>());
      c.baseClasses.back().ref = m_l.getLexeme();
      m_l.advance();
   }
}

void parser::parseAttributes()
{
   while(m_l.getToken() == lexor::kLBracket)
   {
      m_l.advance();

      m_l.demand(lexor::kName);
      m_nFac.deferAttribute(m_l.getLexeme());
      m_l.advance();

      m_l.demandAndEat(lexor::kRBracket);
   }
}

void parser::parseClassMembers(classNode& c)
{
}

} // namespace araceli
