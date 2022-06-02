.seg const
.zero:
.data, "0" <b> 0 

.seg const
.one:
.data, "1" <b> 0 

.seg const
.other:
.data, "other!" <b> 0 

.seg code            
.entrypoint:         
                     push, rbp                 
                     push, rbx                 
                     mov, rbp, rsp             
                     sub, rsp, 8               
                     sub, rsp, 32              
                     mov, rcx, 8               ; shape:hoist imm from call
                     call, ._getflg            ; (call label)
                     add, rsp, 32              
                     mov, [rbp-8], rax         ; =
                     xor, rbx, rbx             
                     cmp, [rbp-8], 1           
                     setlts, rbx               
                     cmp, rbx, 0               
                     je, .entrypoint.else.0    
                     sub, rsp, 32              
                     lea, rcx, qwordptr .zero  
                     call, ._print             ; (call label)
                     add, rsp, 32              
                     goto, .entrypoint.endif.1 
.seg code            
.entrypoint.else.0:  
                     xor, rbx, rbx             
                     cmp, [rbp-8], 2           
                     setlts, rbx               
                     cmp, rbx, 0               
                     je, .entrypoint.else.2    
                     sub, rsp, 32              
                     lea, rcx, qwordptr .one   
                     call, ._print             ; (call label)
                     add, rsp, 32              
                     goto, .entrypoint.endif.3 
.seg code            
.entrypoint.else.2:  
                     sub, rsp, 32              
                     lea, rcx, qwordptr .other 
                     call, ._print             ; (call label)
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

