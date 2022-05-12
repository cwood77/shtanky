.seg const
.const0:
.data, "hello world!" <b> 0 

.seg code       
.test.test.run: 
                push, rbx         
                push, rdi         
                sub, rsp, 32      
                mov, rbx, [rcx+8] ; fieldaccess: owner of _vtbl
                mov, rdi, [rbx]   ; fieldaccess: owner of printLn
                mov, rdx, .const0 ; shape:hoist imm from call
                mov, rbx, rcx     ; (preserve) [combiner]
                mov, rcx, [rbx+8] ; shape:hoist addrOf from call
                call, [rdi]       ; (call ptr)
                add, rsp, 32      
                pop, rdi          
                pop, rbx          
                ret               

.seg code         
.test.test.cctor: 
                  ret

.seg code         
.test.test.cdtor: 
                  ret

.seg const
.test.test_vtbl_inst:
.data, .test.test.run 

.seg code         
.test.test_sctor: 
                  sub, rsp, 32                   
                  call, .sht.cons.program_sctor  ; (call label)
                  add, rsp, 32                   
                  mov, r10, .test.test_vtbl_inst ; codeshape decomp
                  mov, [rcx], r10                ; =
                  sub, rsp, 32                   
                  call, .test.test.cctor         ; (call label)
                  add, rsp, 32                   
                  ret                            

.seg code         
.test.test_sdtor: 
                  mov, r10, .test.test_vtbl_inst ; codeshape decomp
                  mov, [rcx], r10                ; =
                  sub, rsp, 32                   
                  call, .test.test.cdtor         ; (call label)
                  add, rsp, 32                   
                  sub, rsp, 32                   
                  call, .sht.cons.program_sdtor  ; (call label)
                  add, rsp, 32                   
                  ret                            

