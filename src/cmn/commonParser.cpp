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
   {
      parseAttributes();

      m_l.demandOneOf(5,
         commonLexor::kClass,
         commonLexor::kInterface,
         commonLexor::kAbstract,
         commonLexor::kFunc,
         commonLexor::kRef);

      if(m_l.getToken() == commonLexor::kFunc)
         parseGlobalFunc(f);
      else if(m_l.getToken() == commonLexor::kRef)
      {
         m_l.advance();

         m_l.demand(commonLexor::kStringLiteral);
         auto& r = m_nFac.appendNewChild<fileRefNode>(f);
         r.ref = m_l.getLexeme();
         m_l.advance();

         m_l.demandAndEat(commonLexor::kSemiColon);
      }
      else
         parseClass(f);
   }
}

void commonParser::parseClass(fileNode& f)
{
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
      parseMethodOrGlobalFuncFromOpenParen(m);
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

// TODO !!!!! lame; nearly identical to methods
void commonParser::parseGlobalFunc(fileNode& f)
{
   m_l.demandAndEat(commonLexor::kFunc);

   auto& n = m_nFac.appendNewChild<funcNode>(f);

   m_l.demand(commonLexor::kName);
   n.name = m_l.getLexeme();
   m_l.advance();

   parseMethodOrGlobalFuncFromOpenParen(n);
}

void commonParser::parseMethodOrGlobalFuncFromOpenParen(node& n)
{
   m_l.demandAndEat(commonLexor::kLParen);

   parseMethodOrGlobalFuncFromAfterOpenParen(n);
}

void commonParser::parseMethodOrGlobalFuncFromAfterOpenParen(node& n)
{
   parseDecledArgList(n);

   m_l.demandAndEat(commonLexor::kRParen);
   m_l.demandAndEat(commonLexor::kColon);

   parseType(n);

   if(m_l.getToken() == commonLexor::kSemiColon)
      m_l.advance(); // decl only
   else
      parseSequence(n);
}

void commonParser::parseDecledArgList(node& owner)
{
   while(m_l.getToken() != commonLexor::kRParen)
   {
      auto& a = m_nFac.appendNewChild<argNode>(owner);
      m_l.demand(commonLexor::kName);
      a.name = m_l.getLexeme();
      m_l.advance();

      m_l.demandAndEat(commonLexor::kColon);

      parseType(a);

      if(m_l.getToken() == commonLexor::kComma)
         m_l.advance();
   }
}

void commonParser::parseSequence(node& owner)
{
   m_l.demandAndEat(commonLexor::kLBrace);

   auto& s = m_nFac.appendNewChild<sequenceNode>(owner);
   parseStatements(s);

   if(m_l.getToken() == commonLexor::kLBrace)
      parseSequence(owner);

   m_l.demandAndEat(commonLexor::kRBrace);
}

void commonParser::parseStatements(node& owner)
{
   while(tryParseStatement(owner));
}

bool commonParser::tryParseStatement(node& owner)
{
   if(m_l.getToken() == commonLexor::kLBrace) return false;
   if(m_l.getToken() == commonLexor::kRBrace) return false;

   std::unique_ptr<node> pInst(&parseLValue());
   if(m_l.getToken() == commonLexor::kArrow)
      parseInvoke(pInst,owner);
   else if(m_l.getToken() == commonLexor::kArrowParen)
   {
      m_l.advance();

      auto& i = m_nFac.appendNewChild<cmn::invokeFuncPtrNode>(owner);
      i.appendChild(*pInst.release());

      parsePassedArgList(i);
      m_l.demandAndEat(commonLexor::kRParen);
      m_l.demandAndEat(commonLexor::kSemiColon);
   }
   else if(m_l.getToken() == commonLexor::kLParen)
      parseCall(pInst,owner);
   else if(m_l.getToken() == commonLexor::kEquals)
      parseAssignment(pInst,owner);
   else
      m_l.demandOneOf(4,commonLexor::kArrow,commonLexor::kArrowParen,commonLexor::kLParen,commonLexor::kEquals);

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

   parsePassedArgList(i);

   m_l.demandAndEat(commonLexor::kRParen);

   m_l.demandAndEat(commonLexor::kSemiColon);
}

void commonParser::parseCall(std::unique_ptr<node>& inst, node& owner)
{
   m_l.demandAndEat(commonLexor::kLParen);

   varRefNode& func = dynamic_cast<varRefNode&>(*inst.get());

   auto& c = m_nFac.appendNewChild<callNode>(owner);
   c.name = func.pDef.ref;

   parsePassedArgList(c);

   m_l.demandAndEat(commonLexor::kRParen);

   m_l.demandAndEat(commonLexor::kSemiColon);
}

void commonParser::parseAssignment(std::unique_ptr<node>& inst, node& owner)
{
   m_l.demandAndEat(commonLexor::kEquals);

   auto& a = m_nFac.appendNewChild<assignmentNode>(owner);
   a.appendChild(*inst.release());

   parseRValue(a);

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

   auto pInst = m_nFac.create<varRefNode>();
   pInst->pDef.ref = name;

   return parseLValuePrime(*pInst);
}

node& commonParser::parseLValuePrime(node& n)
{
   if(m_l.getToken() == commonLexor::kColon)
   {
      m_l.advance();

      auto i = m_nFac.create<fieldAccessNode>();
      i->appendChild(n);

      m_l.demand(commonLexor::kName);
      i->name = m_l.getLexeme();
      m_l.advance();

      return parseLValuePrime(*i);
   }
#if 0
   else if(m_l.getToken() == commonLexor::kArrowParen)
   {
      m_l.advance();

      auto c = m_nFac.create<invokeFuncPtrNode>();
      c->appendChild(n);

      parsePassedArgList(*c);
      m_l.demandAndEat(commonLexor::kRParen);
      m_l.demandAndEat(commonLexor::kSemiColon);
      //parseMethodOrGlobalFuncFromAfterOpenParen(*c);
      return *c;
   }
#endif

   return n;
}

void commonParser::parseRValue(node& owner, node *pExprRoot)
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

   if(m_l.getTokenClass() & commonLexor::kClassBop)
      parseBop(owner,pExprRoot);
}

void commonParser::parseBop(node& owner, node *pExprRoot)
{
   std::unique_ptr<bopNode> pOp(m_nFac.create<bopNode>());
   pOp->op = m_l.getLexeme();
   m_l.advance();

   // inject an op node between owner and owner's most recent kid
   pExprRoot->lastChild()->injectAbove(*pOp);
   auto nOp = pOp.release();

   parseRValue(*nOp,pExprRoot);
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
   else if(m_l.getToken() == commonLexor::kPtr)
   {
      m_nFac.appendNewChild<ptrTypeNode>(owner);
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
