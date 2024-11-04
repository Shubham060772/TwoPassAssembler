start: ldc 5          ; Load constant 5 into register A
       adc 3          ; Add constant 3 to register A
       stl 0          ; Store the result in memory at location 0
       ldc 10         ; Load constant 10 into register A
       add            ; Add register B to A
       brz end        ; Branch to "end" if result in A is zero
       adj -1         ; Adjust stack pointer by -1
       HALT           ; Stop execution

end:   ldc 0          ; Load constant 0 (no-op here for demonstration)
       HALT           ; Stop execution
