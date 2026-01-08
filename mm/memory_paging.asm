; Memory Management - Paging Setup
; Part 2: Memory Management
; Enable virtual memory and paging

global enable_paging
global disable_paging
global flush_tlb
global set_page_table

; Enable paging
enable_paging:
    ; rdi = page table physical address
    
    ; Set CR3 to page table address
    mov cr3, rdi
    
    ; Set CR0 paging bit (bit 31)
    mov rax, cr0
    or rax, 0x80000000
    mov cr0, rax
    
    ret

; Disable paging
disable_paging:
    mov rax, cr0
    and rax, 0x7FFFFFFF
    mov cr0, rax
    ret

; Flush TLB (Translation Lookaside Buffer)
flush_tlb:
    ; rdi = virtual address to flush (0 = all)
    
    cmp rdi, 0
    jne flush_single
    
    ; Flush all TLB entries
    mov rax, cr3
    mov cr3, rax
    ret
    
flush_single:
    ; Flush single TLB entry
    invlpg [rdi]
    ret

; Set page table base
set_page_table:
    ; rdi = page table address
    mov cr3, rdi
    ret
