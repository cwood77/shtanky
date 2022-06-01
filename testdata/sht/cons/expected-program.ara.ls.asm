.seg code                  
.sht.cons.iStream.printLn: 
                           push, rbp
                           mov, rbp, rsp
                           mov, rsp, rbp
                           pop, rbp
                           ret

.seg code                
.sht.cons.iStream.cctor: 
                         push, rbp
                         mov, rbp, rsp
                         mov, rsp, rbp
                         pop, rbp
                         ret

.seg code                
.sht.cons.iStream.cdtor: 
                         push, rbp
                         mov, rbp, rsp
                         mov, rsp, rbp
                         pop, rbp
                         ret

.seg code                 
.sht.cons.stdout.printLn: 
                          push, rbp     
                          mov, rbp, rsp 
                          sub, rsp, 32  
                          call, ._print ; (call label)
                          add, rsp, 32  
                          mov, rsp, rbp 
                          pop, rbp      
                          ret           

.seg code               
.sht.cons.stdout.cctor: 
                        push, rbp
                        mov, rbp, rsp
                        mov, rsp, rbp
                        pop, rbp
                        ret

.seg code               
.sht.cons.stdout.cdtor: 
                        push, rbp
                        mov, rbp, rsp
                        mov, rsp, rbp
                        pop, rbp
                        ret

.seg code              
.sht.cons.program.run: 
                       push, rbp
                       mov, rbp, rsp
                       mov, rsp, rbp
                       pop, rbp
                       ret

.seg code                
.sht.cons.program.cctor: 
                         push, rbp
                         mov, rbp, rsp
                         mov, rsp, rbp
                         pop, rbp
                         ret

.seg code                
.sht.cons.program.cdtor: 
                         push, rbp
                         mov, rbp, rsp
                         mov, rsp, rbp
                         pop, rbp
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
                         push, rbp                                      
                         mov, rbp, rsp                                  
                         sub, rsp, 32                                   
                         call, .sht.core.object_sctor                   ; (call label)
                         add, rsp, 32                                   
                         mov, r10, qwordptr .sht.cons.iStream_vtbl_inst ; codeshape decomp
                         mov, [rcx], r10                                ; =
                         sub, rsp, 32                                   
                         call, .sht.cons.iStream.cctor                  ; (call label)
                         add, rsp, 32                                   
                         mov, rsp, rbp                                  
                         pop, rbp                                       
                         ret                                            

.seg code                
.sht.cons.iStream_sdtor: 
                         push, rbp                                      
                         mov, rbp, rsp                                  
                         mov, r10, qwordptr .sht.cons.iStream_vtbl_inst ; codeshape decomp
                         mov, [rcx], r10                                ; =
                         sub, rsp, 32                                   
                         call, .sht.cons.iStream.cdtor                  ; (call label)
                         add, rsp, 32                                   
                         sub, rsp, 32                                   
                         call, .sht.core.object_sdtor                   ; (call label)
                         add, rsp, 32                                   
                         mov, rsp, rbp                                  
                         pop, rbp                                       
                         ret                                            

.seg code                
.sht.cons.program_sctor: 
                         push, rbp                                      
                         mov, rbp, rsp                                  
                         sub, rsp, 32                                   
                         call, .sht.core.object_sctor                   ; (call label)
                         add, rsp, 32                                   
                         mov, r10, qwordptr .sht.cons.program_vtbl_inst ; codeshape decomp
                         mov, [rcx], r10                                ; =
                         sub, rsp, 32                                   
                         call, .sht.cons.program.cctor                  ; (call label)
                         add, rsp, 32                                   
                         mov, rsp, rbp                                  
                         pop, rbp                                       
                         ret                                            

.seg code                
.sht.cons.program_sdtor: 
                         push, rbp                                      
                         mov, rbp, rsp                                  
                         mov, r10, qwordptr .sht.cons.program_vtbl_inst ; codeshape decomp
                         mov, [rcx], r10                                ; =
                         sub, rsp, 32                                   
                         call, .sht.cons.program.cdtor                  ; (call label)
                         add, rsp, 32                                   
                         sub, rsp, 32                                   
                         call, .sht.core.object_sdtor                   ; (call label)
                         add, rsp, 32                                   
                         mov, rsp, rbp                                  
                         pop, rbp                                       
                         ret                                            

.seg code               
.sht.cons.stdout_sctor: 
                        push, rbp                                     
                        mov, rbp, rsp                                 
                        sub, rsp, 32                                  
                        call, .sht.cons.iStream_sctor                 ; (call label)
                        add, rsp, 32                                  
                        mov, r10, qwordptr .sht.cons.stdout_vtbl_inst ; codeshape decomp
                        mov, [rcx], r10                               ; =
                        sub, rsp, 32                                  
                        call, .sht.cons.stdout.cctor                  ; (call label)
                        add, rsp, 32                                  
                        mov, rsp, rbp                                 
                        pop, rbp                                      
                        ret                                           

.seg code               
.sht.cons.stdout_sdtor: 
                        push, rbp                                     
                        mov, rbp, rsp                                 
                        mov, r10, qwordptr .sht.cons.stdout_vtbl_inst ; codeshape decomp
                        mov, [rcx], r10                               ; =
                        sub, rsp, 32                                  
                        call, .sht.cons.stdout.cdtor                  ; (call label)
                        add, rsp, 32                                  
                        sub, rsp, 32                                  
                        call, .sht.cons.iStream_sdtor                 ; (call label)
                        add, rsp, 32                                  
                        mov, rsp, rbp                                 
                        pop, rbp                                      
                        ret                                           

