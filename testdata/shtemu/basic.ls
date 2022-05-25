[entrypoint]
func .whatever() : void
{
   if(true)
      ._osCall(1,0);
}

func ._osCall(code : str, payload : str) : void;
