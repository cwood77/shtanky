.seg const
.const0:
.data, "hello world!" <b> 0 

.seg code            
.entrypoint:         
                     push, rbp                  
                     push, rbx                  
                     mov, rbp, rsp              
                     mov, r10, 0                ; codeshape decomp
                     cmp, r10, 7                
                     setlts, rbx                
                     cmp, rbx, 0                
                     je, .entrypoint.else.0     
                     sub, rsp, 32               
                     lea, rdx, qwordptr .const0 
                     mov, rcx, 1                ; shape:hoist imm from call
                     call, ._osCall             ; (call label)
                     add, rsp, 32               
                     goto, .entrypoint.endif.1  
.seg code            
.entrypoint.else.0:  
                     mov, r10, 0                ; codeshape decomp
                     cmp, r10, 0                
                     je, .entrypoint.else.2     
                     sub, rsp, 32               
                     lea, rdx, qwordptr .const0 
                     mov, rcx, 1                ; shape:hoist imm from call
                     call, ._osCall             ; (call label)
                     add, rsp, 32               
                     goto, .entrypoint.endif.3  
.seg code            
.entrypoint.else.2:  
                     sub, rsp, 32               
                     lea, rdx, qwordptr .const0 
                     mov, rcx, 1                ; shape:hoist imm from call
                     call, ._osCall             ; (call label)
                     add, rsp, 32               
                     goto, .entrypoint.endif.3  
.seg code            
.entrypoint.endif.3: 
                     goto, .entrypoint.endif.1  
.seg code            
.entrypoint.endif.1: 
                     mov, rsp, rbp              
                     pop, rbx                   
                     pop, rbp                   
                     ret                        

