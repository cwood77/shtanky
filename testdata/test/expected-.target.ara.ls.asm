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
             call, .test.test_sctor       ; (call label)
             add, rsp, 32                 
             mov, [rcx+8], rdi            ; =
             sub, rsp, 32                 
             mov, rdx, rbx                ;       (args req for rdx) [splitter]
             call, [rcx]                  ; (vtbl call)
             add, rsp, 32                 
             sub, rsp, 32                 
             call, .test.test_sdtor       ; (call label)
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
.test.consoleTarget.cctor: 
                           push, rbp
                           mov, rbp, rsp
                           mov, rsp, rbp
                           pop, rbp
                           ret

.seg code                  
.test.consoleTarget.cdtor: 
                           push, rbp
                           mov, rbp, rsp
                           mov, rsp, rbp
                           pop, rbp
                           ret

.seg code
.test.consoleTarget_vtbl_inst:

.seg code                  
.test.consoleTarget_sctor: 
                           push, rbp                                        
                           mov, rbp, rsp                                    
                           sub, rsp, 32                                     
                           call, .sht.core.object_sctor                     ; (call label)
                           add, rsp, 32                                     
                           mov, r10, qwordptr .test.consoleTarget_vtbl_inst ; codeshape decomp
                           mov, [rcx], r10                                  ; =
                           sub, rsp, 32                                     
                           call, .test.consoleTarget.cctor                  ; (call label)
                           add, rsp, 32                                     
                           mov, rsp, rbp                                    
                           pop, rbp                                         
                           ret                                              

.seg code                  
.test.consoleTarget_sdtor: 
                           push, rbp                                        
                           mov, rbp, rsp                                    
                           mov, r10, qwordptr .test.consoleTarget_vtbl_inst ; codeshape decomp
                           mov, [rcx], r10                                  ; =
                           sub, rsp, 32                                     
                           call, .test.consoleTarget.cdtor                  ; (call label)
                           add, rsp, 32                                     
                           sub, rsp, 32                                     
                           call, .sht.core.object_sdtor                     ; (call label)
                           add, rsp, 32                                     
                           mov, rsp, rbp                                    
                           pop, rbp                                         
                           ret                                              

