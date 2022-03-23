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

std::unique_ptr<cmn::fileNode> parser::parseFile()
{
   std::unique_ptr<cmn::fileNode> pFile(m_nFac.create<cmn::fileNode>());
   parseFile(*pFile.get());
   return pFile;
}

void parser::parseFile(cmn::fileNode& f)
{
   while(m_l.getToken() != lexor::kEOI)
      parseClass(f);
}

void parser::parseClass(cmn::fileNode& f)
{
   parseAttributes();

   cmn::classNode& c = m_nFac.appendNewChild<cmn::classNode>(f);
   if(m_l.getToken() == lexor::kClass)
      ;
   else if(m_l.getToken() == lexor::kInterface)
      c.flags |= cmn::nodeFlags::kInterface;
   else if(m_l.getToken() == lexor::kAbstract)
      c.flags |= cmn::nodeFlags::kAbstract;
   else
      m_l.demandOneOf(3,lexor::kClass,lexor::kInterface,lexor::kAbstract);
   m_l.advance();

   m_l.demand(lexor::kName);
   c.name = m_l.getLexeme();
   m_l.advance();

   parseClassBases(c);

   m_l.demandAndEat(lexor::kLBrace);

   parseClassMembers(c);

   m_l.demandAndEat(lexor::kRBrace);
}

