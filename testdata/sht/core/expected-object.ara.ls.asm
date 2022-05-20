.seg code               
.sht.core.object.cctor: 
                        push, rbp
                        mov, rbp, rsp
                        mov, rsp, rbp
                        pop, rbp
                        ret

.seg code               
.sht.core.object.cdtor: 
                        push, rbp
                        mov, rbp, rsp
                        mov, rsp, rbp
                        pop, rbp
                        ret

.seg const
.sht.core.object_vtbl_inst:
.data, 

.seg code               
.sht.core.object_sctor: 
                        push, rbp                                     
                        mov, rbp, rsp                                 
                        push, r10                                     ; codeshape spill
                        mov, r10, qwordptr .sht.core.object_vtbl_inst ; codeshape decomp
                        mov, [rcx], r10                               ; =
                        pop, r10                                      ; codeshape restore
                        sub, rsp, 32                                  
                        call, .sht.core.object.cctor                  ; (call label)
                        add, rsp, 32                                  
                        mov, rsp, rbp                                 
                        pop, rbp                                      
                        ret                                           

.seg code               
.sht.core.object_sdtor: 
                        push, rbp                                     
                        mov, rbp, rsp                                 
                        push, r10                                     ; codeshape spill
                        mov, r10, qwordptr .sht.core.object_vtbl_inst ; codeshape decomp
                        mov, [rcx], r10                               ; =
                        pop, r10                                      ; codeshape restore
                        sub, rsp, 32                                  
                        call, .sht.core.object.cdtor                  ; (call label)
                        add, rsp, 32                                  
                        mov, rsp, rbp                                 
                        pop, rbp                                      
                        ret                                           

