.seg code    
.entrypoint: 
             sub, rsp, 8
             sub, rsp, 8
             add, rsp, 8
             add, rsp, 8
             ret

.seg code                  
.test.consoleTarget.cctor: 
                           ret

.seg code                  
.test.consoleTarget.cdtor: 
                           ret

.seg const
.test.consoleTarget_vtbl_inst:
.data, 

.seg code                  
.test.consoleTarget_sctor: 
                           sub, rsp, 32                              
                           call, .sht.core.object_sctor              ; (call label)
                           add, rsp, 32                              
                           mov, [rcx], .test.consoleTarget_vtbl_inst ; =
                           sub, rsp, 32                              
                           call, .test.consoleTarget.cctor           ; (call label)
                           add, rsp, 32                              
                           ret                                       

