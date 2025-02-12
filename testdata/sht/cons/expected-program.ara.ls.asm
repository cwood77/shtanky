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
                          sub, rsp, 32  ; 32 = (passing size)32 + (align pad)0
                          mov, rcx, rdx ;       (msg req for rcx) [splitter]
                          call, ._print ; (call label)
                          add, rsp, 32  ; 32 = (passing size)32 + (align pad)0
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

.seg code                    
.sht.cons.iStream_vtbl_inst: 
                             goto, .sht.cons.iStream.printLn

.seg code                    
.sht.cons.program_vtbl_inst: 
                             goto, .sht.cons.program.run

.seg code                   
.sht.cons.stdout_vtbl_inst: 
                            goto, .sht.cons.stdout.printLn

.seg code                
.sht.cons.iStream_sctor: 
                         push, rbp                                      
                         push, rbx                                      
                         mov, rbp, rsp                                  
                         sub, rsp, 40                                   ; 40 = (passing size)32 + (align pad)8
                         call, .sht.core.object_sctor                   ; (call label)
                         add, rsp, 40                                   ; 40 = (passing size)32 + (align pad)8
                         lea, rbx, qwordptr .sht.cons.iStream_vtbl_inst 
                         mov, [rcx], rbx                                ; =
                         sub, rsp, 40                                   ; 40 = (passing size)32 + (align pad)8
                         call, .sht.cons.iStream.cctor                  ; (call label)
                         add, rsp, 40                                   ; 40 = (passing size)32 + (align pad)8
                         mov, rsp, rbp                                  
                         pop, rbx                                       
                         pop, rbp                                       
                         ret                                            

.seg code                
.sht.cons.iStream_sdtor: 
                         push, rbp                                      
                         push, rbx                                      
                         mov, rbp, rsp                                  
                         lea, rbx, qwordptr .sht.cons.iStream_vtbl_inst 
                         mov, [rcx], rbx                                ; =
                         sub, rsp, 40                                   ; 40 = (passing size)32 + (align pad)8
                         call, .sht.cons.iStream.cdtor                  ; (call label)
                         add, rsp, 40                                   ; 40 = (passing size)32 + (align pad)8
                         sub, rsp, 40                                   ; 40 = (passing size)32 + (align pad)8
                         call, .sht.core.object_sdtor                   ; (call label)
                         add, rsp, 40                                   ; 40 = (passing size)32 + (align pad)8
                         mov, rsp, rbp                                  
                         pop, rbx                                       
                         pop, rbp                                       
                         ret                                            

.seg code                
.sht.cons.program_sctor: 
                         push, rbp                                      
                         push, rbx                                      
                         mov, rbp, rsp                                  
                         sub, rsp, 40                                   ; 40 = (passing size)32 + (align pad)8
                         call, .sht.core.object_sctor                   ; (call label)
                         add, rsp, 40                                   ; 40 = (passing size)32 + (align pad)8
                         lea, rbx, qwordptr .sht.cons.program_vtbl_inst 
                         mov, [rcx], rbx                                ; =
                         sub, rsp, 40                                   ; 40 = (passing size)32 + (align pad)8
                         call, .sht.cons.program.cctor                  ; (call label)
                         add, rsp, 40                                   ; 40 = (passing size)32 + (align pad)8
                         mov, rsp, rbp                                  
                         pop, rbx                                       
                         pop, rbp                                       
                         ret                                            

.seg code                
.sht.cons.program_sdtor: 
                         push, rbp                                      
                         push, rbx                                      
                         mov, rbp, rsp                                  
                         lea, rbx, qwordptr .sht.cons.program_vtbl_inst 
                         mov, [rcx], rbx                                ; =
                         sub, rsp, 40                                   ; 40 = (passing size)32 + (align pad)8
                         call, .sht.cons.program.cdtor                  ; (call label)
                         add, rsp, 40                                   ; 40 = (passing size)32 + (align pad)8
                         sub, rsp, 40                                   ; 40 = (passing size)32 + (align pad)8
                         call, .sht.core.object_sdtor                   ; (call label)
                         add, rsp, 40                                   ; 40 = (passing size)32 + (align pad)8
                         mov, rsp, rbp                                  
                         pop, rbx                                       
                         pop, rbp                                       
                         ret                                            

.seg code               
.sht.cons.stdout_sctor: 
                        push, rbp                                     
                        push, rbx                                     
                        mov, rbp, rsp                                 
                        sub, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        call, .sht.cons.iStream_sctor                 ; (call label)
                        add, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        lea, rbx, qwordptr .sht.cons.stdout_vtbl_inst 
                        mov, [rcx], rbx                               ; =
                        sub, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        call, .sht.cons.stdout.cctor                  ; (call label)
                        add, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        mov, rsp, rbp                                 
                        pop, rbx                                      
                        pop, rbp                                      
                        ret                                           

.seg code               
.sht.cons.stdout_sdtor: 
                        push, rbp                                     
                        push, rbx                                     
                        mov, rbp, rsp                                 
                        lea, rbx, qwordptr .sht.cons.stdout_vtbl_inst 
                        mov, [rcx], rbx                               ; =
                        sub, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        call, .sht.cons.stdout.cdtor                  ; (call label)
                        add, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        sub, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        call, .sht.cons.iStream_sdtor                 ; (call label)
                        add, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        mov, rsp, rbp                                 
                        pop, rbx                                      
                        pop, rbp                                      
                        ret                                           

