.seg code       
.test.test.run: 
                push, rbx         
                push, rdi         
                sub, rsp, 32      
                mov, rdi, [rcx+8] ; fieldaccess: _vtbl
                mov, rdx, .const0 ; shape:hoist imm from call
                mov, rbx, rcx     ;       (preserve) [combiner]
                mov, rcx, [rbx+8] ; shape:hoist addrOf from call
                call, [rdi]       ; (call ptr)
                add, rsp, 32      
                pop, rdi          
                pop, rbx          
                ret               

.seg const
.const0:
.data, "hello world!" <b> 0

