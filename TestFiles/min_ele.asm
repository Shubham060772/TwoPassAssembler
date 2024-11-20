;DATA
array:
data 0
data 1
data 2
data 3
data 4
data 5

length: data 6
counter: data 0
minvalue: data 0

;CODE
main:
; Initialize minvalue to the first element of the array
ldc array           ; Load the address of 'array'
a2sp
ldl 0               ; Load the first element of the array
stl minvalue        ; Store it in minvalue

; Initialize counter to 1 (since the first element is already considered)
ldc 1
a2sp
stl counter         ; Store 1 in counter

; Loop to find the minimum element
minloop:
; Load length and counter, and compare them
ldc length          ; Load the address of 'length'
a2sp
ldl 0               ; Load the value of 'length'

ldc counter         ; Load the address of 'counter'
a2sp
ldl 0               ; Load the value of 'counter'

sub                 ; Subtract counter from length
brz endmin          ; If counter == length, exit the loop

; Load the array element (counter-th index)
ldc counter         ; Load the address of 'counter'
a2sp
ldl 0               ; Load the value of 'counter'

ldc array           ; Load the address of 'array'
add                 ; Add counter to base address of array
a2sp                ; Move to stack pointer
ldl 0               ; Load the value at the array[index]

; Compare current element with minvalue
ldc minvalue        ; Load the address of 'minvalue'
a2sp
ldl 0               ; Load the current minvalue
sub                 ; Subtract minvalue from the array element
brz skipupdate      ; If the result is zero, they are equal, skip update

; If the result is negative, update minvalue (array element < minvalue)
ldc counter         ; Load the address of 'counter'
a2sp
ldl 0               ; Load the value of 'counter'

ldc array           ; Load the address of 'array'
add                 ; Add counter to base address of array
a2sp                ; Move to stack pointer
ldl 0               ; Load the value at the array[index]
stl minvalue        ; Store the new minimum value

skipupdate:
; Increment counter and repeat
ldc counter         ; Load the address of 'counter'
a2sp                ; Move to stack pointer
ldl 0               ; Load the value of 'counter'
adc 1               ; Increment counter by 1
stl counter         ; Store the updated counter
br minloop          ; Repeat the loop

endmin:
HALT                ; Terminate the program
