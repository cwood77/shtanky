func .philemon.bootstrap.run(
   self : .philemon.bootstrap,
   args : .sht.core.array<.sht.core.string>) : void
{
   var L : .philemon.list<str>;
   .philemon.list<str>_sctor(L);
   var A2 : .sht.core.array<.sht.core.array<.sht.core.string>>;
   .sht.core.array<.sht.core.array<.sht.core.string>>_sctor(A2);
   var L2 : .sht.core.array<.philemon.list<str>>;
   .sht.core.array<.philemon.list<str>>_sctor(L2);
   .sht.core.array<.philemon.list<str>>_sdtor(L2);
   .sht.core.array<.sht.core.array<.sht.core.string>>_sdtor(A2);
   .philemon.list<str>_sdtor(L);
}

func .philemon.bootstrap.cctor(
   self : .philemon.bootstrap) : .philemon.bootstrap
{
}

func .philemon.bootstrap.cdtor(
   self : .philemon.bootstrap) : .philemon.bootstrap
{
}

func .philemon.list<str>.cctor(
   self : .philemon.list<str>) : .philemon.list<str>
{
}

func .philemon.list<str>.cdtor(
   self : .philemon.list<str>) : .philemon.list<str>
{
}

[vtbl]
const .philemon.bootstrap_vtbl_inst : .philemon.bootstrap_vtbl = { .philemon.bootstrap.run };

[vtbl]
const .philemon.list<str>_vtbl_inst : .philemon.list<str>_vtbl = {  };

func .philemon.bootstrap_sctor(
   self : .philemon.bootstrap) : void
{
   .sht.cons.program_sctor(self);
   self:_vtbl = .philemon.bootstrap_vtbl_inst;
   .philemon.bootstrap.cctor(self);
}

func .philemon.bootstrap_sdtor(
   self : .philemon.bootstrap) : void
{
   self:_vtbl = .philemon.bootstrap_vtbl_inst;
   .philemon.bootstrap.cdtor(self);
   .sht.cons.program_sdtor(self);
}

func .philemon.list<str>_sctor(
   self : .philemon.list<str>) : void
{
   .sht.core.object_sctor(self);
   self:_vtbl = .philemon.list<str>_vtbl_inst;
   .philemon.list<str>.cctor(self);
}

func .philemon.list<str>_sdtor(
   self : .philemon.list<str>) : void
{
   self:_vtbl = .philemon.list<str>_vtbl_inst;
   .philemon.list<str>.cdtor(self);
   .sht.core.object_sdtor(self);
}


ref "..\sht\prims.lh";

ref ".\list.ara.lh";
ref "..\sht\cons\program.ara.lh";
ref "..\sht\core\array.ara.lh";
ref "..\sht\core\object.ara.lh";
