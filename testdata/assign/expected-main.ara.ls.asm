.seg code                 
.assign.assignTester.run: 
                          push, rbp
                          mov, rbp, rsp
                          mov, rsp, rbp
                          pop, rbp
                          ret

.seg code                               
.assign.assignTester.readFromSubObject: 
                                        push, rbp         
                                        mov, rbp, rsp     
                                        mov, r8, [rdx+16] ; =
                                        mov, rsp, rbp     
                                        pop, rbp          
                                        ret               

.seg code                                
.assign.assignTester.writeIntoSubObject: 
                                         push, rbp             
                                         mov, rbp, rsp         
                                         sub, rsp, 24          
                                         lea, rcx, [rbp-24]    ; a
                                         sub, rsp, 32          
                                         call, .assign.A_sctor ; (call label)
                                         add, rsp, 32          
                                         mov, [rcx+16], 7      ; =
                                         sub, rsp, 32          
                                         call, .assign.A_sdtor ; (call label)
                                         add, rsp, 32          
                                         mov, rsp, rbp         
                                         pop, rbp              
                                         ret                   

.seg code                                  
.assign.assignTester.readFromSubSubObject: 
                                           push, rbp          
                                           push, rbx          
                                           mov, rbp, rsp      
                                           mov, rbx, [rdx+16] ; fieldaccess: owner of _x
                                           mov, r8, [rbx+16]  ; =
                                           mov, rsp, rbp      
                                           pop, rbx           
                                           pop, rbp           
                                           ret                

.seg code                                   
.assign.assignTester.writeIntoSubSubObject: 
                                            push, rbp             
                                            push, rbx             
                                            mov, rbp, rsp         
                                            sub, rsp, 24          
                                            lea, rcx, [rbp-24]    ; wrong
                                            sub, rsp, 32          
                                            call, .assign.A_sctor ; (call label)
                                            add, rsp, 32          
                                            mov, rbx, [rcx+16]    ; fieldaccess: owner of _x
                                            mov, [rbx+16], 7      ; =
                                            sub, rsp, 32          
                                            call, .assign.A_sdtor ; (call label)
                                            add, rsp, 32          
                                            mov, rsp, rbp         
                                            pop, rbx              
                                            pop, rbp              
                                            ret                   

.seg code                              
.assign.assignTester.bopAssociativity: 
                                       push, rbp         
                                       push, rbx         
                                       push, rdi         
                                       mov, rbp, rsp     
                                       sub, rsp, 8       
                                       mov, rbx, 1       
                                       add, rbx, 2       
                                       mov, rdi, rbx     
                                       add, rdi, 3       
                                       mov, [rbp-8], rdi ; =
                                       mov, rsp, rbp     
                                       pop, rdi          
                                       pop, rbx          
                                       pop, rbp          
                                       ret               

.seg code                   
.assign.assignTester.cctor: 
                            push, rbp
                            mov, rbp, rsp
                            mov, rsp, rbp
                            pop, rbp
                            ret

.seg code                   
.assign.assignTester.cdtor: 
                            push, rbp
                            mov, rbp, rsp
                            mov, rsp, rbp
                            pop, rbp
                            ret

.seg code        
.assign.A.cctor: 
                 push, rbp       
                 mov, rbp, rsp   
                 mov, [rcx+8], 0 ; =
                 mov, rsp, rbp   
                 pop, rbp        
                 ret             

.seg code        
.assign.A.cdtor: 
                 push, rbp
                 mov, rbp, rsp
                 mov, rsp, rbp
                 pop, rbp
                 ret

.seg code        
.assign.B.cctor: 
                 push, rbp        
                 mov, rbp, rsp    
                 mov, [rcx+8], 0  ; =
                 mov, [rcx+16], 0 ; =
                 mov, rsp, rbp    
                 pop, rbp         
                 ret              

.seg code        
.assign.B.cdtor: 
                 push, rbp
                 mov, rbp, rsp
                 mov, rsp, rbp
                 pop, rbp
                 ret

.seg code
.assign.A_vtbl_inst:

