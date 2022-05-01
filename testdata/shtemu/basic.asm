.seg data
helloWorld: .data, "hello world" <b> 0

.seg code
.entrypoint:
   push, rbp
   mov, rbp, rsp
   sub, rsp, 32

   mov, rcx, 1
   mov, rdx, 0
   call, ._osCall_impl

   mov, rsp, rbp
   pop, rbp
   ret
