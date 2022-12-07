const ._strLit_For_0 : str = "===for";

const ._strLit_14_1 : str = "(1-4)";

const ._strLit_0_2 : str = "  0";

const ._strLit_1_3 : str = "  1";

const ._strLit_2_4 : str = "  2";

const ._strLit_3_5 : str = "  3";

const ._strLit_4_6 : str = "  4";

func .uats.loopTest.run(
   self : .uats.loopTest,
   args : .sht.core.array<.sht.core.string>) : void
{
   ._print(._strLit_For_0);
   ._print(._strLit_14_1);
   {
      var i : .sht.core.forLoopInst;
      .sht.core.forLoopInst_sctor(i);
      for[i](1,4)
      {
         .uats.loopTest.printI(self,.sht.core.forLoopInst.getValue(i));
      }

      .sht.core.forLoopInst_sdtor(i);
   }

}

func .uats.loopTest.printI(
   self : .uats.loopTest,
   i : int) : void
{
   if(i < 1)
   {
      ._print(._strLit_0_2);
   }


   else if(i < 2)
   {
      ._print(._strLit_1_3);
   }


   else if(i < 3)
   {
      ._print(._strLit_2_4);
   }


   else if(i < 4)
   {
      ._print(._strLit_3_5);
   }


   else
   {
      ._print(._strLit_4_6);
   }

}

func .uats.loopTest.cctor(
   self : .uats.loopTest) : .uats.loopTest
{
}

func .uats.loopTest.cdtor(
   self : .uats.loopTest) : .uats.loopTest
{
}

[vtbl]
const .uats.loopTest_vtbl_inst : .uats.loopTest_vtbl = { .uats.loopTest.run };

func .uats.loopTest_sctor(
   self : .uats.loopTest) : void
{
   .sht.cons.program_sctor(self);
   self:_vtbl = .uats.loopTest_vtbl_inst;
   .uats.loopTest.cctor(self);
}

func .uats.loopTest_sdtor(
   self : .uats.loopTest) : void
{
   self:_vtbl = .uats.loopTest_vtbl_inst;
   .uats.loopTest.cdtor(self);
   .sht.cons.program_sdtor(self);
}


ref "..\sht\prims.lh";

ref "..\sht\cons\program.ara.lh";
ref "..\sht\core\array.ara.lh";
ref "..\sht\core\loopInst.ara.lh";
ref ".\loop.ara.lh";
