[entrypoint]
func .assign.consoleTarget.main(
   args : str[]) : void
{
   var cout : .sht.cons.stdout;
   .sht.cons.stdout_sctor(cout);
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


func ._osCall(code : str, payload : str) : void;

ref ".target.ara.lh";
ref "..\sht\cons\program.ara.lh";
ref "..\sht\core\object.ara.lh";
