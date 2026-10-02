// RAM[2] = RAM[1] * RAM[0]

@i
M=0              // i = 0

@0
D=M              // D = RAM[0]

@n
M=D              // n = RAM[0]

@result
M=0              // result = 0


(LOOP)

// while (i < n)

@i
D=M              // D = i

@n
D=D-M            // D = i - n

@END
D;JEQ            // if i == n, goto END


// result = result + RAM[1]

@1
D=M              // D = RAM[1]

@result
M=D+M            // result = RAM[1] + result


// i = i + 1

@i
M=M+1

@LOOP
0;JMP


(END)

// RAM[2] = result

@result
D=M              // D = result

@2
M=D              // RAM[2] = result


@END
0;JMP