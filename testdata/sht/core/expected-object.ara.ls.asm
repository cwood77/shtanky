.seg code               
.sht.core.object.cctor: 
                        ret

.seg code               
.sht.core.object.cdtor: 
                        ret

.seg const
.sht.core.object_vtbl_inst:
.data, 

.seg code               
.sht.core.object_sctor: 
                        mov, r10, .sht.core.object_vtbl_inst ; codeshape decomp
                        mov, [rcx], r10                      ; =
                        sub, rsp, 32                         
                        call, .sht.core.object.cctor         ; (call label)
                        add, rsp, 32                         
                        ret                                  

.seg code               
.sht.core.object_sdtor: 
                        mov, r10, .sht.core.object_vtbl_inst ; codeshape decomp
                        mov, [rcx], r10                      ; =
                        sub, rsp, 32                         
                        call, .sht.core.object.cdtor         ; (call label)
                        add, rsp, 32                         
                        ret                                  

