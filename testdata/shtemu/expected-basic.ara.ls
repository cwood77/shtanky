const ._strLit_startingProgra_etc_0 : str = "starting program";

const ._strLit_insideLoop_1 : str = "inside loop";

const ._strLit_leavingProgram_2 : str = "leaving program";

func .shtemu.foo.run(
   self : .shtemu.foo,
   args : .sht.core.array<.sht.core.string>) : void
{
   ._print(._strLit_startingProgra_etc_0);
   {
      var i : .sht.core.whileLoopInst;
      .sht.core.whileLoopInst_sctor(i);
      while[i](.sht.core.whileLoopInst.getValue(i) < 2)
      {
         ._print(._strLit_insideLoop_1);
      }

      .sht.core.whileLoopInst_sdtor(i);
   }

   ._print(._strLit_leavingProgram_2);
}

func .shtemu.foo.cctor(
   self : .shtemu.foo) : .shtemu.foo
{
}

func .shtemu.foo.cdtor(
   self : .shtemu.foo) : .shtemu.foo
{
}

[vtbl]
const .shtemu.foo_vtbl_inst : .shtemu.foo_vtbl = { .shtemu.foo.run };

func .shtemu.foo_sctor(
   self : .shtemu.foo) : void
{
   .sht.cons.program_sctor(self);
   self:_vtbl = .shtemu.foo_vtbl_inst;
   .shtemu.foo.cctor(self);
}

func .shtemu.foo_sdtor(
   self : .shtemu.foo) : void
{
   self:_vtbl = .shtemu.foo_vtbl_inst;
   .shtemu.foo.cdtor(self);
   .sht.cons.program_sdtor(self);
}


ref "..\sht\prims.lh";

ref "..\sht\cons\program.ara.lh";
ref "..\sht\core\array.ara.lh";
ref "..\sht\core\loopInst.ara.lh";
ref ".\basic.ara.lh";
