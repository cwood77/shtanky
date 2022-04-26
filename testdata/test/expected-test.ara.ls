const .const0 : str = "hello world!";

func .test.test.run(
   self : .test.test,
   args : str[]) : void
{
   self:_out:_vtbl:printLn->(self:_out,.const0);
}

func .test.test.cctor(
   self : .test.test) : .test.test
{
}

func .test.test.cdtor(
   self : .test.test) : .test.test
{
}

const test_vtbl_inst : .test.test_vtbl = { .test.test.run };


func ._osCall(code : str, payload : str) : void;

ref "test.ara.lh";
