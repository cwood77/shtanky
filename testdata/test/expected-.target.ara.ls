[entrypoint]
func .test.consoleTarget.main(
   args : str[]) : void
{
   var cout : .sht.cons.iStream;
}

func .test.consoleTarget.cctor(
   self : .test.consoleTarget) : .test.consoleTarget
{
}

func .test.consoleTarget.cdtor(
   self : .test.consoleTarget) : .test.consoleTarget
{
}

const .test.consoleTarget_vtbl_inst : .test.consoleTarget_vtbl = {  };

func .test.consoleTarget_sctor(
   self : .test.consoleTarget) : void
{
   .sht.core.object_sctor(self);
   self:_vtbl = .test.consoleTarget_vtbl_inst;
   .test.consoleTarget.cctor(self);
}


func ._osCall(code : str, payload : str) : void;

ref "..\sht\cons\program.ara.lh";
ref "..\sht\core\object.ara.lh";
ref ".target.ara.lh";
