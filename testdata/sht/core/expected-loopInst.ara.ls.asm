.seg code                      
.sht.core.loopInstBase.setDir: 
                               push, rbp         
                               mov, rbp, rsp     
                               mov, [rcx+8], rdx ; =
                               mov, rsp, rbp     
                               pop, rbp          
                               ret               

.seg code                            
.sht.core.loopInstBase.getCount:     
                                     push, rbp                                 
                                     mov, rbp, rsp                             
                                     mov, rax, [rcx+16]                        ;       (self req for rax) [splitter]
                                     goto, .sht.core.loopInstBase.getCount.end ; early return
.seg code                            
.sht.core.loopInstBase.getCount.end: 
                                     mov, rsp, rbp                             
                                     pop, rbp                                  
                                     ret                                       

.seg code                            
.sht.core.loopInstBase.bump:         
                                     push, rbp                                 
                                     push, rbx                                 
                                     mov, rbp, rsp                             
                                     cmp, [rcx+8], 0                           
                                     je, .sht.core.loopInstBase.bump.else.0    
                                     mov, rbx, [rcx+16]                        
                                     add, rbx, 1                               
                                     mov, [rcx+16], rbx                        ; =
                                     goto, .sht.core.loopInstBase.bump.endif.1 
.seg code                            
.sht.core.loopInstBase.bump.else.0:  
                                     goto, .sht.core.loopInstBase.bump.endif.1 ; label decomp
.seg code                            
.sht.core.loopInstBase.bump.endif.1: 
                                     mov, rsp, rbp                             
                                     pop, rbx                                  
                                     pop, rbp                                  
                                     ret                                       

.seg code                     
.sht.core.loopInstBase.break: 
                              push, rbp
                              mov, rbp, rsp
                              mov, rsp, rbp
                              pop, rbp
                              ret

.seg code                               
.sht.core.loopInstBase.breakWithReason: 
                                        push, rbp
                                        mov, rbp, rsp
                                        mov, rsp, rbp
                                        pop, rbp
                                        ret

.seg code                        
.sht.core.loopInstBase.continue: 
                                 push, rbp
                                 mov, rbp, rsp
                                 mov, rsp, rbp
                                 pop, rbp
                                 ret

.seg code                        
.sht.core.loopInstBase.setCount: 
                                 push, rbp          
                                 mov, rbp, rsp      
                                 mov, [rcx+16], rdx ; =
                                 mov, rsp, rbp      
                                 pop, rbp           
                                 ret                

.seg code                     
.sht.core.loopInstBase.cctor: 
                              push, rbp        
                              mov, rbp, rsp    
                              mov, [rcx+8], 1  ; =
                              mov, [rcx+16], 0 ; =
                              mov, rsp, rbp    
                              pop, rbp         
                              ret              

.seg code                     
.sht.core.loopInstBase.cdtor: 
                              push, rbp
                              mov, rbp, rsp
                              mov, rsp, rbp
                              pop, rbp
                              ret

.seg code                        
.sht.core.forLoopInst.setBounds: 
                                 push, rbp                             
                                 push, rbx                             
                                 mov, rbp, rsp                         
                                 sub, rsp, 40                          
                                 mov, rbx, r8                          ; (preserve) [combiner]
                                 call, .sht.core.loopInstBase.setCount ; (call label)
                                 add, rsp, 40                          
                                 mov, [rcx+32], rbx                    ; =
                                 mov, rsp, rbp                         
                                 pop, rbx                              
                                 pop, rbp                              
                                 ret                                   

.seg code                               
.sht.core.forLoopInst.inBounds:         
                                        push, rbp                                    
                                        push, rbx                                    
                                        push, rdi                                    
                                        mov, rbp, rsp                                
                                        sub, rsp, 32                                 
                                        call, .sht.core.loopInstBase.getCount        ; (call label)
                                        add, rsp, 32                                 
                                        xor, rbx, rbx                                
                                        cmp, rax, [rcx+24]                           
                                        setlts, rbx                                  
                                        cmp, rbx, 0                                  
                                        je, .sht.core.forLoopInst.inBounds.else.0    
                                        mov, rax, 0                                  ;       (0 req for rax) [splitter]
                                        goto, .sht.core.forLoopInst.inBounds.end     ; early return
                                        goto, .sht.core.forLoopInst.inBounds.endif.1 
