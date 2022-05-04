[program] 
class assignTester : .sht.cons.program {

   override run(args : str[]) : void
   {
   }
   private readFromSubObject(a : A, v : void) : void
   {
      v = a:b;
   }
   private writeIntoSubObject() : void
   {
      var a : A;
      a:b = 7;
   }
   private readFromSubSubObject(a : A, wrong : void) : void
   {
      wrong = a:b:x;
   }
   private writeIntoSubSubObject() : void
   {
      var wrong : A;
      wrong:b:x = 7;
   }

}

class A {

   public x : void;
   public b : B;

}

class B {

   public unused : void;
   public x : void;

}

