.test.test.run: 
                push, rbx         
                push, rdi         
                sub, rsp, 32      
                mov, rbx, [rcx+8] ;    :_out
                mov, rdi, [rbx]   ;    :_vtbl
                mov, rbx, [rdi]   ;    :printLn
                mov, rbx, rcx     ;       (preserve) [combiner]
                mov, rcx, [rbx+8] ;    :_out
                call, rbx         
                add, rsp, 32      
                pop, rdi          
                pop, rbx          