.seg code
.assign.B_vtbl_inst:

.seg code                       
.assign.assignTester_vtbl_inst: 
                                goto, .assign.assignTester.run

.seg code        
.assign.A_sctor: 
                 push, rbp                              
                 push, rbx                              
                 mov, rbp, rsp                          
                 sub, rsp, 32                           
                 call, .sht.core.object_sctor           ; (call label)
                 add, rsp, 32                           
                 lea, rbx, qwordptr .assign.A_vtbl_inst 
                 mov, [rcx], rbx                        ; =
                 sub, rsp, 32                           
                 call, .assign.A.cctor                  ; (call label)
                 add, rsp, 32                           
                 mov, rsp, rbp                          
                 pop, rbx                               
                 pop, rbp                               
                 ret                                    

.seg code        
.assign.A_sdtor: 
                 push, rbp                              
                 push, rbx                              
                 mov, rbp, rsp                          
                 lea, rbx, qwordptr .assign.A_vtbl_inst 
                 mov, [rcx], rbx                        ; =
                 sub, rsp, 32                           
                 call, .assign.A.cdtor                  ; (call label)
                 add, rsp, 32                           
                 sub, rsp, 32                           
                 call, .sht.core.object_sdtor           ; (call label)
                 add, rsp, 32                           
                 mov, rsp, rbp                          
                 pop, rbx                               
                 pop, rbp                               
                 ret                                    

.seg code        
.assign.B_sctor: 
                 push, rbp                              
                 push, rbx                              
                 mov, rbp, rsp                          
                 sub, rsp, 32                           
                 call, .sht.core.object_sctor           ; (call label)
                 add, rsp, 32                           
                 lea, rbx, qwordptr .assign.B_vtbl_inst 
                 mov, [rcx], rbx                        ; =
                 sub, rsp, 32                           
                 call, .assign.B.cctor                  ; (call label)
                 add, rsp, 32                           
                 mov, rsp, rbp                          
                 pop, rbx                               
                 pop, rbp                               
                 ret                                    

.seg code        
.assign.B_sdtor: 
                 push, rbp                              
                 push, rbx                              
                 mov, rbp, rsp                          
                 lea, rbx, qwordptr .assign.B_vtbl_inst 
                 mov, [rcx], rbx                        ; =
                 sub, rsp, 32                           
                 call, .assign.B.cdtor                  ; (call label)
                 add, rsp, 32                           
                 sub, rsp, 32                           
                 call, .sht.core.object_sdtor           ; (call label)
                 add, rsp, 32                           
                 mov, rsp, rbp                          
                 pop, rbx                               
                 pop, rbp                               
                 ret                                    

.seg code                   
.assign.assignTester_sctor: 
                            push, rbp                                         
                            push, rbx                                         
                            mov, rbp, rsp                                     
                            sub, rsp, 32                                      
                            call, .sht.cons.program_sctor                     ; (call label)
                            add, rsp, 32                                      
                            lea, rbx, qwordptr .assign.assignTester_vtbl_inst 
                            mov, [rcx], rbx                                   ; =
                            sub, rsp, 32                                      
                            call, .assign.assignTester.cctor                  ; (call label)
                            add, rsp, 32                                      
                            mov, rsp, rbp                                     
                            pop, rbx                                          
                            pop, rbp                                          
                            ret                                               

.seg code                   
.assign.assignTester_sdtor: 
                            push, rbp                                         
                            push, rbx                                         
                            mov, rbp, rsp                                     
                            lea, rbx, qwordptr .assign.assignTester_vtbl_inst 
                            mov, [rcx], rbx                                   ; =
                            sub, rsp, 32                                      
                            call, .assign.assignTester.cdtor                  ; (call label)
                            add, rsp, 32                                      
                            sub, rsp, 32                                      
                            call, .sht.cons.program_sdtor                     ; (call label)
                            add, rsp, 32                                      
                            mov, rsp, rbp                                     
                            pop, rbx                                          
                            pop, rbp                                          
                            ret                                               

