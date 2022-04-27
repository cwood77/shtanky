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
.sht.cons.iStream_vtbl_inst:
.data, .sht.cons.iStream.printLn 

.seg const
.sht.cons.program_vtbl_inst:
.data, .sht.cons.program.run 

.seg const
.sht.cons.stdout_vtbl_inst:
.data, .sht.cons.stdout.printLn 

.seg code                
.sht.cons.iStream_sctor: 
                         sub, rsp, 32                            
                         call, .sht.core.object_sctor            ; (call label)
                         add, rsp, 32                            
                         mov, [rcx], .sht.cons.iStream_vtbl_inst ; =
                         sub, rsp, 32                            
                         call, .sht.cons.iStream.cctor           ; (call label)
                         add, rsp, 32                            
                         ret                                     

.seg code                
.sht.cons.program_sctor: 
                         sub, rsp, 32                            
                         call, .sht.core.object_sctor            ; (call label)
                         add, rsp, 32                            
                         mov, [rcx], .sht.cons.program_vtbl_inst ; =
                         sub, rsp, 32                            
                         call, .sht.cons.program.cctor           ; (call label)
                         add, rsp, 32                            
                         ret                                     

.seg code               
.sht.cons.stdout_sctor: 
                        sub, rsp, 32                           
                        call, .sht.cons.iStream_sctor          ; (call label)
                        add, rsp, 32                           
                        mov, [rcx], .sht.cons.stdout_vtbl_inst ; =
                        sub, rsp, 32                           
                        call, .sht.cons.stdout.cctor           ; (call label)
                        add, rsp, 32                           
                        ret                                    

