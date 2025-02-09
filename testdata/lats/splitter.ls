const .splitter.label : str = "- splitter";
const .splitter.test.label : str = "  - simple";

func .splitter.subFunc2(x : int, y : int) : int
{
   return x + y;
}

func .splitter.subFunc1(x : int) : int
{
   return .splitter.subFunc2(7, x);
}

func .splitter.test() : void
{
   ._print(.splitter.label);
   var X : bool;

   X = .splitter.subFunc1(12) == 19;
   .main.check(.splitter.test.label,X);
}
