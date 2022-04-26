func .sht.cons.iStream.printLn(
   self : .sht.cons.iStream,
   msg : str) : void
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
   msg : str) : void
{
   ._osCall(1,msg);
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
   args : str[]) : void
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

const iStream_vtbl_inst : .sht.cons.iStream_vtbl = { .sht.cons.iStream.printLn };

const program_vtbl_inst : .sht.cons.program_vtbl = { .sht.cons.program.run };

const stdout_vtbl_inst : .sht.cons.stdout_vtbl = { .sht.cons.stdout.printLn };


func ._osCall(code : str, payload : str) : void;

ref "program.ara.lh";
