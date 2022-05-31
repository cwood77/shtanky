
class byte {

   public read(location : int) : int
   {
   }

   public write(location : int) : int
   {
   }

}

abstract flagsByteBase {

   flagsByteBase(location : int) : void
   {
   }

   public void read() : void
   {
   }

   public void write() : void
   {
   }

}

%params(name,b8,b7,b6,b5,b4,b3,b2,b1) // but this can't handle multi-bit flags!

class $(name) : flagsByteBase {

   public has$(b8)() : bool
   {
   }

}

.apply memoryMap {
   {
      name: 'isr'
      offset: 010b
      dir: r
      comment: 'interrupt status register'
      fields: [
         {
            comment: 'receiver's FIFO trigger level'
            width: 2
            offset: 0
         },
         {
            dontcare;
         }
      ]
   }
}

%decl(int,offset,0)
%for(field,$(fields))      // but how count current bit offset?

%if($(field.width),=,1)
%else
   public get$(field.name)() : int
   {
      return .bitwiseAnd(_value,$(offset),$(field.width));
      %add(offset,$(field.width))
   }
%endif

%endfor

| addr | dir | comment                   | name | b7   | b6   | b5   | b4   | b3   | b2   | b1   | b0   |
| 000b |  R  | Receiver Holding Register |  RHR |               Character               |
| 010b |  RW | FIFO Control Register     |  FCR |  Rx FIFO FL |      |

Ug, this is uglier than I expected

myUart->getLsr()->read();
x = myUart->getLsr()->hasTxEmpty();


What if I did the YAML-like memory map idea, but put offsets in the YAML.  I could then offload the yaml to a separate file

Is writing the YAML easier/better than writing the code?
