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
             sub, rsp, 32                        ; 32 = (passing size)32 + (align pad)0
             lea, rcx, qwordptr .main.startLabel 
             call, ._print                       ; (call label)
             add, rsp, 32                        ; 32 = (passing size)32 + (align pad)0
             sub, rsp, 32                        ; 32 = (passing size)32 + (align pad)0
             call, .splitter.test                ; (call label)
             add, rsp, 32                        ; 32 = (passing size)32 + (align pad)0
             sub, rsp, 32                        ; 32 = (passing size)32 + (align pad)0
             lea, rcx, qwordptr .main.endLabel   
             call, ._print                       ; (call label)
             add, rsp, 32                        ; 32 = (passing size)32 + (align pad)0
             mov, rsp, rbp                       
             pop, rbp                            
             ret                                 

.seg code            
.main.check:         
                     push, rbp                          
                     push, rbx                          
                     mov, rbp, rsp                      
                     sub, rsp, 40                       ; 40 = (passing size)32 + (align pad)8
                     mov, rbx, rdx                      ; (preserve) [combiner]
                     call, ._print                      ; (call label)
                     add, rsp, 40                       ; 40 = (passing size)32 + (align pad)8
                     cmp, rbx, 0                        
                     je, .main.check.else.0             
                     sub, rsp, 40                       ; 40 = (passing size)32 + (align pad)8
                     lea, rcx, qwordptr .main.passLabel 
                     call, ._print                      ; (call label)
                     add, rsp, 40                       ; 40 = (passing size)32 + (align pad)8
                     goto, .main.check.endif.1          
.seg code            
.main.check.else.0:  
                     sub, rsp, 40                       ; 40 = (passing size)32 + (align pad)8
                     lea, rcx, qwordptr .main.failLabel 
                     call, ._print                      ; (call label)
                     add, rsp, 40                       ; 40 = (passing size)32 + (align pad)8
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

.seg code               
.splitter.subFunc2:     
                        push, rbp                    
                        mov, rbp, rsp                
                        mov, rax, rcx                
                        add, rax, rdx                
                        goto, .splitter.subFunc2.end ; early return
.seg code               
.splitter.subFunc2.end: 
                        mov, rsp, rbp                
                        pop, rbp                     
                        ret                          

.seg code               
.splitter.subFunc1:     
                        push, rbp                    
                        mov, rbp, rsp                
                        sub, rsp, 32                 ; 32 = (passing size)32 + (align pad)0
                        mov, rdx, rcx                ;       (x req for rdx) [splitter]
                        mov, rcx, 7                  ;       (7 req for rcx) [splitter]
                        call, .splitter.subFunc2     ; (call label)
                        add, rsp, 32                 ; 32 = (passing size)32 + (align pad)0
                        goto, .splitter.subFunc1.end ; early return
.seg code               
.splitter.subFunc1.end: 
                        mov, rsp, rbp                
                        pop, rbp                     
                        ret                          

.seg code       
.splitter.test: 
                push, rbp                               
                push, rbx                               
                mov, rbp, rsp                           
                sub, rsp, 8                             
                sub, rsp, 32                            ; 32 = (passing size)32 + (align pad)0
                lea, rcx, qwordptr .splitter.label      
                call, ._print                           ; (call label)
                add, rsp, 32                            ; 32 = (passing size)32 + (align pad)0
                sub, rsp, 32                            ; 32 = (passing size)32 + (align pad)0
                mov, rcx, 12                            ;       (12 req for rcx) [splitter]
                call, .splitter.subFunc1                ; (call label)
                add, rsp, 32                            ; 32 = (passing size)32 + (align pad)0
                xor, rbx, rbx                           
                cmp, rax, 19                            
                setet, rbx                              
                mov, [rbp-8], rbx                       ; =
                sub, rsp, 32                            ; 32 = (passing size)32 + (align pad)0
                lea, rcx, qwordptr .splitter.test.label 
                mov, rdx, [rbp-8]                       ;       (X req for rdx) [splitter]
                call, .main.check                       ; (call label)
                add, rsp, 32                            ; 32 = (passing size)32 + (align pad)0
                mov, rsp, rbp                           
                pop, rbx                                
                pop, rbp                                
                ret                                     

