.seg data
._osCall_impl: .data, <qw> 0

.seg code
.entrypoint:
   mov, rcx, 1
   mov, rdx, 0
   sub, rsp, 32
   call, ._osCall_impl
   add, rsp, 32
   ret
