#include "commonLexor.hpp"
#include "commonParser.hpp"
#include "trace.hpp"

namespace cmn {

commonParser::commonParser(commonLexor& l)
: m_nFac(l)
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

      std::string genericExpr;
      if(m_l.getToken() == commonLexor::kGenericTypeExpr)
      {
         genericExpr = m_l.getLexeme();
         m_l.advance();
         m_l.demand(cdwLoc,commonLexor::kClass); // temp
      }

      m_l.demandOneOf(cdwLoc,7,
         commonLexor::kClass,
         commonLexor::kInterface,
         commonLexor::kAbstract,
         commonLexor::kConst,
         commonLexor::kFunc,
         commonLexor::kRef,
         commonLexor::kInstantiate);

      if(m_l.getToken() == commonLexor::kConst)
         parseGlobalConst(f);
      else if(m_l.getToken() == commonLexor::kFunc)
         parseGlobalFunc(f);
      else if(m_l.getToken() == commonLexor::kRef)
      {
         m_l.advance();

         m_l.demand(cdwLoc,commonLexor::kStringLiteral);
         auto& r = m_nFac.appendNewChild<fileRefNode>(f);
         r.ref = m_l.getLexeme();
         m_l.advance();

         m_l.demandAndEat(cdwLoc,commonLexor::kSemiColon);
      }
      else if(m_l.getToken() == commonLexor::kInstantiate)
      {
         m_l.advance();

         m_l.demand(cdwLoc,commonLexor::kGenericTypeExpr);
         auto& i = m_nFac.appendNewChild<instantiateNode>(f);
         i.text = m_l.getLexeme();
         m_l.advance();

         m_l.demandAndEat(cdwLoc,commonLexor::kSemiColon);
      }
      else
         parseClass(f,genericExpr);
   }
}

void commonParser::parseClass(fileNode& f, const std::string& genericTypeExpr)
{
   classNode& c = m_nFac.appendNewChild<classNode>(f);
   if(m_l.getToken() == commonLexor::kClass)
      ;
   else if(m_l.getToken() == commonLexor::kInterface)
      c.flags |= nodeFlags::kInterface;
   else if(m_l.getToken() == commonLexor::kAbstract)
      c.flags |= nodeFlags::kAbstract;
   else
      m_l.demandOneOf(cdwLoc,3,
         commonLexor::kClass,
         commonLexor::kInterface,
         commonLexor::kAbstract);
   m_l.advance();

   if(!genericTypeExpr.empty())
      parseGeneric(c,genericTypeExpr);

   m_l.demandOneOf(cdwLoc,2,commonLexor::kName,commonLexor::kGenericTypeExpr);
   c.name = m_l.getLexeme();
   m_l.advance();

   parseClassBases(c);

   m_l.demandAndEat(cdwLoc,commonLexor::kLBrace);

   if(m_l.getToken() != commonLexor::kRBrace)
      parseClassMembers(c);

   m_l.demandAndEat(cdwLoc,commonLexor::kRBrace);
}

void commonParser::parseClassBases(classNode& c)
{
   // at most 1 for now
   if(m_l.getToken() != commonLexor::kLBrace)
   {
      m_l.demandAndEat(cdwLoc,commonLexor::kColon);

      m_l.demand(cdwLoc,commonLexor::kName);
      c.baseClasses.push_back(link<classNode>());
      c.baseClasses.back().ref = m_l.getLexeme();
      m_l.advance();
   }
}

void commonParser::parseClassMembers(classNode& c)
{
   parseAttributes();

   size_t flags = 0;
   parseMemberKeywords(flags);

   m_l.demand(cdwLoc,commonLexor::kName);
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
      m_l.demandOneOf(cdwLoc,2,commonLexor::kLParen,commonLexor::kColon);

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
   m_l.demandAndEat(cdwLoc,commonLexor::kColon);

   parseType(n);

   if(m_l.getToken() == commonLexor::kEquals)
   {
      m_l.advance();
      parseRValue(n);
      m_l.demandAndEat(cdwLoc,commonLexor::kSemiColon);
   }
   else if(m_l.getToken() == commonLexor::kSemiColon)
      m_l.advance();
   else
      m_l.demandOneOf(cdwLoc,2,commonLexor::kEquals,commonLexor::kSemiColon);
}

void commonParser::parseMethodOrGlobalFuncFromOpenParen(node& n)
{
   m_l.demandAndEat(cdwLoc,commonLexor::kLParen);

   parseMethodOrGlobalFuncFromAfterOpenParen(n);
}

