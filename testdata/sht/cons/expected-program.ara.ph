interface iStream {

   abstract printLn(msg : str) : void;

}

class stdout : iStream {

   override printLn(msg : str) : void
   {
      ._osCall(1,msg);
   }

}

abstract program {

   public _out : iStream;
   abstract run(args : str[]) : void;

}

