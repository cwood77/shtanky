.seg data
helloWorld: .data, "hello world" <b> 0

.seg data
nopeMsg: .data, "nope" <b> 0

.seg data
atDone: .data, "done!" <b> 0

.seg code
.entrypoint:
   push, rbp
   mov, rbp, rsp
   sub, rsp, 32

;  mov, r10, 1
;  xor, r11, r11  ;;; <isLessThan?>, r11, r10, 1
;  cmp, r10, 1
;  sblt, r11      ;;; --end--
;  cmp, r11, 1    ;;; <ifTrue>, r11, .entrypoint.nope
;  je, .entrypoint.nope       ;;; --end--

   mov, rcx, 1
   lea, rdx, qwordptr helloWorld
   call, ._osCall
   goto, .entrypoint.done

.seg code
.entrypoint.nope:
   mov, rcx, 1
   lea, rdx, qwordptr nopeMsg
   call, ._osCall

.seg code
.entrypoint.done:
   mov, rcx, 1
   lea, rdx, qwordptr atDone
   call, ._osCall

;   add, rsp, 32 ; add is not sufficient - req of Win64 calling conv?
   mov, rsp, rbp
   pop, rbp
   ret
