.seg const
._strLit_If_0:
.data, "===if" <b> 0 

.seg const
._strLit_1_1:
.data, "1)" <b> 0 

.seg const
._strLit_2_2:
.data, "2)" <b> 0 

.seg const
._strLit_3_3:
.data, "3)" <b> 0 

.seg const
._strLit_4_4:
.data, "4)" <b> 0 

.seg const
._strLit__5:
.data, "---" <b> 0 

.seg const
._strLit_1_6:
.data, "1" <b> 0 

.seg const
._strLit_2_7:
.data, "2" <b> 0 

.seg const
._strLit_34_8:
.data, "3-4" <b> 0 

.seg const
._strLit_13_9:
.data, "1-3" <b> 0 

.seg const
._strLit_4_10:
.data, "4" <b> 0 

.seg code         
.uats.ifTest.run: 
                  push, rbp                        
                  push, rbx                        
                  mov, rbp, rsp                    
                  sub, rsp, 32                     
                  mov, rbx, rcx                    ; (preserve) [combiner]
                  lea, rcx, qwordptr ._strLit_If_0 
                  call, ._print                    ; (call label)
                  add, rsp, 32                     
                  sub, rsp, 32                     
                  lea, rcx, qwordptr ._strLit_1_1  
                  call, ._print                    ; (call label)
                  add, rsp, 32                     
                  sub, rsp, 32                     
                  mov, rdx, 1                      ;       (1 req for rdx) [splitter]
                  mov, rcx, rbx                    ; (restore [combiner])
                  call, .uats.ifTest.doTest        ; (call label)
                  add, rsp, 32                     
                  sub, rsp, 32                     
                  mov, rbx, rcx                    ; (preserve) [combiner]
                  lea, rcx, qwordptr ._strLit_2_2  
                  call, ._print                    ; (call label)
                  add, rsp, 32                     
                  sub, rsp, 32                     
                  mov, rdx, 2                      ;       (2 req for rdx) [splitter]
                  mov, rcx, rbx                    ; (restore [combiner])
                  call, .uats.ifTest.doTest        ; (call label)
                  add, rsp, 32                     
                  sub, rsp, 32                     
                  mov, rbx, rcx                    ; (preserve) [combiner]
                  lea, rcx, qwordptr ._strLit_3_3  
                  call, ._print                    ; (call label)
                  add, rsp, 32                     
                  sub, rsp, 32                     
                  mov, rdx, 3                      ;       (3 req for rdx) [splitter]
                  mov, rcx, rbx                    ; (restore [combiner])
                  call, .uats.ifTest.doTest        ; (call label)
                  add, rsp, 32                     
                  sub, rsp, 32                     
                  mov, rbx, rcx                    ; (preserve) [combiner]
                  lea, rcx, qwordptr ._strLit_4_4  
                  call, ._print                    ; (call label)
                  add, rsp, 32                     
                  sub, rsp, 32                     
                  mov, rdx, 4                      ;       (4 req for rdx) [splitter]
                  mov, rcx, rbx                    ; (restore [combiner])
                  call, .uats.ifTest.doTest        ; (call label)
                  add, rsp, 32                     
                  mov, rsp, rbp                    
                  pop, rbx                         
                  pop, rbp                         
                  ret                              

.seg code                    
.uats.ifTest.doTest:         
                             push, rbp                         
                             push, rbx                         
                             push, rdi                         
                             mov, rbp, rsp                     
                             sub, rsp, 32                      
                             lea, rcx, qwordptr ._strLit__5    
                             mov, rbx, rdx                     ; (preserve) [combiner]
                             call, ._print                     ; (call label)
                             add, rsp, 32                      
                             xor, rdi, rdi                     
                             cmp, rbx, 2                       
                             setlts, rdi                       
                             cmp, rdi, 0                       
                             je, .uats.ifTest.doTest.else.0    
                             sub, rsp, 32                      
                             lea, rcx, qwordptr ._strLit_1_6   
                             call, ._print                     ; (call label)
                             add, rsp, 32                      
                             goto, .uats.ifTest.doTest.endif.1 
