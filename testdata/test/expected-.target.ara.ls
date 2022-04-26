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

const consoleTarget_vtbl_inst : .test.consoleTarget_vtbl = {  };


func ._osCall(code : str, payload : str) : void;

ref "..\sht\cons\program.ara.lh";
ref ".target.ara.lh";
