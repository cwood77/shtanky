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
   while(m_l.getToken() != lexor::kEOI)
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

   m_l.demandAndEat(lexor::kRBrace);
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
   bool o = false;
   memberNode::accessTypes at = memberNode::kPrivate;
   parseMemberKeywords(o,at);

   m_l.demand(lexor::kName);
   auto name = m_l.getLexeme();
   m_l.advance();

   if(m_l.getToken() == lexor::kLParen)
   {
      auto& m = m_nFac.appendNewChild<methodNode>(c);
      m.access = at;
      m.name = name;
      m.isOverride = o;
      parseMethod(m);
   }
   else if(m_l.getToken() == lexor::kColon)
   {
      auto& m = m_nFac.appendNewChild<fieldNode>(c);
      m.access = at;
      m.name = name;
      parseField(m);
   }
   else
      m_l.demandOneOf(2,lexor::kLParen,lexor::kColon);

   if(m_l.getToken() != lexor::kRBrace)
      parseClassMembers(c);
}

void parser::parseMemberKeywords(bool& o, memberNode::accessTypes& at)
{
   if(m_l.getToken() == lexor::kOverride)
      o = true;
   else if(m_l.getToken() == lexor::kAbstract)
      ; // TODO unimpled
   else if(m_l.getToken() == lexor::kPublic)
      at = memberNode::kPublic;
   else if(m_l.getToken() == lexor::kProtected)
      at = memberNode::kProtected;
   else if(m_l.getToken() == lexor::kPrivate)
      at = memberNode::kPrivate;
   else
      return;

   m_l.advance();
   parseMemberKeywords(o,at);
}

void parser::parseMethod(methodNode& n)
{
   m_l.demandAndEat(lexor::kLParen);

   // only 1 arg so far
   auto& a = m_nFac.appendNewChild<argNode>(n);
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

void parser::parseField(fieldNode& n)
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

   auto& s = m_nFac.appendNewChild<sequenceNode>(owner);
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

   auto& i = m_nFac.appendNewChild<invokeNode>(owner);
   i.name = name;
   i.appendChild(*inst.release());

   parseRValue(i);

   m_l.demandAndEat(lexor::kRParen);

   m_l.demandAndEat(lexor::kSemiColon);
}

cmn::node& parser::parseLValue()
{
   m_l.demand(lexor::kName);
   auto name = m_l.getLexeme();
   m_l.advance();

   std::unique_ptr<varRefNode> pInst(m_nFac.create<varRefNode>());
   pInst->name = name;
   return *pInst.release();
}

void parser::parseRValue(cmn::node& owner)
{
   if(m_l.getToken() == lexor::kStringLiteral)
   {
      auto& l = m_nFac.appendNewChild<stringLiteralNode>(owner);
      l.value = m_l.getLexeme();
      m_l.advance();
   }
   else if(m_l.getToken() == lexor::kBoolLiteral)
   {
      auto& l = m_nFac.appendNewChild<boolLiteralNode>(owner);
      if(m_l.getLexeme() == "false")
         l.value = false;
      else
         l.value = true;
      m_l.advance();
   }
   else
      owner.appendChild(parseLValue());
}

void parser::parseType(cmn::node& owner)
{
   if(m_l.getToken() == lexor::kStr)
   {
      auto& t = m_nFac.appendNewChild<strTypeNode>(owner);
      m_l.advance();

      if(m_l.getToken() == lexor::kLBracket)
      {
         m_l.advance();
         m_l.demandAndEat(lexor::kRBracket);
         m_nFac.appendNewChild<arrayTypeNode>(t);
      }
   }
   else if(m_l.getToken() == lexor::kVoid)
   {
      m_nFac.appendNewChild<voidTypeNode>(owner);
      m_l.advance();
   }
   else if(m_l.getToken() == lexor::kName)
   {
      auto& t = m_nFac.appendNewChild<userTypeNode>(owner);
      t.name = m_l.getLexeme();
      m_l.advance();
   }
   else
      m_l.demandOneOf(3,lexor::kStr,lexor::kVoid,lexor::kName);
}

} // namespace araceli
