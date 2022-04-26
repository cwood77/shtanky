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
consoleTarget_vtbl_inst:
.data, 