void commonParser::parseMethodOrGlobalFuncFromAfterOpenParen(node& n)
{
   parseDecledArgList(n);

   m_l.demandAndEat(cdwLoc,commonLexor::kRParen);
   m_l.demandAndEat(cdwLoc,commonLexor::kColon);

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
      m_l.demand(cdwLoc,commonLexor::kName);
      a.name = m_l.getLexeme();
      m_l.advance();

      m_l.demandAndEat(cdwLoc,commonLexor::kColon);

      parseType(a);

      if(m_l.getToken() == commonLexor::kComma)
         m_l.advance();
   }
}

void commonParser::parseGlobalConst(fileNode& f)
{
   m_l.demandAndEat(cdwLoc,commonLexor::kConst);

   auto& n = m_nFac.appendNewChild<constNode>(f);

   m_l.demand(cdwLoc,commonLexor::kName);
   n.name = m_l.getLexeme();
   m_l.advance();

   m_l.demandAndEat(cdwLoc,commonLexor::kColon);

   parseType(n);

   m_l.demandAndEat(cdwLoc,commonLexor::kEquals);

   parseRValue(n);

   m_l.demandAndEat(cdwLoc,commonLexor::kSemiColon);
}

void commonParser::parseGlobalFunc(fileNode& f)
{
   m_l.demandAndEat(cdwLoc,commonLexor::kFunc);

   auto& n = m_nFac.appendNewChild<funcNode>(f);

   m_l.demandOneOf(cdwLoc,2,commonLexor::kName,commonLexor::kGenericTypeExpr);
   n.name = m_l.getLexeme();
   m_l.advance();

   parseMethodOrGlobalFuncFromOpenParen(n);
}

void commonParser::parseSequence(node& owner)
{
   m_l.demandAndEat(cdwLoc,commonLexor::kLBrace);

   auto& s = m_nFac.appendNewChild<sequenceNode>(owner);
   parseStatements(s);

   if(m_l.getToken() == commonLexor::kLBrace)
      parseSequence(owner);

   m_l.demandAndEat(cdwLoc,commonLexor::kRBrace);
}

void commonParser::parseStatements(node& owner)
{
   while(tryParseStatement(owner));
}

bool commonParser::tryParseStatement(node& owner)
{
   if(m_l.getToken() == commonLexor::kLBrace) return false;
   if(m_l.getToken() == commonLexor::kRBrace) return false;

   if(m_l.getToken() == commonLexor::kVar)
      parseVar(owner);
   else
   {
      std::unique_ptr<node> pInst(&parseLValue());
      if(m_l.getToken() == commonLexor::kEquals)
         parseAssignment(pInst,owner);
      else
         parseCallAndFriends(pInst,owner,/*require*/true);

      m_l.demandAndEat(cdwLoc,commonLexor::kSemiColon);
   }

   return true;
}

void commonParser::parseVar(node& owner)
{
   m_l.demandAndEat(cdwLoc,commonLexor::kVar);

   m_l.demand(cdwLoc,commonLexor::kName);
   auto& l = m_nFac.appendNewChild<localDeclNode>(owner);
   l.name = m_l.getLexeme();
   m_l.advance();

   if(m_l.getToken() == commonLexor::kColon)
   {
      m_l.advance();
      parseType(l);

      if(m_l.getToken() == commonLexor::kEquals)
      {
         m_l.advance();
         parseRValue(l);
      }
   }
   else if(m_l.getToken() == commonLexor::kEquals)
   {
      m_l.advance();
      parseRValue(l);
   }
   else
      m_l.demandOneOf(cdwLoc,2,commonLexor::kColon,commonLexor::kEquals);

   m_l.demandAndEat(cdwLoc,commonLexor::kSemiColon);
}

bool commonParser::parseCallAndFriends(std::unique_ptr<node>& inst, node& owner, bool require)
{
   if(m_l.getToken() == commonLexor::kArrow)
      parseInvoke(inst,owner);
   else if(m_l.getToken() == commonLexor::kArrowParen)
   {
      m_l.advance();

      auto& i = m_nFac.appendNewChild<cmn::invokeFuncPtrNode>(owner);
      i.appendChild(*inst.release());

      parsePassedArgList(i);
      m_l.demandAndEat(cdwLoc,commonLexor::kRParen);
   }
   else if(m_l.getToken() == commonLexor::kLParen)
      parseCall(inst,owner);
   else if(require)
      m_l.demandOneOf(cdwLoc,4,
         commonLexor::kEquals,
         commonLexor::kArrow,
         commonLexor::kArrowParen,
         commonLexor::kLParen);
   else
      return false;
   return true;
}

