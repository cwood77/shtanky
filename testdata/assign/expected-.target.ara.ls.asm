.seg code    
.entrypoint: 
             push, rbx                        
             push, rdi                        
             sub, rsp, 24                     
             sub, rsp, 8                      
             sub, rsp, 32                     
             mov, rbx, rcx                    ;       (preserve) [combiner]
             mov, rcx, [rbp-8]                ;       (cout req for rcx) [splitter]
             call, .sht.cons.stdout_sctor     ; (call label)
             add, rsp, 32                     
             sub, rsp, 16                     
             sub, rsp, 32                     
             mov, rdi, rcx                    ;       (preserve) [combiner]
             mov, rcx, [rbp-8]                ;       (obj0 req for rcx) [splitter]
             call, .assign.assignTester_sctor ; (call label)
             add, rsp, 32                     
             mov, [rcx+8], rdi                ; =
             sub, rsp, 32                     
             mov, rdi, [rcx]                  ; fieldaccess: owner of run
             mov, rdx, rcx                    ;       (args req for rdx) [splitter]
             call, [rdi]                      ; (call ptr)
             add, rsp, 32                     
             sub, rsp, 32                     
             call, .assign.assignTester_sdtor ; (call label)
             add, rsp, 32                     
             sub, rsp, 32                     
             call, .sht.cons.stdout_sdtor     ; (call label)
             add, rsp, 32                     
             add, rsp, 16                     
             add, rsp, 8                      
             add, rsp, 24                     
             pop, rdi                         
             pop, rbx                         
             ret                              

.seg code                    
.assign.consoleTarget.cctor: 
                             ret

.seg code                    
.assign.consoleTarget.cdtor: 
                             ret

.seg const
.assign.consoleTarget_vtbl_inst:
.data, 

.seg code                    
.assign.consoleTarget_sctor: 
                             sub, rsp, 32                                
                             call, .sht.core.object_sctor                ; (call label)
                             add, rsp, 32                                
                             mov, [rcx], .assign.consoleTarget_vtbl_inst ; =
                             sub, rsp, 32                                
                             call, .assign.consoleTarget.cctor           ; (call label)
                             add, rsp, 32                                
                             ret                                         

.seg code                    
.assign.consoleTarget_sdtor: 
                             mov, [rcx], .assign.consoleTarget_vtbl_inst ; =
                             sub, rsp, 32                                
                             call, .assign.consoleTarget.cdtor           ; (call label)
                             add, rsp, 32                                
                             sub, rsp, 32                                
                             call, .sht.core.object_sdtor                ; (call label)
                             add, rsp, 32                                
                             ret                                         

