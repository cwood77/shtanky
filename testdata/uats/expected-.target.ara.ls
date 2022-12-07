[entrypoint]
func .uats.consoleTarget.main(
   args : .sht.core.string[]) : void
{
   var cout : .sht.cons.stdout;
   .sht.cons.stdout_sctor(cout);
   var obj0 : .uats.ifTest;
   .uats.ifTest_sctor(obj0);
   obj0:_out = cout;
   var obj1 : .uats.loopTest;
   .uats.loopTest_sctor(obj1);
   obj1:_out = cout;
   obj0:_vtbl:run->(obj0,args);
   obj1:_vtbl:run->(obj1,args);
   .uats.loopTest_sdtor(obj1);
   .uats.ifTest_sdtor(obj0);
   .sht.cons.stdout_sdtor(cout);
}

func .uats.consoleTarget.cctor(
   self : .uats.consoleTarget) : .uats.consoleTarget
{
}

func .uats.consoleTarget.cdtor(
   self : .uats.consoleTarget) : .uats.consoleTarget
{
}

[vtbl]
const .uats.consoleTarget_vtbl_inst : .uats.consoleTarget_vtbl = {  };

func .uats.consoleTarget_sctor(
   self : .uats.consoleTarget) : void
{
   .sht.core.object_sctor(self);
   self:_vtbl = .uats.consoleTarget_vtbl_inst;
   .uats.consoleTarget.cctor(self);
}

func .uats.consoleTarget_sdtor(
   self : .uats.consoleTarget) : void
{
   self:_vtbl = .uats.consoleTarget_vtbl_inst;
   .uats.consoleTarget.cdtor(self);
   .sht.core.object_sdtor(self);
}


ref "..\sht\prims.lh";

ref "..\sht\cons\program.ara.lh";
ref "..\sht\core\object.ara.lh";
ref "..\sht\core\string.ara.lh";
ref ".target.ara.lh";
ref ".\if.ara.lh";
ref ".\loop.ara.lh";