void commonParser::parseInvoke(std::unique_ptr<node>& inst, node& owner)
{
   m_l.demandAndEat(cdwLoc,commonLexor::kArrow);

   m_l.demand(cdwLoc,commonLexor::kName);
   auto name = m_l.getLexeme();
   m_l.advance();

   m_l.demandAndEat(cdwLoc,commonLexor::kLParen);

   auto& i = m_nFac.appendNewChild<invokeNode>(owner);
   i.proto.ref = name;
   i.appendChild(*inst.release());

   parsePassedArgList(i);

   m_l.demandAndEat(cdwLoc,commonLexor::kRParen);
}

void commonParser::parseCall(std::unique_ptr<node>& inst, node& owner)
{
   m_l.demandAndEat(cdwLoc,commonLexor::kLParen);

   varRefNode& func = dynamic_cast<varRefNode&>(*inst.get());

   auto& c = m_nFac.appendNewChild<callNode>(owner);
   c.pTarget.ref = func.pSrc.ref;

   parsePassedArgList(c);

   m_l.demandAndEat(cdwLoc,commonLexor::kRParen);
}

void commonParser::parseAssignment(std::unique_ptr<node>& inst, node& owner)
{
   m_l.demandAndEat(cdwLoc,commonLexor::kEquals);

   auto& a = m_nFac.appendNewChild<assignmentNode>(owner);
   a.appendChild(*inst.release());

   parseRValue(a);
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
   m_l.demand(cdwLoc,commonLexor::kName);
   auto name = m_l.getLexeme();
   m_l.advance();

   auto pInst = m_nFac.create<varRefNode>();
   pInst->pSrc.ref = name;

   return parseLValuePrime(*pInst);
}

node& commonParser::parseLValuePrime(node& n)
{
   if(m_l.getToken() == commonLexor::kColon)
   {
      m_l.advance();

      auto i = m_nFac.create<fieldAccessNode>();
      i->appendChild(n);

      m_l.demand(cdwLoc,commonLexor::kName);
      i->name = m_l.getLexeme();
      m_l.advance();

      return parseLValuePrime(*i);
   }
   else if(m_l.getToken() == commonLexor::kLBracket)
   {
      m_l.advance();

      auto i = m_nFac.create<indexNode>();
      i->appendChild(n);

      parseRValue(*i);

      m_l.demandAndEat(cdwLoc,commonLexor::kRBracket);

      return parseLValuePrime(*i);
   }

   return n;
}

void commonParser::parseRValue(node& owner, node *pExprRoot)
{
   if(!parseLiteral(owner))
   {
      std::unique_ptr<node> pInst(&parseLValue());
      if(!parseCallAndFriends(pInst,owner,false))
         owner.appendChild(*pInst.release());
   }

   if(m_l.getTokenClass() & commonLexor::kClassBop)
      parseBop(owner,pExprRoot);
}

bool commonParser::parseLiteral(node& owner)
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
      l.lexeme = m_l.getLexeme();
      m_l.advance();
   }
   else if(m_l.getToken() == commonLexor::kLBrace)
   {
      m_l.advance();

      auto& n = m_nFac.appendNewChild<structLiteralNode>(owner);
      parseStructLiteralPart(n);

      m_l.demandAndEat(cdwLoc,commonLexor::kRBrace);
   }
   else
      return false;

   return true;
}

void commonParser::parseStructLiteralPart(node& owner)
{
   if(m_l.getToken() == commonLexor::kRBrace)
      return;

   if(!parseLiteral(owner))
   {
      m_l.demand(cdwLoc,commonLexor::kName);
      auto name = m_l.getLexeme();
      m_l.advance();

      auto& ref = m_nFac.appendNewChild<varRefNode>(owner);
      ref.pSrc.ref = name;
   }

   if(m_l.getToken() == commonLexor::kComma)
   {
      m_l.advance();
      parseStructLiteralPart(owner);
   }
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
   typeNode *pType = NULL;

   if(m_l.getToken() == commonLexor::kStr)
   {
      pType = &m_nFac.appendNewChild<strTypeNode>(owner);
      m_l.advance();
   }
   else if(m_l.getToken() == commonLexor::kVoid)
   {
      pType = &m_nFac.appendNewChild<voidTypeNode>(owner);
      m_l.advance();
   }
   else if(m_l.getToken() == commonLexor::kPtr)
   {
      pType = &m_nFac.appendNewChild<ptrTypeNode>(owner);
      m_l.advance();
   }
   else if(m_l.getToken() == commonLexor::kBool)
   {
      pType = &m_nFac.appendNewChild<boolTypeNode>(owner);
      m_l.advance();
   }
   else if(m_l.getToken() == commonLexor::kInt)
   {
      pType = &m_nFac.appendNewChild<intTypeNode>(owner);
      m_l.advance();
   }
   else if(m_l.getToken() == commonLexor::kName ||
           m_l.getToken() == commonLexor::kGenericTypeExpr)
   {
      pType = &m_nFac.appendNewChild<userTypeNode>(owner);
      dynamic_cast<userTypeNode*>(pType)->pDef.ref = m_l.getLexeme();
      m_l.advance();
   }
   else
      m_l.demandOneOf(cdwLoc,7,
         commonLexor::kStr,
         commonLexor::kVoid,
         commonLexor::kPtr,
         commonLexor::kBool,
         commonLexor::kInt,
         commonLexor::kName,
         commonLexor::kGenericTypeExpr);

   while(m_l.getToken() == commonLexor::kLBracket)
   {
      m_l.advance();
      m_l.demandAndEat(cdwLoc,commonLexor::kRBracket);
      pType = &m_nFac.appendNewChild<arrayTypeNode>(*pType);
   }
}

