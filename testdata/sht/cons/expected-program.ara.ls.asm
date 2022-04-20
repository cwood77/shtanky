.seg code                 
.sht.cons.stdout.printLn: 
                          sub, rsp, 32   
                          mov, rcx, 1    ; shape:hoist imm from call
                          call, ._osCall ; (call label)
                          add, rsp, 32   
                          ret            

