;NAME- Shubham Agrawal
;Roll Number- 2301AI44
;Declaration of Authorship
;Binary Search Program to find the index value of the target
; Data section
target: data 7    ; Target to search for
ans:    data 100  ; Answer location
l:      data 0    ; Left index (0)
r:      data 9    ; Right index (9)
mid:    data 100  ; Mid index (dummy initial value)
array:  data 6    ; Array elements
        data 4
        data 7
        data 10
        data 8
        data 5
        data 9
        data 2
        data 1
        data 3

; Initialize the stack pointer and local variables
ldc 1000          ; Load base address
a2sp              ; Adjust the stack pointer

; Allocate space for local variables
adj -1            ; Adjust for the local variables (array, target, etc.)
ldc 9             ; Load r (right index)
stl 0             ; Store it in local variable 0 (r)
ldc 0             ; Load l (left index)
stl 1             ; Store it in local variable 1 (l)
ldc 7             ; Load target value
stl 2             ; Store it in local variable 2 (target)
ldc array         ; Load the address of the array
stl 3             ; Store the array base address in local variable 3

; Set up the loop
loop:
    ldl 0         ; Load r (right index) from local 0
    ldl 1         ; Load l (left index) from local 1
    sub           ; Subtract: r - l
    brz done      ; If r - l == 0, jump to done
    ldl 0         ; Load r (right index)
    ldl 1         ; Load l (left index)
    add           ; Add r + l
    ldc 1         ; Load constant 1
    shr           ; Divide (r + l) / 2, storing the result in mid
    stl 4         ; Store the mid index in local 4

    ; Load the value at mid
    ldl 4         ; Load mid
    ldl 3         ; Load array base address
    add           ; Add the base address of the array to mid
    ldl 4         ; Load mid
    ldnl 0        ; Dereference array[mid]
    ldl 2         ; Load target value
    sub           ; Subtract target - array[mid]
    brz found     ; If target == array[mid], jump to found
    brlz updatel  ; If target < array[mid], update r
    br updatel    ; If target > array[mid], update l

updatel:
    ldl 4         ; Load mid
    adc 1         ; Increment mid by 1 (move l to mid + 1)
    stl 1         ; Update l with mid + 1
    br loop       ; Repeat the loop

update_r:
    ldl 4         ; Load mid
    adc -1        ; Decrement mid by 1 (move r to mid - 1)
    stl 0         ; Update r with mid - 1
    br loop       ; Repeat the loop

found:
    ldl 4         ; Load mid
    ldl 5         ; Load address of ans
    stnl 0        ; Store mid in ans
    HALT          ; Stop execution

done:
    HALT          ; Stop execution