.seg code                    
.uats.ifTest.doTest.else.0:  
                             xor, rdi, rdi                     
                             cmp, rbx, 3                       
                             setlts, rdi                       
                             cmp, rdi, 0                       
                             je, .uats.ifTest.doTest.else.2    
                             sub, rsp, 32                      
                             lea, rcx, qwordptr ._strLit_2_7   
                             call, ._print                     ; (call label)
                             add, rsp, 32                      
                             goto, .uats.ifTest.doTest.endif.3 
.seg code                    
.uats.ifTest.doTest.else.2:  
                             sub, rsp, 32                      
                             lea, rcx, qwordptr ._strLit_34_8  
                             call, ._print                     ; (call label)
                             add, rsp, 32                      
                             goto, .uats.ifTest.doTest.endif.3 
.seg code                    
.uats.ifTest.doTest.endif.3: 
                             goto, .uats.ifTest.doTest.endif.1 
.seg code                    
.uats.ifTest.doTest.endif.1: 
                             xor, rdi, rdi                     
                             cmp, rbx, 4                       
                             setlts, rdi                       
                             cmp, rdi, 0                       
                             je, .uats.ifTest.doTest.else.4    
                             sub, rsp, 32                      
                             lea, rcx, qwordptr ._strLit_13_9  
                             call, ._print                     ; (call label)
                             add, rsp, 32                      
                             goto, .uats.ifTest.doTest.endif.5 
.seg code                    
.uats.ifTest.doTest.else.4:  
                             sub, rsp, 32                      
                             lea, rcx, qwordptr ._strLit_4_10  
                             call, ._print                     ; (call label)
                             add, rsp, 32                      
                             goto, .uats.ifTest.doTest.endif.5 
.seg code                    
.uats.ifTest.doTest.endif.5: 
                             mov, rsp, rbp                     
                             pop, rdi                          
                             pop, rbx                          
                             pop, rbp                          
                             ret                               

.seg code           
.uats.ifTest.cctor: 
                    push, rbp
                    mov, rbp, rsp
                    mov, rsp, rbp
                    pop, rbp
                    ret

.seg code           
.uats.ifTest.cdtor: 
                    push, rbp
                    mov, rbp, rsp
                    mov, rsp, rbp
                    pop, rbp
                    ret

.seg code               
.uats.ifTest_vtbl_inst: 
                        goto, .uats.ifTest.run

.seg code           
.uats.ifTest_sctor: 
                    push, rbp                                 
                    push, rbx                                 
                    mov, rbp, rsp                             
                    sub, rsp, 32                              
                    call, .sht.cons.program_sctor             ; (call label)
                    add, rsp, 32                              
                    lea, rbx, qwordptr .uats.ifTest_vtbl_inst 
                    mov, [rcx], rbx                           ; =
                    sub, rsp, 32                              
                    call, .uats.ifTest.cctor                  ; (call label)
                    add, rsp, 32                              
                    mov, rsp, rbp                             
                    pop, rbx                                  
                    pop, rbp                                  
                    ret                                       

.seg code           
.uats.ifTest_sdtor: 
                    push, rbp                                 
                    push, rbx                                 
                    mov, rbp, rsp                             
                    lea, rbx, qwordptr .uats.ifTest_vtbl_inst 
                    mov, [rcx], rbx                           ; =
                    sub, rsp, 32                              
                    call, .uats.ifTest.cdtor                  ; (call label)
                    add, rsp, 32                              
                    sub, rsp, 32                              
                    call, .sht.cons.program_sdtor             ; (call label)
                    add, rsp, 32                              
                    mov, rsp, rbp                             
                    pop, rbx                                  
                    pop, rbp                                  
                    ret                                       

