# NanoProc

NanoProc is a Fantasy Computer. It consists of a 32-bit fantasy CPU with a
fairly flexible instruction set.

The following is example assembly code to loop through the numbers 0 through 100
then halt the computer:

```{.asm}
            MOVE  $ACC,   0  ; Initialize register ACC to 0
loop_begin: CMPGE $ACC, 100  ; Compare if register ACC is greater than or equal
                             ; to 100
            JUMPC loop_end   ; If the previous condition was true jump to the end
                             ; of the loop
            INC   $ACC       ; Increment register ACC by 1
            JUMP  loop_begin ; Jump to beginning of loop
loop_end:   HALT             ; Halt CPU execution
```

## Subprojects

- **[NP-EMU](np-emu/README.md)** - The Emulator for the NanoProc system
  - **[NP-TERM](np-term/README.md)** - A terminal virtual hardware device for
    the NanoProc system.
- **[NP-ASM](np-asm/README.md)** - The Assembler for generating binaries for
  the NanoProc system
