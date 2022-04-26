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

func .assign.assignTester.cctor(
   self : .assign.assignTester) : .assign.assignTester
{
}

func .assign.assignTester.cdtor(
   self : .assign.assignTester) : .assign.assignTester
{
}

func .assign.A.cctor(
   self : .assign.A) : .assign.A
{
}

func .assign.A.cdtor(
   self : .assign.A) : .assign.A
{
}

func .assign.B.cctor(
   self : .assign.B) : .assign.B
{
}

func .assign.B.cdtor(
   self : .assign.B) : .assign.B
{
}

const A_vtbl_inst : .assign.A_vtbl = {  };

const B_vtbl_inst : .assign.B_vtbl = {  };

const assignTester_vtbl_inst : .assign.assignTester_vtbl = { .assign.assignTester.run };


func ._osCall(code : str, payload : str) : void;

ref "main.ara.lh";
