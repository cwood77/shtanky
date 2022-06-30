func .sht.core.loopInstBase.setDir(
   self : .sht.core.loopInstBase,
   up : bool) : void
{
   self:_up = up;
}

func .sht.core.loopInstBase.getCount(
   self : .sht.core.loopInstBase) : int
{
   return self:_count;
}

func .sht.core.loopInstBase.bump(
   self : .sht.core.loopInstBase) : void
{
   if(self:_up)
   {
      self:_count = self:_count + 1;
   }

}

func .sht.core.loopInstBase.break(
   self : .sht.core.loopInstBase) : void
{
}

func .sht.core.loopInstBase.breakWithReason(
   self : .sht.core.loopInstBase,
   reason : .sht.core.string) : void
{
}

func .sht.core.loopInstBase.continue(
   self : .sht.core.loopInstBase) : void
{
}

func .sht.core.loopInstBase.cctor(
   self : .sht.core.loopInstBase) : .sht.core.loopInstBase
{
   self:_up = 1;
   self:_count = 0;
}

func .sht.core.loopInstBase.cdtor(
   self : .sht.core.loopInstBase) : .sht.core.loopInstBase
{
}

func .sht.core.forLoopInst.setBounds(
   self : .sht.core.forLoopInst,
   start : int,
   stop : int) : void
{
}

func .sht.core.forLoopInst.inBounds(
   self : .sht.core.forLoopInst) : bool
{
}

func .sht.core.forLoopInst.getValue(
   self : .sht.core.forLoopInst) : int
{
}

func .sht.core.forLoopInst.cctor(
   self : .sht.core.forLoopInst) : .sht.core.forLoopInst
{
   self:_up = 1;
   self:_count = 0;
}

func .sht.core.forLoopInst.cdtor(
   self : .sht.core.forLoopInst) : .sht.core.forLoopInst
{
}

func .sht.core.whileLoopInst.getValue(
   self : .sht.core.whileLoopInst) : int
{
   return .sht.core.loopInstBase.getCount(self);
}

func .sht.core.whileLoopInst.cctor(
   self : .sht.core.whileLoopInst) : .sht.core.whileLoopInst
{
   self:_up = 1;
   self:_count = 0;
}

func .sht.core.whileLoopInst.cdtor(
   self : .sht.core.whileLoopInst) : .sht.core.whileLoopInst
{
}

[vtbl]
const .sht.core.forLoopInst_vtbl_inst : .sht.core.forLoopInst_vtbl = { .sht.core.loopInstBase.bump };

[vtbl]
const .sht.core.loopInstBase_vtbl_inst : .sht.core.loopInstBase_vtbl = { .sht.core.loopInstBase.bump };

[vtbl]
const .sht.core.whileLoopInst_vtbl_inst : .sht.core.whileLoopInst_vtbl = { .sht.core.loopInstBase.bump };

func .sht.core.forLoopInst_sctor(
   self : .sht.core.forLoopInst) : void
{
   .sht.core.loopInstBase_sctor(self);
   self:_vtbl = .sht.core.forLoopInst_vtbl_inst;
   .sht.core.forLoopInst.cctor(self);
}

func .sht.core.forLoopInst_sdtor(
   self : .sht.core.forLoopInst) : void
{
   self:_vtbl = .sht.core.forLoopInst_vtbl_inst;
   .sht.core.forLoopInst.cdtor(self);
   .sht.core.loopInstBase_sdtor(self);
}

func .sht.core.loopInstBase_sctor(
   self : .sht.core.loopInstBase) : void
{
   .sht.core.object_sctor(self);
   self:_vtbl = .sht.core.loopInstBase_vtbl_inst;
   .sht.core.loopInstBase.cctor(self);
}

func .sht.core.loopInstBase_sdtor(
   self : .sht.core.loopInstBase) : void
{
   self:_vtbl = .sht.core.loopInstBase_vtbl_inst;
   .sht.core.loopInstBase.cdtor(self);
   .sht.core.object_sdtor(self);
}

func .sht.core.whileLoopInst_sctor(
   self : .sht.core.whileLoopInst) : void
{
   .sht.core.loopInstBase_sctor(self);
   self:_vtbl = .sht.core.whileLoopInst_vtbl_inst;
   .sht.core.whileLoopInst.cctor(self);
}

func .sht.core.whileLoopInst_sdtor(
   self : .sht.core.whileLoopInst) : void
{
   self:_vtbl = .sht.core.whileLoopInst_vtbl_inst;
   .sht.core.whileLoopInst.cdtor(self);
   .sht.core.loopInstBase_sdtor(self);
}


ref "..\prims.lh";

ref ".\loopInst.ara.lh";
ref ".\object.ara.lh";
ref ".\string.ara.lh";
