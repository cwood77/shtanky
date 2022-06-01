const .zero : str = "0";
const .one : str = "1";
const .other : str = "other!";

// BUGS!
// - ifs need XOR
// - locals are double-allocated (but only once cleaned)
// - local clean-up is technically unnecessary

[entrypoint]
func .whatever() : void
{
   var f : int;
   f = ._getflg(8);

   if(f < 1)
      ._print(.zero);
   else if(f < 2)
      ._print(.one);
   else
      ._print(.other);
}

func ._getflg(index : int) : int;
func ._osCall(code : str, payload : str) : void;
func ._print(msg : str) : void;
