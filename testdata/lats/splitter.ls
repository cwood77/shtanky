const .splitter.label : str = "- splitter";
const .splitter.test.label : str = "  - simple";

const .main.progress : str = "       progress";

func .splitter.subFunc2(x : int, y : int) : int
{
   ._print(.main.progress);
   return x + y;
}

func .splitter.subFunc1(x : int) : int
{
   var unused_for_stack_padding : bool;
   ._print(.main.progress);
   return .splitter.subFunc2(7, x);
}

func .splitter.test() : void
{
   ._print(.splitter.label);
   var X : bool;

   ._print(.main.progress);
   X = .splitter.subFunc1(12) == 19;
   ._print(.main.progress);
   .main.check(.splitter.test.label,X);
}
