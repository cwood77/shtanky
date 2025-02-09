.seg const
._strLit_For_0:
.data, "===for" <b> 0 

.seg const
._strLit_14_1:
.data, "(1-4)" <b> 0 

.seg const
._strLit_0_2:
.data, "  0" <b> 0 

.seg const
._strLit_1_3:
.data, "  1" <b> 0 

.seg const
._strLit_2_4:
.data, "  2" <b> 0 

.seg const
._strLit_3_5:
.data, "  3" <b> 0 

.seg const
._strLit_4_6:
.data, "  4" <b> 0 

.seg code                           
.uats.loopTest.run:                 
                                    push, rbp                                
                                    push, rbx                                
                                    push, rsi                                
                                    push, rdi                                
                                    mov, rbp, rsp                            
                                    sub, rsp, 40                             
                                    sub, rsp, 32                             
                                    mov, rbx, rcx                            ; (preserve) [combiner]
                                    lea, rcx, qwordptr ._strLit_For_0        
                                    call, ._print                            ; (call label)
                                    add, rsp, 32                             
                                    sub, rsp, 32                             
                                    lea, rcx, qwordptr ._strLit_14_1         
                                    call, ._print                            ; (call label)
                                    add, rsp, 32                             
                                    lea, rcx, [rbp-40]                       ; i
                                    sub, rsp, 32                             
                                    call, .sht.core.forLoopInst_sctor        ; (call label)
                                    add, rsp, 32                             
                                    sub, rsp, 32                             
                                    mov, rdx, 0                              ;       (0 req for rdx) [splitter]
                                    mov, r8, 1                               ;       (1 req for r8) [splitter]
                                    call, .sht.core.forLoopInst.setBounds    ; (call label)
                                    add, rsp, 32                             
                                    goto, .uats.loopTest.run.loop_i_24_start ; label decomp
.seg code                           
.uats.loopTest.run.loop_i_24_start: 
                                    sub, rsp, 32                             
                                    mov, rdi, rdx                            ; (preserve) [combiner]
                                    mov, rsi, r8                             ; (preserve) [combiner]
                                    call, .sht.core.forLoopInst.inBounds     ; (call label)
                                    add, rsp, 32                             
                                    cmp, rax, 0                              
                                    je, .uats.loopTest.run.else.0            
                                    goto, .uats.loopTest.run.endif.1         
.seg code                           
.uats.loopTest.run.else.0:          
                                    goto, .uats.loopTest.run.loop_i_24_end   
                                    goto, .uats.loopTest.run.endif.1         
.seg code                           
.uats.loopTest.run.endif.1:         
                                    sub, rsp, 32                             
                                    sub, rsp, 32                             
                                    call, .sht.core.forLoopInst.getValue     ; (call label)
                                    add, rsp, 32                             
                                    mov, rdx, rax                            ;       (rval5 req for rdx) [splitter]
                                    mov, rdi, rcx                            ; (preserve) [combiner]
                                    mov, rcx, rbx                            ; (restore [combiner])
                                    call, .uats.loopTest.printI              ; (call label)
                                    add, rsp, 32                             
                                    sub, rsp, 32                             
                                    mov, rcx, rdi                            ; (restore [combiner])
                                    call, [rcx]                              ; vtbl call to .sht.core.forLoopInst_vtbl::bump
                                    add, rsp, 32                             
                                    goto, .uats.loopTest.run.loop_i_24_start 
.seg code                           
.uats.loopTest.run.loop_i_24_end:   
                                    sub, rsp, 32                             
                                    mov, rdx, 1                              ;       (1 req for rdx) [splitter]
                                    call, .sht.core.loopInstBase.setDir      ; (call label)
                                    add, rsp, 32                             
                                    sub, rsp, 32                             
                                    call, .sht.core.forLoopInst_sdtor        ; (call label)
                                    add, rsp, 32                             
                                    mov, rsp, rbp                            
                                    pop, rdi                                 
                                    pop, rsi                                 
                                    pop, rbx                                 
                                    pop, rbp                                 
                                    ret                                      

.seg code                      
.uats.loopTest.printI:         
                               push, rbp                           
                               push, rbx                           
                               push, rdi                           
                               mov, rbp, rsp                       
                               xor, rbx, rbx                       
                               cmp, rdx, 1                         
                               setlts, rbx                         
                               cmp, rbx, 0                         
                               je, .uats.loopTest.printI.else.0    
                               sub, rsp, 32                        
                               lea, rcx, qwordptr ._strLit_0_2     
                               mov, rbx, rdx                       ; (preserve) [combiner]
                               call, ._print                       ; (call label)
                               add, rsp, 32                        
                               goto, .uats.loopTest.printI.endif.1 
