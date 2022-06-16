const .text0 : str = "text 0";
const .text1 : str = "text 1";
const .text2 : str = "text 2";

func .virtFunc0() : void
{
   ._print(.text0);
}

func .virtFunc1() : void
{
   ._print(.text1);
}

class .v {
   f0 : ptr;
   f1 : ptr;
}

[vtbl]
const .vinst : .v = { .virtFunc0, .virtFunc1 };

class .obj {
   _vtbl : .v;
}

func .ctor(self : .obj) : void
{
   self:_vtbl = .vinst;
}

[entrypoint]
func .whatever() : void
{
   var o : .obj;
   .ctor(o);
   ._print(.text2);
   o:_vtbl:f0->();
   o:_vtbl:f1->();
   o:_vtbl:f0->();
   o:_vtbl:f1->();
}

func ._getflg(index : int) : int;
func ._osCall(code : str, payload : str) : void;
func ._print(msg : str) : void;
