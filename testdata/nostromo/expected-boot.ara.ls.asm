.seg code                   
.nostromo.bootstrapper.run: 
                            push, rbp
                            mov, rbp, rsp
                            mov, rsp, rbp
                            pop, rbp
                            ret

.seg code                     
.nostromo.bootstrapper.cctor: 
                              push, rbp
                              mov, rbp, rsp
                              mov, rsp, rbp
                              pop, rbp
                              ret

.seg code                     
.nostromo.bootstrapper.cdtor: 
                              push, rbp
                              mov, rbp, rsp
                              mov, rsp, rbp
                              pop, rbp
                              ret

.seg const
.nostromo.bootstrapper_vtbl_inst:
.data, .nostromo.bootstrapper.run 

.seg code                     
.nostromo.bootstrapper_sctor: 
                              push, rbp                                           
                              mov, rbp, rsp                                       
                              sub, rsp, 32                                        
                              call, .sht.cons.program_sctor                       ; (call label)
                              add, rsp, 32                                        
                              push, r10                                           ; codeshape spill
                              mov, r10, qwordptr .nostromo.bootstrapper_vtbl_inst ; codeshape decomp
                              mov, [rcx], r10                                     ; =
                              pop, r10                                            ; codeshape restore
                              sub, rsp, 32                                        
                              call, .nostromo.bootstrapper.cctor                  ; (call label)
                              add, rsp, 32                                        
                              mov, rsp, rbp                                       
                              pop, rbp                                            
                              ret                                                 

.seg code                     
.nostromo.bootstrapper_sdtor: 
                              push, rbp                                           
                              mov, rbp, rsp                                       
                              push, r10                                           ; codeshape spill
                              mov, r10, qwordptr .nostromo.bootstrapper_vtbl_inst ; codeshape decomp
                              mov, [rcx], r10                                     ; =
                              pop, r10                                            ; codeshape restore
                              sub, rsp, 32                                        
                              call, .nostromo.bootstrapper.cdtor                  ; (call label)
                              add, rsp, 32                                        
                              sub, rsp, 32                                        
                              call, .sht.cons.program_sdtor                       ; (call label)
                              add, rsp, 32                                        
                              mov, rsp, rbp                                       
                              pop, rbp                                            
                              ret                                                 

