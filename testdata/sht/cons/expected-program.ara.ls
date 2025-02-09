func .sht.cons.iStream.printLn(
   self : .sht.cons.iStream,
   msg : .sht.core.string) : void
{
}

func .sht.cons.iStream.cctor(
   self : .sht.cons.iStream) : .sht.cons.iStream
{
}

func .sht.cons.iStream.cdtor(
   self : .sht.cons.iStream) : .sht.cons.iStream
{
}

func .sht.cons.stdout.printLn(
   self : .sht.cons.stdout,
   msg : .sht.core.string) : void
{
   ._print(msg);
}

func .sht.cons.stdout.cctor(
   self : .sht.cons.stdout) : .sht.cons.stdout
{
}

func .sht.cons.stdout.cdtor(
   self : .sht.cons.stdout) : .sht.cons.stdout
{
}

func .sht.cons.program.run(
   self : .sht.cons.program,
   args : .sht.core.array<.sht.core.string>) : void
{
}

func .sht.cons.program.cctor(
   self : .sht.cons.program) : .sht.cons.program
{
}

func .sht.cons.program.cdtor(
   self : .sht.cons.program) : .sht.cons.program
{
}

[vtbl]
const .sht.cons.iStream_vtbl_inst : .sht.cons.iStream_vtbl = { .sht.cons.iStream.printLn };

[vtbl]
const .sht.cons.program_vtbl_inst : .sht.cons.program_vtbl = { .sht.cons.program.run };

[vtbl]
const .sht.cons.stdout_vtbl_inst : .sht.cons.stdout_vtbl = { .sht.cons.stdout.printLn };

func .sht.cons.iStream_sctor(
   self : .sht.cons.iStream) : void
{
   .sht.core.object_sctor(self);
   self:_vtbl = .sht.cons.iStream_vtbl_inst;
   .sht.cons.iStream.cctor(self);
}

func .sht.cons.iStream_sdtor(
   self : .sht.cons.iStream) : void
{
   self:_vtbl = .sht.cons.iStream_vtbl_inst;
   .sht.cons.iStream.cdtor(self);
   .sht.core.object_sdtor(self);
}

func .sht.cons.program_sctor(
   self : .sht.cons.program) : void
{
   .sht.core.object_sctor(self);
   self:_vtbl = .sht.cons.program_vtbl_inst;
   .sht.cons.program.cctor(self);
}

func .sht.cons.program_sdtor(
   self : .sht.cons.program) : void
{
   self:_vtbl = .sht.cons.program_vtbl_inst;
   .sht.cons.program.cdtor(self);
   .sht.core.object_sdtor(self);
}

func .sht.cons.stdout_sctor(
   self : .sht.cons.stdout) : void
{
   .sht.cons.iStream_sctor(self);
   self:_vtbl = .sht.cons.stdout_vtbl_inst;
   .sht.cons.stdout.cctor(self);
}

func .sht.cons.stdout_sdtor(
   self : .sht.cons.stdout) : void
{
   self:_vtbl = .sht.cons.stdout_vtbl_inst;
   .sht.cons.stdout.cdtor(self);
   .sht.cons.iStream_sdtor(self);
}


ref "..\prims.lh";

ref ".\program.ara.lh";
ref "..\core\array.ara.lh";
ref "..\core\object.ara.lh";
ref "..\core\string.ara.lh";
