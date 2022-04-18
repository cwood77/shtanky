func .sht.cons.stdout.printLn(
   self : .sht.cons.stdout,
   msg : str) : void
{
   ._osCall(1,msg);
}

func ._osCall(code : str, payload : str) : void;

ref "program.ara.lh";
