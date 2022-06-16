.seg const
._strLit_helloWorld_0:
.data, "hello world!" <b> 0 

.seg code       
.test.test.run: 
                push, rbp                                
                push, rbx                                
                mov, rbp, rsp                            
                sub, rsp, 32                             
                mov, rbx, [rcx+8]                        ; fieldaccess: owner of _vtbl
                lea, rdx, qwordptr ._strLit_helloWorld_0 
                call, [rbx]                              ; (vtbl call)
                add, rsp, 32                             
                mov, rsp, rbp                            
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
                  push, rbx                               
                  mov, rbp, rsp                           
                  sub, rsp, 32                            
                  call, .sht.cons.program_sctor           ; (call label)
                  add, rsp, 32                            
                  lea, rbx, qwordptr .test.test_vtbl_inst 
                  mov, [rcx], rbx                         ; =
                  sub, rsp, 32                            
                  call, .test.test.cctor                  ; (call label)
                  add, rsp, 32                            
                  mov, rsp, rbp                           
                  pop, rbx                                
                  pop, rbp                                
                  ret                                     

.seg code         
.test.test_sdtor: 
                  push, rbp                               
                  push, rbx                               
                  mov, rbp, rsp                           
                  lea, rbx, qwordptr .test.test_vtbl_inst 
                  mov, [rcx], rbx                         ; =
                  sub, rsp, 32                            
                  call, .test.test.cdtor                  ; (call label)
                  add, rsp, 32                            
                  sub, rsp, 32                            
                  call, .sht.cons.program_sdtor           ; (call label)
                  add, rsp, 32                            
                  mov, rsp, rbp                           
                  pop, rbx                                
                  pop, rbp                                
                  ret                                     

