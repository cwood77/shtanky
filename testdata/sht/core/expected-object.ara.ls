func .sht.core.object.cctor(
   self : .sht.core.object) : .sht.core.object
{
}

func .sht.core.object.cdtor(
   self : .sht.core.object) : .sht.core.object
{
}

const .sht.core.object_vtbl_inst : .sht.core.object_vtbl = {  };

func .sht.core.object_sctor(
   self : .sht.core.object) : void
{
   self:_vtbl = .sht.core.object_vtbl_inst;
   .sht.core.object.cctor(self);
}

func .sht.core.object_sdtor(
   self : .sht.core.object) : void
{
   self:_vtbl = .sht.core.object_vtbl_inst;
   .sht.core.object.cdtor(self);
}


ref "..\prims.lh";

ref ".\object.ara.lh";
