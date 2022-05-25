.seg code            
.entrypoint:         
                     push, rbp                 
                     mov, rbp, rsp             
                     mov, r10, 1               ; codeshape decomp
                     cmp, r10, 0               
                     je, .entrypoint.else.0    
                     sub, rsp, 32              
                     mov, rcx, 1               ; shape:hoist imm from call
                     mov, rdx, 0               ; shape:hoist imm from call
                     call, ._osCall            ; (call label)
                     add, rsp, 32              
                     goto, .entrypoint.endif.1 
.seg code            
.entrypoint.else.0:  
                     goto, .entrypoint.endif.1 
.seg code            
.entrypoint.endif.1: 
                     mov, rsp, rbp             
                     pop, rbp                  
                     ret                       

