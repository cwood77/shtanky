.seg const
.nostromo.gDbgOut:
.data, 

.seg code                 
.nostromo.debugOut.write: 
                          push, rbp                         
                          push, rbx                         
                          push, rdi                         
                          mov, rbp, rsp                     
                          sub, rsp, 8                       
                          sub, rsp, 40                      ; 40 = (passing size)32 + (align pad)8
                          mov, rbx, rcx                     ; (preserve) [combiner]
                          mov, rcx, rdx                     ;       (msg req for rcx) [splitter]
                          call, .sht.core.string.length     ; (call label)
                          add, rsp, 40                      ; 40 = (passing size)32 + (align pad)8
                          mov, [rbp-8], rax                 ; =
                          sub, rsp, 40                      ; 40 = (passing size)32 + (align pad)8
                          mov, rdi, [rbx+8]                 ; fieldaccess: owner of _vtbl
                          sub, rsp, 40                      ; 40 = (passing size)32 + (align pad)8
                          mov, rdx, 0                       ;       (0 req for rdx) [splitter]
                          call, .sht.core.string.indexOpGet ; (call label)
                          add, rsp, 40                      ; 40 = (passing size)32 + (align pad)8
                          mov, rcx, [rbx+8]                 ; shape:hoist addrOf from call
                          mov, rdx, rax                     ;       (rval1 req for rdx) [splitter]
                          call, [rdi]                       ; vtbl call to .nostromo.uart16550.driver_vtbl::write8
                          add, rsp, 40                      ; 40 = (passing size)32 + (align pad)8
                          mov, rsp, rbp                     
                          pop, rdi                          
                          pop, rbx                          
                          pop, rbp                          
                          ret                               

.seg code                 
.nostromo.debugOut.cctor: 
                          push, rbp
                          mov, rbp, rsp
                          mov, rsp, rbp
                          pop, rbp
                          ret

.seg code                 
.nostromo.debugOut.cdtor: 
                          push, rbp
                          mov, rbp, rsp
                          mov, rsp, rbp
                          pop, rbp
                          ret

.seg code
.nostromo.debugOut_vtbl_inst:

.seg code                 
.nostromo.debugOut_sctor: 
                          push, rbp                                       
                          push, rbx                                       
                          mov, rbp, rsp                                   
                          sub, rsp, 40                                    ; 40 = (passing size)32 + (align pad)8
                          call, .sht.core.object_sctor                    ; (call label)
                          add, rsp, 40                                    ; 40 = (passing size)32 + (align pad)8
                          lea, rbx, qwordptr .nostromo.debugOut_vtbl_inst 
                          mov, [rcx], rbx                                 ; =
                          sub, rsp, 40                                    ; 40 = (passing size)32 + (align pad)8
                          call, .nostromo.debugOut.cctor                  ; (call label)
                          add, rsp, 40                                    ; 40 = (passing size)32 + (align pad)8
                          mov, rsp, rbp                                   
                          pop, rbx                                        
                          pop, rbp                                        
                          ret                                             

.seg code                 
.nostromo.debugOut_sdtor: 
                          push, rbp                                       
                          push, rbx                                       
                          mov, rbp, rsp                                   
                          lea, rbx, qwordptr .nostromo.debugOut_vtbl_inst 
                          mov, [rcx], rbx                                 ; =
                          sub, rsp, 40                                    ; 40 = (passing size)32 + (align pad)8
                          call, .nostromo.debugOut.cdtor                  ; (call label)
                          add, rsp, 40                                    ; 40 = (passing size)32 + (align pad)8
                          sub, rsp, 40                                    ; 40 = (passing size)32 + (align pad)8
                          call, .sht.core.object_sdtor                    ; (call label)
                          add, rsp, 40                                    ; 40 = (passing size)32 + (align pad)8
                          mov, rsp, rbp                                   
                          pop, rbx                                        
                          pop, rbp                                        
                          ret                                             

