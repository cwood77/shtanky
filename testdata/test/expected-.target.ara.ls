[entrypoint]
func .test.consoleTarget.main(
   args : str[]) : void
{
   var cout : .sht.cons.iStream;
   .sht.cons.iStream_sctor(cout);
   .sht.cons.iStream_sdtor(cout);
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

func .test.consoleTarget_sdtor(
   self : .test.consoleTarget) : void
{
   self:_vtbl = .test.consoleTarget_vtbl_inst;
   .test.consoleTarget.cdtor(self);
   .sht.core.object_sdtor(self);
}


func ._osCall(code : str, payload : str) : void;

ref "..\sht\cons\program.ara.lh";
ref "..\sht\core\object.ara.lh";
ref ".target.ara.lh";
