.seg code    
.entrypoint: 
             push, rbp                    
             push, rbx                    
             push, rdi                    
             mov, rbp, rsp                
             sub, rsp, 24                 
             mov, rbx, rcx                ; (preserve) [combiner]
             lea, rcx, [rbp-8]            ; cout
             sub, rsp, 40                 ; 40 = (passing size)32 + (align pad)8
             call, .sht.cons.stdout_sctor ; (call label)
             add, rsp, 40                 ; 40 = (passing size)32 + (align pad)8
             mov, rdi, rcx                ; (preserve) [combiner]
             lea, rcx, [rbp-24]           ; obj0
             sub, rsp, 40                 ; 40 = (passing size)32 + (align pad)8
             call, .test.test_sctor       ; (call label)
             add, rsp, 40                 ; 40 = (passing size)32 + (align pad)8
             mov, [rcx+8], rdi            ; =
             sub, rsp, 40                 ; 40 = (passing size)32 + (align pad)8
             mov, rdx, rbx                ;       (args req for rdx) [splitter]
             call, [rcx]                  ; vtbl call to .test.test_vtbl::run
             add, rsp, 40                 ; 40 = (passing size)32 + (align pad)8
             sub, rsp, 40                 ; 40 = (passing size)32 + (align pad)8
             call, .test.test_sdtor       ; (call label)
             add, rsp, 40                 ; 40 = (passing size)32 + (align pad)8
             sub, rsp, 40                 ; 40 = (passing size)32 + (align pad)8
             mov, rcx, rdi                ; (restore [combiner])
             call, .sht.cons.stdout_sdtor ; (call label)
             add, rsp, 40                 ; 40 = (passing size)32 + (align pad)8
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
                           push, rbx                                        
                           mov, rbp, rsp                                    
                           sub, rsp, 40                                     ; 40 = (passing size)32 + (align pad)8
                           call, .sht.core.object_sctor                     ; (call label)
                           add, rsp, 40                                     ; 40 = (passing size)32 + (align pad)8
                           lea, rbx, qwordptr .test.consoleTarget_vtbl_inst 
                           mov, [rcx], rbx                                  ; =
                           sub, rsp, 40                                     ; 40 = (passing size)32 + (align pad)8
                           call, .test.consoleTarget.cctor                  ; (call label)
                           add, rsp, 40                                     ; 40 = (passing size)32 + (align pad)8
                           mov, rsp, rbp                                    
                           pop, rbx                                         
                           pop, rbp                                         
                           ret                                              

.seg code                  
.test.consoleTarget_sdtor: 
                           push, rbp                                        
                           push, rbx                                        
                           mov, rbp, rsp                                    
                           lea, rbx, qwordptr .test.consoleTarget_vtbl_inst 
                           mov, [rcx], rbx                                  ; =
                           sub, rsp, 40                                     ; 40 = (passing size)32 + (align pad)8
                           call, .test.consoleTarget.cdtor                  ; (call label)
                           add, rsp, 40                                     ; 40 = (passing size)32 + (align pad)8
                           sub, rsp, 40                                     ; 40 = (passing size)32 + (align pad)8
                           call, .sht.core.object_sdtor                     ; (call label)
                           add, rsp, 40                                     ; 40 = (passing size)32 + (align pad)8
                           mov, rsp, rbp                                    
                           pop, rbx                                         
                           pop, rbp                                         
                           ret                                              

