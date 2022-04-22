#pragma once
#include "../cmn/ast.hpp"

namespace araceli {

// order
// - selfDecomposition (add self param, scope fields, decomp invokes)
// - methodGlobalizer
// - abstractGenerator
// - ctorDtorDecomposition
// - vTableDecomposition
// - stackObjDecomp

// * move out now
//
// total list of transforms                now         should be
// + add self params to functions       [codegen]      [member]      memberDecomposition
// + generate a vtbl                    [codegen]    * [vtbl]        vTableDecomposition
// + add a vtbl field to classes        [codegen]    * [vtbl]
// + move methods to global functions   [codegen]    * [vtbl]
// + change invokes to invokePtrs       [declasser]    [member]
// + scope fields with self             [declasser]    [member]
// + generate compiler ctor/dtor        [n/a]          [ctordtor]    ctorDtorGenerator
// + generate super ctor/dtor           [n/a]          [ctordtor]
// + generate abstract methods          [n/a]          [ctordtor]
// + generate dtor calls for stack vars [n/a]          [stackobj]    stackObjectDecomposition
// + generate ctor calls for stack vars [n/a]          [stackobj]
//
// - seperate header and source         [codegen]      [codegen]
// - generate #includes                 [codegen]      [codegen]

// for varRefs
// - if these refer to fields, scope then with 'self'

// for methods: two things
// - inject self args
// - change to invokeFuncPtr node with vtable

class declasser : public cmn::araceliVisitor<> {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }

   virtual void visit(cmn::invokeNode& n);
   virtual void visit(cmn::varRefNode& n);
};

} // namespace araceli
