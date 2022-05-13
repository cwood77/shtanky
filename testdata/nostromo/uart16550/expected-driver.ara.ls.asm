.seg code                            
.nostromo.uart16550.fakeBase.write8: 
                                     ret

.seg code                           
.nostromo.uart16550.fakeBase.cctor: 
                                    ret

.seg code                           
.nostromo.uart16550.fakeBase.cdtor: 
                                    ret

.seg code                          
.nostromo.uart16550.driver.write8: 
                                   ret

.seg code                         
.nostromo.uart16550.driver.cctor: 
                                  ret

.seg code                         
.nostromo.uart16550.driver.cdtor: 
                                  ret

.seg const
.nostromo.uart16550.driver_vtbl_inst:
.data, .nostromo.uart16550.driver.write8 

.seg const
.nostromo.uart16550.fakeBase_vtbl_inst:
.data, .nostromo.uart16550.fakeBase.write8 

.seg code                         
.nostromo.uart16550.driver_sctor: 
                                  sub, rsp, 32                                            
                                  call, .nostromo.uart16550.fakeBase_sctor                ; (call label)
                                  add, rsp, 32                                            
                                  mov, r10, qwordptr .nostromo.uart16550.driver_vtbl_inst ; codeshape decomp
                                  mov, [rcx], r10                                         ; =
                                  sub, rsp, 32                                            
                                  call, .nostromo.uart16550.driver.cctor                  ; (call label)
                                  add, rsp, 32                                            
                                  ret                                                     

.seg code                         
.nostromo.uart16550.driver_sdtor: 
                                  mov, r10, qwordptr .nostromo.uart16550.driver_vtbl_inst ; codeshape decomp
                                  mov, [rcx], r10                                         ; =
                                  sub, rsp, 32                                            
                                  call, .nostromo.uart16550.driver.cdtor                  ; (call label)
                                  add, rsp, 32                                            
                                  sub, rsp, 32                                            
                                  call, .nostromo.uart16550.fakeBase_sdtor                ; (call label)
                                  add, rsp, 32                                            
                                  ret                                                     

.seg code                           
.nostromo.uart16550.fakeBase_sctor: 
                                    sub, rsp, 32                                              
                                    call, .sht.core.object_sctor                              ; (call label)
                                    add, rsp, 32                                              
                                    mov, r10, qwordptr .nostromo.uart16550.fakeBase_vtbl_inst ; codeshape decomp
                                    mov, [rcx], r10                                           ; =
                                    sub, rsp, 32                                              
                                    call, .nostromo.uart16550.fakeBase.cctor                  ; (call label)
                                    add, rsp, 32                                              
                                    ret                                                       

.seg code                           
.nostromo.uart16550.fakeBase_sdtor: 
                                    mov, r10, qwordptr .nostromo.uart16550.fakeBase_vtbl_inst ; codeshape decomp
                                    mov, [rcx], r10                                           ; =
                                    sub, rsp, 32                                              
                                    call, .nostromo.uart16550.fakeBase.cdtor                  ; (call label)
                                    add, rsp, 32                                              
                                    sub, rsp, 32                                              
                                    call, .sht.core.object_sdtor                              ; (call label)
                                    add, rsp, 32                                              
                                    ret                                                       

