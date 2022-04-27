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
                        mov, [rcx], .sht.core.object_vtbl_inst ; =
                        sub, rsp, 32                           
                        call, .sht.core.object.cctor           ; (call label)
                        add, rsp, 32                           
                        ret                                    

.seg code               
.sht.core.object_sdtor: 
                        mov, [rcx], .sht.core.object_vtbl_inst ; =
                        sub, rsp, 32                           
                        call, .sht.core.object.cdtor           ; (call label)
                        add, rsp, 32                           
                        ret                                    

