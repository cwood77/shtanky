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
consoleTarget_vtbl_inst:
.data, 

