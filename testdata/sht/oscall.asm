.seg data
._osCall_impl: .data, <qw> 0

.seg code
._osCall:
   push, rbp
   mov, rbp, rsp
   sub, rsp, 32

   call, qwordptr ._osCall_impl

   mov, rsp, rbp
   pop, rbp
   ret

.seg code
._print:
   push, rbp
   mov, rbp, rsp
   sub, rsp, 32

   mov, rdx, rcx
   mov, rcx, 1
   call, ._osCall

   mov, rsp, rbp
   pop, rbp
   ret
