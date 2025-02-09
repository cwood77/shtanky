.seg const
._strLit_startingProgra_etc_0:
.data, "starting program" <b> 0 

.seg const
._strLit_insideLoop_1:
.data, "inside loop" <b> 0 

.seg const
._strLit_leavingProgram_2:
.data, "leaving program" <b> 0 

.seg code                        
.shtemu.foo.run:                 
                                 push, rbp                                        
                                 push, rbx                                        
                                 mov, rbp, rsp                                    
                                 sub, rsp, 24                                     
                                 sub, rsp, 32                                     
                                 lea, rcx, qwordptr ._strLit_startingProgra_etc_0 
                                 call, ._print                                    ; (call label)
                                 add, rsp, 32                                     
                                 lea, rcx, [rbp-24]                               ; i
                                 sub, rsp, 32                                     
                                 call, .sht.core.whileLoopInst_sctor              ; (call label)
                                 add, rsp, 32                                     
                                 goto, .shtemu.foo.run.loop_i_15_start            ; label decomp
.seg code                        
.shtemu.foo.run.loop_i_15_start: 
                                 sub, rsp, 32                                     
                                 call, .sht.core.whileLoopInst.getValue           ; (call label)
                                 add, rsp, 32                                     
                                 xor, rbx, rbx                                    
                                 cmp, rax, 2                                      
                                 setlts, rbx                                      
                                 cmp, rbx, 0                                      
                                 je, .shtemu.foo.run.else.0                       
                                 goto, .shtemu.foo.run.endif.1                    
.seg code                        
.shtemu.foo.run.else.0:          
                                 goto, .shtemu.foo.run.loop_i_15_end              
                                 goto, .shtemu.foo.run.endif.1                    
.seg code                        
.shtemu.foo.run.endif.1:         
                                 sub, rsp, 32                                     
                                 mov, rbx, rcx                                    ; (preserve) [combiner]
                                 lea, rcx, qwordptr ._strLit_insideLoop_1         
                                 call, ._print                                    ; (call label)
                                 add, rsp, 32                                     
                                 sub, rsp, 32                                     
                                 mov, rcx, rbx                                    ; (restore [combiner])
                                 call, [rcx]                                      ; vtbl call to .sht.core.whileLoopInst_vtbl::bump
                                 add, rsp, 32                                     
                                 goto, .shtemu.foo.run.loop_i_15_start            
.seg code                        
.shtemu.foo.run.loop_i_15_end:   
                                 sub, rsp, 32                                     
                                 call, .sht.core.whileLoopInst_sdtor              ; (call label)
                                 add, rsp, 32                                     
                                 sub, rsp, 32                                     
                                 lea, rcx, qwordptr ._strLit_leavingProgram_2     
                                 call, ._print                                    ; (call label)
                                 add, rsp, 32                                     
                                 mov, rsp, rbp                                    
                                 pop, rbx                                         
                                 pop, rbp                                         
                                 ret                                              

.seg code          
.shtemu.foo.cctor: 
                   push, rbp
                   mov, rbp, rsp
                   mov, rsp, rbp
                   pop, rbp
                   ret

.seg code          
.shtemu.foo.cdtor: 
                   push, rbp
                   mov, rbp, rsp
                   mov, rsp, rbp
                   pop, rbp
                   ret

.seg code              
.shtemu.foo_vtbl_inst: 
                       goto, .shtemu.foo.run

.seg code          
.shtemu.foo_sctor: 
                   push, rbp                                
                   push, rbx                                
                   mov, rbp, rsp                            
                   sub, rsp, 32                             
                   call, .sht.cons.program_sctor            ; (call label)
                   add, rsp, 32                             
                   lea, rbx, qwordptr .shtemu.foo_vtbl_inst 
                   mov, [rcx], rbx                          ; =
                   sub, rsp, 32                             
                   call, .shtemu.foo.cctor                  ; (call label)
                   add, rsp, 32                             
                   mov, rsp, rbp                            
                   pop, rbx                                 
                   pop, rbp                                 
                   ret                                      

.seg code          
.shtemu.foo_sdtor: 
                   push, rbp                                
                   push, rbx                                
                   mov, rbp, rsp                            
                   lea, rbx, qwordptr .shtemu.foo_vtbl_inst 
                   mov, [rcx], rbx                          ; =
                   sub, rsp, 32                             
                   call, .shtemu.foo.cdtor                  ; (call label)
                   add, rsp, 32                             
                   sub, rsp, 32                             
                   call, .sht.cons.program_sdtor            ; (call label)
                   add, rsp, 32                             
                   mov, rsp, rbp                            
                   pop, rbx                                 
                   pop, rbp                                 
                   ret                                      

