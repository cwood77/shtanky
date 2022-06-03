.seg const
.here:
.data, "made it here" <b> 0 

.seg const
.here0:
.data, "made it here (0)" <b> 0 

.seg const
.here1:
.data, "made it here (1)" <b> 0 

.seg const
.here2:
.data, "made it here (2)" <b> 0 

.seg const
.objcctor:
.data, "obj ctor" <b> 0 

.seg const
.progtxt:
.data, "program" <b> 0 

.seg const
.insideRun:
.data, "inside test.run" <b> 0 

;; ============================ experiment using VStudio as example =================

.seg code
.vfunc:
   push, rbp
   mov, rbp, rsp

push, rcx
push, rdx
sub, rsp, 32                                  
lea, rcx, qwordptr .here0
call, ._print
add, rsp, 32                                  
pop, rdx
pop, rcx

   mov, rsp, rbp
   pop, rbp
   ret

.seg const
.example_vtbl:
.data, .vfunc

.seg code
.entrypoint_vstudio:
   push, rbp
   mov, rbp, rsp

   ; construct object
   sub, rsp, 8
   lea, r10, [rbp-8] ; r10 = objptr
   lea, r11, qwordptr .example_vtbl
   mov, [r10], r11

   ; invoke vfunc
   mov, r11, [r10]
   sub, rsp, 32                                  
   call, r11
   add, rsp, 32                                  

   mov, rsp, rbp
   pop, rbp
   ret

;; ============================ theories from I64 docs =================

