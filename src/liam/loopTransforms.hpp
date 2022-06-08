#pragma once
#include "../cmn/ast.hpp"

// original code
//     for[+i](i < l)
//     {
//        _uart->write8(msg[i]);
//        loop(i)->break();
//     }
//
// --during link-- (required here to avoid unresolved links)
// replace loop(i) func calls with intrinsic node             ----+
// replace refs of 'i' with 'i:count'                             |
// drop a loop instance and replace intrinsics with varRefs   <---+
//
//     var i : .sht.core.loop; // if public
//     {
//        var i : .sht.core.loop; // if private
//
//        for(i:count < l)
//        {
//           _uart->write8(msg[i:count]);
//           i->break();
//        }
//     }
//
// this should be a fully linkable graph

// --in liam--
// break/continue transform - replace refences to break continue methods with nodes
// start/end transform - replace high-level loop node with start/end nodes
//
//     var i : .sht.core.loop; // if public
//     {
//        var i : .sht.core.loop; // if private
//
//        {
//           <loopstartnode>
//           i:exitedNormally = i:count < 1;
//           if(i:exitedNormally)
//              <breaknode>
//           _uart->write8(msg[i:count]);
//           <breaknode>
//           <loopendnode>
//        }
//     }

namespace liam {

class loopInstMethodDecomposer : public cmn::hNodeVisitor {
};

class loopDecomposer : public cmn::hNodeVisitor {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::forLoopNode& n);

   virtual void _implementLanguage() {} // all
};

} // namespace liam
