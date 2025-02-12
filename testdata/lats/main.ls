const .main.startLabel : str = "~~ lats start ~~";
const .main.endLabel : str   = "~~ lats end ~~";
const .main.passLabel : str = "    pass";
const .main.failLabel : str = "    !!!!!!!! FAIL !!!!!!!!";

func ._print(msg : str) : void;

[entrypoint]
func .main.entrypoint() : void
{
   ._print(.main.startLabel);

   .splitter.test();

   ._print(.main.endLabel);
}

func .main.check(label : str, value : bool) : void
{
   var unused_for_stack_padding : bool;
   ._print(label);
   if(value)
      ._print(.main.passLabel);
   else
      ._print(.main.failLabel);
}

ref ".\splitter.ls";
