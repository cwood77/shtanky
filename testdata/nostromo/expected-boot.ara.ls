func .nostromo.bootstrapper.run(
   self : .nostromo.bootstrapper,
   args : .sht.core.array<.sht.core.string>) : void
{
}

func .nostromo.bootstrapper.cctor(
   self : .nostromo.bootstrapper) : .nostromo.bootstrapper
{
}

func .nostromo.bootstrapper.cdtor(
   self : .nostromo.bootstrapper) : .nostromo.bootstrapper
{
}

const .nostromo.bootstrapper_vtbl_inst : .nostromo.bootstrapper_vtbl = { .nostromo.bootstrapper.run };

func .nostromo.bootstrapper_sctor(
   self : .nostromo.bootstrapper) : void
{
   .sht.cons.program_sctor(self);
   self:_vtbl = .nostromo.bootstrapper_vtbl_inst;
   .nostromo.bootstrapper.cctor(self);
}

func .nostromo.bootstrapper_sdtor(
   self : .nostromo.bootstrapper) : void
{
   self:_vtbl = .nostromo.bootstrapper_vtbl_inst;
   .nostromo.bootstrapper.cdtor(self);
   .sht.cons.program_sdtor(self);
}


ref "..\sht\prims.lh";

ref ".\boot.ara.lh";
ref "..\sht\cons\program.ara.lh";
ref "..\sht\core\array.ara.lh";
