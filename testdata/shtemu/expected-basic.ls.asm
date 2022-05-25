.seg const
.yes:
.data, "YES!" <b> 0 

.seg code            
.entrypoint:         
                     push, rbp                 
                     mov, rbp, rsp             
                     mov, r10, 1               ; codeshape decomp
                     cmp, r10, 0               
                     je, .entrypoint.else.0    
                     sub, rsp, 32              
                     mov, rcx, 1               ; shape:hoist imm from call
                     mov, rdx, qwordptr .yes   ; shape:hoist imm from call
                     call, ._osCall            ; (call label)
                     add, rsp, 32              
                     goto, .entrypoint.endif.1 
.entrypoint.else.0:  
                     sub, rsp, 0               
.entrypoint.endif.1: 
                     sub, rsp, 0               
                     mov, rsp, rbp             
                     pop, rbp                  
                     ret                       

