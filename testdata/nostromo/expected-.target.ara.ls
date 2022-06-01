[entrypoint]
func .nostromo.consoleTarget.main(
   args : .sht.core.string[]) : void
{
   var cout : .sht.cons.stdout;
   .sht.cons.stdout_sctor(cout);
   var obj0 : .nostromo.bootstrapper;
   .nostromo.bootstrapper_sctor(obj0);
   obj0:_out = cout;
   obj0:_vtbl:run->(obj0,args);
   .nostromo.bootstrapper_sdtor(obj0);
   .sht.cons.stdout_sdtor(cout);
}

func .nostromo.consoleTarget.cctor(
   self : .nostromo.consoleTarget) : .nostromo.consoleTarget
{
}

func .nostromo.consoleTarget.cdtor(
   self : .nostromo.consoleTarget) : .nostromo.consoleTarget
{
}

const .nostromo.consoleTarget_vtbl_inst : .nostromo.consoleTarget_vtbl = {  };

func .nostromo.consoleTarget_sctor(
   self : .nostromo.consoleTarget) : void
{
   .sht.core.object_sctor(self);
   self:_vtbl = .nostromo.consoleTarget_vtbl_inst;
   .nostromo.consoleTarget.cctor(self);
}

func .nostromo.consoleTarget_sdtor(
   self : .nostromo.consoleTarget) : void
{
   self:_vtbl = .nostromo.consoleTarget_vtbl_inst;
   .nostromo.consoleTarget.cdtor(self);
   .sht.core.object_sdtor(self);
}


ref "..\sht\prims.lh";

ref ".target.ara.lh";
ref ".\boot.ara.lh";
ref "..\sht\cons\program.ara.lh";
ref "..\sht\core\object.ara.lh";
ref "..\sht\core\string.ara.lh";
