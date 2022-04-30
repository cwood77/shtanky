.seg code                 
.assign.assignTester.run: 
                          ret

.seg code                               
.assign.assignTester.readFromSubObject: 
                                        mov, r8, [rdx+16] ; =
                                        ret               

.seg code                                
.assign.assignTester.writeIntoSubObject: 
                                         sub, rsp, 24          
                                         sub, rsp, 24          
                                         sub, rsp, 32          
                                         call, .assign.A_sctor ; (call label)
                                         add, rsp, 32          
                                         mov, [rcx+16], 7      ; =
                                         sub, rsp, 32          
                                         call, .assign.A_sdtor ; (call label)
                                         add, rsp, 32          
                                         add, rsp, 24          
                                         add, rsp, 24          
                                         ret                   

.seg code                                  
.assign.assignTester.readFromSubSubObject: 
                                           push, rbx          
                                           push, rdi          
                                           mov, rbx, [rdi+16] ; fieldaccess: owner of x
                                           mov, r8, [rbx+16]  ; =
                                           pop, rdi           
                                           pop, rbx           
                                           ret                

.seg code                                   
.assign.assignTester.writeIntoSubSubObject: 
                                            push, rbx             
                                            sub, rsp, 24          
                                            sub, rsp, 24          
                                            sub, rsp, 32          
                                            call, .assign.A_sctor ; (call label)
                                            add, rsp, 32          
                                            mov, rbx, [rcx+16]    ; fieldaccess: owner of x
                                            mov, [rbx+16], 7      ; =
                                            sub, rsp, 32          
                                            call, .assign.A_sdtor ; (call label)
                                            add, rsp, 32          
                                            add, rsp, 24          
                                            add, rsp, 24          
                                            pop, rbx              
                                            ret                   

.seg code                   
.assign.assignTester.cctor: 
                            ret

.seg code                   
.assign.assignTester.cdtor: 
                            ret

.seg code        
.assign.A.cctor: 
                 ret

.seg code        
.assign.A.cdtor: 
                 ret

.seg code        
.assign.B.cctor: 
                 ret

.seg code        
.assign.B.cdtor: 
                 ret

.seg const
.assign.A_vtbl_inst:
.data, 

.seg const
.assign.B_vtbl_inst:
.data, 

.seg const
.assign.assignTester_vtbl_inst:
.data, .assign.assignTester.run 

.seg code        
.assign.A_sctor: 
                 sub, rsp, 32                  
                 call, .sht.core.object_sctor  ; (call label)
                 add, rsp, 32                  
                 mov, r10, .assign.A_vtbl_inst ; codeshape decomp
                 mov, [rcx], r10               ; =
                 sub, rsp, 32                  
                 call, .assign.A.cctor         ; (call label)
                 add, rsp, 32                  
                 ret                           

.seg code        
.assign.A_sdtor: 
                 mov, r10, .assign.A_vtbl_inst ; codeshape decomp
                 mov, [rcx], r10               ; =
                 sub, rsp, 32                  
                 call, .assign.A.cdtor         ; (call label)
                 add, rsp, 32                  
                 sub, rsp, 32                  
                 call, .sht.core.object_sdtor  ; (call label)
                 add, rsp, 32                  
                 ret                           

.seg code        
.assign.B_sctor: 
                 sub, rsp, 32                  
                 call, .sht.core.object_sctor  ; (call label)
                 add, rsp, 32                  
                 mov, r10, .assign.B_vtbl_inst ; codeshape decomp
                 mov, [rcx], r10               ; =
                 sub, rsp, 32                  
                 call, .assign.B.cctor         ; (call label)
                 add, rsp, 32                  
                 ret                           

.seg code        
.assign.B_sdtor: 
                 mov, r10, .assign.B_vtbl_inst ; codeshape decomp
                 mov, [rcx], r10               ; =
                 sub, rsp, 32                  
                 call, .assign.B.cdtor         ; (call label)
                 add, rsp, 32                  
                 sub, rsp, 32                  
                 call, .sht.core.object_sdtor  ; (call label)
                 add, rsp, 32                  
                 ret                           

.seg code                   
.assign.assignTester_sctor: 
                            sub, rsp, 32                             
                            call, .sht.cons.program_sctor            ; (call label)
                            add, rsp, 32                             
                            mov, rcx, .assign.assignTester_vtbl_inst ; codeshape decomp
                            mov, [rcx], rax                          ; =
                            sub, rsp, 32                             
                            call, .assign.assignTester.cctor         ; (call label)
                            add, rsp, 32                             
                            ret                                      

.seg code                   
.assign.assignTester_sdtor: 
                            mov, rax, .assign.assignTester_vtbl_inst ; codeshape decomp
                            mov, [rcx], r10                          ; =
                            sub, rsp, 32                             
                            call, .assign.assignTester.cdtor         ; (call label)
                            add, rsp, 32                             
                            sub, rsp, 32                             
                            call, .sht.cons.program_sdtor            ; (call label)
                            add, rsp, 32                             
                            ret                                      