.seg code                      
.uats.loopTest.printI.else.0:  
                               xor, rdi, rdi                       
                               cmp, rbx, 2                         
                               setlts, rdi                         
                               cmp, rdi, 0                         
                               je, .uats.loopTest.printI.else.2    
                               sub, rsp, 32                        
                               lea, rcx, qwordptr ._strLit_1_3     
                               call, ._print                       ; (call label)
                               add, rsp, 32                        
                               goto, .uats.loopTest.printI.endif.3 
.seg code                      
.uats.loopTest.printI.else.2:  
                               xor, rdi, rdi                       
                               cmp, rbx, 3                         
                               setlts, rdi                         
                               cmp, rdi, 0                         
                               je, .uats.loopTest.printI.else.4    
                               sub, rsp, 32                        
                               lea, rcx, qwordptr ._strLit_2_4     
                               call, ._print                       ; (call label)
                               add, rsp, 32                        
                               goto, .uats.loopTest.printI.endif.5 
.seg code                      
.uats.loopTest.printI.else.4:  
                               xor, rdi, rdi                       
                               cmp, rbx, 4                         
                               setlts, rdi                         
                               cmp, rdi, 0                         
                               je, .uats.loopTest.printI.else.6    
                               sub, rsp, 32                        
                               lea, rcx, qwordptr ._strLit_3_5     
                               call, ._print                       ; (call label)
                               add, rsp, 32                        
                               goto, .uats.loopTest.printI.endif.7 
.seg code                      
.uats.loopTest.printI.else.6:  
                               sub, rsp, 32                        
                               lea, rcx, qwordptr ._strLit_4_6     
                               call, ._print                       ; (call label)
                               add, rsp, 32                        
                               goto, .uats.loopTest.printI.endif.7 
.seg code                      
.uats.loopTest.printI.endif.7: 
                               goto, .uats.loopTest.printI.endif.5 
.seg code                      
.uats.loopTest.printI.endif.5: 
                               goto, .uats.loopTest.printI.endif.3 
.seg code                      
.uats.loopTest.printI.endif.3: 
                               goto, .uats.loopTest.printI.endif.1 
.seg code                      
.uats.loopTest.printI.endif.1: 
                               mov, rsp, rbp                       
                               pop, rdi                            
                               pop, rbx                            
                               pop, rbp                            
                               ret                                 

.seg code             
.uats.loopTest.cctor: 
                      push, rbp
                      mov, rbp, rsp
                      mov, rsp, rbp
                      pop, rbp
                      ret

.seg code             
.uats.loopTest.cdtor: 
                      push, rbp
                      mov, rbp, rsp
                      mov, rsp, rbp
                      pop, rbp
                      ret

.seg code                 
.uats.loopTest_vtbl_inst: 
                          goto, .uats.loopTest.run

.seg code             
.uats.loopTest_sctor: 
                      push, rbp                                   
                      push, rbx                                   
                      mov, rbp, rsp                               
                      sub, rsp, 32                                
                      call, .sht.cons.program_sctor               ; (call label)
                      add, rsp, 32                                
                      lea, rbx, qwordptr .uats.loopTest_vtbl_inst 
                      mov, [rcx], rbx                             ; =
                      sub, rsp, 32                                
                      call, .uats.loopTest.cctor                  ; (call label)
                      add, rsp, 32                                
                      mov, rsp, rbp                               
                      pop, rbx                                    
                      pop, rbp                                    
                      ret                                         

.seg code             
.uats.loopTest_sdtor: 
                      push, rbp                                   
                      push, rbx                                   
                      mov, rbp, rsp                               
                      lea, rbx, qwordptr .uats.loopTest_vtbl_inst 
                      mov, [rcx], rbx                             ; =
                      sub, rsp, 32                                
                      call, .uats.loopTest.cdtor                  ; (call label)
                      add, rsp, 32                                
                      sub, rsp, 32                                
                      call, .sht.cons.program_sdtor               ; (call label)
                      add, rsp, 32                                
                      mov, rsp, rbp                               
                      pop, rbx                                    
                      pop, rbp                                    
                      ret                                         

