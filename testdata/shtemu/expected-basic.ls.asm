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
            lea, rcx, qwordptr .text0 
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
            lea, rcx, qwordptr .text1 
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
             push, rdi                 
             mov, rbp, rsp             
             sub, rsp, 24              
             lea, rcx, [rbp-24]        ; o
             sub, rsp, 32              
             call, .ctor               ; (call label)
             add, rsp, 32              
             sub, rsp, 32              
             mov, rbx, rcx             ; (preserve) [combiner]
             lea, rcx, qwordptr .text2 
             call, ._print             ; (call label)
             add, rsp, 32              
             sub, rsp, 32              
             call, [rbx]               ; (vtbl call)
             add, rsp, 32              
             sub, rsp, 32              
             mov, r10, [rbx]           ; codeshape decomp
             mov, [rbp-8], r10         
             mov, r10, 5               ; codeshape decomp
             add, [rbp-8], r10         
             lea, rdi, [rbp-8]         
             call, [rdi]               ; (vtbl call)
             add, rsp, 32              
             sub, rsp, 32              
             call, [rbx]               ; (vtbl call)
             add, rsp, 32              
             sub, rsp, 32              
             mov, r10, [rbx]           ; codeshape decomp
             mov, [rbp-16], r10        
             mov, r10, 5               ; codeshape decomp
             add, [rbp-16], r10        
             lea, rbx, [rbp-16]        
             call, [rbx]               ; (vtbl call)
             add, rsp, 32              
             mov, rsp, rbp             
             pop, rdi                  
             pop, rbx                  
             pop, rbp                  
             ret                       

