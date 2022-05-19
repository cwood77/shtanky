.seg data
helloWorld: .data, "hello world" <b> 0

.seg code
.entrypoint:
   push, rbp
   mov, rbp, rsp
   sub, rsp, 32

   mov, rcx, 1
   lea, rdx, qwordptr helloWorld
   call, ._osCall

;   add, rsp, 32 ; add is not sufficient
   mov, rsp, rbp
   pop, rbp
   ret
