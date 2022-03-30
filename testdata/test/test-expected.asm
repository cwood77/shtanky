<decl>, rcx, rdx        ; func .test.test.run
mov, rbx, [rcx]         ;    :_out
mov, r10, [rbx]         ;    :type
mov, rbx, rcx           ;       (preserve rcx) [combiner]
mov, rcx, [r10]         ;    :printLn
mov, rdx, [rbx]         ;    :_out
call, rax, rcx, rdx, r8 