.seg code                  
.sht.cons.iStream.printLn: 
                           ret

.seg code                
.sht.cons.iStream.cctor: 
                         ret

.seg code                
.sht.cons.iStream.cdtor: 
                         ret

.seg code                 
.sht.cons.stdout.printLn: 
                          sub, rsp, 32   
                          mov, rcx, 1    ; shape:hoist imm from call
                          call, ._osCall ; (call label)
                          add, rsp, 32   
                          ret            

.seg code               
.sht.cons.stdout.cctor: 
                        ret

.seg code               
.sht.cons.stdout.cdtor: 
                        ret

.seg code              
.sht.cons.program.run: 
                       ret

.seg code                
.sht.cons.program.cctor: 
                         ret

.seg code                
.sht.cons.program.cdtor: 
                         ret

.seg const
iStream_vtbl_inst:
.data, .sht.cons.iStream.printLn 

.seg const
program_vtbl_inst:
.data, .sht.cons.program.run 

.seg const
stdout_vtbl_inst:
.data, .sht.cons.stdout.printLn 

