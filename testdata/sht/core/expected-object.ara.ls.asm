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

.seg code
.sht.core.object_vtbl_inst:

.seg code               
.sht.core.object_sctor: 
                        push, rbp                                     
                        push, rbx                                     
                        mov, rbp, rsp                                 
                        lea, rbx, qwordptr .sht.core.object_vtbl_inst 
                        mov, [rcx], rbx                               ; =
                        sub, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        call, .sht.core.object.cctor                  ; (call label)
                        add, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        mov, rsp, rbp                                 
                        pop, rbx                                      
                        pop, rbp                                      
                        ret                                           

.seg code               
.sht.core.object_sdtor: 
                        push, rbp                                     
                        push, rbx                                     
                        mov, rbp, rsp                                 
                        lea, rbx, qwordptr .sht.core.object_vtbl_inst 
                        mov, [rcx], rbx                               ; =
                        sub, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        call, .sht.core.object.cdtor                  ; (call label)
                        add, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        mov, rsp, rbp                                 
                        pop, rbx                                      
                        pop, rbp                                      
                        ret                                           

