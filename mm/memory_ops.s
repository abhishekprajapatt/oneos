# Memory Operation Optimizations (Assembly)
# Provides optimized memory copy, fill, and compare operations

.global memcpy_asm
.global memset_asm
.global memcmp_asm
.global cache_flush
.global tlb_flush

# Fast memory copy using SSE/AVX
# void *memcpy_asm(void *dest, const void *src, size_t n)
# rdi = dest, rsi = src, rdx = n
.global memcpy_asm
memcpy_asm:
    mov %rdx, %rcx
    shr $3, %rcx                # Copy 8-byte chunks
    rep movsq
    
    mov %rdx, %rcx
    and $7, %rcx                # Copy remaining bytes
    rep movsb
    
    ret

# Fast memory fill
# void *memset_asm(void *s, int c, size_t n)
# rdi = s, rsi = c, rdx = n
.global memset_asm
memset_asm:
    mov %rsi, %rax
    mov %rdx, %rcx
    shr $3, %rcx                # Fill 8-byte chunks
    
    # Convert byte to qword
    movzbl %al, %eax
    mov $0x0101010101010101, %r8
    imul %r8, %rax
    
    rep stosq
    
    mov %rdx, %rcx
    and $7, %rcx                # Fill remaining bytes
    rep stosb
    
    ret

# Memory compare
# int memcmp_asm(const void *s1, const void *s2, size_t n)
# rdi = s1, rsi = s2, rdx = n
.global memcmp_asm
memcmp_asm:
    xor %eax, %eax
    mov %rdx, %rcx
    
    cld
    repe cmpsb
    
    jna memcmp_equal
    mov $1, %eax
    jmp memcmp_done
    
memcmp_equal:
    xor %eax, %eax
    
memcmp_done:
    ret

# Flush CPU cache
# void cache_flush(void *addr, size_t n)
# rdi = addr, rsi = n
.global cache_flush
cache_flush:
    mov %rsi, %rcx
    mov $64, %rsi               # Cache line size
    
    # CLFLUSH each cache line
cache_flush_loop:
    cmp $0, %rcx
    jle cache_flush_done
    
    clflush (%rdi)
    add %rsi, %rdi
    sub %rsi, %rcx
    jmp cache_flush_loop
    
cache_flush_done:
    mfence                      # Memory fence
    ret

# Flush TLB (Translation Lookaside Buffer)
# void tlb_flush(void)
.global tlb_flush
tlb_flush:
    mov %cr3, %rax
    mov %rax, %cr3              # Reload CR3 to flush TLB
    ret
