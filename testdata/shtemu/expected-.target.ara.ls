[entrypoint]
func .shtemu.consoleTarget.main(
   args : .sht.core.string[]) : void
{
   var cout : .sht.cons.stdout;
   .sht.cons.stdout_sctor(cout);
   var obj0 : .shtemu.foo;
   .shtemu.foo_sctor(obj0);
   obj0:_out = cout;
   obj0:_vtbl:run->(obj0,args);
   .shtemu.foo_sdtor(obj0);
   .sht.cons.stdout_sdtor(cout);
}

func .shtemu.consoleTarget.cctor(
   self : .shtemu.consoleTarget) : .shtemu.consoleTarget
{
}

func .shtemu.consoleTarget.cdtor(
   self : .shtemu.consoleTarget) : .shtemu.consoleTarget
{
}

[vtbl]
const .shtemu.consoleTarget_vtbl_inst : .shtemu.consoleTarget_vtbl = {  };

func .shtemu.consoleTarget_sctor(
   self : .shtemu.consoleTarget) : void
{
   .sht.core.object_sctor(self);
   self:_vtbl = .shtemu.consoleTarget_vtbl_inst;
   .shtemu.consoleTarget.cctor(self);
}

func .shtemu.consoleTarget_sdtor(
   self : .shtemu.consoleTarget) : void
{
   self:_vtbl = .shtemu.consoleTarget_vtbl_inst;
   .shtemu.consoleTarget.cdtor(self);
   .sht.core.object_sdtor(self);
}


ref "..\sht\prims.lh";

ref "..\sht\cons\program.ara.lh";
ref "..\sht\core\object.ara.lh";
ref "..\sht\core\string.ara.lh";
ref ".target.ara.lh";
ref ".\basic.ara.lh";
