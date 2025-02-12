.seg const
.main.startLabel:
.data, "~~ lats start ~~" <b> 0 

.seg const
.main.endLabel:
.data, "~~ lats end ~~" <b> 0 

.seg const
.main.passLabel:
.data, "    pass" <b> 0 

.seg const
.main.failLabel:
.data, "    !!!!!!!! FAIL !!!!!!!!" <b> 0 

.seg code    
.entrypoint: 
             push, rbp                           
             mov, rbp, rsp                       
             sub, rsp, 32                        
             lea, rcx, qwordptr .main.startLabel 
             call, ._print                       ; (call label)
             add, rsp, 32                        
             sub, rsp, 32                        
             call, .splitter.test                ; (call label)
             add, rsp, 32                        
             sub, rsp, 32                        
             lea, rcx, qwordptr .main.endLabel   
             call, ._print                       ; (call label)
             add, rsp, 32                        
             mov, rsp, rbp                       
             pop, rbp                            
             ret                                 

.seg code            
.main.check:         
                     push, rbp                          
                     push, rbx                          
                     mov, rbp, rsp                      
                     sub, rsp, 8                        
                     sub, rsp, 32                       
                     mov, rbx, rdx                      ; (preserve) [combiner]
                     call, ._print                      ; (call label)
                     add, rsp, 32                       
                     cmp, rbx, 0                        
                     je, .main.check.else.0             
                     sub, rsp, 32                       
                     lea, rcx, qwordptr .main.passLabel 
                     call, ._print                      ; (call label)
                     add, rsp, 32                       
                     goto, .main.check.endif.1          
.seg code            
.main.check.else.0:  
                     sub, rsp, 32                       
                     lea, rcx, qwordptr .main.failLabel 
                     call, ._print                      ; (call label)
                     add, rsp, 32                       
                     goto, .main.check.endif.1          
.seg code            
.main.check.endif.1: 
                     mov, rsp, rbp                      
                     pop, rbx                           
                     pop, rbp                           
                     ret                                

.seg const
.splitter.label:
.data, "- splitter" <b> 0 

.seg const
.splitter.test.label:
.data, "  - simple" <b> 0 

.seg const
.main.progress:
.data, "       progress" <b> 0 

.seg code               
.splitter.subFunc2:     
                        push, rbp                         
                        push, rbx                         
                        push, rdi                         
                        mov, rbp, rsp                     
                        sub, rsp, 32                      
                        mov, rbx, rcx                     ; (preserve) [combiner]
                        lea, rcx, qwordptr .main.progress 
                        mov, rdi, rdx                     ; (preserve) [combiner]
                        call, ._print                     ; (call label)
                        add, rsp, 32                      
                        mov, rax, rbx                     
                        add, rax, rdi                     
                        goto, .splitter.subFunc2.end      ; early return
.seg code               
.splitter.subFunc2.end: 
                        mov, rsp, rbp                     
                        pop, rdi                          
                        pop, rbx                          
                        pop, rbp                          
                        ret                               

.seg code               
.splitter.subFunc1:     
                        push, rbp                         
                        push, rbx                         
                        mov, rbp, rsp                     
                        sub, rsp, 8                       
                        sub, rsp, 32                      
                        mov, rbx, rcx                     ; (preserve) [combiner]
                        lea, rcx, qwordptr .main.progress 
                        call, ._print                     ; (call label)
                        add, rsp, 32                      
                        sub, rsp, 32                      
                        mov, rdx, rbx                     ;       (x req for rdx) [splitter]
                        mov, rcx, 7                       ;       (7 req for rcx) [splitter]
                        call, .splitter.subFunc2          ; (call label)
                        add, rsp, 32                      
                        goto, .splitter.subFunc1.end      ; early return
                        goto, .splitter.subFunc1.end      ; label decomp
.seg code               
.splitter.subFunc1.end: 
                        mov, rsp, rbp                     
                        pop, rbx                          
                        pop, rbp                          
                        ret                               

.seg code       
.splitter.test: 
                push, rbp                               
                push, rbx                               
                mov, rbp, rsp                           
                sub, rsp, 8                             
                sub, rsp, 32                            
                lea, rcx, qwordptr .splitter.label      
                call, ._print                           ; (call label)
                add, rsp, 32                            
                sub, rsp, 32                            
                lea, rcx, qwordptr .main.progress       
                call, ._print                           ; (call label)
                add, rsp, 32                            
                sub, rsp, 32                            
                mov, rcx, 12                            ;       (12 req for rcx) [splitter]
                call, .splitter.subFunc1                ; (call label)
                add, rsp, 32                            
                xor, rbx, rbx                           
                cmp, rax, 19                            
                setet, rbx                              
                mov, [rbp-8], rbx                       ; =
                sub, rsp, 32                            
                lea, rcx, qwordptr .main.progress       
                call, ._print                           ; (call label)
                add, rsp, 32                            
                sub, rsp, 32                            
                lea, rcx, qwordptr .splitter.test.label 
                mov, rdx, [rbp-8]                       ;       (X req for rdx) [splitter]
                call, .main.check                       ; (call label)
                add, rsp, 32                            
                mov, rsp, rbp                           
                pop, rbx                                
                pop, rbp                                
                ret                                     

