const .combiner.label : str = "- combiner";
const .combiner.test.label : str = "  - simple";

func .combiner.subFunc2(a : int, b : int, c : int) : int
{
}

// this function intentionally stomps on the contents of rdx (i.e. value b)
func .combiner.subFunc1(a : int, b : int, c : int) : int
{
   var sanity : bool;
   sanity = b == 2;
   .main.check(.combiner.test.label,sanity);

   .combiner.subFunc2(3,3,3);
}

func .combiner.test() : void
{
   ._print(.combiner.label);

   .combiner.subFunc1(1,2,3);

   // this second call is problematic b/c the first call would have stomped on rdx
   // the splitter should reload 2 to rdx to fix this
   .combiner.subFunc1(1,2,3);
}
