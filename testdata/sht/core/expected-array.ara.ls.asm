.seg code                                 
.sht.core.array<.sht.core.string>.resize: 
                                          push, rbp          
                                          push, rbx          
                                          push, rdi          
                                          mov, rbp, rsp      
                                          sub, rsp, 32       
                                          mov, rbx, rcx      ; (preserve) [combiner]
                                          mov, rcx, [rbx+16] ; shape:hoist addrOf from call
                                          mov, rdi, rdx      ; (preserve) [combiner]
                                          mov, rdx, [rbx+8]  ; shape:hoist addrOf from call
                                          mov, r8, rdi       ;       (size req for r8) [splitter]
                                          call, ._arrresize  ; (call label)
                                          add, rsp, 32       
                                          mov, [rbx+16], rax ; =
                                          mov, [rbx+8], r8   ; =
                                          mov, rsp, rbp      
                                          pop, rdi           
                                          pop, rbx           
                                          pop, rbp           
                                          ret                

.seg code                                    
.sht.core.array<.sht.core.string>.getLength: 
                                             push, rbp
                                             mov, rbp, rsp
                                             mov, rsp, rbp
                                             pop, rbp
                                             ret

.seg code                                 
.sht.core.array<.sht.core.string>.append: 
                                          push, rbp
                                          mov, rbp, rsp
                                          mov, rsp, rbp
                                          pop, rbp
                                          ret

.seg code                                     
.sht.core.array<.sht.core.string>.indexOpGet: 
                                              push, rbp          
                                              push, rbx          
                                              mov, rbp, rsp      
                                              sub, rsp, 40       
                                              mov, rbx, rcx      ; (preserve) [combiner]
                                              mov, rcx, [rbx+16] ; shape:hoist addrOf from call
                                              call, ._arrgidx    ; (call label)
                                              add, rsp, 40       
                                              mov, rsp, rbp      
                                              pop, rbx           
                                              pop, rbp           
                                              ret                

.seg code                                     
.sht.core.array<.sht.core.string>.indexOpSet: 
                                              push, rbp          
                                              push, rbx          
                                              mov, rbp, rsp      
                                              sub, rsp, 40       
                                              mov, rbx, rcx      ; (preserve) [combiner]
                                              mov, rcx, [rbx+16] ; shape:hoist addrOf from call
                                              call, ._arrsidx    ; (call label)
                                              add, rsp, 40       
                                              mov, rsp, rbp      
                                              pop, rbx           
                                              pop, rbp           
                                              ret                

.seg code                                
.sht.core.array<.sht.core.string>.cctor: 
                                         push, rbp        
                                         mov, rbp, rsp    
                                         mov, [rcx+8], 0  ; =
                                         mov, [rcx+16], 0 ; =
                                         mov, rsp, rbp    
                                         pop, rbp         
                                         ret              

.seg code                                
.sht.core.array<.sht.core.string>.cdtor: 
                                         push, rbp
                                         mov, rbp, rsp
                                         mov, rsp, rbp
                                         pop, rbp
                                         ret

.seg code
.sht.core.array<.sht.core.string>_vtbl_inst:

.seg code                                
.sht.core.array<.sht.core.string>_sctor: 
                                         push, rbp                                                      
                                         push, rbx                                                      
                                         mov, rbp, rsp                                                  
                                         sub, rsp, 40                                                   
                                         call, .sht.core.object_sctor                                   ; (call label)
                                         add, rsp, 40                                                   
                                         lea, rbx, qwordptr .sht.core.array<.sht.core.string>_vtbl_inst 
                                         mov, [rcx], rbx                                                ; =
                                         sub, rsp, 40                                                   
                                         call, .sht.core.array<.sht.core.string>.cctor                  ; (call label)
                                         add, rsp, 40                                                   
                                         mov, rsp, rbp                                                  
                                         pop, rbx                                                       
                                         pop, rbp                                                       
                                         ret                                                            

.seg code                                
.sht.core.array<.sht.core.string>_sdtor: 
                                         push, rbp                                                      
                                         push, rbx                                                      
                                         mov, rbp, rsp                                                  
                                         lea, rbx, qwordptr .sht.core.array<.sht.core.string>_vtbl_inst 
                                         mov, [rcx], rbx                                                ; =
                                         sub, rsp, 40                                                   
                                         call, .sht.core.array<.sht.core.string>.cdtor                  ; (call label)
                                         add, rsp, 40                                                   
                                         sub, rsp, 40                                                   
                                         call, .sht.core.object_sdtor                                   ; (call label)
                                         add, rsp, 40                                                   
                                         mov, rsp, rbp                                                  
                                         pop, rbx                                                       
                                         pop, rbp                                                       
                                         ret                                                            

