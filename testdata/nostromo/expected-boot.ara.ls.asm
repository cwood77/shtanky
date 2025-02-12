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

.seg code                         
.nostromo.bootstrapper_vtbl_inst: 
                                  goto, .nostromo.bootstrapper.run

.seg code                     
.nostromo.bootstrapper_sctor: 
                              push, rbp                                           
                              push, rbx                                           
                              mov, rbp, rsp                                       
                              sub, rsp, 40                                        
                              call, .sht.cons.program_sctor                       ; (call label)
                              add, rsp, 40                                        
                              lea, rbx, qwordptr .nostromo.bootstrapper_vtbl_inst 
                              mov, [rcx], rbx                                     ; =
                              sub, rsp, 40                                        
                              call, .nostromo.bootstrapper.cctor                  ; (call label)
                              add, rsp, 40                                        
                              mov, rsp, rbp                                       
                              pop, rbx                                            
                              pop, rbp                                            
                              ret                                                 

.seg code                     
.nostromo.bootstrapper_sdtor: 
                              push, rbp                                           
                              push, rbx                                           
                              mov, rbp, rsp                                       
                              lea, rbx, qwordptr .nostromo.bootstrapper_vtbl_inst 
                              mov, [rcx], rbx                                     ; =
                              sub, rsp, 40                                        
                              call, .nostromo.bootstrapper.cdtor                  ; (call label)
                              add, rsp, 40                                        
                              sub, rsp, 40                                        
                              call, .sht.cons.program_sdtor                       ; (call label)
                              add, rsp, 40                                        
                              mov, rsp, rbp                                       
                              pop, rbx                                            
                              pop, rbp                                            
                              ret                                                 

