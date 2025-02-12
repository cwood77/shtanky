.seg code    
.entrypoint: 
             push, rbp                          
             push, rbx                          
             push, rdi                          
             mov, rbp, rsp                      
             sub, rsp, 24                       
             mov, rbx, rcx                      ; (preserve) [combiner]
             lea, rcx, [rbp-8]                  ; cout
             sub, rsp, 40                       
             call, .sht.cons.stdout_sctor       ; (call label)
             add, rsp, 40                       
             mov, rdi, rcx                      ; (preserve) [combiner]
             lea, rcx, [rbp-24]                 ; obj0
             sub, rsp, 40                       
             call, .nostromo.bootstrapper_sctor ; (call label)
             add, rsp, 40                       
             mov, [rcx+8], rdi                  ; =
             sub, rsp, 40                       
             mov, rdx, rbx                      ;       (args req for rdx) [splitter]
             call, [rcx]                        ; vtbl call to .nostromo.bootstrapper_vtbl::run
             add, rsp, 40                       
             sub, rsp, 40                       
             call, .nostromo.bootstrapper_sdtor ; (call label)
             add, rsp, 40                       
             sub, rsp, 40                       
             mov, rcx, rdi                      ; (restore [combiner])
             call, .sht.cons.stdout_sdtor       ; (call label)
             add, rsp, 40                       
             mov, rsp, rbp                      
             pop, rdi                           
             pop, rbx                           
             pop, rbp                           
             ret                                

.seg code                      
.nostromo.consoleTarget.cctor: 
                               push, rbp
                               mov, rbp, rsp
                               mov, rsp, rbp
                               pop, rbp
                               ret

.seg code                      
.nostromo.consoleTarget.cdtor: 
                               push, rbp
                               mov, rbp, rsp
                               mov, rsp, rbp
                               pop, rbp
                               ret

.seg code
.nostromo.consoleTarget_vtbl_inst:

.seg code                      
.nostromo.consoleTarget_sctor: 
                               push, rbp                                            
                               push, rbx                                            
                               mov, rbp, rsp                                        
                               sub, rsp, 40                                         
                               call, .sht.core.object_sctor                         ; (call label)
                               add, rsp, 40                                         
                               lea, rbx, qwordptr .nostromo.consoleTarget_vtbl_inst 
                               mov, [rcx], rbx                                      ; =
                               sub, rsp, 40                                         
                               call, .nostromo.consoleTarget.cctor                  ; (call label)
                               add, rsp, 40                                         
                               mov, rsp, rbp                                        
                               pop, rbx                                             
                               pop, rbp                                             
                               ret                                                  

.seg code                      
.nostromo.consoleTarget_sdtor: 
                               push, rbp                                            
                               push, rbx                                            
                               mov, rbp, rsp                                        
                               lea, rbx, qwordptr .nostromo.consoleTarget_vtbl_inst 
                               mov, [rcx], rbx                                      ; =
                               sub, rsp, 40                                         
                               call, .nostromo.consoleTarget.cdtor                  ; (call label)
                               add, rsp, 40                                         
                               sub, rsp, 40                                         
                               call, .sht.core.object_sdtor                         ; (call label)
                               add, rsp, 40                                         
                               mov, rsp, rbp                                        
                               pop, rbx                                             
                               pop, rbp                                             
                               ret                                                  

