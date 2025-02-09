const ._strLit_helloWorld_0 : str = "hello world!";

func .test.test.run(
   self : .test.test,
   args : .sht.core.array<.sht.core.string>) : void
{
   self:_out:_vtbl:printLn->(self:_out,._strLit_helloWorld_0);
}

func .test.test.cctor(
   self : .test.test) : .test.test
{
}

func .test.test.cdtor(
   self : .test.test) : .test.test
{
}

[vtbl]
const .test.test_vtbl_inst : .test.test_vtbl = { .test.test.run };

func .test.test_sctor(
   self : .test.test) : void
{
   .sht.cons.program_sctor(self);
   self:_vtbl = .test.test_vtbl_inst;
   .test.test.cctor(self);
}

func .test.test_sdtor(
   self : .test.test) : void
{
   self:_vtbl = .test.test_vtbl_inst;
   .test.test.cdtor(self);
   .sht.cons.program_sdtor(self);
}


ref "..\sht\prims.lh";

ref "..\sht\cons\program.ara.lh";
ref "..\sht\core\array.ara.lh";
ref ".\test.ara.lh";
