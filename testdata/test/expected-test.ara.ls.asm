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
                mov, rbx, rcx     ;       (preserve) [combiner]
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
test_vtbl_inst:
.data, .test.test.run 

