.seg data
helloWorld: .data, "hello world" <b> 0

;.seg data
;helloWorldPtr: .data, helloWorld

.seg code
.entrypoint:
   push, rbp
   mov, rbp, rsp
   sub, rsp, 32

   mov, rcx, 1
   ; there is no way around this LEA
   ; at first, I thought I could generate a second ptr
   ; allocation in the datasegment that could patch, and then
   ; reference that with a MOV.  That doesn't work, because
   ; the address in the ptr allocation is still patched at link-time,
   ; not load-time, so it's still wrong at run-time.
   lea, rdx, qwordptr helloWorld
;  mov, rdx, qwordptr helloWorldPtr
   call, ._osCall

;   add, rsp, 32 ; add is not sufficient
   mov, rsp, rbp
   pop, rbp
   ret
