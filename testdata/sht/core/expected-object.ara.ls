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


func ._osCall(code : str, payload : str) : void;
func ._strld(litoff : ptr) : ptr;
func ._strlen(s : ptr) : int;
func ._strgidx(s : ptr, i : int) : int;
func ._strsidx(s : ptr, i : int, c : int) : ptr;

ref "object.ara.lh";
