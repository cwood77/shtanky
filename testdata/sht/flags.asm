.seg data
._getflg_flags: .data, <qw> 0 <qw> 1 <qw> 2 <qw> 3

.seg code
._getflg:
   lea, r10, qwordptr ._getflg_flags
   add, r10, rcx
   mov, rax, [r10]

   ret
