.seg code    
.entrypoint: 
             push, rbp                          
             push, rbx                          
             push, rdi                          
             mov, rbp, rsp                      
             sub, rsp, 24                       
             sub, rsp, 8                        
             sub, rsp, 32                       
             mov, rbx, rcx                      ; (preserve) [combiner]
             mov, rcx, [rbp-8]                  ;       (cout req for rcx) [splitter]
             call, .sht.cons.stdout_sctor       ; (call label)
             add, rsp, 32                       
             sub, rsp, 16                       
             sub, rsp, 32                       
             mov, rdi, rcx                      ; (preserve) [combiner]
             mov, rcx, [rbp-8]                  ;       (obj0 req for rcx) [splitter]
             call, .nostromo.bootstrapper_sctor ; (call label)
             add, rsp, 32                       
             mov, [rcx+8], rdi                  ; =
             sub, rsp, 32                       
             mov, rbx, [rcx]                    ; fieldaccess: owner of run
             mov, rdx, rbx                      ;       (args req for rdx) [splitter]
             call, [rbx]                        ; (call ptr)
             add, rsp, 32                       
             sub, rsp, 32                       
             call, .nostromo.bootstrapper_sdtor ; (call label)
             add, rsp, 32                       
             sub, rsp, 32                       
             mov, rbx, rcx                      ; (preserve) [combiner]
             mov, rcx, rdi                      ; (restore [combiner])
             call, .sht.cons.stdout_sdtor       ; (call label)
             add, rsp, 32                       
             add, rsp, 16                       
             add, rsp, 8                        
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

.seg const
.nostromo.consoleTarget_vtbl_inst:
.data, 

.seg code                      
.nostromo.consoleTarget_sctor: 
                               push, rbp                                            
                               mov, rbp, rsp                                        
                               sub, rsp, 32                                         
                               call, .sht.core.object_sctor                         ; (call label)
                               add, rsp, 32                                         
                               push, r10                                            ; codeshape spill
                               mov, r10, qwordptr .nostromo.consoleTarget_vtbl_inst ; codeshape decomp
                               mov, [rcx], r10                                      ; =
                               pop, r10                                             ; codeshape restore
                               sub, rsp, 32                                         
                               call, .nostromo.consoleTarget.cctor                  ; (call label)
                               add, rsp, 32                                         
                               mov, rsp, rbp                                        
                               pop, rbp                                             
                               ret                                                  

.seg code                      
.nostromo.consoleTarget_sdtor: 
                               push, rbp                                            
                               mov, rbp, rsp                                        
                               push, r10                                            ; codeshape spill
                               mov, r10, qwordptr .nostromo.consoleTarget_vtbl_inst ; codeshape decomp
                               mov, [rcx], r10                                      ; =
                               pop, r10                                             ; codeshape restore
                               sub, rsp, 32                                         
                               call, .nostromo.consoleTarget.cdtor                  ; (call label)
                               add, rsp, 32                                         
                               sub, rsp, 32                                         
                               call, .sht.core.object_sdtor                         ; (call label)
                               add, rsp, 32                                         
                               mov, rsp, rbp                                        
                               pop, rbp                                             
                               ret                                                  

