func .sht.core.string.set(
   self : .sht.core.string,
   literal : ptr) : void
{
   self:_p = ._strld(literal);
}

func .sht.core.string.length(
   self : .sht.core.string) : int
{
   ._strlen(self:_p);
}

func .sht.core.string.indexOpGet(
   self : .sht.core.string,
   i : int) : int
{
   ._strgidx(self:_p);
}

func .sht.core.string.indexOpSet(
   self : .sht.core.string,
   i : int,
   val : int) : void
{
   ._strsidx(self:_p,i,val);
}

func .sht.core.string.cctor(
   self : .sht.core.string) : .sht.core.string
{
   self:_p = 0;
}

func .sht.core.string.cdtor(
   self : .sht.core.string) : .sht.core.string
{
}

[vtbl]
const .sht.core.string_vtbl_inst : .sht.core.string_vtbl = {  };

func .sht.core.string_sctor(
   self : .sht.core.string) : void
{
   .sht.core.object_sctor(self);
   self:_vtbl = .sht.core.string_vtbl_inst;
   .sht.core.string.cctor(self);
}

func .sht.core.string_sdtor(
   self : .sht.core.string) : void
{
   self:_vtbl = .sht.core.string_vtbl_inst;
   .sht.core.string.cdtor(self);
   .sht.core.object_sdtor(self);
}


ref "..\prims.lh";

ref ".\object.ara.lh";
ref ".\string.ara.lh";
