.seg code             
.sht.core.string.set: 
                      push, rbp         
                      push, rbx         
                      mov, rbp, rsp     
                      sub, rsp, 40      ; 40 = (passing size)32 + (align pad)8
                      mov, rbx, rcx     ; (preserve) [combiner]
                      mov, rcx, rdx     ;       (literal req for rcx) [splitter]
                      call, ._strld     ; (call label)
                      add, rsp, 40      ; 40 = (passing size)32 + (align pad)8
                      mov, [rbx+8], rax ; =
                      mov, rsp, rbp     
                      pop, rbx          
                      pop, rbp          
                      ret               

.seg code                
.sht.core.string.length: 
                         push, rbp         
                         push, rbx         
                         mov, rbp, rsp     
                         sub, rsp, 40      ; 40 = (passing size)32 + (align pad)8
                         mov, rbx, rcx     ; (preserve) [combiner]
                         mov, rcx, [rbx+8] ; shape:hoist addrOf from call
                         call, ._strlen    ; (call label)
                         add, rsp, 40      ; 40 = (passing size)32 + (align pad)8
                         mov, rsp, rbp     
                         pop, rbx          
                         pop, rbp          
                         ret               

.seg code                    
.sht.core.string.indexOpGet: 
                             push, rbp         
                             push, rbx         
                             mov, rbp, rsp     
                             sub, rsp, 40      ; 40 = (passing size)32 + (align pad)8
                             mov, rbx, rcx     ; (preserve) [combiner]
                             mov, rcx, [rbx+8] ; shape:hoist addrOf from call
                             call, ._strgidx   ; (call label)
                             add, rsp, 40      ; 40 = (passing size)32 + (align pad)8
                             mov, rsp, rbp     
                             pop, rbx          
                             pop, rbp          
                             ret               

.seg code                    
.sht.core.string.indexOpSet: 
                             push, rbp         
                             push, rbx         
                             mov, rbp, rsp     
                             sub, rsp, 40      ; 40 = (passing size)32 + (align pad)8
                             mov, rbx, rcx     ; (preserve) [combiner]
                             mov, rcx, [rbx+8] ; shape:hoist addrOf from call
                             call, ._strsidx   ; (call label)
                             add, rsp, 40      ; 40 = (passing size)32 + (align pad)8
                             mov, rsp, rbp     
                             pop, rbx          
                             pop, rbp          
                             ret               

.seg code               
.sht.core.string.cctor: 
                        push, rbp       
                        mov, rbp, rsp   
                        mov, [rcx+8], 0 ; =
                        mov, rsp, rbp   
                        pop, rbp        
                        ret             

.seg code               
.sht.core.string.cdtor: 
                        push, rbp
                        mov, rbp, rsp
                        mov, rsp, rbp
                        pop, rbp
                        ret

.seg code
.sht.core.string_vtbl_inst:

.seg code               
.sht.core.string_sctor: 
                        push, rbp                                     
                        push, rbx                                     
                        mov, rbp, rsp                                 
                        sub, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        call, .sht.core.object_sctor                  ; (call label)
                        add, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        lea, rbx, qwordptr .sht.core.string_vtbl_inst 
                        mov, [rcx], rbx                               ; =
                        sub, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        call, .sht.core.string.cctor                  ; (call label)
                        add, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        mov, rsp, rbp                                 
                        pop, rbx                                      
                        pop, rbp                                      
                        ret                                           

.seg code               
.sht.core.string_sdtor: 
                        push, rbp                                     
                        push, rbx                                     
                        mov, rbp, rsp                                 
                        lea, rbx, qwordptr .sht.core.string_vtbl_inst 
                        mov, [rcx], rbx                               ; =
                        sub, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        call, .sht.core.string.cdtor                  ; (call label)
                        add, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        sub, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        call, .sht.core.object_sdtor                  ; (call label)
                        add, rsp, 40                                  ; 40 = (passing size)32 + (align pad)8
                        mov, rsp, rbp                                 
                        pop, rbx                                      
                        pop, rbp                                      
                        ret                                           

