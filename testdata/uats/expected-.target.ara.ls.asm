.seg code    
.entrypoint: 
             push, rbp                    
             push, rbx                    
             push, rsi                    
             push, rdi                    
             mov, rbp, rsp                
             sub, rsp, 40                 
             mov, rbx, rcx                ; (preserve) [combiner]
             lea, rcx, [rbp-8]            ; cout
             sub, rsp, 32                 
             call, .sht.cons.stdout_sctor ; (call label)
             add, rsp, 32                 
             mov, rdi, rcx                ; (preserve) [combiner]
             lea, rcx, [rbp-24]           ; obj0
             sub, rsp, 32                 
             call, .uats.ifTest_sctor     ; (call label)
             add, rsp, 32                 
             mov, [rcx+8], rdi            ; =
             mov, rsi, rcx                ; (preserve) [combiner]
             lea, rcx, [rbp-40]           ; obj1
             sub, rsp, 32                 
             call, .uats.loopTest_sctor   ; (call label)
             add, rsp, 32                 
             mov, [rcx+8], rdi            ; =
             sub, rsp, 32                 
             mov, rdx, rbx                ;       (args req for rdx) [splitter]
             mov, rbx, rcx                ; (preserve) [combiner]
             mov, rcx, rsi                ; (restore [combiner])
             call, [rcx]                  ; vtbl call to .uats.ifTest_vtbl::run
             add, rsp, 32                 
             sub, rsp, 32                 
             mov, rsi, rcx                ; (preserve) [combiner]
             mov, rcx, rbx                ; (restore [combiner])
             call, [rcx]                  ; vtbl call to .uats.loopTest_vtbl::run
             add, rsp, 32                 
             sub, rsp, 32                 
             call, .uats.loopTest_sdtor   ; (call label)
             add, rsp, 32                 
             sub, rsp, 32                 
             mov, rcx, rsi                ; (restore [combiner])
             call, .uats.ifTest_sdtor     ; (call label)
             add, rsp, 32                 
             sub, rsp, 32                 
             mov, rcx, rdi                ; (restore [combiner])
             call, .sht.cons.stdout_sdtor ; (call label)
             add, rsp, 32                 
             mov, rsp, rbp                
             pop, rdi                     
             pop, rsi                     
             pop, rbx                     
             pop, rbp                     
             ret                          

.seg code                  
.uats.consoleTarget.cctor: 
                           push, rbp
                           mov, rbp, rsp
                           mov, rsp, rbp
                           pop, rbp
                           ret

.seg code                  
.uats.consoleTarget.cdtor: 
                           push, rbp
                           mov, rbp, rsp
                           mov, rsp, rbp
                           pop, rbp
                           ret

.seg code
.uats.consoleTarget_vtbl_inst:

.seg code                  
.uats.consoleTarget_sctor: 
                           push, rbp                                        
                           push, rbx                                        
                           mov, rbp, rsp                                    
                           sub, rsp, 32                                     
                           call, .sht.core.object_sctor                     ; (call label)
                           add, rsp, 32                                     
                           lea, rbx, qwordptr .uats.consoleTarget_vtbl_inst 
                           mov, [rcx], rbx                                  ; =
                           sub, rsp, 32                                     
                           call, .uats.consoleTarget.cctor                  ; (call label)
                           add, rsp, 32                                     
                           mov, rsp, rbp                                    
                           pop, rbx                                         
                           pop, rbp                                         
                           ret                                              

.seg code                  
.uats.consoleTarget_sdtor: 
                           push, rbp                                        
                           push, rbx                                        
                           mov, rbp, rsp                                    
                           lea, rbx, qwordptr .uats.consoleTarget_vtbl_inst 
                           mov, [rcx], rbx                                  ; =
                           sub, rsp, 32                                     
                           call, .uats.consoleTarget.cdtor                  ; (call label)
                           add, rsp, 32                                     
                           sub, rsp, 32                                     
                           call, .sht.core.object_sdtor                     ; (call label)
                           add, rsp, 32                                     
                           mov, rsp, rbp                                    
                           pop, rbx                                         
                           pop, rbp                                         
                           ret                                              