.seg code                               
.sht.core.forLoopInst.inBounds.else.0:  
                                        goto, .sht.core.forLoopInst.inBounds.endif.1 ; label decomp
.seg code                               
.sht.core.forLoopInst.inBounds.endif.1: 
                                        sub, rsp, 32                                 
                                        mov, rbx, rax                                ; (preserve) [combiner]
                                        call, .sht.core.loopInstBase.getCount        ; (call label)
                                        add, rsp, 32                                 
                                        xor, rdi, rdi                                
                                        mov, r10, [rcx+32]                           ; codeshape decomp
                                        cmp, r10, rax                                
                                        setlts, rdi                                  
                                        cmp, rdi, 0                                  
                                        je, .sht.core.forLoopInst.inBounds.else.2    
                                        mov, rax, rbx                                ; (restore [combiner])
                                        mov, rax, 0                                  ;       (0 req for rax) [splitter]
                                        goto, .sht.core.forLoopInst.inBounds.end     ; early return
                                        goto, .sht.core.forLoopInst.inBounds.endif.3 
.seg code                               
.sht.core.forLoopInst.inBounds.else.2:  
                                        goto, .sht.core.forLoopInst.inBounds.endif.3 ; label decomp
.seg code                               
.sht.core.forLoopInst.inBounds.endif.3: 
                                        mov, rax, 1                                  ;       (1 req for rax) [splitter]
                                        goto, .sht.core.forLoopInst.inBounds.end     ; early return
.seg code                               
.sht.core.forLoopInst.inBounds.end:     
                                        mov, rsp, rbp                                
                                        pop, rdi                                     
                                        pop, rbx                                     
                                        pop, rbp                                     
                                        ret                                          

.seg code                           
.sht.core.forLoopInst.getValue:     
                                    push, rbp                                
                                    mov, rbp, rsp                            
                                    sub, rsp, 32                             
                                    call, .sht.core.loopInstBase.getCount    ; (call label)
                                    add, rsp, 32                             
                                    goto, .sht.core.forLoopInst.getValue.end ; early return
.seg code                           
.sht.core.forLoopInst.getValue.end: 
                                    mov, rsp, rbp                            
                                    pop, rbp                                 
                                    ret                                      

.seg code                    
.sht.core.forLoopInst.cctor: 
                             push, rbp        
                             mov, rbp, rsp    
                             mov, [rcx+8], 1  ; =
                             mov, [rcx+16], 0 ; =
                             mov, [rcx+24], 0 ; =
                             mov, [rcx+32], 0 ; =
                             mov, rsp, rbp    
                             pop, rbp         
                             ret              

.seg code                    
.sht.core.forLoopInst.cdtor: 
                             push, rbp
                             mov, rbp, rsp
                             mov, rsp, rbp
                             pop, rbp
                             ret

.seg code                             
.sht.core.whileLoopInst.getValue:     
                                      push, rbp                                  
                                      mov, rbp, rsp                              
                                      sub, rsp, 32                               
                                      call, .sht.core.loopInstBase.getCount      ; (call label)
                                      add, rsp, 32                               
                                      goto, .sht.core.whileLoopInst.getValue.end ; early return
.seg code                             
.sht.core.whileLoopInst.getValue.end: 
                                      mov, rsp, rbp                              
                                      pop, rbp                                   
                                      ret                                        

.seg code                      
.sht.core.whileLoopInst.cctor: 
                               push, rbp        
                               mov, rbp, rsp    
                               mov, [rcx+8], 1  ; =
                               mov, [rcx+16], 0 ; =
                               mov, rsp, rbp    
                               pop, rbp         
                               ret              

.seg code                      
.sht.core.whileLoopInst.cdtor: 
                               push, rbp
                               mov, rbp, rsp
                               mov, rsp, rbp
                               pop, rbp
                               ret

.seg code                        
.sht.core.forLoopInst_vtbl_inst: 
                                 goto, .sht.core.loopInstBase.bump

.seg code                         
.sht.core.loopInstBase_vtbl_inst: 
                                  goto, .sht.core.loopInstBase.bump

.seg code                          
.sht.core.whileLoopInst_vtbl_inst: 
                                   goto, .sht.core.loopInstBase.bump

