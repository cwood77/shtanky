.seg 1                    
.sht.cons.stdout.printLn: 
                          push, rbx      
                          sub, rsp, 32   
                          mov, rbx, rcx  ;       (preserve) [combiner]
                          mov, rcx, 1    ;       (:intlit:10 req for rcx) [splitter]
                          call, ._osCall 
                          add, rsp, 32   
                          pop, rbx       

