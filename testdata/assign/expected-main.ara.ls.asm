.seg code                 
.assign.assignTester.run: 
                          push, rbx         
                          sub, rsp, 24      
                          sub, rsp, 8       
                          mov, [rbp+8], 7   ; =
                          sub, rsp, 16      
                          mov, [rbp-16], 7  ; =
                          mov, rbx, [rbp-8] ; fieldaccess: x
                          mov, rbx, 7       ; =
                          add, rsp, 16      
                          add, rsp, 8       
                          add, rsp, 24      
                          pop, rbx          
                          ret               

