.seg data
isLessThan: .data, "YES!" <b> 0

.seg data
isNotLessThan: .data, "NO!" <b> 0

.seg data
atDone: .data, "done!" <b> 0

.seg code
.entrypoint:
   push, rbp
   mov, rbp, rsp
   sub, rsp, 32

   mov, r10, 4

   xor, r11, r11  ;;; <isLessThan?>, r11, r10, 3
   cmp, r10, 3
   setlts, r11      ;;; --end--

   cmp, r11, 1    ;;; <ifTrue>, r11, .entrypoint.nope
   je, .entrypoint.isLessThan       ;;; --end--

   mov, rcx, 1
   lea, rdx, qwordptr isNotLessThan
   call, ._osCall
   goto, .entrypoint.done

.seg code
.entrypoint.isLessThan:
   mov, rcx, 1
   lea, rdx, qwordptr isLessThan
   call, ._osCall
   goto, .entrypoint.done

.seg code
.entrypoint.done:
   mov, rcx, 1
   lea, rdx, qwordptr atDone
   call, ._osCall

;   add, rsp, 32 ; add is not sufficient - req of Win64 calling conv?
   mov, rsp, rbp
   pop, rbp
   ret
