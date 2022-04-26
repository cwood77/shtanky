.seg code                 
.assign.assignTester.run: 
                          ret

.seg code                               
.assign.assignTester.readFromSubObject: 
                                        mov, r8, [rdx+16] ; =
                                        ret               

.seg code                                
.assign.assignTester.writeIntoSubObject: 
                                         push, rbx        
                                         sub, rsp, 24     
                                         sub, rsp, 24     
                                         mov, [rbx+16], 7 ; =
                                         add, rsp, 24     
                                         add, rsp, 24     
                                         pop, rbx         
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
                                            push, rdi          
                                            sub, rsp, 24       
                                            sub, rsp, 24       
                                            mov, rbx, [rdi+16] ; fieldaccess: owner of x
                                            mov, [rbx+16], 7   ; =
                                            add, rsp, 24       
                                            add, rsp, 24       
                                            pop, rdi           
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
A_vtbl_inst:
.data, 

.seg const
B_vtbl_inst:
.data, 

.seg const
assignTester_vtbl_inst:
.data, .assign.assignTester.run 

