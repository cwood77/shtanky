# Stack vs. Heap Objects

Priciples
- Heap is cheap
- User doesn't care
- Need RAII
- Need to support ownership models

## Examples

Case 1: Stack
```
   var inst : MyClass;
   thingee->method(inst);
```
- `inst` is created/destroyed in caller, on the stack
- `thingee` receives a pointer to `inst`

Case N: Heap

Case N: Global

Case N: Singleton

Case N: Unique ptr

Case N: Shared ptr
```
   var child2 : Child;
   child2->setSharedParent(child1->parent);
   ...
   child1 = null;
```
- The parent is shared between both children; either of which may be deallocated first

solution:
- emit ref counting code

Case N: Hand off
```
   // append to list
   var nodeB : Node;
   nodeB->name = "tail node";
   nodeA->append(&nodeB);   // new op!
   return nodeA;
```
- `nodeB` _cannot_ be stack allocated, because ownership is passed to `nodeA`

solution:
- introduce `&` operator, which relinquishes ownership
- at compile time, if `&` is reachable, allocation is made in the heap
- introduce `null` value
- at run time, assignment to `null` releases heap allocations
- at compile time, trace `&` usage to fields
- at compile time, generate memory frees for eligivle fields in the dtor

Case N: Delete this

## Solutions

1. Auto Araceli + Explicit Liam
Allocations in Araceli are either heap or stack implicitly by heuristic without user control.  Allocations in Liam are either heap or stack explicitly by direct user statements.

Araceli Examples:
```
   var x = myClass.create(1,3,"word up"); // heap
   var y = 4; // stack
```

Liam Examples:
```
   var x = .sht.mem.alloc(13);
   myClass.create(x,1,3,"word up");
   .sht.obj.dec(x);

   var y = 4;
```
