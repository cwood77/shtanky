.seg code    
.entrypoint: 
             sub, rsp, 8
             sub, rsp, 8
             add, rsp, 8
             add, rsp, 8
             ret

.seg code                    
.assign.consoleTarget.cctor: 
                             ret

.seg code                    
.assign.consoleTarget.cdtor: 
                             ret

.seg const
.assign.consoleTarget_vtbl_inst:
.data, 

.seg code                    
.assign.consoleTarget_sctor: 
                             sub, rsp, 32                                
                             call, .sht.core.object_sctor                ; (call label)
                             add, rsp, 32                                
                             mov, [rcx], .assign.consoleTarget_vtbl_inst ; =
                             sub, rsp, 32                                
                             call, .assign.consoleTarget.cctor           ; (call label)
                             add, rsp, 32                                
                             ret                                         

