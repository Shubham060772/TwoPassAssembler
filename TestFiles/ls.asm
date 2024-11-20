; DATA SECTION
array:      data 35    ; Array elements
            data 72
            data 1
            data 13
            data 98
            data 12

length:     data 6     ; Length of the array
target:     data 13    ; Target value to search for
counter:    data 0     ; Counter for traversing the array
position:   data -1    ; Position of the target (-1 if not found)

; CODE SECTION

main:
; Initialize counter to 0
ldc counter
a2sp
ldc 0
stl 0
br searchloop

; SEARCH LOOP
searchloop:
; Check termination condition: counter == length
ldc length
a2sp
ldl 0
ldc counter
a2sp
ldl 0
sub
brz notfound  ; If counter == length, target not found
br checkvalue

checkvalue:
; Load array[counter] and compare with target
ldc counter
a2sp
ldl 0
ldc array
add
a2sp
ldl 0
ldc target
sub
brz found       ; If array[counter] == target, go to found
br iterate      ; Otherwise, move to the next index

found:
; Store the current counter value in position
ldc counter
a2sp
ldl 0
ldc position
a2sp
stl 0
br end         ; Exit program

iterate:
; Increment counter and repeat the loop
ldc counter
a2sp
ldl 0
adc 1
stl 0
br searchloop

notfound:
; Store -1 in position (already initialized to -1)
br end

end:
HALT
