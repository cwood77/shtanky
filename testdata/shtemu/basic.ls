const .const0 : str = "hello world!";

[entrypoint]
func .whatever() : void
{
   if(true)
      ._osCall(1,.const0);
   else if(false)
      ._osCall(1,.const0);
   else
      ._osCall(1,.const0);
}

func ._osCall(code : str, payload : str) : void;
