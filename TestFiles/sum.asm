;NAME-         Shubham Agrawal
;Roll Number-   2301AI44
;Declaration of Authorship
;Program to find the sum of values in an array
; Data section
array:  data 6        ; Array elements
        data 4
        data 7
        data 10
        data 8
        data 5
        data 9
        data 2
        data 1
        data 3


        ldc 0         ; Load constant 0 (initialize sum)
        stl 0         ; Store in local variable 0 (sum)

        ldc 9         ; Load the initial count (number of elements in the array)
        stl 1         ; Store in local variable 1 (count)

        ldc 0         ; Initialize array pointer (start at array[0])
        stl 2         ; Store in local variable 2 (array index)

startloop:
        ldl 1         ; Load count
        brz endloop  ; If count == 0, exit the loop

        ldl 2         ; Load current array index
        ldc array     ; Load base address of the array
        add           ; Calculate address of array[array index]
        ldl 3         ; Load the value at the calculated address
        stl 3         ; Store the fetched array element

        ldl 0         ; Load current sum
        add           ; Add the current array element to the sum
        stl 0         ; Store updated sum

        ldl 2         ; Load array index
        adc 1         ; Increment array index
        stl 2         ; Store updated index

        ldl 1         ; Load count
        adc -1        ; Decrement count
        stl 1         ; Store updated count

        br startloop ; Repeat the loop

endloop:
        HALT          ; End the program

