.seg code                   
.nostromo.bootstrapper.run: 
                            ret

.seg code                     
.nostromo.bootstrapper.cctor: 
                              ret

.seg code                     
.nostromo.bootstrapper.cdtor: 
                              ret

.seg const
.nostromo.bootstrapper_vtbl_inst:
.data, .nostromo.bootstrapper.run 

.seg code                     
.nostromo.bootstrapper_sctor: 
                              sub, rsp, 32                               
                              call, .sht.cons.program_sctor              ; (call label)
                              add, rsp, 32                               
                              mov, r10, .nostromo.bootstrapper_vtbl_inst ; codeshape decomp
                              mov, [rcx], r10                            ; =
                              sub, rsp, 32                               
                              call, .nostromo.bootstrapper.cctor         ; (call label)
                              add, rsp, 32                               
                              ret                                        

.seg code                     
.nostromo.bootstrapper_sdtor: 
                              mov, r10, .nostromo.bootstrapper_vtbl_inst ; codeshape decomp
                              mov, [rcx], r10                            ; =
                              sub, rsp, 32                               
                              call, .nostromo.bootstrapper.cdtor         ; (call label)
                              add, rsp, 32                               
                              sub, rsp, 32                               
                              call, .sht.cons.program_sdtor              ; (call label)
                              add, rsp, 32                               
                              ret                                        