void parser::parseClassBases(cmn::classNode& c)
{
   // at most 1 for now
   if(m_l.getToken() != lexor::kLBrace)
   {
      m_l.demandAndEat(lexor::kColon);

      m_l.demand(lexor::kName);
      c.baseClasses.push_back(cmn::link<cmn::classNode>());
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

void parser::parseClassMembers(cmn::classNode& c)
{
   size_t flags = 0;
   parseMemberKeywords(flags);

   m_l.demand(lexor::kName);
   auto name = m_l.getLexeme();
   m_l.advance();

   if(m_l.getToken() == lexor::kLParen)
   {
      auto& m = m_nFac.appendNewChild<cmn::methodNode>(c);
      m.flags = flags;
      m.name = name;
      m.baseImpl.ref = name;
      parseMethod(m);
   }
   else if(m_l.getToken() == lexor::kColon)
   {
      auto& m = m_nFac.appendNewChild<cmn::fieldNode>(c);
      m.flags = flags;
      m.name = name;
      parseField(m);
   }
   else
      m_l.demandOneOf(2,lexor::kLParen,lexor::kColon);

   if(m_l.getToken() != lexor::kRBrace)
      parseClassMembers(c);
}

void parser::parseMemberKeywords(size_t& flags)
{
   if(m_l.getToken() == lexor::kOverride)
      flags |= cmn::nodeFlags::kOverride;
   else if(m_l.getToken() == lexor::kAbstract)
      flags |= cmn::nodeFlags::kAbstract;
   else if(m_l.getToken() == lexor::kStatic)
      flags |= cmn::nodeFlags::kStatic;
   else if(m_l.getToken() == lexor::kPublic)
      flags |= cmn::nodeFlags::kPublic;
   else if(m_l.getToken() == lexor::kProtected)
      flags |= cmn::nodeFlags::kProtected;
   else if(m_l.getToken() == lexor::kPrivate)
      flags |= cmn::nodeFlags::kPrivate;
   else
      return;

   m_l.advance();
   parseMemberKeywords(flags);
}

void parser::parseMethod(cmn::methodNode& n)
{
   m_l.demandAndEat(lexor::kLParen);

   // only 1 arg so far
   auto& a = m_nFac.appendNewChild<cmn::argNode>(n);
   m_l.demand(lexor::kName);
   a.name = m_l.getLexeme();
   m_l.advance();

   m_l.demandAndEat(lexor::kColon);

   parseType(a);

   m_l.demandAndEat(lexor::kRParen);
   m_l.demandAndEat(lexor::kColon);

   parseType(n);

   if(m_l.getToken() == lexor::kSemiColon)
      m_l.advance(); // decl only
   else
      parseSequence(n);
}

void parser::parseField(cmn::fieldNode& n)
{
   m_l.demandAndEat(lexor::kColon);

   parseType(n);

   if(m_l.getToken() == lexor::kEquals)
   {
      m_l.advance();
      parseRValue(n);
      m_l.demandAndEat(lexor::kSemiColon);
   }
   else if(m_l.getToken() == lexor::kSemiColon)
      m_l.advance();
   else
      m_l.demandOneOf(2,lexor::kEquals,lexor::kSemiColon);
}

void parser::parseSequence(cmn::node& owner)
{
   m_l.demandAndEat(lexor::kLBrace);

   auto& s = m_nFac.appendNewChild<cmn::sequenceNode>(owner);
   parseStatements(s);

   m_l.demandAndEat(lexor::kRBrace);
}

void parser::parseStatements(cmn::node& owner)
{
   while(tryParseStatement(owner));
}

bool parser::tryParseStatement(cmn::node& owner)
{
   if(m_l.getToken() == lexor::kRBrace) return false;

   std::unique_ptr<cmn::node> pInst(&parseLValue());
   if(m_l.getToken() == lexor::kLParen)
      parseCall(pInst,owner);
   else
      parseInvoke(pInst,owner);

   return true;
}

void parser::parseInvoke(std::unique_ptr<cmn::node>& inst, cmn::node& owner)
{
   m_l.demandAndEat(lexor::kArrow);

   m_l.demand(lexor::kName);
   auto name = m_l.getLexeme();
   m_l.advance();

   m_l.demandAndEat(lexor::kLParen);

   auto& i = m_nFac.appendNewChild<cmn::invokeNode>(owner);
   i.proto.ref = name;
   i.appendChild(*inst.release());

   parsePassedArgList(owner);

   m_l.demandAndEat(lexor::kRParen);

   m_l.demandAndEat(lexor::kSemiColon);
}

void parser::parseCall(std::unique_ptr<cmn::node>& inst, cmn::node& owner)
{
   m_l.demandAndEat(lexor::kLParen);

   cmn::varRefNode& func = dynamic_cast<cmn::varRefNode&>(*inst.get());

   auto& c = m_nFac.appendNewChild<cmn::callNode>(owner);
   c.name = func.pDef.ref;

   parsePassedArgList(owner);

   m_l.demandAndEat(lexor::kRParen);

   m_l.demandAndEat(lexor::kSemiColon);
}

void parser::parsePassedArgList(cmn::node& owner)
{
   while(m_l.getToken() != lexor::kRParen)
   {
      parseRValue(owner);

      if(m_l.getToken() == lexor::kComma)
         m_l.advance();
   }
}

cmn::node& parser::parseLValue()
{
   m_l.demand(lexor::kName);
   auto name = m_l.getLexeme();
   m_l.advance();

   std::unique_ptr<cmn::varRefNode> pInst(m_nFac.create<cmn::varRefNode>());
   pInst->pDef.ref = name;
   return *pInst.release();
}

void parser::parseRValue(cmn::node& owner)
{
   if(m_l.getToken() == lexor::kStringLiteral)
   {
      auto& l = m_nFac.appendNewChild<cmn::stringLiteralNode>(owner);
      l.value = m_l.getLexeme();
      m_l.advance();
   }
   else if(m_l.getToken() == lexor::kBoolLiteral)
   {
      auto& l = m_nFac.appendNewChild<cmn::boolLiteralNode>(owner);
      if(m_l.getLexeme() == "false")
         l.value = false;
      else
         l.value = true;
      m_l.advance();
   }
   else if(m_l.getToken() == lexor::kIntLiteral)
   {
      auto& l = m_nFac.appendNewChild<cmn::intLiteralNode>(owner);
      l.value = m_l.getLexeme();
      m_l.advance();
   }
   else
      owner.appendChild(parseLValue());
}

void parser::parseType(cmn::node& owner)
{
   if(m_l.getToken() == lexor::kStr)
   {
      auto& t = m_nFac.appendNewChild<cmn::strTypeNode>(owner);
      m_l.advance();

      if(m_l.getToken() == lexor::kLBracket)
      {
         m_l.advance();
         m_l.demandAndEat(lexor::kRBracket);
         m_nFac.appendNewChild<cmn::arrayTypeNode>(t);
      }
   }
   else if(m_l.getToken() == lexor::kVoid)
   {
      m_nFac.appendNewChild<cmn::voidTypeNode>(owner);
      m_l.advance();
   }
   else if(m_l.getToken() == lexor::kName)
   {
      auto& t = m_nFac.appendNewChild<cmn::userTypeNode>(owner);
      t.pDef.ref = m_l.getLexeme();
      m_l.advance();
   }
   else
      m_l.demandOneOf(3,lexor::kStr,lexor::kVoid,lexor::kName);
}

} // namespace araceli
