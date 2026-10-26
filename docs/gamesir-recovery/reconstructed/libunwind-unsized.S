/* Original GameSir libunwind assembly reconstruction, analysis only.
 * Not linked into Mesa; bundled compiler runtime remains authoritative.
 * Exact instruction words are checked against SHA-bound original ELF bytes.
 * Ranges are entry-to-next-entry spans with terminal RET, not ELF st_size.
 * GCS/BTI instruction words remain exact, including the conditional sequence;
 * no target execution, assembler build or runtime/GPU validation performed.
 */
.text
.p2align 2

.type gs_restored_unwind_jumpto, %function
gs_restored_unwind_jumpto:
   .inst 0xd50324df /* bti jc */                 // ELF0xce5094,word0xd50324df
   ldp x2, x3, [x0, #0x10]                       // ELF0xce5098,word0xa9410c02
   ldp x4, x5, [x0, #0x20]                       // ELF0xce509c,word0xa9421404
   ldp x6, x7, [x0, #0x30]                       // ELF0xce50a0,word0xa9431c06
   ldp x8, x9, [x0, #0x40]                       // ELF0xce50a4,word0xa9442408
   ldp x10, x11, [x0, #0x50]                     // ELF0xce50a8,word0xa9452c0a
   ldp x12, x13, [x0, #0x60]                     // ELF0xce50ac,word0xa946340c
   ldp x14, x15, [x0, #0x70]                     // ELF0xce50b0,word0xa9473c0e
   ldp x18, x19, [x0, #0x90]                     // ELF0xce50b4,word0xa9494c12
   ldp x20, x21, [x0, #0xa0]                     // ELF0xce50b8,word0xa94a5414
   ldp x22, x23, [x0, #0xb0]                     // ELF0xce50bc,word0xa94b5c16
   ldp x24, x25, [x0, #0xc0]                     // ELF0xce50c0,word0xa94c6418
   ldp x26, x27, [x0, #0xd0]                     // ELF0xce50c4,word0xa94d6c1a
   ldp x28, x29, [x0, #0xe0]                     // ELF0xce50c8,word0xa94e741c
   ldr x30, [x0, #0x100]                         // ELF0xce50cc,word0xf940801e
   ldp d0, d1, [x0, #0x110]                      // ELF0xce50d0,word0x6d510400
   ldp d2, d3, [x0, #0x120]                      // ELF0xce50d4,word0x6d520c02
   ldp d4, d5, [x0, #0x130]                      // ELF0xce50d8,word0x6d531404
   ldp d6, d7, [x0, #0x140]                      // ELF0xce50dc,word0x6d541c06
   ldp d8, d9, [x0, #0x150]                      // ELF0xce50e0,word0x6d552408
   ldp d10, d11, [x0, #0x160]                    // ELF0xce50e4,word0x6d562c0a
   ldp d12, d13, [x0, #0x170]                    // ELF0xce50e8,word0x6d57340c
   ldp d14, d15, [x0, #0x180]                    // ELF0xce50ec,word0x6d583c0e
   ldp d16, d17, [x0, #0x190]                    // ELF0xce50f0,word0x6d594410
   ldp d18, d19, [x0, #0x1a0]                    // ELF0xce50f4,word0x6d5a4c12
   ldp d20, d21, [x0, #0x1b0]                    // ELF0xce50f8,word0x6d5b5414
   ldp d22, d23, [x0, #0x1c0]                    // ELF0xce50fc,word0x6d5c5c16
   ldp d24, d25, [x0, #0x1d0]                    // ELF0xce5100,word0x6d5d6418
   ldp d26, d27, [x0, #0x1e0]                    // ELF0xce5104,word0x6d5e6c1a
   ldp d28, d29, [x0, #0x1f0]                    // ELF0xce5108,word0x6d5f741c
   ldr d30, [x0, #0x200]                         // ELF0xce510c,word0xfd41001e
   ldr d31, [x0, #0x208]                         // ELF0xce5110,word0xfd41041f
   ldr x16, [x0, #0xf8]                          // ELF0xce5114,word0xf9407c10
   ldp x0, x1, [x0]                              // ELF0xce5118,word0xa9400400
   mov sp, x16                                   // ELF0xce511c,word0x9100021f
   mov x16, #0x1                                 // ELF0xce5120,word0xd2800030
   .inst 0xd503251f /* chkfeat x16 */            // ELF0xce5124,word0xd503251f
   cbnz x16, .Lgs_unwind_skip_gcs                // ELF0xce5128,word0xb5000050
   .inst 0xd50b771e /* gcspushm x30 */           // ELF0xce512c,word0xd50b771e
.Lgs_unwind_skip_gcs:
   ret                                           // ELF0xce5130,word0xd65f03c0
.size gs_restored_unwind_jumpto, .-gs_restored_unwind_jumpto

.type gs_restored_unw_getcontext, %function
gs_restored_unw_getcontext:
   .inst 0xd50324df /* bti jc */                 // ELF0xce5134,word0xd50324df
   stp x0, x1, [x0]                              // ELF0xce5138,word0xa9000400
   stp x2, x3, [x0, #0x10]                       // ELF0xce513c,word0xa9010c02
   stp x4, x5, [x0, #0x20]                       // ELF0xce5140,word0xa9021404
   stp x6, x7, [x0, #0x30]                       // ELF0xce5144,word0xa9031c06
   stp x8, x9, [x0, #0x40]                       // ELF0xce5148,word0xa9042408
   stp x10, x11, [x0, #0x50]                     // ELF0xce514c,word0xa9052c0a
   stp x12, x13, [x0, #0x60]                     // ELF0xce5150,word0xa906340c
   stp x14, x15, [x0, #0x70]                     // ELF0xce5154,word0xa9073c0e
   stp x16, x17, [x0, #0x80]                     // ELF0xce5158,word0xa9084410
   stp x18, x19, [x0, #0x90]                     // ELF0xce515c,word0xa9094c12
   stp x20, x21, [x0, #0xa0]                     // ELF0xce5160,word0xa90a5414
   stp x22, x23, [x0, #0xb0]                     // ELF0xce5164,word0xa90b5c16
   stp x24, x25, [x0, #0xc0]                     // ELF0xce5168,word0xa90c6418
   stp x26, x27, [x0, #0xd0]                     // ELF0xce516c,word0xa90d6c1a
   stp x28, x29, [x0, #0xe0]                     // ELF0xce5170,word0xa90e741c
   str x30, [x0, #0xf0]                          // ELF0xce5174,word0xf900781e
   mov x1, sp                                    // ELF0xce5178,word0x910003e1
   str x1, [x0, #0xf8]                           // ELF0xce517c,word0xf9007c01
   str x30, [x0, #0x100]                         // ELF0xce5180,word0xf900801e
   stp d0, d1, [x0, #0x110]                      // ELF0xce5184,word0x6d110400
   stp d2, d3, [x0, #0x120]                      // ELF0xce5188,word0x6d120c02
   stp d4, d5, [x0, #0x130]                      // ELF0xce518c,word0x6d131404
   stp d6, d7, [x0, #0x140]                      // ELF0xce5190,word0x6d141c06
   stp d8, d9, [x0, #0x150]                      // ELF0xce5194,word0x6d152408
   stp d10, d11, [x0, #0x160]                    // ELF0xce5198,word0x6d162c0a
   stp d12, d13, [x0, #0x170]                    // ELF0xce519c,word0x6d17340c
   stp d14, d15, [x0, #0x180]                    // ELF0xce51a0,word0x6d183c0e
   stp d16, d17, [x0, #0x190]                    // ELF0xce51a4,word0x6d194410
   stp d18, d19, [x0, #0x1a0]                    // ELF0xce51a8,word0x6d1a4c12
   stp d20, d21, [x0, #0x1b0]                    // ELF0xce51ac,word0x6d1b5414
   stp d22, d23, [x0, #0x1c0]                    // ELF0xce51b0,word0x6d1c5c16
   stp d24, d25, [x0, #0x1d0]                    // ELF0xce51b4,word0x6d1d6418
   stp d26, d27, [x0, #0x1e0]                    // ELF0xce51b8,word0x6d1e6c1a
   stp d28, d29, [x0, #0x1f0]                    // ELF0xce51bc,word0x6d1f741c
   str d30, [x0, #0x200]                         // ELF0xce51c0,word0xfd01001e
   str d31, [x0, #0x208]                         // ELF0xce51c4,word0xfd01041f
   mov x0, #0x0                                  // ELF0xce51c8,word0xd2800000
   ret                                           // ELF0xce51cc,word0xd65f03c0
.size gs_restored_unw_getcontext, .-gs_restored_unw_getcontext
