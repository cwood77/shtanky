const .text0 : str = "text 0";
const .text1 : str = "text 1";
const .text2 : str = "text 2";

func ._getflg(index : int) : int;
func ._osCall(code : str, payload : str) : void;
func ._print(msg : str) : void;

func .subFunc2(x : int, y : int) : int
{
   return x + y;
}

func .subFunc1(x : int) : int
{
   return .subFunc2(7, x);
}

[entrypoint]
func .whatever() : void
{
   .subFunc1(12);
}
