.seg code                            
.nostromo.uart16550.fakeBase.write8: 
                                     push, rbp
                                     mov, rbp, rsp
                                     mov, rsp, rbp
                                     pop, rbp
                                     ret

.seg code                           
.nostromo.uart16550.fakeBase.cctor: 
                                    push, rbp
                                    mov, rbp, rsp
                                    mov, rsp, rbp
                                    pop, rbp
                                    ret

.seg code                           
.nostromo.uart16550.fakeBase.cdtor: 
                                    push, rbp
                                    mov, rbp, rsp
                                    mov, rsp, rbp
                                    pop, rbp
                                    ret

.seg code                          
.nostromo.uart16550.driver.write8: 
                                   push, rbp
                                   mov, rbp, rsp
                                   mov, rsp, rbp
                                   pop, rbp
                                   ret

.seg code                         
.nostromo.uart16550.driver.cctor: 
                                  push, rbp
                                  mov, rbp, rsp
                                  mov, rsp, rbp
                                  pop, rbp
                                  ret

.seg code                         
.nostromo.uart16550.driver.cdtor: 
                                  push, rbp
                                  mov, rbp, rsp
                                  mov, rsp, rbp
                                  pop, rbp
                                  ret

.seg const
.nostromo.uart16550.driver_vtbl_inst:
.data, .nostromo.uart16550.driver.write8 

.seg const
.nostromo.uart16550.fakeBase_vtbl_inst:
.data, .nostromo.uart16550.fakeBase.write8 

.seg code                         
.nostromo.uart16550.driver_sctor: 
                                  push, rbp                                               
                                  mov, rbp, rsp                                           
                                  sub, rsp, 32                                            
                                  call, .nostromo.uart16550.fakeBase_sctor                ; (call label)
                                  add, rsp, 32                                            
                                  mov, r10, qwordptr .nostromo.uart16550.driver_vtbl_inst ; codeshape decomp
                                  mov, [rcx], r10                                         ; =
                                  sub, rsp, 32                                            
                                  call, .nostromo.uart16550.driver.cctor                  ; (call label)
                                  add, rsp, 32                                            
                                  mov, rsp, rbp                                           
                                  pop, rbp                                                
                                  ret                                                     

.seg code                         
.nostromo.uart16550.driver_sdtor: 
                                  push, rbp                                               
                                  mov, rbp, rsp                                           
                                  mov, r10, qwordptr .nostromo.uart16550.driver_vtbl_inst ; codeshape decomp
                                  mov, [rcx], r10                                         ; =
                                  sub, rsp, 32                                            
                                  call, .nostromo.uart16550.driver.cdtor                  ; (call label)
                                  add, rsp, 32                                            
                                  sub, rsp, 32                                            
                                  call, .nostromo.uart16550.fakeBase_sdtor                ; (call label)
                                  add, rsp, 32                                            
                                  mov, rsp, rbp                                           
                                  pop, rbp                                                
                                  ret                                                     

.seg code                           
.nostromo.uart16550.fakeBase_sctor: 
                                    push, rbp                                                 
                                    mov, rbp, rsp                                             
                                    sub, rsp, 32                                              
                                    call, .sht.core.object_sctor                              ; (call label)
                                    add, rsp, 32                                              
                                    mov, r10, qwordptr .nostromo.uart16550.fakeBase_vtbl_inst ; codeshape decomp
                                    mov, [rcx], r10                                           ; =
                                    sub, rsp, 32                                              
                                    call, .nostromo.uart16550.fakeBase.cctor                  ; (call label)
                                    add, rsp, 32                                              
                                    mov, rsp, rbp                                             
                                    pop, rbp                                                  
                                    ret                                                       

.seg code                           
.nostromo.uart16550.fakeBase_sdtor: 
                                    push, rbp                                                 
                                    mov, rbp, rsp                                             
                                    mov, r10, qwordptr .nostromo.uart16550.fakeBase_vtbl_inst ; codeshape decomp
                                    mov, [rcx], r10                                           ; =
                                    sub, rsp, 32                                              
                                    call, .nostromo.uart16550.fakeBase.cdtor                  ; (call label)
                                    add, rsp, 32                                              
                                    sub, rsp, 32                                              
                                    call, .sht.core.object_sdtor                              ; (call label)
                                    add, rsp, 32                                              
                                    mov, rsp, rbp                                             
                                    pop, rbp                                                  
                                    ret                                                       

