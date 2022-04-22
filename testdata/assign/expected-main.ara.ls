func .assign.assignTester.run(
   self : .assign.assignTester,
   args : str[]) : void
{
   var x : void;
   x = 7;
   var a : .assign.A;
   a:x = 7;
   a:b:x = 7;
}

func ._osCall(code : str, payload : str) : void;

ref "main.ara.lh";
