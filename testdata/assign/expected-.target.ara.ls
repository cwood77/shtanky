[entrypoint]
func .assign.consoleTarget.main(
   args : str[]) : void
{
   var cout : .sht.cons.iStream;
}

func .assign.consoleTarget.cctor(
   self : .assign.consoleTarget) : .assign.consoleTarget
{
}

func .assign.consoleTarget.cdtor(
   self : .assign.consoleTarget) : .assign.consoleTarget
{
}

const consoleTarget_vtbl_inst : .assign.consoleTarget_vtbl = {  };


func ._osCall(code : str, payload : str) : void;

ref ".target.ara.lh";
ref "..\sht\cons\program.ara.lh";
