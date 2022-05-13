.seg code    
.entrypoint: 
             push, rbx                    
             push, rdi                    
             sub, rsp, 24                 
             sub, rsp, 8                  
             sub, rsp, 32                 
             mov, rbx, rcx                ; (preserve) [combiner]
             mov, rcx, [rbp-8]            ;       (cout req for rcx) [splitter]
             call, .sht.cons.stdout_sctor ; (call label)
             add, rsp, 32                 
             sub, rsp, 16                 
             sub, rsp, 32                 
             mov, rdi, rcx                ; (preserve) [combiner]
             mov, rcx, [rbp-8]            ;       (obj0 req for rcx) [splitter]
             call, .test.test_sctor       ; (call label)
             add, rsp, 32                 
             mov, [rcx+8], rdi            ; =
             sub, rsp, 32                 
             mov, rbx, [rcx]              ; fieldaccess: owner of run
             mov, rdx, rbx                ;       (args req for rdx) [splitter]
             call, [rbx]                  ; (call ptr)
             add, rsp, 32                 
             sub, rsp, 32                 
             call, .test.test_sdtor       ; (call label)
             add, rsp, 32                 
             sub, rsp, 32                 
             mov, rbx, rcx                ; (preserve) [combiner]
             mov, rcx, rdi                ; (restore [combiner])
             call, .sht.cons.stdout_sdtor ; (call label)
             add, rsp, 32                 
             add, rsp, 16                 
             add, rsp, 8                  
             add, rsp, 24                 
             pop, rdi                     
             pop, rbx                     
             ret                          

.seg code                  
.test.consoleTarget.cctor: 
                           ret

.seg code                  
.test.consoleTarget.cdtor: 
                           ret

.seg const
.test.consoleTarget_vtbl_inst:
.data, 

.seg code                  
.test.consoleTarget_sctor: 
                           sub, rsp, 32                                     
                           call, .sht.core.object_sctor                     ; (call label)
                           add, rsp, 32                                     
                           mov, r10, qwordptr .test.consoleTarget_vtbl_inst ; codeshape decomp
                           mov, [rcx], r10                                  ; =
                           sub, rsp, 32                                     
                           call, .test.consoleTarget.cctor                  ; (call label)
                           add, rsp, 32                                     
                           ret                                              

.seg code                  
.test.consoleTarget_sdtor: 
                           mov, r10, qwordptr .test.consoleTarget_vtbl_inst ; codeshape decomp
                           mov, [rcx], r10                                  ; =
                           sub, rsp, 32                                     
                           call, .test.consoleTarget.cdtor                  ; (call label)
                           add, rsp, 32                                     
                           sub, rsp, 32                                     
                           call, .sht.core.object_sdtor                     ; (call label)
                           add, rsp, 32                                     
                           ret                                              

