const .nostromo.gDbgOut : .nostromo.debugOut = 0;

func .nostromo.debugOut.write(
   self : .nostromo.debugOut,
   msg : .sht.core.string) : void
{
   var l : int;
   l = .sht.core.string.length(msg);
   self:_uart:_vtbl:write8->(self:_uart,.sht.core.string.indexOpGet(msg,0));
}

func .nostromo.debugOut.cctor(
   self : .nostromo.debugOut) : .nostromo.debugOut
{
}

func .nostromo.debugOut.cdtor(
   self : .nostromo.debugOut) : .nostromo.debugOut
{
}

const .nostromo.debugOut_vtbl_inst : .nostromo.debugOut_vtbl = {  };

func .nostromo.debugOut_sctor(
   self : .nostromo.debugOut) : void
{
   .sht.core.object_sctor(self);
   self:_vtbl = .nostromo.debugOut_vtbl_inst;
   .nostromo.debugOut.cctor(self);
}

func .nostromo.debugOut_sdtor(
   self : .nostromo.debugOut) : void
{
   self:_vtbl = .nostromo.debugOut_vtbl_inst;
   .nostromo.debugOut.cdtor(self);
   .sht.core.object_sdtor(self);
}


func ._osCall(code : str, payload : str) : void;

ref "stringout.ara.lh";
ref "..\sht\core\object.ara.lh";
ref "..\sht\core\string.ara.lh";
