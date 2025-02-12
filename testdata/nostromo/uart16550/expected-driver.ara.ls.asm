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

.seg code                             
.nostromo.uart16550.driver_vtbl_inst: 
                                      goto, .nostromo.uart16550.driver.write8

.seg code                               
.nostromo.uart16550.fakeBase_vtbl_inst: 
                                        goto, .nostromo.uart16550.fakeBase.write8

.seg code                         
.nostromo.uart16550.driver_sctor: 
                                  push, rbp                                               
                                  push, rbx                                               
                                  mov, rbp, rsp                                           
                                  sub, rsp, 40                                            ; 40 = (passing size)32 + (align pad)8
                                  call, .nostromo.uart16550.fakeBase_sctor                ; (call label)
                                  add, rsp, 40                                            ; 40 = (passing size)32 + (align pad)8
                                  lea, rbx, qwordptr .nostromo.uart16550.driver_vtbl_inst 
                                  mov, [rcx], rbx                                         ; =
                                  sub, rsp, 40                                            ; 40 = (passing size)32 + (align pad)8
                                  call, .nostromo.uart16550.driver.cctor                  ; (call label)
                                  add, rsp, 40                                            ; 40 = (passing size)32 + (align pad)8
                                  mov, rsp, rbp                                           
                                  pop, rbx                                                
                                  pop, rbp                                                
                                  ret                                                     

.seg code                         
.nostromo.uart16550.driver_sdtor: 
                                  push, rbp                                               
                                  push, rbx                                               
                                  mov, rbp, rsp                                           
                                  lea, rbx, qwordptr .nostromo.uart16550.driver_vtbl_inst 
                                  mov, [rcx], rbx                                         ; =
                                  sub, rsp, 40                                            ; 40 = (passing size)32 + (align pad)8
                                  call, .nostromo.uart16550.driver.cdtor                  ; (call label)
                                  add, rsp, 40                                            ; 40 = (passing size)32 + (align pad)8
                                  sub, rsp, 40                                            ; 40 = (passing size)32 + (align pad)8
                                  call, .nostromo.uart16550.fakeBase_sdtor                ; (call label)
                                  add, rsp, 40                                            ; 40 = (passing size)32 + (align pad)8
                                  mov, rsp, rbp                                           
                                  pop, rbx                                                
                                  pop, rbp                                                
                                  ret                                                     

.seg code                           
.nostromo.uart16550.fakeBase_sctor: 
                                    push, rbp                                                 
                                    push, rbx                                                 
                                    mov, rbp, rsp                                             
                                    sub, rsp, 40                                              ; 40 = (passing size)32 + (align pad)8
                                    call, .sht.core.object_sctor                              ; (call label)
                                    add, rsp, 40                                              ; 40 = (passing size)32 + (align pad)8
                                    lea, rbx, qwordptr .nostromo.uart16550.fakeBase_vtbl_inst 
                                    mov, [rcx], rbx                                           ; =
                                    sub, rsp, 40                                              ; 40 = (passing size)32 + (align pad)8
                                    call, .nostromo.uart16550.fakeBase.cctor                  ; (call label)
                                    add, rsp, 40                                              ; 40 = (passing size)32 + (align pad)8
                                    mov, rsp, rbp                                             
                                    pop, rbx                                                  
                                    pop, rbp                                                  
                                    ret                                                       

.seg code                           
.nostromo.uart16550.fakeBase_sdtor: 
                                    push, rbp                                                 
                                    push, rbx                                                 
                                    mov, rbp, rsp                                             
                                    lea, rbx, qwordptr .nostromo.uart16550.fakeBase_vtbl_inst 
                                    mov, [rcx], rbx                                           ; =
                                    sub, rsp, 40                                              ; 40 = (passing size)32 + (align pad)8
                                    call, .nostromo.uart16550.fakeBase.cdtor                  ; (call label)
                                    add, rsp, 40                                              ; 40 = (passing size)32 + (align pad)8
                                    sub, rsp, 40                                              ; 40 = (passing size)32 + (align pad)8
                                    call, .sht.core.object_sdtor                              ; (call label)
                                    add, rsp, 40                                              ; 40 = (passing size)32 + (align pad)8
                                    mov, rsp, rbp                                             
                                    pop, rbx                                                  
                                    pop, rbp                                                  
                                    ret                                                       

