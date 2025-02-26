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
.subFunc2:     
               push, rbp           
               mov, rbp, rsp       
               mov, rax, rcx       
               add, rax, rdx       
               goto, .subFunc2.end ; early return
.seg code      
.subFunc2.end: 
               mov, rsp, rbp       
               pop, rbp            
               ret                 

.seg code      
.subFunc1:     
               push, rbp           
               mov, rbp, rsp       
               sub, rsp, 32        ; 32 = (passing size)32 + (align pad)0
               mov, rdx, rcx       ;       (x req for rdx) [splitter]
               mov, rcx, 7         ;       (7 req for rcx) [splitter]
               call, .subFunc2     ; (call label)
               add, rsp, 32        ; 32 = (passing size)32 + (align pad)0
               goto, .subFunc1.end ; early return
.seg code      
.subFunc1.end: 
               mov, rsp, rbp       
               pop, rbp            
               ret                 

.seg code    
.entrypoint: 
             push, rbp       
             mov, rbp, rsp   
             sub, rsp, 32    ; 32 = (passing size)32 + (align pad)0
             mov, rcx, 12    ;       (12 req for rcx) [splitter]
             call, .subFunc1 ; (call label)
             add, rsp, 32    ; 32 = (passing size)32 + (align pad)0
             mov, rsp, rbp   
             pop, rbp        
             ret             

