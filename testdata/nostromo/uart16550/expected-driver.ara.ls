func .nostromo.uart16550.fakeBase.write8(
   self : .nostromo.uart16550.fakeBase,
   byte : ptr) : void
{
}

func .nostromo.uart16550.fakeBase.cctor(
   self : .nostromo.uart16550.fakeBase) : .nostromo.uart16550.fakeBase
{
}

func .nostromo.uart16550.fakeBase.cdtor(
   self : .nostromo.uart16550.fakeBase) : .nostromo.uart16550.fakeBase
{
}

func .nostromo.uart16550.driver.write8(
   self : .nostromo.uart16550.driver,
   byte : ptr) : void
{
}

func .nostromo.uart16550.driver.cctor(
   self : .nostromo.uart16550.driver) : .nostromo.uart16550.driver
{
}

func .nostromo.uart16550.driver.cdtor(
   self : .nostromo.uart16550.driver) : .nostromo.uart16550.driver
{
}

[vtbl]
const .nostromo.uart16550.driver_vtbl_inst : .nostromo.uart16550.driver_vtbl = { .nostromo.uart16550.driver.write8 };

[vtbl]
const .nostromo.uart16550.fakeBase_vtbl_inst : .nostromo.uart16550.fakeBase_vtbl = { .nostromo.uart16550.fakeBase.write8 };

func .nostromo.uart16550.driver_sctor(
   self : .nostromo.uart16550.driver) : void
{
   .nostromo.uart16550.fakeBase_sctor(self);
   self:_vtbl = .nostromo.uart16550.driver_vtbl_inst;
   .nostromo.uart16550.driver.cctor(self);
}

func .nostromo.uart16550.driver_sdtor(
   self : .nostromo.uart16550.driver) : void
{
   self:_vtbl = .nostromo.uart16550.driver_vtbl_inst;
   .nostromo.uart16550.driver.cdtor(self);
   .nostromo.uart16550.fakeBase_sdtor(self);
}

func .nostromo.uart16550.fakeBase_sctor(
   self : .nostromo.uart16550.fakeBase) : void
{
   .sht.core.object_sctor(self);
   self:_vtbl = .nostromo.uart16550.fakeBase_vtbl_inst;
   .nostromo.uart16550.fakeBase.cctor(self);
}

func .nostromo.uart16550.fakeBase_sdtor(
   self : .nostromo.uart16550.fakeBase) : void
{
   self:_vtbl = .nostromo.uart16550.fakeBase_vtbl_inst;
   .nostromo.uart16550.fakeBase.cdtor(self);
   .sht.core.object_sdtor(self);
}


ref "..\..\sht\prims.lh";

ref ".\driver.ara.lh";
ref "..\..\sht\core\object.ara.lh";
