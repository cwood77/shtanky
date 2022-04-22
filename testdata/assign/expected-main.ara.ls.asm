.seg code                 
.assign.assignTester.run: 
                          ret

.seg code                               
.assign.assignTester.readFromSubObject: 
                                        mov, r8, [rdx+8] ; =
                                        ret              

.seg code                                
.assign.assignTester.writeIntoSubObject: 
                                         push, rbx       
                                         sub, rsp, 16    
                                         sub, rsp, 16    
                                         mov, [rbx+8], 7 ; =
                                         add, rsp, 16    
                                         add, rsp, 16    
                                         pop, rbx        
                                         ret             

.seg code                                  
.assign.assignTester.readFromSubSubObject: 
                                           push, rbx         
                                           push, rdi         
                                           mov, rbx, [rdi+8] ; fieldaccess: owner of x
                                           mov, r8, [rbx+8]  ; =
                                           pop, rdi          
                                           pop, rbx          
                                           ret               

.seg code                                   
.assign.assignTester.writeIntoSubSubObject: 
                                            push, rbx         
                                            push, rdi         
                                            sub, rsp, 16      
                                            sub, rsp, 16      
                                            mov, rbx, [rdi+8] ; fieldaccess: owner of x
                                            mov, [rbx+8], 7   ; =
                                            add, rsp, 16      
                                            add, rsp, 16      
                                            pop, rdi          
                                            pop, rbx          
                                            ret               

