func .assign.assignTester.run(
   self : .assign.assignTester,
   args : str[]) : void
{
}

func .assign.assignTester.readFromSubObject(
   self : .assign.assignTester,
   a : .assign.A,
   v : void) : void
{
   v = a:b;
}

func .assign.assignTester.writeIntoSubObject(
   self : .assign.assignTester) : void
{
   var a : .assign.A;
   a:b = 7;
}

func .assign.assignTester.readFromSubSubObject(
   self : .assign.assignTester,
   a : .assign.A,
   wrong : void) : void
{
   wrong = a:b:x;
}

func .assign.assignTester.writeIntoSubSubObject(
   self : .assign.assignTester) : void
{
   var wrong : .assign.A;
   wrong:b:x = 7;
}

func ._osCall(code : str, payload : str) : void;

ref "main.ara.lh";
