.seg const
.text0:
.data, "text 0" <b> 0 

.seg const
.text1:
.data, "text 1" <b> 0 

.seg const
.text2:
.data, "text 2" <b> 0 

.seg code   
.virtFunc0: 
            push, rbp                 
            mov, rbp, rsp             
            sub, rsp, 32              
            lea, rcx, qwordptr .text2 
            call, ._print             ; (call label)
            add, rsp, 32              
            mov, rsp, rbp             
            pop, rbp                  
            ret                       

.seg code   
.virtFunc1: 
            push, rbp                 
            mov, rbp, rsp             
            sub, rsp, 32              
            lea, rcx, qwordptr .text2 
            call, ._print             ; (call label)
            add, rsp, 32              
            mov, rsp, rbp             
            pop, rbp                  
            ret                       

.seg code 
.vinst:   
          goto, .virtFunc0
          goto, .virtFunc1

.seg code 
.ctor:    
          push, rbp                 
          push, rbx                 
          mov, rbp, rsp             
          lea, rbx, qwordptr .vinst 
          mov, [rcx], rbx           ; =
          mov, rsp, rbp             
          pop, rbx                  
          pop, rbp                  
          ret                       

.seg code    
.entrypoint: 
             push, rbp                 
             push, rbx                 
             mov, rbp, rsp             
             sub, rsp, 8               
             lea, rcx, [rbp-8]         ; o
             sub, rsp, 32              
             call, .ctor               ; (call label)
             add, rsp, 32              
             sub, rsp, 32              
             mov, rbx, rcx             ; (preserve) [combiner]
             lea, rcx, qwordptr .text0 
             call, ._print             ; (call label)
             add, rsp, 32              
             sub, rsp, 32              
             call, [rbx]               ; (vtbl call)
             add, rsp, 32              
             mov, rsp, rbp             
             pop, rbx                  
             pop, rbp                  
             ret                       

