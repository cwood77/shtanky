.seg code    
.entrypoint: 
             push, rbp                    
             push, rbx                    
             push, rdi                    
             mov, rbp, rsp                
             sub, rsp, 24                 
             mov, rbx, rcx                ; (preserve) [combiner]
             lea, rcx, [rbp-8]            ; cout
             sub, rsp, 32                 
             call, .sht.cons.stdout_sctor ; (call label)
             add, rsp, 32                 
             mov, rdi, rcx                ; (preserve) [combiner]
             lea, rcx, [rbp-24]           ; obj0
             sub, rsp, 32                 
             call, .shtemu.foo_sctor      ; (call label)
             add, rsp, 32                 
             mov, [rcx+8], rdi            ; =
             sub, rsp, 32                 
             mov, rdx, rbx                ;       (args req for rdx) [splitter]
             call, [rcx]                  ; vtbl call to .shtemu.foo_vtbl::run
             add, rsp, 32                 
             sub, rsp, 32                 
             call, .shtemu.foo_sdtor      ; (call label)
             add, rsp, 32                 
             sub, rsp, 32                 
             mov, rcx, rdi                ; (restore [combiner])
             call, .sht.cons.stdout_sdtor ; (call label)
             add, rsp, 32                 
             mov, rsp, rbp                
             pop, rdi                     
             pop, rbx                     
             pop, rbp                     
             ret                          

.seg code                    
.shtemu.consoleTarget.cctor: 
                             push, rbp
                             mov, rbp, rsp
                             mov, rsp, rbp
                             pop, rbp
                             ret

.seg code                    
.shtemu.consoleTarget.cdtor: 
                             push, rbp
                             mov, rbp, rsp
                             mov, rsp, rbp
                             pop, rbp
                             ret

.seg code
.shtemu.consoleTarget_vtbl_inst:

.seg code                    
.shtemu.consoleTarget_sctor: 
                             push, rbp                                          
                             push, rbx                                          
                             mov, rbp, rsp                                      
                             sub, rsp, 32                                       
                             call, .sht.core.object_sctor                       ; (call label)
                             add, rsp, 32                                       
                             lea, rbx, qwordptr .shtemu.consoleTarget_vtbl_inst 
                             mov, [rcx], rbx                                    ; =
                             sub, rsp, 32                                       
                             call, .shtemu.consoleTarget.cctor                  ; (call label)
                             add, rsp, 32                                       
                             mov, rsp, rbp                                      
                             pop, rbx                                           
                             pop, rbp                                           
                             ret                                                

.seg code                    
.shtemu.consoleTarget_sdtor: 
                             push, rbp                                          
                             push, rbx                                          
                             mov, rbp, rsp                                      
                             lea, rbx, qwordptr .shtemu.consoleTarget_vtbl_inst 
                             mov, [rcx], rbx                                    ; =
                             sub, rsp, 32                                       
                             call, .shtemu.consoleTarget.cdtor                  ; (call label)
                             add, rsp, 32                                       
                             sub, rsp, 32                                       
                             call, .sht.core.object_sdtor                       ; (call label)
                             add, rsp, 32                                       
                             mov, rsp, rbp                                      
                             pop, rbx                                           
                             pop, rbp                                           
                             ret                                                

