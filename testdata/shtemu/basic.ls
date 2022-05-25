const .yes : str = "YES!";

[entrypoint]
func .whatever() : void
{
   ._osCall(1,.yes);
}

func ._osCall(code : str, payload : str) : void;