;;;;;;; this actually WORKS!  WOW
;; so what are the necessary ingridents?
;; - call instrs can call absolute indirect ptrs, or RIP-rel only!!
;;   runtime formats (i.e. not embedded in instr) are abs ind
;;   impact: I _have_ to stuff a func pointer in a memory block _then_
;;   call it
;; - lea _can_ derive an absolute address from a RIP-rel offset
;; - can't build a vtable directly because it'll containly meaningless
;;   RIP-rel offsets
;; - CAN build a vtable of jumps (they're 5 bytes -- e9 xx xx xx xx)

.seg code
.vfunc0:
   push, rbp
   mov, rbp, rsp

push, rcx
push, rdx
sub, rsp, 32                                  
lea, rcx, qwordptr .here0
call, ._print
add, rsp, 32                                  
pop, rdx
pop, rcx

   mov, rsp, rbp
   pop, rbp
   ret

.seg code
.vfunc1:
   push, rbp
   mov, rbp, rsp

push, rcx
push, rdx
sub, rsp, 32                                  
lea, rcx, qwordptr .here1
call, ._print
add, rsp, 32                                  
pop, rdx
pop, rcx

   mov, rsp, rbp
   pop, rbp
   ret

.seg code
.vfunc2:
   push, rbp
   mov, rbp, rsp

push, rcx
push, rdx
sub, rsp, 32                                  
lea, rcx, qwordptr .here2
call, ._print
add, rsp, 32                                  
pop, rdx
pop, rcx

   mov, rsp, rbp
   pop, rbp
   ret

.seg code
.jmp_vtbl:
   goto, .vfunc0
   goto, .vfunc1
   goto, .vfunc2

.seg code
.entrypoint:
   push, rbp
   mov, rbp, rsp

   xor, rcx, rcx
   call, ._getflg

   lea, r10, qwordptr .jmp_vtbl
   add, r10, rax
   sub, rsp, 8
   mov, [rbp-8], r10
   call, [rbp-8]

   mov, rsp, rbp
   pop, rbp
   ret

;; ============================ end ===================================

.seg code    
.entrypoint_123098a09: 
             push, rbp                    
             push, rbx                    
             push, rdi                    
             mov, rbp, rsp                
             sub, rsp, 24                 
             mov, rbx, rcx     ; args     ; (preserve) [combiner]
             lea, rcx, [rbp-8]            ; cout
             sub, rsp, 32                 
             call, .sht.cons.stdout_sctor ; (call label)                ; survived this call!
             add, rsp, 32                 
             mov, rdi, rcx     ; stdout   ; (preserve) [combiner]
             lea, rcx, [rbp-24]           ; obj0
             sub, rsp, 32                 
             call, .test.test_sctor       ; (call label)                ; survived this call!
             add, rsp, 32                 
             mov, [rcx+8], rdi  ;field    ; =
             sub, rsp, 32                 
         mov, r10, [rcx]   ;vtbl      ; fieldaccess: owner of run    ;;;;;;;;;;;;;;; isn't this stomping?
         mov, rdx, rbx                ;       (args req for rdx) [splitter]
         call, [r10]   ;(test,args)   ; (call ptr)
    ;        mov, rbx, [rcx]   ;vtbl      ; fieldaccess: owner of run    ;;;;;;;;;;;;;;; isn't this stomping?
    ;        mov, rdx, rbx                ;       (args req for rdx) [splitter]
    ;        call, [rbx]   ;(test,vtbl)   ; (call ptr)
             add, rsp, 32                 
push, rcx
push, rdx
sub, rsp, 32                                  
lea, rcx, qwordptr .here
call, ._print
add, rsp, 32                                  
pop, rdx
pop, rcx

             sub, rsp, 32                 
             call, .test.test_sdtor       ; (call label)                ; did not survive this :(
             add, rsp, 32                 

             sub, rsp, 32                 
             mov, rcx, rdi                ; (restore [combiner])
             call, .sht.cons.stdout_sdtor ; (call label)
             add, rsp, 32                 
             mov, rsp, rbp                
             pop, rdi                     
             pop, rbx                     
             pop, rbp                     
             ret                          

.seg code                  
.sht.cons.iStream.printLn: 
                           push, rbp
                           mov, rbp, rsp
                           mov, rsp, rbp
                           pop, rbp
                           ret

.seg code                
.sht.cons.iStream.cctor: 
                         push, rbp
                         mov, rbp, rsp
                         mov, rsp, rbp
                         pop, rbp
                         ret

.seg code                
.sht.cons.iStream.cdtor: 
                         push, rbp
                         mov, rbp, rsp
                         mov, rsp, rbp
                         pop, rbp
                         ret

.seg code                 
.sht.cons.stdout.printLn: 
                          push, rbp     
                          mov, rbp, rsp 
                          sub, rsp, 32  
                          call, ._print ; (call label)
                          add, rsp, 32  
                          mov, rsp, rbp 
                          pop, rbp      
                          ret           

.seg code               
.sht.cons.stdout.cctor: 
                        push, rbp
                        mov, rbp, rsp
                        mov, rsp, rbp
                        pop, rbp
                        ret

.seg code               
.sht.cons.stdout.cdtor: 
                        push, rbp
                        mov, rbp, rsp
                        mov, rsp, rbp
                        pop, rbp
                        ret

.seg code              
.sht.cons.program.run: 
                       push, rbp
                       mov, rbp, rsp
                       mov, rsp, rbp
                       pop, rbp
                       ret

.seg code                
.sht.cons.program.cctor: 
                         push, rbp
                         mov, rbp, rsp
                         mov, rsp, rbp
                         pop, rbp
                         ret

.seg code                
.sht.cons.program.cdtor: 
                         push, rbp
                         mov, rbp, rsp
                         mov, rsp, rbp
                         pop, rbp
                         ret

.seg const
.sht.cons.iStream_vtbl_inst:
.data, .sht.cons.iStream.printLn 

.seg const
.sht.cons.program_vtbl_inst:
.data, .sht.cons.program.run 

.seg const
.sht.cons.stdout_vtbl_inst:
.data, .sht.cons.stdout.printLn 

.seg code                
.sht.cons.iStream_sctor: 
                         push, rbp                                      
                         mov, rbp, rsp                                  
                         sub, rsp, 32                                   
                         call, .sht.core.object_sctor                   ; (call label)
                         add, rsp, 32                                   
                         mov, r10, qwordptr .sht.cons.iStream_vtbl_inst ; codeshape decomp
                         mov, [rcx], r10                                ; =
                         sub, rsp, 32                                   
                         call, .sht.cons.iStream.cctor                  ; (call label)
                         add, rsp, 32                                   
                         mov, rsp, rbp                                  
                         pop, rbp                                       
                         ret                                            

.seg code                
.sht.cons.iStream_sdtor: 
                         push, rbp                                      
                         mov, rbp, rsp                                  
                         mov, r10, qwordptr .sht.cons.iStream_vtbl_inst ; codeshape decomp
                         mov, [rcx], r10                                ; =
                         sub, rsp, 32                                   
                         call, .sht.cons.iStream.cdtor                  ; (call label)
                         add, rsp, 32                                   
                         sub, rsp, 32                                   
                         call, .sht.core.object_sdtor                   ; (call label)
                         add, rsp, 32                                   
                         mov, rsp, rbp                                  
                         pop, rbp                                       
                         ret                                            

.seg code                
.sht.cons.program_sctor: 
                         push, rbp                                      
                         mov, rbp, rsp                                  
                         sub, rsp, 32                                   
                         call, .sht.core.object_sctor                   ; (call label)
                         add, rsp, 32                                   
                         mov, r10, qwordptr .sht.cons.program_vtbl_inst ; codeshape decomp
                         mov, [rcx], r10                                ; =
                         sub, rsp, 32                                   
                         call, .sht.cons.program.cctor                  ; (call label)
                         add, rsp, 32                                   
                         mov, rsp, rbp                                  
                         pop, rbp                                       
                         ret                                            

.seg code                
.sht.cons.program_sdtor: 
                         push, rbp                                      
                         mov, rbp, rsp                                  
                         mov, r10, qwordptr .sht.cons.program_vtbl_inst ; codeshape decomp
                         mov, [rcx], r10                                ; =
                         sub, rsp, 32                                   
                         call, .sht.cons.program.cdtor                  ; (call label)
                         add, rsp, 32                                   
                         sub, rsp, 32                                   
                         call, .sht.core.object_sdtor                   ; (call label)
                         add, rsp, 32                                   
                         mov, rsp, rbp                                  
                         pop, rbp                                       
                         ret                                            

.seg code               
.sht.cons.stdout_sctor: 
                        push, rbp                                     
                        mov, rbp, rsp                                 
                        sub, rsp, 32                                  
                        call, .sht.cons.iStream_sctor                 ; (call label)
                        add, rsp, 32                                  
                        mov, r10, qwordptr .sht.cons.stdout_vtbl_inst ; codeshape decomp
                        mov, [rcx], r10                               ; =
                        sub, rsp, 32                                  
                        call, .sht.cons.stdout.cctor                  ; (call label)
                        add, rsp, 32                                  
                        mov, rsp, rbp                                 
                        pop, rbp                                      
                        ret                                           

.seg code               
.sht.cons.stdout_sdtor: 
                        push, rbp                                     
                        mov, rbp, rsp                                 
                        mov, r10, qwordptr .sht.cons.stdout_vtbl_inst ; codeshape decomp
                        mov, [rcx], r10                               ; =
                        sub, rsp, 32                                  
                        call, .sht.cons.stdout.cdtor                  ; (call label)
                        add, rsp, 32                                  
                        sub, rsp, 32                                  
                        call, .sht.cons.iStream_sdtor                 ; (call label)
                        add, rsp, 32                                  
                        mov, rsp, rbp                                 
                        pop, rbp                                      
                        ret                                           

.seg code               
.sht.core.object.cctor: 
                        push, rbp
                        mov, rbp, rsp
                        mov, rsp, rbp
                        pop, rbp
                        ret

.seg code               
.sht.core.object.cdtor: 
                        push, rbp
                        mov, rbp, rsp
                        mov, rsp, rbp
                        pop, rbp
                        ret

.seg const
.sht.core.object_vtbl_inst:
.data, 

.seg code               
.sht.core.object_sctor: 
                        push, rbp                                     
                        mov, rbp, rsp                                 
                        mov, r10, qwordptr .sht.core.object_vtbl_inst ; codeshape decomp
                        mov, [rcx], r10                               ; =
                        sub, rsp, 32                                  
                        call, .sht.core.object.cctor                  ; (call label)
                        add, rsp, 32                                  
                        mov, rsp, rbp                                 
                        pop, rbp                                      
                        ret                                           

.seg code               
.sht.core.object_sdtor: 
                        push, rbp                                     
                        mov, rbp, rsp                                 
                        mov, r10, qwordptr .sht.core.object_vtbl_inst ; codeshape decomp
                        mov, [rcx], r10                               ; =
                        sub, rsp, 32                                  
                        call, .sht.core.object.cdtor                  ; (call label)
                        add, rsp, 32                                  
                        mov, rsp, rbp                                 
                        pop, rbp                                      
                        ret                                           

.seg const
.const0:
.data, "hello world!" <b> 0 

.seg code       
.test.test.run: 
                push, rbp                  
                push, rbx                  
                push, rdi                  
                mov, rbp, rsp              
push, rcx
push, rdx
sub, rsp, 32                                  
lea, rcx, qwordptr .insideRun
call, ._print
add, rsp, 32                                  
pop, rdx
pop, rcx

                sub, rsp, 32               
                mov, rbx, [rcx+8]          ; fieldaccess: owner of _vtbl
                mov, rdi, [rbx]            ; fieldaccess: owner of printLn
                lea, rdx, qwordptr .const0 
                mov, rbx, rcx              ; (preserve) [combiner]
                mov, rcx, [rbx+8]          ; shape:hoist addrOf from call
                call, [rdi]                ; (call ptr)
                add, rsp, 32               
                mov, rsp, rbp              
                pop, rdi                   
                pop, rbx                   
                pop, rbp                   
                ret                        

.seg code         
.test.test.cctor: 
                  push, rbp
                  mov, rbp, rsp
                  mov, rsp, rbp
                  pop, rbp
                  ret

.seg code         
.test.test.cdtor: 
                  push, rbp
                  mov, rbp, rsp
                  mov, rsp, rbp
                  pop, rbp
                  ret

.seg const
.test.test_vtbl_inst:
.data, .test.test.run 

.seg code         
.test.test_sctor: 

; TODO VStudio sets up vtables as follows
;
; mov rax, qword ptr [this]
; lea rcx, [cat::vftable]
; mov qword ptr [rax], rcx
; mov rax, qword ptr [this]
;
; and calls vtables as follows
;
; mov rax, qword ptr [a] ; i.e. the object to invoke on
; mov rax, qword ptr [rax]
; mov rcx, qword ptr [a]
; call qword ptr[rax]    ; i.e. 0xff 0x10 ... CALL{ff /2}.. r/m64 near abs indir
;
; VStudio vtables have fully-resolved 64-bit pointers... :(  YES!
; actually it looks like it's emitting a table of jmps
;           e9 5f 16 00 00  JMP{E9 cd}
; which is where you go immediate after the call
;
; I VStudio is a bad example here; not PIC.  I'm going to just operate off the I64 manual.
;

                  push, rbp                               
                  mov, rbp, rsp                           
                  sub, rsp, 32                            
                  call, .sht.cons.program_sctor           ; (call label)
                  add, rsp, 32                            
                  mov, r10, qwordptr .test.test_vtbl_inst ; codeshape decomp
                  mov, [rcx], r10                         ; =
                  sub, rsp, 32                            
                  call, .test.test.cctor                  ; (call label)
                  add, rsp, 32                            
                  mov, rsp, rbp                           
                  pop, rbp                                
                  ret                                     

.seg code         
.test.test_sdtor: 
                  push, rbp                               
                  mov, rbp, rsp                           
                  mov, r10, qwordptr .test.test_vtbl_inst ; codeshape decomp
                  mov, [rcx], r10                         ; =
                  sub, rsp, 32                            
                  call, .test.test.cdtor                  ; (call label)
                  add, rsp, 32                            
                  sub, rsp, 32                            
                  call, .sht.cons.program_sdtor           ; (call label)
                  add, rsp, 32                            
                  mov, rsp, rbp                           
                  pop, rbp                                
                  ret                                     

