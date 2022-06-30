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
                                 mov, rbp, rsp
                                 mov, rsp, rbp
                                 pop, rbp
                                 ret

.seg code                       
.sht.core.forLoopInst.inBounds: 
                                push, rbp
                                mov, rbp, rsp
                                mov, rsp, rbp
                                pop, rbp
                                ret

.seg code                       
.sht.core.forLoopInst.getValue: 
                                push, rbp
                                mov, rbp, rsp
                                mov, rsp, rbp
                                pop, rbp
                                ret

.seg code                    
.sht.core.forLoopInst.cctor: 
                             push, rbp        
                             mov, rbp, rsp    
                             mov, [rcx+8], 1  ; =
                             mov, [rcx+16], 0 ; =
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
                             sub, rsp, 32                                       
                             call, .sht.core.loopInstBase_sctor                 ; (call label)
                             add, rsp, 32                                       
                             lea, rbx, qwordptr .sht.core.forLoopInst_vtbl_inst 
                             mov, [rcx], rbx                                    ; =
                             sub, rsp, 32                                       
                             call, .sht.core.forLoopInst.cctor                  ; (call label)
                             add, rsp, 32                                       
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
                             sub, rsp, 32                                       
                             call, .sht.core.forLoopInst.cdtor                  ; (call label)
                             add, rsp, 32                                       
                             sub, rsp, 32                                       
                             call, .sht.core.loopInstBase_sdtor                 ; (call label)
                             add, rsp, 32                                       
                             mov, rsp, rbp                                      
                             pop, rbx                                           
                             pop, rbp                                           
                             ret                                                

.seg code                     
.sht.core.loopInstBase_sctor: 
                              push, rbp                                           
                              push, rbx                                           
                              mov, rbp, rsp                                       
                              sub, rsp, 32                                        
                              call, .sht.core.object_sctor                        ; (call label)
                              add, rsp, 32                                        
                              lea, rbx, qwordptr .sht.core.loopInstBase_vtbl_inst 
                              mov, [rcx], rbx                                     ; =
                              sub, rsp, 32                                        
                              call, .sht.core.loopInstBase.cctor                  ; (call label)
                              add, rsp, 32                                        
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
                              sub, rsp, 32                                        
                              call, .sht.core.loopInstBase.cdtor                  ; (call label)
                              add, rsp, 32                                        
                              sub, rsp, 32                                        
                              call, .sht.core.object_sdtor                        ; (call label)
                              add, rsp, 32                                        
                              mov, rsp, rbp                                       
                              pop, rbx                                            
                              pop, rbp                                            
                              ret                                                 

.seg code                      
.sht.core.whileLoopInst_sctor: 
                               push, rbp                                            
                               push, rbx                                            
                               mov, rbp, rsp                                        
                               sub, rsp, 32                                         
                               call, .sht.core.loopInstBase_sctor                   ; (call label)
                               add, rsp, 32                                         
                               lea, rbx, qwordptr .sht.core.whileLoopInst_vtbl_inst 
                               mov, [rcx], rbx                                      ; =
                               sub, rsp, 32                                         
                               call, .sht.core.whileLoopInst.cctor                  ; (call label)
                               add, rsp, 32                                         
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
                               sub, rsp, 32                                         
                               call, .sht.core.whileLoopInst.cdtor                  ; (call label)
                               add, rsp, 32                                         
                               sub, rsp, 32                                         
                               call, .sht.core.loopInstBase_sdtor                   ; (call label)
                               add, rsp, 32                                         
                               mov, rsp, rbp                                        
                               pop, rbx                                             
                               pop, rbp                                             
                               ret                                                  

