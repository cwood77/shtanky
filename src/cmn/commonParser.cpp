#include "commonLexor.hpp"
#include "commonParser.hpp"

namespace cmn {

parserBase::parserBase(lexorBase& l)
: m_nFac(l)
{
}

} // namespace cmn

namespace cmn {

commonParser::commonParser(commonLexor& l)
: parserBase(l)
, m_l(l)
{
}

std::unique_ptr<fileNode> commonParser::parseFile()
{
   std::unique_ptr<fileNode> pFile(m_nFac.create<fileNode>());
   parseFile(*pFile.get());
   return pFile;
}

void commonParser::parseFile(fileNode& f)
{
   while(m_l.getToken() != commonLexor::kEOI)
      parseClass(f);
}

void commonParser::parseClass(fileNode& f)
{
   parseAttributes();

   classNode& c = m_nFac.appendNewChild<classNode>(f);
   if(m_l.getToken() == commonLexor::kClass)
      ;
   else if(m_l.getToken() == commonLexor::kInterface)
      c.flags |= nodeFlags::kInterface;
   else if(m_l.getToken() == commonLexor::kAbstract)
      c.flags |= nodeFlags::kAbstract;
   else
      m_l.demandOneOf(3,commonLexor::kClass,commonLexor::kInterface,commonLexor::kAbstract);
   m_l.advance();

   m_l.demand(commonLexor::kName);
   c.name = m_l.getLexeme();
   m_l.advance();

   parseClassBases(c);

   m_l.demandAndEat(commonLexor::kLBrace);

   parseClassMembers(c);

   m_l.demandAndEat(commonLexor::kRBrace);
}

void commonParser::parseClassBases(classNode& c)
{
   // at most 1 for now
   if(m_l.getToken() != commonLexor::kLBrace)
   {
      m_l.demandAndEat(commonLexor::kColon);

      m_l.demand(commonLexor::kName);
      c.baseClasses.push_back(link<classNode>());
      c.baseClasses.back().ref = m_l.getLexeme();
      m_l.advance();
   }
}

void commonParser::parseClassMembers(classNode& c)
{
   size_t flags = 0;
   parseMemberKeywords(flags);

   m_l.demand(commonLexor::kName);
   auto name = m_l.getLexeme();
   m_l.advance();

   if(m_l.getToken() == commonLexor::kLParen)
   {
      auto& m = m_nFac.appendNewChild<methodNode>(c);
      m.flags = flags;
      m.name = name;
      m.baseImpl.ref = name;
      parseMethod(m);
   }
   else if(m_l.getToken() == commonLexor::kColon)
   {
      auto& m = m_nFac.appendNewChild<fieldNode>(c);
      m.flags = flags;
      m.name = name;
      parseField(m);
   }
   else
      m_l.demandOneOf(2,commonLexor::kLParen,commonLexor::kColon);

   if(m_l.getToken() != commonLexor::kRBrace)
      parseClassMembers(c);
}

void commonParser::parseMemberKeywords(size_t& flags)
{
   if(m_l.getToken() == commonLexor::kOverride)
      flags |= nodeFlags::kOverride;
   else if(m_l.getToken() == commonLexor::kAbstract)
      flags |= nodeFlags::kAbstract;
   else if(m_l.getToken() == commonLexor::kStatic)
      flags |= nodeFlags::kStatic;
   else if(m_l.getToken() == commonLexor::kPublic)
      flags |= nodeFlags::kPublic;
   else if(m_l.getToken() == commonLexor::kProtected)
      flags |= nodeFlags::kProtected;
   else if(m_l.getToken() == commonLexor::kPrivate)
      flags |= nodeFlags::kPrivate;
   else
      return;

   m_l.advance();
   parseMemberKeywords(flags);
}

void commonParser::parseMethod(methodNode& n)
{
   m_l.demandAndEat(commonLexor::kLParen);

   // only 1 arg so far
   auto& a = m_nFac.appendNewChild<argNode>(n);
   m_l.demand(commonLexor::kName);
   a.name = m_l.getLexeme();
   m_l.advance();

   m_l.demandAndEat(commonLexor::kColon);

   parseType(a);

   m_l.demandAndEat(commonLexor::kRParen);
   m_l.demandAndEat(commonLexor::kColon);

   parseType(n);

   if(m_l.getToken() == commonLexor::kSemiColon)
      m_l.advance(); // decl only
   else
      parseSequence(n);
}

void commonParser::parseField(fieldNode& n)
{
   m_l.demandAndEat(commonLexor::kColon);

   parseType(n);

   if(m_l.getToken() == commonLexor::kEquals)
   {
      m_l.advance();
      parseRValue(n);
      m_l.demandAndEat(commonLexor::kSemiColon);
   }
   else if(m_l.getToken() == commonLexor::kSemiColon)
      m_l.advance();
   else
      m_l.demandOneOf(2,commonLexor::kEquals,commonLexor::kSemiColon);
}

void commonParser::parseSequence(node& owner)
{
   m_l.demandAndEat(commonLexor::kLBrace);

   auto& s = m_nFac.appendNewChild<sequenceNode>(owner);
   parseStatements(s);

   m_l.demandAndEat(commonLexor::kRBrace);
}

void commonParser::parseStatements(node& owner)
{
   while(tryParseStatement(owner));
}

bool commonParser::tryParseStatement(node& owner)
{
   if(m_l.getToken() == commonLexor::kRBrace) return false;

   std::unique_ptr<node> pInst(&parseLValue());
   if(m_l.getToken() == commonLexor::kLParen)
      parseCall(pInst,owner);
   else
      parseInvoke(pInst,owner);

   return true;
}

void commonParser::parseInvoke(std::unique_ptr<node>& inst, node& owner)
{
   m_l.demandAndEat(commonLexor::kArrow);

   m_l.demand(commonLexor::kName);
   auto name = m_l.getLexeme();
   m_l.advance();

   m_l.demandAndEat(commonLexor::kLParen);

   auto& i = m_nFac.appendNewChild<invokeNode>(owner);
   i.proto.ref = name;
   i.appendChild(*inst.release());

   parsePassedArgList(owner);

   m_l.demandAndEat(commonLexor::kRParen);

   m_l.demandAndEat(commonLexor::kSemiColon);
}

void commonParser::parseCall(std::unique_ptr<node>& inst, node& owner)
{
   m_l.demandAndEat(commonLexor::kLParen);

   varRefNode& func = dynamic_cast<varRefNode&>(*inst.get());

   auto& c = m_nFac.appendNewChild<callNode>(owner);
   c.name = func.pDef.ref;

   parsePassedArgList(owner);

   m_l.demandAndEat(commonLexor::kRParen);

   m_l.demandAndEat(commonLexor::kSemiColon);
}

void commonParser::parsePassedArgList(node& owner)
{
   while(m_l.getToken() != commonLexor::kRParen)
   {
      parseRValue(owner);

      if(m_l.getToken() == commonLexor::kComma)
         m_l.advance();
   }
}

node& commonParser::parseLValue()
{
   m_l.demand(commonLexor::kName);
   auto name = m_l.getLexeme();
   m_l.advance();

   std::unique_ptr<varRefNode> pInst(m_nFac.create<varRefNode>());
   pInst->pDef.ref = name;
   return *pInst.release();
}

void commonParser::parseRValue(node& owner)
{
   if(m_l.getToken() == commonLexor::kStringLiteral)
   {
      auto& l = m_nFac.appendNewChild<stringLiteralNode>(owner);
      l.value = m_l.getLexeme();
      m_l.advance();
   }
   else if(m_l.getToken() == commonLexor::kBoolLiteral)
   {
      auto& l = m_nFac.appendNewChild<boolLiteralNode>(owner);
      if(m_l.getLexeme() == "false")
         l.value = false;
      else
         l.value = true;
      m_l.advance();
   }
   else if(m_l.getToken() == commonLexor::kIntLiteral)
   {
      auto& l = m_nFac.appendNewChild<intLiteralNode>(owner);
      l.value = m_l.getLexeme();
      m_l.advance();
   }
   else
      owner.appendChild(parseLValue());
}

void commonParser::parseType(node& owner)
{
   if(m_l.getToken() == commonLexor::kStr)
   {
      auto& t = m_nFac.appendNewChild<strTypeNode>(owner);
      m_l.advance();

      if(m_l.getToken() == commonLexor::kLBracket)
      {
         m_l.advance();
         m_l.demandAndEat(commonLexor::kRBracket);
         m_nFac.appendNewChild<arrayTypeNode>(t);
      }
   }
   else if(m_l.getToken() == commonLexor::kVoid)
   {
      m_nFac.appendNewChild<voidTypeNode>(owner);
      m_l.advance();
   }
   else if(m_l.getToken() == commonLexor::kName)
   {
      auto& t = m_nFac.appendNewChild<userTypeNode>(owner);
      t.pDef.ref = m_l.getLexeme();
      m_l.advance();
   }
   else
      m_l.demandOneOf(3,commonLexor::kStr,commonLexor::kVoid,commonLexor::kName);
}

void commonParser::parseAttributes()
{
   while(m_l.getToken() == commonLexor::kLBracket)
   {
      m_l.advance();

      m_l.demand(commonLexor::kName);
      m_nFac.deferAttribute(m_l.getLexeme());
      m_l.advance();

      m_l.demandAndEat(commonLexor::kRBracket);
   }
}

} // namespace cmn
