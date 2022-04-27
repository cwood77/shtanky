const .test.const0 : str = "hello world!";

func .test.test.run(
   self : .test.test,
   args : str[]) : void
{
   self:_out:_vtbl:printLn->(self:_out,.test.const0);
}

func .test.test.cctor(
   self : .test.test) : .test.test
{
}

func .test.test.cdtor(
   self : .test.test) : .test.test
{
}

const .test.test_vtbl_inst : .test.test_vtbl = { .test.test.run };

func .test.test_sctor(
   self : .test.test) : void
{
   .sht.cons.program_sctor(self);
   self:_vtbl = .test.test_vtbl_inst;
   .test.test.cctor(self);
}


func ._osCall(code : str, payload : str) : void;

ref "..\sht\cons\program.ara.lh";
ref "test.ara.lh";
