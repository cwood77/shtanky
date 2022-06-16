[entrypoint]
func .assign.consoleTarget.main(
   args : .sht.core.string[]) : void
{
   var cout : .sht.cons.stdout;
   .sht.cons.stdout_sctor(cout);
   var obj0 : .assign.assignTester;
   .assign.assignTester_sctor(obj0);
   obj0:_out = cout;
   obj0:_vtbl:run->(obj0,args);
   .assign.assignTester_sdtor(obj0);
   .sht.cons.stdout_sdtor(cout);
}

func .assign.consoleTarget.cctor(
   self : .assign.consoleTarget) : .assign.consoleTarget
{
}

func .assign.consoleTarget.cdtor(
   self : .assign.consoleTarget) : .assign.consoleTarget
{
}

[vtbl]
const .assign.consoleTarget_vtbl_inst : .assign.consoleTarget_vtbl = {  };

func .assign.consoleTarget_sctor(
   self : .assign.consoleTarget) : void
{
   .sht.core.object_sctor(self);
   self:_vtbl = .assign.consoleTarget_vtbl_inst;
   .assign.consoleTarget.cctor(self);
}

func .assign.consoleTarget_sdtor(
   self : .assign.consoleTarget) : void
{
   self:_vtbl = .assign.consoleTarget_vtbl_inst;
   .assign.consoleTarget.cdtor(self);
   .sht.core.object_sdtor(self);
}


ref "..\sht\prims.lh";

ref ".target.ara.lh";
ref ".\main.ara.lh";
ref "..\sht\cons\program.ara.lh";
ref "..\sht\core\object.ara.lh";
ref "..\sht\core\string.ara.lh";
