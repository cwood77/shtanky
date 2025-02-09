func .sht.core.array<.sht.core.string>.resize(
   self : .sht.core.array<.sht.core.string>,
   size : int) : void
{
   self:_ptr = ._arrresize(self:_ptr,self:_length,size);
   self:_length = size;
}

func .sht.core.array<.sht.core.string>.getLength(
   self : .sht.core.array<.sht.core.string>) : int
{
}

func .sht.core.array<.sht.core.string>.append(
   self : .sht.core.array<.sht.core.string>,
   value : .sht.core.string) : void
{
}

func .sht.core.array<.sht.core.string>.indexOpGet(
   self : .sht.core.array<.sht.core.string>,
   i : int) : .sht.core.string
{
   ._arrgidx(self:_ptr,i);
}

func .sht.core.array<.sht.core.string>.indexOpSet(
   self : .sht.core.array<.sht.core.string>,
   i : int,
   val : .sht.core.string) : void
{
   ._arrsidx(self:_ptr,i,val);
}

func .sht.core.array<.sht.core.string>.cctor(
   self : .sht.core.array<.sht.core.string>) : .sht.core.array<.sht.core.string>
{
   self:_length = 0;
   self:_ptr = 0;
}

func .sht.core.array<.sht.core.string>.cdtor(
   self : .sht.core.array<.sht.core.string>) : .sht.core.array<.sht.core.string>
{
}

[vtbl]
const .sht.core.array<.sht.core.string>_vtbl_inst : .sht.core.array<.sht.core.string>_vtbl = {  };

func .sht.core.array<.sht.core.string>_sctor(
   self : .sht.core.array<.sht.core.string>) : void
{
   .sht.core.object_sctor(self);
   self:_vtbl = .sht.core.array<.sht.core.string>_vtbl_inst;
   .sht.core.array<.sht.core.string>.cctor(self);
}

func .sht.core.array<.sht.core.string>_sdtor(
   self : .sht.core.array<.sht.core.string>) : void
{
   self:_vtbl = .sht.core.array<.sht.core.string>_vtbl_inst;
   .sht.core.array<.sht.core.string>.cdtor(self);
   .sht.core.object_sdtor(self);
}


ref "..\prims.lh";

ref ".\array.ara.lh";
ref ".\object.ara.lh";
ref ".\string.ara.lh";
