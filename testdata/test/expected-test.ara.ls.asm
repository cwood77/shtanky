.seg const
.const0:
.data, "hello world!" <b> 0

.seg code       
.test.test.run: 
                push, rbx         
                sub, rsp, 32      
                mov, rbx, [rcx+8] ; fieldaccess: _vtbl
                mov, rdx, .const0 ; shape:hoist imm from call
                mov, rbx, rcx     ;       (preserve) [combiner]
                mov, rcx, [rbx+8] ; shape:hoist addrOf from call
                call, [rbx]       ; (call ptr)
                add, rsp, 32      
                pop, rbx          
                ret               

