#include "ast.hpp"
#include "out.hpp"
#include "trace.hpp"

namespace cmn {

void iNodeVisitor::visitChildren(node& n)
{
   auto v = n.getChildren();
   for(auto it=v.begin();it!=v.end();++it)
      (*it)->acceptVisitor(*this);
}

node::~node()
{
   for(auto it=m_children.begin();it!=m_children.end();++it)
      delete *it;
}

void node::injectAbove(node& n)
{
   m_pParent->appendChild(n);
   n.appendChild(*this);

   n.m_pParent->removeChild(*this);
}

void node::appendChild(node& n)
{
   m_children.push_back(&n);
   n.m_pParent = this;
}

void node::insertChild(size_t i, node& n)
{
   m_children.insert(m_children.begin()+i,&n);
   n.m_pParent = this;
}

void node::insertChildBefore(node& noob, node& antecedent)
{
   for(auto it=m_children.begin();it!=m_children.end();++it)
   {
      if(*it == &antecedent)
      {
         m_children.insert(it,&noob);
         noob.m_pParent = this;
         return;
      }
   }

   cdwTHROW("can't find antecedent in insert");
}

void node::insertChildAfter(node& noob, node& antecedent)
{
   for(auto it=m_children.begin();it!=m_children.end();++it)
   {
      if(*it == &antecedent)
      {
         m_children.insert(it+1,&noob);
         noob.m_pParent = this;
         return;
      }
   }

   cdwTHROW("can't find antecedent in insert");
}

node *node::replaceChild(node& old, node& nu)
{
   int i=0;
   for(auto it=m_children.begin();it!=m_children.end();++it,i++)
   {
      if(*it == &old)
      {
         m_children[i] = &nu;
         nu.m_pParent = this;
         return &old;
      }
   }

   cdwTHROW("can't find child to replace");
}

void node::removeChild(node& n)
{
   for(auto it=m_children.begin();it!=m_children.end();++it)
   {
      if(*it == &n)
      {
         m_children.erase(it);
         break;
      }
   }
}

node *node::lastChild()
{
   if(m_children.size() == 0) return NULL;
   return *(--(m_children.end()));
}

std::list<classNode*> classNode::computeLineage()
{
   std::list<classNode*> l;
   computeLineage(l);
   return l;
}

void classNode::computeLineage(std::list<classNode*>& l)
{
   for(auto it=baseClasses.begin();it!=baseClasses.end();++it)
      it->getRefee()->computeLineage(l);
   l.push_back(this);
}

void diagVisitor::visit(araceliProjectNode& n)
{
   cdwDEBUG("%sproject; target=%s\n",
      getIndent().c_str(),
      n.targetType.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(liamProjectNode& n)
{
   cdwDEBUG("%sproject; path=%s; #included=%lld\n",
      getIndent().c_str(),
      n.sourceFullPath.c_str(),
      n.loadedPaths.size());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(scopeNode& n)
{
   cdwDEBUG("%sscope; path=%s; name=%s; inProject=%d; loaded=%d\n",
      getIndent().c_str(),
      n.path.c_str(),
      n.scopeName.c_str(),
      n.inProject ? 1 : 0,
      n.loaded ? 1 : 0);

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(fileNode& n)
{
   cdwDEBUG("%sfile path:%s\n",
      getIndent().c_str(),
      n.fullPath.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(fileRefNode& n)
{
   cdwDEBUG("%sfileRef path:%s\n",
      getIndent().c_str(),
      n.ref.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(classNode& n)
{
   cdwDEBUG("%sclass; name=%s; #bases=%lld\n",
      getIndent().c_str(),
      n.name.c_str(),
      n.baseClasses.size());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(memberNode& n)
{
   cdwDEBUG("%smember; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(methodNode& n)
{
   cdwDEBUG("%smethod; baseLinked?=%d\n",
      getIndent().c_str(),
      n.baseImpl.getRefee() ? 1 : 0);

   hNodeVisitor::visit(n);
}

void diagVisitor::visit(fieldNode& n)
{
   cdwDEBUG("%sfield\n",
      getIndent().c_str());

   hNodeVisitor::visit(n);
}

void diagVisitor::visit(constNode& n)
{
   cdwDEBUG("%sconst; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(funcNode& n)
{
   cdwDEBUG("%sfunc; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(intrinsicNode& n)
{
   cdwDEBUG("%sintrinsic; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(argNode& n)
{
   cdwDEBUG("%sarg; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(strTypeNode& n)
{
   cdwDEBUG("%sstrType\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(boolTypeNode& n)
{
   cdwDEBUG("%sboolType\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(intTypeNode& n)
{
   cdwDEBUG("%sintType\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(arrayTypeNode& n)
{
   cdwDEBUG("%sarrayType\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(voidTypeNode& n)
{
   cdwDEBUG("%svoidType\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(userTypeNode& n)
{
   cdwDEBUG("%suserType name=%s linked?=%d\n",
      getIndent().c_str(),
      n.pDef.ref.c_str(),
      n.pDef.getRefee() ? 1 : 0);

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(ptrTypeNode& n)
{
   cdwDEBUG("%sptrType\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(sequenceNode& n)
{
   cdwDEBUG("%ssequence\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(invokeNode& n)
{
   cdwDEBUG("%sinvoke; name=%s; protoLinked?=%d\n",
      getIndent().c_str(),
      n.proto.ref.c_str(),
      n.proto.getRefee() ? 1 : 0);

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(invokeFuncPtrNode& n)
{
   cdwDEBUG("%sinvoke func ptr\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(invokeVTableNode& n)
{
   cdwDEBUG("%sinvoke v-table %lld\n",
      getIndent().c_str(),
      n.index);

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(fieldAccessNode& n)
{
   cdwDEBUG("%sfield access; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(callNode& n)
{
   cdwDEBUG("%scall; name=%s; linked?=%d\n",
      getIndent().c_str(),
      n.pTarget.ref.c_str(),
      n.pTarget.getRefee() ? 1 : 0);

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(localDeclNode& n)
{
   cdwDEBUG("%slocalDeclNode; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(varRefNode& n)
{
   cdwDEBUG("%svarRef; name=%s; linked?=%d\n",
      getIndent().c_str(),
      n.pSrc.ref.c_str(),
      n.pSrc.getRefee() ? 1 : 0);

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(assignmentNode& n)
{
   cdwDEBUG("%sassignment\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(bopNode& n)
{
   cdwDEBUG("%sbop %s\n",
      getIndent().c_str(),
      n.op.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(indexNode& n)
{
   cdwDEBUG("%sindex\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(ifNode& n)
{
   cdwDEBUG("%sif\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(loopIntrinsicNode& n)
{
   cdwDEBUG("%sloopIntrinsic '%s'\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(forLoopNode& n)
{
   cdwDEBUG("%sforLoop '%s' '%s', decomposed?=\n",
      getIndent().c_str(),
      n.name.c_str(),
      n.scoped ? "-" : "+",
      n.decomposed ? "Y" : "N");

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(loopStartNode& n)
{
   cdwDEBUG("%sloopStart '%s'\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(loopBreakNode& n)
{
   cdwDEBUG("%sloopBreak '%s'\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(loopEndNode& n)
{
   cdwDEBUG("%sloopEnd '%s'\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(stringLiteralNode& n)
{
   cdwDEBUG("%sstrLit; value=%s\n",
      getIndent().c_str(),
      n.value.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(boolLiteralNode& n)
{
   cdwDEBUG("%sboolLit; value=%d\n",
      getIndent().c_str(),
      n.value ? 1 : 0);

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(intLiteralNode& n)
{
   cdwDEBUG("%sintLit; value=%s\n",
      getIndent().c_str(),
      n.lexeme.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(structLiteralNode& n)
{
   cdwDEBUG("%sstructLit\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(genericNode& n)
{
   cdwDEBUG("%sgenericNode\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(constraintNode& n)
{
   cdwDEBUG("%sconstraintNode; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(instantiateNode& n)
{
   cdwDEBUG("%sinstantiateNode; text=%s, impled?=%d\n",
      getIndent().c_str(),
      n.text.c_str(),
      n.impled ? 1 : 0);

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

diagVisitor::autoIndent::autoIndent(diagVisitor& v)
: m_v(v)
{
   m_v.m_nIndents++;
}

diagVisitor::autoIndent::~autoIndent()
{
   m_v.m_nIndents--;
}

std::string diagVisitor::getIndent() const
{
   return std::string(m_nIndents*3,' ');
}

// TODO astFormatter

namespace {

void dumpAstLink(outStream& s, linkBase& field)
{
   s.stream() << field.ref << "->";
   node *pT = field._getRefee();
   if(!pT)
      s.stream() << "0";
   else
      s.stream() << typeid(*pT).name();
}

template<class T>
void dumpAstField(outStream& s, const char *name, T& field, bool omitIfEmpty)
{
   if(omitIfEmpty && (field == T()))
      return;
   s.stream() << " " << name << "=" << field;
}

template<>
void dumpAstField(outStream& s, const char *name, std::set<std::string>& field, bool omitIfEmpty)
{
   if(omitIfEmpty && field.size() == 0)
      return;
   s.stream() << " " << name << "=[";
   for(auto& v : field)
      s.stream() << " " << v;
   s.stream() << " ]";
}

void dumpAstLinkField(outStream& s, const char *name, linkBase& field)
{
   s.stream() << " " << name << "=";
   dumpAstLink(s,field);
}

template<>
void dumpAstField(outStream& s, const char *name, std::vector<link<classNode> >& field, bool omitIfEmpty)
{
   s.stream() << " " << name << "=[";
   for(auto& v : field)
   {
      s.stream() << " ";
      dumpAstLink(s,v);
   }
   s.stream() << " ]";
}

void dumpAstFlagsOpt(outStream& s, const char *name, size_t& field)
{
   if(field == 0)
      return;
   s.stream() << " " << name << "=";
   if(field & nodeFlags::kVirtual) s.stream()             << "v";
   if(field & nodeFlags::kOverride) s.stream()            << "o";
   if(field & nodeFlags::kAbstract) s.stream()            << "a";
   if(field & nodeFlags::kStatic) s.stream()              << "s";
   if(field & nodeFlags::kInterface) s.stream()           << "i";
   if(field & nodeFlags::kPublic) s.stream()              << "+";
   if(field & nodeFlags::kProtected) s.stream()           << "#";
   if(field & nodeFlags::kPrivate) s.stream()             << "-";
   if(field & nodeFlags::kAddressableForWrite) s.stream() << "W";
   if(field == 0) s.stream()                              << "0";
}

}

#define cdwDumpAstField_(__fmtfunc__,__name__) \
   __fmtfunc__(m_s,#__name__,n.__name__);

#define cdwDumpAstField(__name__) \
   dumpAstField<>(m_s,#__name__,n.__name__,false);

#define cdwDumpAstFieldOpt(__name__) \
   dumpAstField<>(m_s,#__name__,n.__name__,true);

#define cdwDumpAstStart(__type__) \
   m_s.stream() << indent(m_s) << #__type__;

#define cdwDumpAstStartAbstract(__type__) \
   m_s.stream() << " " << #__type__;

#define cdwDumpAstStartSilent(__type__)

#define cdwDumpAstEnd() \
   hNodeVisitor::visit(n);

#define cdwDumpAstEndSilent() \
   m_s.stream() << std::endl; \
   hNodeVisitor::visit(n);

void astFormatter::visit(node& n)
{
   cdwDumpAstStartSilent(node)
   cdwDumpAstFieldOpt(lineNumber)
   cdwDumpAstFieldOpt(attributes)
   cdwDumpAstField_(dumpAstFlagsOpt,flags)
   cdwDumpAstEndSilent()

   autoIndent _i(m_s);
   visitChildren(n);
}

void astFormatter::visit(araceliProjectNode& n)
{
   cdwDumpAstStart(araceliProjectNode)
   cdwDumpAstField(targetType)
   cdwDumpAstEnd()
}

void astFormatter::visit(liamProjectNode& n)
{
   cdwDumpAstStart(liamProjectNode)
   cdwDumpAstField(sourceFullPath)
   cdwDumpAstField(loadedPaths)
   cdwDumpAstEnd()
}

void astFormatter::visit(scopeNode& n)
{
   cdwDumpAstStart(scopeNode)
   cdwDumpAstField(path)
   cdwDumpAstField(scopeName)
   cdwDumpAstField(inProject)
   cdwDumpAstField(loaded)
   cdwDumpAstEnd()
}

void astFormatter::visit(fileNode& n)
{
   cdwDumpAstStart(fileNode)
   cdwDumpAstField(fullPath)
   cdwDumpAstEnd()
}

void astFormatter::visit(fileRefNode& n)
{
   cdwDumpAstStart(fileRefNode)
   cdwDumpAstField(ref)
   cdwDumpAstEnd()
}

void astFormatter::visit(classNode& n)
{
   cdwDumpAstStart(classNode)
   cdwDumpAstField(name)
   cdwDumpAstField(baseClasses)
   cdwDumpAstEnd()
}

void astFormatter::visit(memberNode& n)
{
   cdwDumpAstStartAbstract(memberNode)
   cdwDumpAstField(name)
   cdwDumpAstEnd()
}

void astFormatter::visit(methodNode& n)
{
   cdwDumpAstStart(methodNode)
   cdwDumpAstField_(dumpAstLinkField,baseImpl)
   cdwDumpAstEnd()
}

void astFormatter::visit(fieldNode& n)
{
   cdwDumpAstStart(fieldNode)
   cdwDumpAstEnd()
}

void astFormatter::visit(constNode& n)
{
   cdwDumpAstStart(constNode)
   cdwDumpAstField(name)
   cdwDumpAstEnd()
}

void astFormatter::visit(funcNode& n)
{
   cdwDumpAstStart(funcNode)
   cdwDumpAstField(name)
   cdwDumpAstEnd()
}

void astFormatter::visit(intrinsicNode& n)
{
   cdwDumpAstStart(intrinsicNode)
   cdwDumpAstEnd()
}

void astFormatter::visit(argNode& n)
{
   cdwDumpAstStart(argNode)
   cdwDumpAstField(name)
   cdwDumpAstEnd()
}

void astFormatter::visit(typeNode& n)
{
   cdwDumpAstStartAbstract(typeNode)
   cdwDumpAstEnd()
}

void astFormatter::visit(strTypeNode& n)
{
   cdwDumpAstStart(strTypeNode)
   cdwDumpAstEnd()
}

void astFormatter::visit(boolTypeNode& n)
{
   cdwDumpAstStart(boolTypeNode)
   cdwDumpAstEnd()
}

void astFormatter::visit(intTypeNode& n)
{
   cdwDumpAstStart(intTypeNode)
   cdwDumpAstEnd()
}

void astFormatter::visit(arrayTypeNode& n)
{
   cdwDumpAstStart(arrayTypeNode)
   cdwDumpAstEnd()
}

void astFormatter::visit(voidTypeNode& n)
{
   cdwDumpAstStart(voidTypeNode)
   cdwDumpAstEnd()
}

void astFormatter::visit(userTypeNode& n)
{
   cdwDumpAstStart(userTypeNode)
   cdwDumpAstField_(dumpAstLinkField,pDef)
   cdwDumpAstEnd()
}

void astFormatter::visit(ptrTypeNode& n)
{
   cdwDumpAstStart(ptrTypeNode)
   cdwDumpAstEnd()
}

void astFormatter::visit(sequenceNode& n)
{
   cdwDumpAstStart(sequenceNode)
   cdwDumpAstEnd()
}

void astFormatter::visit(invokeNode& n)
{
   cdwDumpAstStart(invokeNode)
   cdwDumpAstField_(dumpAstLinkField,proto)
   cdwDumpAstEnd()
}

void astFormatter::visit(invokeFuncPtrNode& n)
{
   cdwDumpAstStart(invokeFuncPtrNode)
   cdwDumpAstEnd()
}

void astFormatter::visit(invokeVTableNode& n)
{
   cdwDumpAstStart(invokeVTableNode)
   cdwDumpAstField(index)
   cdwDumpAstEnd()
}

void astFormatter::visit(fieldAccessNode& n)
{
   cdwDumpAstStart(fieldAccessNode)
   cdwDumpAstField(name)
   cdwDumpAstEnd()
}

void astFormatter::visit(callNode& n)
{
   cdwDumpAstStart(callNode)
   cdwDumpAstField_(dumpAstLinkField,pTarget)
   cdwDumpAstEnd()
}

void astFormatter::visit(localDeclNode& n)
{
   cdwDumpAstStart(localDeclNode)
   cdwDumpAstField(name)
   cdwDumpAstEnd()
}

void astFormatter::visit(varRefNode& n)
{
   cdwDumpAstStart(varRefNode)
   cdwDumpAstField_(dumpAstLinkField,pSrc)
   cdwDumpAstEnd()
}

void astFormatter::visit(assignmentNode& n)
{
   cdwDumpAstStart(assignmentNode)
   cdwDumpAstEnd()
}

void astFormatter::visit(bopNode& n)
{
   cdwDumpAstStart(bopNode)
   cdwDumpAstField(op)
   cdwDumpAstEnd()
}

void astFormatter::visit(indexNode& n)
{
   cdwDumpAstStart(indexNode)
   cdwDumpAstEnd()
}

void astFormatter::visit(ifNode& n)
{
   cdwDumpAstStart(ifNode)
   cdwDumpAstEnd()
}

void astFormatter::visit(loopIntrinsicNode& n)
{
   cdwDumpAstStart(loopIntrinsicNode)
   cdwDumpAstField(name)
   cdwDumpAstEnd()
}

void astFormatter::visit(forLoopNode& n)
{
   cdwDumpAstStart(forLoopNode)
   cdwDumpAstField(name)
   cdwDumpAstField(scoped)
   cdwDumpAstField(decomposed)
   cdwDumpAstEnd()
}

void astFormatter::visit(loopStartNode& n)
{
   cdwDumpAstStart(loopStartNode)
   cdwDumpAstField(name)
   cdwDumpAstEnd()
}

void astFormatter::visit(loopBreakNode& n)
{
   cdwDumpAstStart(loopBreakNode)
   cdwDumpAstField(name)
   cdwDumpAstEnd()
}

void astFormatter::visit(loopEndNode& n)
{
   cdwDumpAstStart(loopEndNode)
   cdwDumpAstField(name)
   cdwDumpAstEnd()
}

void astFormatter::visit(stringLiteralNode& n)
{
   cdwDumpAstStart(stringLiteralNode)
   cdwDumpAstField(value)
   cdwDumpAstEnd()
}

void astFormatter::visit(boolLiteralNode& n)
{
   cdwDumpAstStart(boolLiteralNode)
   cdwDumpAstField(value)
   cdwDumpAstEnd()
}

void astFormatter::visit(intLiteralNode& n)
{
   cdwDumpAstStart(intLiteralNode)
   cdwDumpAstField(lexeme)
   cdwDumpAstEnd()
}

void astFormatter::visit(structLiteralNode& n)
{
   cdwDumpAstStart(structLiteralNode)
   cdwDumpAstEnd()
}

void astFormatter::visit(genericNode& n)
{
   cdwDumpAstStart(genericNode)
   cdwDumpAstEnd()
}

void astFormatter::visit(constraintNode& n)
{
   cdwDumpAstStart(constraintNode)
   cdwDumpAstField(name)
   cdwDumpAstEnd()
}

void astFormatter::visit(instantiateNode& n)
{
   cdwDumpAstStart(instantiateNode)
   cdwDumpAstField(impled)
   cdwDumpAstField(text)
   cdwDumpAstEnd()
}

std::string fullyQualifiedName::build(cmn::node& n, const std::string& start)
{
   if(!start.empty() && start.c_str()[0] == '.')
      // if it's already a FQN, just return it
      return start;

   fullyQualifiedName self;
   self.m_fqn = start;
   n.acceptVisitor(self);
   return self.m_fqn;
}

void fullyQualifiedName::visit(cmn::node& n)
{
   cmn::node *pParent = n.getParent();
   if(pParent)
      pParent->acceptVisitor(*this);
   else
      makeAbsolute();
}

void fullyQualifiedName::visit(scopeNode& n)
{
   prepend(n.scopeName);
   hNodeVisitor::visit(n);
}

void fullyQualifiedName::visit(classNode& n)
{
   prepend(n.name);
   hNodeVisitor::visit(n);
}

void fullyQualifiedName::makeAbsolute()
{
   if(m_fqn.empty()) return;

   if(m_fqn.c_str()[0] != '.')
      m_fqn = std::string(".") + m_fqn;
}

void fullyQualifiedName::prepend(const std::string& n)
{
   if(n.empty()) return;

   makeAbsolute();

   m_fqn = n + m_fqn;
}

void cloningNodeVisitor::visit(node& n)
{
   as<node>().lineNumber = n.lineNumber;
   as<node>().attributes = n.attributes;
   as<node>().flags = n.flags;
}

void cloningNodeVisitor::visit(classNode& n)
{
   as<classNode>().name = n.name;
   as<classNode>().baseClasses.resize(n.baseClasses.size());
   for(size_t i=0;i<n.baseClasses.size();i++)
      as<classNode>().baseClasses[i].ref = n.baseClasses[i].ref;
   hNodeVisitor::visit(n);
}

void cloningNodeVisitor::visit(memberNode& n)
{
   as<memberNode>().name = n.name;
   hNodeVisitor::visit(n);
}

void cloningNodeVisitor::visit(methodNode& n)
{
   as<methodNode>().baseImpl.ref = n.baseImpl.ref;
   if(n.baseImpl.getRefee())
      as<methodNode>().baseImpl.bind(*n.baseImpl.getRefee());
   hNodeVisitor::visit(n);
}

void cloningNodeVisitor::visit(argNode& n)
{
   as<argNode>().name = n.name;
   hNodeVisitor::visit(n);
}

void cloningNodeVisitor::visit(userTypeNode& n)
{
   as<userTypeNode>().pDef.ref = n.pDef.ref;
   if(n.pDef._getRefee())
      as<userTypeNode>().pDef.bind(*n.pDef._getRefee());
   hNodeVisitor::visit(n);
}

void cloningNodeVisitor::visit(invokeVTableNode& n)
{
   as<invokeVTableNode>().index = n.index;
   hNodeVisitor::visit(n);
}

void cloningNodeVisitor::visit(callNode& n)
{
   as<callNode>().pTarget.ref = n.pTarget.ref;
   if(n.pTarget._getRefee())
      as<callNode>().pTarget.bind(*n.pTarget._getRefee());
   hNodeVisitor::visit(n);
}

void cloningNodeVisitor::visit(varRefNode& n)
{
   hNodeVisitor::visit(n);
   as<varRefNode>().pSrc.ref = n.pSrc.ref;
   if(n.pSrc._getRefee())
      as<varRefNode>().pSrc.bind(*n.pSrc._getRefee());
}

node& cloneTree(node& n)
{
   creatingNodeVisitor creator;
   n.acceptVisitor(creator);
   { cloningNodeVisitor v(*creator.inst.get()); n.acceptVisitor(v); }

   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
      creator.inst->appendChild(cloneTree(**it));

   return *creator.inst.release();
}

} // namespace cmn
