[entrypoint]
func .test.consoleTarget.main(
   args : .sht.core.string[]) : void
{
   var cout : .sht.cons.stdout;
   .sht.cons.stdout_sctor(cout);
   var obj0 : .test.test;
   .test.test_sctor(obj0);
   obj0:_out = cout;
   obj0:_vtbl:run->(obj0,args);
   .test.test_sdtor(obj0);
   .sht.cons.stdout_sdtor(cout);
}

func .test.consoleTarget.cctor(
   self : .test.consoleTarget) : .test.consoleTarget
{
}

func .test.consoleTarget.cdtor(
   self : .test.consoleTarget) : .test.consoleTarget
{
}

[vtbl]
const .test.consoleTarget_vtbl_inst : .test.consoleTarget_vtbl = {  };

func .test.consoleTarget_sctor(
   self : .test.consoleTarget) : void
{
   .sht.core.object_sctor(self);
   self:_vtbl = .test.consoleTarget_vtbl_inst;
   .test.consoleTarget.cctor(self);
}

func .test.consoleTarget_sdtor(
   self : .test.consoleTarget) : void
{
   self:_vtbl = .test.consoleTarget_vtbl_inst;
   .test.consoleTarget.cdtor(self);
   .sht.core.object_sdtor(self);
}


ref "..\sht\prims.lh";

ref "..\sht\cons\program.ara.lh";
ref "..\sht\core\object.ara.lh";
ref "..\sht\core\string.ara.lh";
ref ".target.ara.lh";
ref ".\test.ara.lh";
