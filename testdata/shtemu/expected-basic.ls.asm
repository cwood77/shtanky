.seg const
.yes:
.data, "YES!" <b> 0 

.seg code    
.entrypoint: 
             push, rbp               
             mov, rbp, rsp           
             sub, rsp, 32            
             mov, rcx, 1             ; shape:hoist imm from call
             mov, rdx, qwordptr .yes ; shape:hoist imm from call
             call, ._osCall          ; (call label)
             add, rsp, 32            
             mov, rsp, rbp           
             pop, rbp                
             ret                     

