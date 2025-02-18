func .assign.assignTester.run(
   self : .assign.assignTester,
   args : .sht.core.array<.sht.core.string>) : void
{
}

func .assign.assignTester.readFromSubObject(
   self : .assign.assignTester,
   a : .assign.A,
   v : void) : void
{
   v = a:_b;
}

func .assign.assignTester.writeIntoSubObject(
   self : .assign.assignTester) : void
{
   var a : .assign.A;
   .assign.A_sctor(a);
   a:_b = 7;
   .assign.A_sdtor(a);
}

func .assign.assignTester.readFromSubSubObject(
   self : .assign.assignTester,
   a : .assign.A,
   wrong : void) : void
{
   wrong = a:_b:_x;
}

func .assign.assignTester.writeIntoSubSubObject(
   self : .assign.assignTester) : void
{
   var wrong : .assign.A;
   .assign.A_sctor(wrong);
   wrong:_b:_x = 7;
   .assign.A_sdtor(wrong);
}

func .assign.assignTester.bopAssociativity(
   self : .assign.assignTester) : void
{
   var v : void;
   v = 1 + 2 + 3;
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
   self:_x = 0;
}

func .assign.A.cdtor(
   self : .assign.A) : .assign.A
{
}

func .assign.B.cctor(
   self : .assign.B) : .assign.B
{
   self:_unused = 0;
   self:_x = 0;
}

func .assign.B.cdtor(
   self : .assign.B) : .assign.B
{
}

[vtbl]
const .assign.A_vtbl_inst : .assign.A_vtbl = {  };

[vtbl]
const .assign.B_vtbl_inst : .assign.B_vtbl = {  };

[vtbl]
const .assign.assignTester_vtbl_inst : .assign.assignTester_vtbl = { .assign.assignTester.run };

func .assign.A_sctor(
   self : .assign.A) : void
{
   .sht.core.object_sctor(self);
   self:_vtbl = .assign.A_vtbl_inst;
   .assign.A.cctor(self);
}

func .assign.A_sdtor(
   self : .assign.A) : void
{
   self:_vtbl = .assign.A_vtbl_inst;
   .assign.A.cdtor(self);
   .sht.core.object_sdtor(self);
}

func .assign.B_sctor(
   self : .assign.B) : void
{
   .sht.core.object_sctor(self);
   self:_vtbl = .assign.B_vtbl_inst;
   .assign.B.cctor(self);
}

func .assign.B_sdtor(
   self : .assign.B) : void
{
   self:_vtbl = .assign.B_vtbl_inst;
   .assign.B.cdtor(self);
   .sht.core.object_sdtor(self);
}

func .assign.assignTester_sctor(
   self : .assign.assignTester) : void
{
   .sht.cons.program_sctor(self);
   self:_vtbl = .assign.assignTester_vtbl_inst;
   .assign.assignTester.cctor(self);
}

func .assign.assignTester_sdtor(
   self : .assign.assignTester) : void
{
   self:_vtbl = .assign.assignTester_vtbl_inst;
   .assign.assignTester.cdtor(self);
   .sht.cons.program_sdtor(self);
}


ref "..\sht\prims.lh";

ref ".\main.ara.lh";
ref "..\sht\cons\program.ara.lh";
ref "..\sht\core\array.ara.lh";
ref "..\sht\core\object.ara.lh";
