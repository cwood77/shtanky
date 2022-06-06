.seg const
.const0:
.data, "hello world!" <b> 0 

.seg code       
.test.test.run: 
                push, rbp                  
                push, rbx                  
                push, rdi                  
                mov, rbp, rsp              
                sub, rsp, 32               
                mov, rdi, [rcx+8]          ; fieldaccess: owner of _vtbl
                lea, rdx, qwordptr .const0 
                mov, rbx, rcx              ; (preserve) [combiner]
                mov, rcx, [rbx+8]          ; shape:hoist addrOf from call
                call, [rdi]                ; (vtbl call)
                add, rsp, 32               
                mov, rsp, rbp              
                pop, rdi                   
                pop, rbx                   
                pop, rbp                   
                ret                        

.seg code         
.test.test.cctor: 
                  push, rbp
                  mov, rbp, rsp
                  mov, rsp, rbp
                  pop, rbp
                  ret

.seg code         
.test.test.cdtor: 
                  push, rbp
                  mov, rbp, rsp
                  mov, rsp, rbp
                  pop, rbp
                  ret

.seg code             
.test.test_vtbl_inst: 
                      goto, .test.test.run

.seg code         
.test.test_sctor: 
                  push, rbp                               
                  mov, rbp, rsp                           
                  sub, rsp, 32                            
                  call, .sht.cons.program_sctor           ; (call label)
                  add, rsp, 32                            
                  mov, r10, qwordptr .test.test_vtbl_inst ; codeshape decomp
                  mov, [rcx], r10                         ; =
                  sub, rsp, 32                            
                  call, .test.test.cctor                  ; (call label)
                  add, rsp, 32                            
                  mov, rsp, rbp                           
                  pop, rbp                                
                  ret                                     

.seg code         
.test.test_sdtor: 
                  push, rbp                               
                  mov, rbp, rsp                           
                  mov, r10, qwordptr .test.test_vtbl_inst ; codeshape decomp
                  mov, [rcx], r10                         ; =
                  sub, rsp, 32                            
                  call, .test.test.cdtor                  ; (call label)
                  add, rsp, 32                            
                  sub, rsp, 32                            
                  call, .sht.cons.program_sdtor           ; (call label)
                  add, rsp, 32                            
                  mov, rsp, rbp                           
                  pop, rbp                                
                  ret                                     