void commonParser::parseAttributes()
{
   while(m_l.getToken() == commonLexor::kLBracket)
   {
      std::string attr;
      while(true)
      {
         m_l.advance();

         m_l.demand(cdwLoc,commonLexor::kName);
         attr += m_l.getLexeme();
         m_l.advance();

         if(m_l.getToken() == commonLexor::kColon)
         {
            attr += ":";
            continue;
         }
         else if(m_l.getToken() == commonLexor::kEquals)
         {
            attr += "=";
            continue;
         }

         m_l.demandAndEat(cdwLoc,commonLexor::kRBracket);

         m_nFac.deferAttribute(attr);
         attr = "";
         break;
      }
   }
}

void commonParser::parseGeneric(node& n, const std::string& genericTypeExpr)
{
   std::unique_ptr<genericNode> pNode(new genericNode());

   genericTypeExprLexor gtL(genericTypeExpr.c_str());
   genericTypeExprParser(gtL).parseGeneric(*pNode.get());

   n.injectAbove(*pNode.release());
}

void genericTypeExprParser::parseGeneric(genericNode& n)
{
   m_l.demandAndEat(cdwLoc,genericTypeExprLexor::kGeneric);
   m_l.demandAndEat(cdwLoc,genericTypeExprLexor::kLessThan);

   parseConstraints(n);

   m_l.demandAndEat(cdwLoc,genericTypeExprLexor::kGreaterThan);
}

void genericTypeExprParser::parseConstraints(genericNode& n)
{
   parseConstraint(n);

   if(m_l.getToken() == genericTypeExprLexor::kComma)
   {
      m_l.advance();
      parseConstraints(n);
   }
}

void genericTypeExprParser::parseConstraint(genericNode& n)
{
   std::unique_ptr<constraintNode> pCons(new constraintNode());

   m_l.demand(cdwLoc,genericTypeExprLexor::kName);
   pCons->name = m_l.getLexeme();
   m_l.advance();

   // handle types here eventually

   n.appendChild(*pCons.release());
}

void genericTypeExprParser::parseTypeParamed(std::string& base, std::list<std::string>& args)
{
   m_l.demand(cdwLoc,genericTypeExprLexor::kName);
   base = m_l.getLexeme();
   m_l.advance();

   parseTypeParamArgList(args);

   m_l.demandAndEat(cdwLoc,genericTypeExprLexor::kEOI);
}

void genericTypeExprParser::parseTypeParamArgList(std::list<std::string>& args)
{
   m_l.demandAndEat(cdwLoc,genericTypeExprLexor::kLessThan);

   while(true)
   {
      parseTypeParamArg(args);

      if(m_l.getToken() != genericTypeExprLexor::kComma)
         break;
   }

   m_l.demandAndEat(cdwLoc,genericTypeExprLexor::kGreaterThan);
}

void genericTypeExprParser::parseTypeParamArg(std::list<std::string>& args)
{
   m_l.demandOneOf(cdwLoc,4,
      genericTypeExprLexor::kStr,
      genericTypeExprLexor::kVoid,
      genericTypeExprLexor::kPtr,
      genericTypeExprLexor::kName
   );

   std::string word = m_l.getLexeme();
   bool checkForNestedGeneric = (m_l.getToken() == genericTypeExprLexor::kName);
   m_l.advance();

   if(checkForNestedGeneric && m_l.getToken() == genericTypeExprLexor::kLessThan)
   {
      std::list<std::string> sublist;
      parseTypeParamArgList(sublist);
      std::stringstream stream;
      stream << word << "<";
      for(auto it=sublist.begin();it!=sublist.end();++it)
      {
         if(it!=sublist.begin())
            stream << ",";
         stream << *it;
      }
      stream << ">";
      word = stream.str();
   }

   if(m_l.getToken() == commonLexor::kLBracket)
   {
      m_l.advance();
      m_l.demandAndEat(cdwLoc,commonLexor::kRBracket);
      word += "[]";
   }

   args.push_back(word);
}

} // namespace cmn
