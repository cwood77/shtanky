.seg data
._osCall_impl: .data, <qw> 0

.seg code
._osCall:
   call, ._osCall_impl
   ret
