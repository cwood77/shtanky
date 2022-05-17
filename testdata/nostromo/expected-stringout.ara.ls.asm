.seg const
.nostromo.gDbgOut:
.data, 

.seg code                 
.nostromo.debugOut.write: 
                          push, rbx                         
                          push, rsi                         
                          push, rdi                         
                          sub, rsp, 32                      
                          mov, rbx, [rcx+8]                 ; fieldaccess: owner of _vtbl
                          mov, rsi, [rbx]                   ; fieldaccess: owner of write8
                          sub, rsp, 32                      
                          mov, rbx, rdx                     ; (preserve) [combiner]
                          mov, rdx, 0                       ; shape:hoist imm from call
                          mov, rdi, rbx                     ; (preserve) [combiner]
                          mov, rbx, rcx                     ; (preserve) [combiner]
                          mov, rcx, rdi                     ;       (msg req for rcx) [splitter]
                          call, .sht.core.string.indexOpGet ; (call label)
                          add, rsp, 32                      
                          mov, rcx, [rbx+8]                 ; shape:hoist addrOf from call
                          mov, rdx, rax                     ;       (rval0 req for rdx) [splitter]
                          call, [rsi]                       ; (call ptr)
                          add, rsp, 32                      
                          pop, rdi                          
                          pop, rsi                          
                          pop, rbx                          
                          ret                               

.seg code                 
.nostromo.debugOut.cctor: 
                          ret

.seg code                 
.nostromo.debugOut.cdtor: 
                          ret

.seg const
.nostromo.debugOut_vtbl_inst:
.data, 

.seg code                 
.nostromo.debugOut_sctor: 
                          sub, rsp, 32                                    
                          call, .sht.core.object_sctor                    ; (call label)
                          add, rsp, 32                                    
                          mov, r10, qwordptr .nostromo.debugOut_vtbl_inst ; codeshape decomp
                          mov, [rcx], r10                                 ; =
                          sub, rsp, 32                                    
                          call, .nostromo.debugOut.cctor                  ; (call label)
                          add, rsp, 32                                    
                          ret                                             

.seg code                 
.nostromo.debugOut_sdtor: 
                          mov, r10, qwordptr .nostromo.debugOut_vtbl_inst ; codeshape decomp
                          mov, [rcx], r10                                 ; =
                          sub, rsp, 32                                    
                          call, .nostromo.debugOut.cdtor                  ; (call label)
                          add, rsp, 32                                    
                          sub, rsp, 32                                    
                          call, .sht.core.object_sdtor                    ; (call label)
                          add, rsp, 32                                    
                          ret                                             