.seg code                    
.sht.core.forLoopInst_sctor: 
                             push, rbp                                          
                             push, rbx                                          
                             mov, rbp, rsp                                      
                             sub, rsp, 40                                       
                             call, .sht.core.loopInstBase_sctor                 ; (call label)
                             add, rsp, 40                                       
                             lea, rbx, qwordptr .sht.core.forLoopInst_vtbl_inst 
                             mov, [rcx], rbx                                    ; =
                             sub, rsp, 40                                       
                             call, .sht.core.forLoopInst.cctor                  ; (call label)
                             add, rsp, 40                                       
                             mov, rsp, rbp                                      
                             pop, rbx                                           
                             pop, rbp                                           
                             ret                                                

.seg code                    
.sht.core.forLoopInst_sdtor: 
                             push, rbp                                          
                             push, rbx                                          
                             mov, rbp, rsp                                      
                             lea, rbx, qwordptr .sht.core.forLoopInst_vtbl_inst 
                             mov, [rcx], rbx                                    ; =
                             sub, rsp, 40                                       
                             call, .sht.core.forLoopInst.cdtor                  ; (call label)
                             add, rsp, 40                                       
                             sub, rsp, 40                                       
                             call, .sht.core.loopInstBase_sdtor                 ; (call label)
                             add, rsp, 40                                       
                             mov, rsp, rbp                                      
                             pop, rbx                                           
                             pop, rbp                                           
                             ret                                                

.seg code                     
.sht.core.loopInstBase_sctor: 
                              push, rbp                                           
                              push, rbx                                           
                              mov, rbp, rsp                                       
                              sub, rsp, 40                                        
                              call, .sht.core.object_sctor                        ; (call label)
                              add, rsp, 40                                        
                              lea, rbx, qwordptr .sht.core.loopInstBase_vtbl_inst 
                              mov, [rcx], rbx                                     ; =
                              sub, rsp, 40                                        
                              call, .sht.core.loopInstBase.cctor                  ; (call label)
                              add, rsp, 40                                        
                              mov, rsp, rbp                                       
                              pop, rbx                                            
                              pop, rbp                                            
                              ret                                                 

.seg code                     
.sht.core.loopInstBase_sdtor: 
                              push, rbp                                           
                              push, rbx                                           
                              mov, rbp, rsp                                       
                              lea, rbx, qwordptr .sht.core.loopInstBase_vtbl_inst 
                              mov, [rcx], rbx                                     ; =
                              sub, rsp, 40                                        
                              call, .sht.core.loopInstBase.cdtor                  ; (call label)
                              add, rsp, 40                                        
                              sub, rsp, 40                                        
                              call, .sht.core.object_sdtor                        ; (call label)
                              add, rsp, 40                                        
                              mov, rsp, rbp                                       
                              pop, rbx                                            
                              pop, rbp                                            
                              ret                                                 

.seg code                      
.sht.core.whileLoopInst_sctor: 
                               push, rbp                                            
                               push, rbx                                            
                               mov, rbp, rsp                                        
                               sub, rsp, 40                                         
                               call, .sht.core.loopInstBase_sctor                   ; (call label)
                               add, rsp, 40                                         
                               lea, rbx, qwordptr .sht.core.whileLoopInst_vtbl_inst 
                               mov, [rcx], rbx                                      ; =
                               sub, rsp, 40                                         
                               call, .sht.core.whileLoopInst.cctor                  ; (call label)
                               add, rsp, 40                                         
                               mov, rsp, rbp                                        
                               pop, rbx                                             
                               pop, rbp                                             
                               ret                                                  

.seg code                      
.sht.core.whileLoopInst_sdtor: 
                               push, rbp                                            
                               push, rbx                                            
                               mov, rbp, rsp                                        
                               lea, rbx, qwordptr .sht.core.whileLoopInst_vtbl_inst 
                               mov, [rcx], rbx                                      ; =
                               sub, rsp, 40                                         
                               call, .sht.core.whileLoopInst.cdtor                  ; (call label)
                               add, rsp, 40                                         
                               sub, rsp, 40                                         
                               call, .sht.core.loopInstBase_sdtor                   ; (call label)
                               add, rsp, 40                                         
                               mov, rsp, rbp                                        
                               pop, rbx                                             
                               pop, rbp                                             
                               ret                                                  

