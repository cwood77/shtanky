#pragma once
#include "../cmn/ast.hpp"

// loop transforms happen in two groups, one in salome at link type,
// and one in liam.  Idea is to do as little at link time as is possible.
//
// original code
//     for[-i][0,7)
//     {
//        _uart->write8(msg[i]);
//        loop(i)->break();
//     }
//
// --during link-- (required here to avoid unresolved links)
// replace loop(i) func calls with intrinsic node             --+   [loopIntrinsicDecomp]
// replace refs of 'i' with 'i->getCount()'                     |   [loopVarRefFixup]     *
// drop a loop instance and replace intrinsics with varRefs   <-+   [loopInstDropper]     *
//                                                                * skippable in some langs
//                                                                  (like philemon)
//
//     var i : .sht.core.loopInst; // if public
//     {
//        var i : .sht.core.loopInst; // if private
//
//        for[-i][0,7)
//        {
//           _uart->write8(msg[i->count()]);
//           i->break();
//        }
//     }
//
// this should be a fully linkable graph

// --in liam--
// for-to-while loop transform
//
//     var i : .sht.core.loopInst; // if public
//     {
//        var i : .sht.core.loopInst; // if private
//
//        i->setBounds(6,0,false);// omit for while loops
//        while[i](i->inbounds()) // i always scoped now
//        {
//           _uart->write8(msg[i->count()]);
//           i->break();
//        }
//     }
//
// break/continue transform - replace refences to break/continue methods with nodes
// start/end transform - replace high-level loop node with start/end nodes + bump
//
//     var i : .sht.core.loopInst; // if public
//     {
//        var i : .sht.core.loopInst; // if private
//
//        {
//           i->setBounds(6,0,false);
//           <loopstartnode>
//           i:_condition = i->inbounds();
//           if(i:_condition)
//              ;
//           else
//              <breaknode>
//           _uart->write8(msg[i->getCount()]);
//           <breaknode> // contrived example just for illustration
//           i->bump();
//           <loopendnode>
//        }
//     }

namespace liam {

class loopInstMethodDecomposer : public cmn::hNodeVisitor {
};

class forLoopToWhileLoopConverter : public cmn::hNodeVisitor {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::forLoopNode& n);

   virtual void _implementLanguage() {} // all
};

class loopDecomposer : public cmn::hNodeVisitor {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::whileLoopNode& n);

   virtual void _implementLanguage() {} // all
};

} // namespace liam
