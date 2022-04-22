func .test.test.run(
   self : .test.test,
   args : str[]) : void
{
   self:_out:_vtbl:printLn->(self:_out,.const0);
}

const .const0 : str = "hello world!";
func ._osCall(code : str, payload : str) : void;

ref "test.ara.lh";
