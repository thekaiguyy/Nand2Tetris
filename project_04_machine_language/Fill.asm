// Fill.asm
// Listens to keyboard input continuously.
//
// If any key is pressed:
//      Fill the entire screen black.
//
// If no key is pressed:
//      Clear the entire screen.
//
// Screen memory:
// SCREEN = RAM[16384]
// KBD    = RAM[24576]
//
// Each RAM word controls 16 pixels.
// Total screen words = 8192.


@i
M=0          // i = screen offset


(LOOP)

// Read keyboard input

@KBD
D=M

@PAINT
D;JNE       // If key pressed, go to PAINT


// No key pressed, clear screen

@CLEAR
0;JMP



(PAINT)

// Check if entire screen is already painted

@i
D=M

@8192
D=D-A       // D = i - 8192

@LOOP
D;JGE       // If i >= 8192, return to checking keyboard



// SCREEN[i] = -1 (make pixels black)

@i
D=M         // D = offset

@SCREEN
A=A+D       // A = SCREEN + offset

M=-1        // Write black pixels



// Increment screen offset

@i
M=M+1


@LOOP
0;JMP



(CLEAR)

// Check if screen is already cleared

@i
D=M

@LOOP
D;JEQ       // If i == 0, return to checking keyboard



// SCREEN[i] = 0 (make pixels white)

@i
D=M         // D = offset

@SCREEN
A=A+D       // A = SCREEN + offset

M=0         // Write white pixels



// Decrement offset

@i
M=M-1


@LOOP
0;JMP