.seg const
.here0:
.data, "print works!" <b> 0 

.seg code
.entrypoint:
   push, rbp
   mov, rbp, rsp

   sub, rsp, 32                                  
   lea, rcx, qwordptr .here0
   call, ._print

   mov, rsp, rbp
   pop, rbp
   ret
