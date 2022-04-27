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

const .assign.A_vtbl_inst : .assign.A_vtbl = {  };

const .assign.B_vtbl_inst : .assign.B_vtbl = {  };

const .assign.assignTester_vtbl_inst : .assign.assignTester_vtbl = { .assign.assignTester.run };

func .assign.A_sctor(
   self : .assign.A) : void
{
   .sht.core.object_sctor(self);
   self:_vtbl = .assign.A_vtbl_inst;
   .assign.A.cctor(self);
}

func .assign.B_sctor(
   self : .assign.B) : void
{
   .sht.core.object_sctor(self);
   self:_vtbl = .assign.B_vtbl_inst;
   .assign.B.cctor(self);
}

func .assign.assignTester_sctor(
   self : .assign.assignTester) : void
{
   .sht.cons.program_sctor(self);
   self:_vtbl = .assign.assignTester_vtbl_inst;
   .assign.assignTester.cctor(self);
}


func ._osCall(code : str, payload : str) : void;

ref "main.ara.lh";
ref "..\sht\cons\program.ara.lh";
ref "..\sht\core\object.ara.lh";
