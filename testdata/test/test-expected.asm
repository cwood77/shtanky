.test.test.run: 
                sub rsp, 32       
                mov rbx, [rcx]    ;    :_out
                mov r12, [rbx]    ;    :type
                mov rbx, rcx      ;       (preserve) [combiner]
                mov rcx, [r12]    ;    :printLn
                mov rdx, [rbx]    ;    :_out
                call rcx, rdx, r8 
                add rsp, 32       