.test.test.run: 
                push rbx          
                push rdi          
                sub rsp, 32       
                mov rbx, [rcx]    ;    :_out
                mov rdi, [rbx]    ;    :type
                mov rbx, rcx      ;       (preserve) [combiner]
                mov rcx, [rdi]    ;    :printLn
                mov rdx, [rbx]    ;    :_out
                call rcx, rdx, r8 
                add rsp, 32       
                pop rdi           
                pop rbx           