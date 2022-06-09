# ideas for templ/gempl/cop

# templ
- this could produce template instances 'on demand' by araceli
- or, this could reproduce araceli loading and handle it itself...

# gempl
- why a separate app?
  - because it 'compiles down' just like araceli does
- design patterns
  - visitor
  - composite
  - null instance
  - decorator
- ariatic templates
  - tuple

# cop
- why a separate app?
  - to keep rules separate
  - to allow customization of rules easier?
  - do I have a good reason?
  - would allow checking without building.. maybe?
- apply rules at different levels (araceli, liam, etc)
- private access, etc.
- is it possible to implement all rules in an external app like this?

# example gempl syntax
```
.apply(name,var1,"var 2"); // whitespace is stripped here, quotes 'optional'

---meanwhile, in name.sat---
%params(name,title,items*)

foo($name)

%for($item,$items)
%endfor



// some actual templates using this syntax?

---byteBits.sat---
%params(name,b1,b2,b3,b4,b5,b6,b7,b8)

class $(name) {

   public has$(b1) : bool
   {
      .bitwiseAnd(_value,1);
   }

}

---visitor.sat---
%params(nodeRoot,vName,hVName)

%// adjust base class with virtual method
%injectMember($(nodeRoot)
%string
   public abstract acceptVisitor(v : $(vName)) : void;
%endstring)

%// implement acceptVisitor everywhere
%for($(node),%descendants($(nodeRoot)))
   %injectMember($(node)

%string
   public override acceptVisitor(v : $(vName)) : void
   {
      v->visit(self);
   }
%endString)

%endfor

---alloction-free module-local reflection---
%params(tag)

generic<T>
class my$(tag)Types {

   count() : int
   {
      return %reflect.count($(tag));
   }

   get(i : int) : T
   {
%for(i,%reflect($(tag)))
      if(i == $(_))
         return _inst$(_);
%endfor
   }

%for(i,%reflect($(tag)))
   _inst$(_) : $(i);
%endfor

}



------ so, what does a salome language need?
; ---- basic
.apply
%params
%//

; ---- for visitor
%injectMember
%string/%endstring
%for/%endfor
%baseOf
