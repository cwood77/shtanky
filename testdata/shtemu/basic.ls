const .const0 : str = "hello world!";

[entrypoint]
func .whatever() : void
{
   if(0 < 7)
      ._print(.const0);
   else if(false)
      ._print(.const0);
   else
      ._print(.const0);
}

func ._osCall(code : str, payload : str) : void;
func ._print(msg : str) : void;
