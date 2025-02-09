const ._strLit_If_0 : str = "===if";

const ._strLit_1_1 : str = "1)";

const ._strLit_2_2 : str = "2)";

const ._strLit_3_3 : str = "3)";

const ._strLit_4_4 : str = "4)";

const ._strLit__5 : str = "---";

const ._strLit_1_6 : str = "1";

const ._strLit_2_7 : str = "2";

const ._strLit_34_8 : str = "3-4";

const ._strLit_13_9 : str = "1-3";

const ._strLit_4_10 : str = "4";

func .uats.ifTest.run(
   self : .uats.ifTest,
   args : .sht.core.array<.sht.core.string>) : void
{
   ._print(._strLit_If_0);
   ._print(._strLit_1_1);
   .uats.ifTest.doTest(self,1);
   ._print(._strLit_2_2);
   .uats.ifTest.doTest(self,2);
   ._print(._strLit_3_3);
   .uats.ifTest.doTest(self,3);
   ._print(._strLit_4_4);
   .uats.ifTest.doTest(self,4);
}

func .uats.ifTest.doTest(
   self : .uats.ifTest,
   flag : int) : void
{
   ._print(._strLit__5);
   if(flag < 2)
   {
      ._print(._strLit_1_6);
   }


   else if(flag < 3)
   {
      ._print(._strLit_2_7);
   }


   else
   {
      ._print(._strLit_34_8);
   }

   if(flag < 4)
   {
      ._print(._strLit_13_9);
   }


   else
   {
      ._print(._strLit_4_10);
   }

}

func .uats.ifTest.cctor(
   self : .uats.ifTest) : .uats.ifTest
{
}

func .uats.ifTest.cdtor(
   self : .uats.ifTest) : .uats.ifTest
{
}

[vtbl]
const .uats.ifTest_vtbl_inst : .uats.ifTest_vtbl = { .uats.ifTest.run };

func .uats.ifTest_sctor(
   self : .uats.ifTest) : void
{
   .sht.cons.program_sctor(self);
   self:_vtbl = .uats.ifTest_vtbl_inst;
   .uats.ifTest.cctor(self);
}

func .uats.ifTest_sdtor(
   self : .uats.ifTest) : void
{
   self:_vtbl = .uats.ifTest_vtbl_inst;
   .uats.ifTest.cdtor(self);
   .sht.cons.program_sdtor(self);
}


ref "..\sht\prims.lh";

ref "..\sht\cons\program.ara.lh";
ref "..\sht\core\array.ara.lh";
ref ".\if.ara.lh";
