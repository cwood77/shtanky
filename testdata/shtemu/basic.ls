const .yes : str = "YES!";

[entrypoint]
func .whatever() : void
{
   if(true)
      ._osCall(1,.yes);
}

func ._osCall(code : str, payload : str) : void;
