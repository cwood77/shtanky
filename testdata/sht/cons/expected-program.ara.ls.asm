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
                         call, .sht.core.object_sctor                   ; (call label)
                         add, rsp, 32                                   
                         mov, r10, qwordptr .sht.cons.iStream_vtbl_inst ; codeshape decomp
                         mov, [rcx], r10                                ; =
                         sub, rsp, 32                                   
                         call, .sht.cons.iStream.cctor                  ; (call label)
                         add, rsp, 32                                   
                         ret                                            

.seg code                
.sht.cons.iStream_sdtor: 
                         mov, r10, qwordptr .sht.cons.iStream_vtbl_inst ; codeshape decomp
                         mov, [rcx], r10                                ; =
                         sub, rsp, 32                                   
                         call, .sht.cons.iStream.cdtor                  ; (call label)
                         add, rsp, 32                                   
                         sub, rsp, 32                                   
                         call, .sht.core.object_sdtor                   ; (call label)
                         add, rsp, 32                                   
                         ret                                            

.seg code                
.sht.cons.program_sctor: 
                         sub, rsp, 32                                   
                         call, .sht.core.object_sctor                   ; (call label)
                         add, rsp, 32                                   
                         mov, r10, qwordptr .sht.cons.program_vtbl_inst ; codeshape decomp
                         mov, [rcx], r10                                ; =
                         sub, rsp, 32                                   
                         call, .sht.cons.program.cctor                  ; (call label)
                         add, rsp, 32                                   
                         ret                                            

.seg code                
.sht.cons.program_sdtor: 
                         mov, r10, qwordptr .sht.cons.program_vtbl_inst ; codeshape decomp
                         mov, [rcx], r10                                ; =
                         sub, rsp, 32                                   
                         call, .sht.cons.program.cdtor                  ; (call label)
                         add, rsp, 32                                   
                         sub, rsp, 32                                   
                         call, .sht.core.object_sdtor                   ; (call label)
                         add, rsp, 32                                   
                         ret                                            

.seg code               
.sht.cons.stdout_sctor: 
                        sub, rsp, 32                                  
                        call, .sht.cons.iStream_sctor                 ; (call label)
                        add, rsp, 32                                  
                        mov, r10, qwordptr .sht.cons.stdout_vtbl_inst ; codeshape decomp
                        mov, [rcx], r10                               ; =
                        sub, rsp, 32                                  
                        call, .sht.cons.stdout.cctor                  ; (call label)
                        add, rsp, 32                                  
                        ret                                           

.seg code               
.sht.cons.stdout_sdtor: 
                        mov, r10, qwordptr .sht.cons.stdout_vtbl_inst ; codeshape decomp
                        mov, [rcx], r10                               ; =
                        sub, rsp, 32                                  
                        call, .sht.cons.stdout.cdtor                  ; (call label)
                        add, rsp, 32                                  
                        sub, rsp, 32                                  
                        call, .sht.cons.iStream_sdtor                 ; (call label)
                        add, rsp, 32                                  
                        ret                                           

