// SmallEdisonVM.cpp - Implementation of the Small Edison Virtual Machine.
// Nov 9, 2021 - Michel de Champlain

#include <stdlib.h>     // exit, EXIT_FAILURE
#include <stdint.h>
#include <string.h>     // string
#include <ctype.h>
#include <stdio.h>

#include "COutForAUnit.h"
#include "bsl_Uart.h"

class Task {
public:
    int16_t bp;
    int16_t sp;
    int16_t pe;
    int16_t ip;

    Task() {
        bp = sp = pe = ip = 0;
    }
};

class Kernel {
public:
    static const int8_t INVALID                 = -1;
    static const uint8_t ADD                     = 0;
    static const uint8_t ALSO                    = 1;
    static const uint8_t AND                     = 2;
    static const uint8_t ASSIGN                  = 3;
    static const uint8_t BLANK                   = 4;
    static const uint8_t COBEGIN                 = 5;
    static const uint8_t CONSTANT                = 6;
    static const uint8_t CONSTRUCT               = 7;
//  static const uint8_t DIFFERENCE              = 8;
    static const uint8_t DIVIDE                  = 9;
    static const uint8_t DO                      = 10;
    static const uint8_t ELSE                    = 11;
    static const uint8_t ENDCODE                 = 12;
    static const uint8_t ENDIF                   = 13;
    static const uint8_t ENDLIB                  = 14;
    static const uint8_t ENDPROC                 = 15;
    static const uint8_t ENDWHEN                 = 16;
    static const uint8_t EQUAL                   = 17;
    static const uint8_t ERROR                   = 18;
    static const uint8_t FIELD                   = 19;
//  static const uint8_t FUNCVAL                 = 20;
    static const uint8_t GOTO                    = 21;
    static const uint8_t GREATER                 = 22;
//  static const uint8_t IN                      = 23;
    static const uint8_t INDEX                   = 24;
    static const uint8_t INSTANCE                = 25;
//  static const uint8_t INTERSECTION            = 26;
    static const uint8_t LESS                    = 27;
    static const uint8_t LIBPROC                 = 28;
    static const uint8_t MINUS                   = 29;
    static const uint8_t MODULO                  = 30;
    static const uint8_t MULTIPLY                = 31;
    static const uint8_t NEWLINE                 = 32;
    static const uint8_t NOT                     = 33;
    static const uint8_t NOTEQUAL                = 34;
    static const uint8_t NOTGREATER              = 35;
    static const uint8_t NOTLESS                 = 36;
    static const uint8_t OR                      = 37;
    static const uint8_t PARAMARG                = 38;
    static const uint8_t PARAMCALL               = 39;
    static const uint8_t PARAMETER               = 40;
    static const uint8_t PROCARG                 = 41;
    static const uint8_t PROCCALL                = 42;
    static const uint8_t PROCEDURE               = 43;
    static const uint8_t PROCESS                 = 44;
    static const uint8_t SUBTRACT                = 45;
//  static const uint8_t UNION                   = 46;
    static const uint8_t VALSPACE                = 47;
    static const uint8_t VALUE                   = 48;
    static const uint8_t VARIABLE                = 49;
    static const uint8_t WAIT                    = 50;
    static const uint8_t WHEN                    = 51;
    static const uint8_t WHILE                   = 52;
    static const uint8_t ADDR                    = 53;
    static const uint8_t HALT                    = 54;
    static const uint8_t OBTAIN                  = 55;
    static const uint8_t PLACE                   = 56;
    static const uint8_t SENSE                   = 57;
    /** Extra */
    static const uint8_t ELEMASSIGN              = 58;
    static const uint8_t ELEMVALUE               = 59;
    static const uint8_t LOCALCASE               = 60;
    static const uint8_t LOCALSET                = 61;
    static const uint8_t LOCALVALUE              = 62;
    static const uint8_t LOCALVAR                = 63;
    static const uint8_t OUTERCALL               = 64;
    static const uint8_t OUTERCASE               = 65;
    static const uint8_t OUTERPARAM              = 66;
    static const uint8_t OUTERSET                = 67;
    static const uint8_t OUTERVALUE              = 68;
    static const uint8_t OUTERVAR                = 69;
    static const uint8_t SETCONST                = 70;
    static const uint8_t SINGLETON               = 71;
    static const uint8_t STRINGCONST             = 72;

    static const uint8_t PUTI                    = 73;
    static const uint8_t PUTC                    = 74;
    static const uint8_t PUTB                    = 75;
    static const uint8_t PUTN                    = 76;
private:
    // temporary fields for stack evaluation
    int16_t v1, v2;

    // kernel
    const uint8_t MAX_KERNEL_STACK_SIZE = 100;
    int16_t* itsKernelStack; // stack
    int16_t itsKernelSP;    //  sp

    // variable stack
    int16_t bp;              //  base pointer
    int16_t sp;              //  stack pointer

    // program
    int16_t ip;              //  instruction pointer
    int16_t pe;              //  program end

    // task
    const uint8_t MAX_QUEUE = 10;

    Task** taskQueue;    // Task taskQueue[];
    uint8_t taskCurrent;     // this
    uint8_t numberOfTasks;   // tasks

    uint8_t taskStackTop;    // stackTop
    uint8_t taskProgTop;     // progTop

    const uint8_t MIN_ADDRESS = 0;
    const uint16_t MAX_ADDRESS = 100;
    const uint8_t SPACE = (int)(' ');
    const uint16_t INSTR_TABLE = 400;
    const uint8_t SET_LENGTH = 0x8;
    const uint8_t SET_LIMIT = 127;

    int16_t* memory;
    int16_t lineNo;

public:
    Kernel() {
        v1 = v2 = 0;
        ip = 0;
        taskStackTop = 0;
        taskProgTop = 0;

        taskCurrent = 0;
        numberOfTasks = 1;
        taskQueue = new Task*[MAX_QUEUE];
        for (int16_t i = 0; i < MAX_QUEUE; i++)
            taskQueue[i] = new Task();

        itsKernelStack = new int16_t [MAX_KERNEL_STACK_SIZE];
        itsKernelSP = 0;

        memory = new int16_t[MAX_ADDRESS];
        for (int16_t i = 0; i < MAX_ADDRESS; i++) // Reset all memory locations.
            memory[i] = 0;

        pe = 1024;
        bp = 0;
        sp = 4;
        memory[sp] = 0; // no return
        lineNo = 0;
    }

    void load() {
        int16_t i = ip = pe;

        int16_t programfile[] = {
          443, 1, 1, 0, 0, 406, 84, 474, 406, 101, 474, 406, 115, 474, 406, 116, 474, 406, 32, 474, 406, 102, 474, 406, 48, 474, 476, 406, 48, 474, 476, 406, 0, 473, 476, 415, 412, -1
        };
        
        uint16_t size = sizeof(programfile)/sizeof(programfile[0]);

        for(uint16_t index = 0; index < size; index++){
           memory[i++] = programfile[index];
        }
    }

    void run() {
        int opcode = 0;
        while(true) {
            //printf("ip=%02x opcode=%d", (ip-1024), opcode);
            switch((opcode=memory[ip++]) - INSTR_TABLE) {
                case ENDPROC:           EndProc();     break;
                case PROCEDURE:         Procedure();   break;
                case INDEX:             Index();       break;
                case COBEGIN:           Cobegin();     break;
    //          case LIBPROC:           Libproc();     break;
                case PARAMCALL:         ParamCall();   break;
                case PROCCALL:          ProcCall();    break;
                case ALSO:              Also();        break;
                case ELSE:              Else();        break;
    //          case ENDLIB:            Endlib();      break;
    //          case FUNCVAL:           Funcval();     break;
    //          case PARAMARG:          Paramarg();    break;
    //          case PARAMETER:         Parameter();   break;
                case PROCARG:           ProcArg();     break;
                case PROCESS:           Process();     break;
                case VARIABLE:          Variable();    break;
                case ASSIGN:            Assign();      break;
    //          case BLANK:             Blank();       break;
                case CONSTANT:          Constant();    break;
                case CONSTRUCT:         Construct();   break;
                case DO:                Do();          break;
    //          case ENDIF:             EndIf();       break;
                case ENDWHEN:           EndWhen();     break;
                case EQUAL:             Equal();       break;
                case FIELD:             Field();       break;
                case GOTO:              Goto();        break;
                case NOTEQUAL:          NotEqual();    break;
                case VALSPACE:          ValSpace();    break;
                case VALUE:             Value();       break;
                case WAIT:              Wait();        break;
                case WHEN:              When();        break;
    //          case ERROR:             Error();       break;
                case ADD:               Add();         break;
                case AND:               And();         break;
                case DIVIDE:            Div();         break;
                case ENDCODE:           EndCode();     break;
                case GREATER:           TestForGreater();  break;
                case LESS:              TestForLessThan(); break;
                case MINUS:             Neg();         break;
                case MODULO:            Mod();         break;
                case MULTIPLY:          Mul();         break;
                case NOT:               Not();         break;
                case NOTGREATER:        TestForLessOrEqual();    break;
                case NOTLESS:           TestForGreaterOrEqual(); break;
                case OR:                Or();          break;
                case SUBTRACT:          Subtract();    break;
    //          case ADDR:              Addr();        break;
    //          case HALT:              Halt();        break;
    //          case OBTAIN:            Obtain();      break;
    //          case PLACE:             Place();       break;
    //          case SENSE:             Sense();       break;
                case INSTANCE:          Instance();    break;
                case PUTI:              PutInteger();   break;
                case PUTC:              PutCharacter(); break;
                case PUTB:              PutBoolean();   break;
                case PUTN:              PutLine();      break;
                default:
                    char string[50];
                    sprintf(string, "Unknown opcode=%d ip=%u\n", opcode, ip);
                    PutS(string);
                    exit(1);
            }
        }
    }
private:
    void runError(const char* msg) {
        PutS(msg);
        PutN();
        exit(1);
    }
    void preempt() {
        taskQueue[taskCurrent]->bp = bp;
        taskQueue[taskCurrent]->sp = sp;
        taskQueue[taskCurrent]->pe = pe;
        taskQueue[taskCurrent]->ip = ip;
    }
    void resume() {
        bp = taskQueue[taskCurrent]->bp;
        sp = taskQueue[taskCurrent]->sp;
        pe = taskQueue[taskCurrent]->pe;
        ip = taskQueue[taskCurrent]->ip ;
//t     printf("Resume: ip = %u\n", ip);
    }

    /** goto: op displacement */
    void Goto() {
//t     printf("Goto\n");
        int16_t displacement = memory[ip];
        ip = ip + displacement - 1;
    }

    /** proc: op   paramLength, varLength, tempLength, lineNo */
    void Procedure() {
        int16_t paramLength = memory[ip];
        int16_t varLength   = memory[ip+1];
        int16_t tempLength  = memory[ip+2];
        lineNo          = memory[ip+3];

//t     printf("Procedure: sp = %u\n", sp);
        memory[bp+2] = bp - paramLength -1;
        sp = sp + varLength;
        if ((sp + tempLength) > pe) {
            runError("Stack overflows: sp");
        }
        ip = ip + 4;
    }

    void EndProc() {
//tt        System.out.println("EndProc");
//tt        printStack();

        if (memory[bp+4] != 0) {
            ip = memory[bp+4];
            pe = memory[bp+3];
            sp = memory[bp+2];
            bp = memory[bp+1];
        }
//t        printStack();
    }
    /** instance: op  steps
        *
        *           Variable stack (before)
        *           ----------------------
        *   A       |   X                | <-- sp
        *           ----------------------
        *   A+1     |                    |
        *           ----------------------
        *
        *           Variable stack (after)
        *           ----------------------
        *   A       |   X                |
        *           ----------------------
        *   A+1     |   link             | <-- sp
        *           ----------------------
        */
    void Instance() {
        int16_t steps = memory[ip];
        int16_t link = bp;
        for (int i=steps; i>0; i--) {
            link = memory[link];
        }
        sp = sp + 1;
        memory[sp] = link;
        ip = ip + 1;
    }
    /** var: op Displacement
        *
        *           Variable stack (before)
        *           ---------------------
        *   A       | Link              | <-- sp
        *           ---------------------
        *   A+1     |                   |
        *           ---------------------
        *
        *           Variable stack (after)
        *           ---------------------
        *   A       | Link + disp       | <-- sp
        *           ---------------------
        *   A+1     |                   |
        *           ---------------------
        */
    void Variable() {
        int16_t displ = memory[ip];
        memory[sp] = memory[sp] + displ;
        ip = ip + 1;
    }

    /** construct: op  number, lineNo
        *
        *           Variable stack (before)
        *           ---------------------
        *   A       |   X0              | <-- sp
        *           ---------------------
        *   A+1     |   X1              |
        *           ---------------------
        *                ..........
        *           ---------------------
        *   A+n     |   Xn              |
        *           ---------------------
        *
        *           Variable stack (after)
        *           ---------------------
        *   A       |   S0              |
        *           ---------------------
        *   A+1     |   S2              |
        *           ---------------------
        *                ..........
        *           ---------------------
        *   A+m     |   Sm              | <-- sp
        *           ---------------------
        *             n = number - 1
        *             m = SET_LENGTH - 1
        */
    void Construct() {
        int16_t number = memory[ip];
        int16_t elem,index,i,val;
        int16_t* tempSet = new int16_t[SET_LENGTH];

//t     printf("Set number = %d\n", number);
        for (i = 0; i<SET_LENGTH; i++)
             tempSet[i] = 0;

        for (i = 0; i<number ; i++) {
            elem = memory[sp-i];
            if ((elem >=0) && (elem <= SET_LIMIT)) {
                index = elem & (SET_LENGTH-1);
                val = elem>> 3;

                tempSet[index] |= (1<<val);
            }
            else
                printf("Error: element has the value outside set range\n");
        }
        sp = sp - number + 1;
        for (i=0; i<SET_LENGTH ; i++) {
            memory[sp+i] = tempSet[i];
        }
        sp = sp + SET_LENGTH - 1;
        ip = ip + 2;
    }

    /** constant: op value
        *
        *           Variable stack (before)
        *           ---------------------
        *   A       | X                 | <-- sp
        *           ---------------------
        *   A+1     |                   |
        *           ---------------------
        *
        *           Variable stack (after)
        *           ---------------------
        *   A       | X                 |
        *           ---------------------
        *   A+1     | value             | <-- sp
        *           ---------------------
        */
    void Constant() {
        int16_t value = memory[ip];
        sp = sp + 1;
        memory[sp] = value;
        ip = ip + 1;
    }

    /** value: op length
        *
        *           Variable stack (before)
        *           ---------------------
        *   A       | link + displ      | <-- sp
        *           ---------------------
        *   A+1     |                   |
        *           ---------------------
        *
        *           Variable stack (after)
        *           ---------------------
        *   A       | value             |
        *           ---------------------
        *   A+1     | value1            |
        *           ---------------------
        *               ............
        *           ---------------------
        *   A+n     | valuen            | <-- sp
        *           ---------------------
        *             n = length-1
        */
    void Value() {
        int16_t length = memory[ip];
        int16_t varAdd = memory[sp];

        for (int16_t i=0; i<length; i++) {
            memory[sp+i] = memory[varAdd+i];
        }
        sp = sp + length - 1;
        ip = ip + 1;
    }

    /** valSpace: op length
        *
        *           Variable stack (before)
        *           ---------------------
        *   A       | X                 | <-- sp
        *           ---------------------
        *   A+1     |                               |
        *           ---------------------
        *
        *           Variable stack (after)
        *           ---------------------
        *   A       | X                 |
        *           ---------------------
        *   A+1     | ?                 |
        *           ---------------------
        *                ............
        *           ---------------------
        *   A+n     | ?                 | <-- sp
        *           ---------------------
        *               n = length
        */
    void ValSpace() {
        int16_t length = memory[ip];
        sp = sp + length;
        ip = ip + 1;
    }

    // Not: r = ~v    before: [v, ...    after:  [r, ...
    void Not() {
        memory[sp] = ~memory[sp];
    }

    // Mul: r = v1 * v2    before: [v1, v2, ...    after:  [r, ...
    void Mul() {
        v2 = memory[sp--];
        memory[sp] = memory[sp] * v2;

        lineNo = memory[ip++];
    }
    // Div: r = v1 / v2    before: [v1, v2, ...    after:  [r, ...
    void Div() {
        v2 = memory[sp--];
        if (v2 == 0) runError("division by zero");
        v1 = memory[sp];
        memory[sp] = v1 / v2;

        lineNo = memory[ip++];
    }

    // Mod: r = v1 % v2    before: [v1, v2, ...    after:  [r, ...
    void Mod() {
        v2 = memory[sp--];
        if (v2 == 0) runError("modulus by zero");
        v1 = memory[sp];
        memory[sp] = v1 % v2;

        lineNo = memory[ip++];
    }

    // And: r = v1 & v2    before: [v1, v2, ...    after:  [r, ...
    void And() {
        v2 = memory[sp--];
        memory[sp] = memory[sp] & v2;
    }

    // Neg: r = -v         before: [v, ...         after:  [r, ...
    void Neg() {
        memory[sp] = -memory[sp];
        ip = ip + 1;
    }

    // Add: r = v1 + v2    before: [v1, v2, ...    after:  [r, ...
    void Add() {
        v2 = memory[sp--];
        memory[sp] += v2;

        lineNo = memory[ip++];
    }

    // Sub: r = v1 - v2    before: [v1, v2, ...    after:  [r, ...
    void Subtract() {
        v2 = memory[sp--];
        memory[sp] -= v2;

        lineNo = memory[ip++];
    }

    // Or: r = v1 | v2     before: [v1, v2, ...    after:  [r, ...
    void Or() {
        v2 = memory[sp--];
        memory[sp] |= v2;
    }
    /** equal: op length
        *
        *           Variable stack (before)
        *           --------------------------
        *   A       | X                      |
        *           --------------------------
        *   A+1     | X1                     |
        *           --------------------------
        *               ..................
        *           --------------------------
        *   A+n     |   Xn                   |
        *           --------------------------
        *   A+n     |   Y                    |
        *    +1     --------------------------
        *   A+n     |   Y+1                  |
        *    +2     --------------------------
        *                .................
        *           --------------------------
        *   A+2n    |   Y+n                  | <-- sp
        *    +1     --------------------------
        *
        *           Variable stack (after)
        *           --------------------------
        *   A       | 1 if(X=Y) - 0 if (X!=Y)| <-- sp
        *           --------------------------
        *   A+1     |   X1                   |
        *           --------------------------
        *               ..................
        *           --------------------------
        *   A+n     |   Xn                   |
        *           --------------------------
        *   A+n     |   Y                    |
        *    +1     --------------------------
        *   A+n     |   Y+1                  |
        *    +2     --------------------------
        *                 .................
        *           --------------------------
        *   A+2n    |   Y+n                  |
        *    +1     --------------------------
        *
        *           n = length - 1
        */
        void Equal() {
            int16_t length = memory[ip];
            int16_t y = sp - length + 1;
            int16_t x = y - length;
            sp = x;
            bool equal = true;
            for (int16_t i=0; i<length; i++) {
                if(memory[x+i] != memory[y+i]) {
                    equal = false;
                    break;
                }
            }
            ip = ip + 1;
            if (equal)
                memory[sp] = 1;
            else
                memory[sp] = 0;
        }
    /** not equal: op length
        *
        *           Variable stack (before)
        *           --------------------------
        *   A       | X                      |
        *           --------------------------
        *   A+1     | X1                     |
        *           --------------------------
        *               ..................
        *           --------------------------
        *   A+n     |   Xn                   |
        *           --------------------------
        *   A+n     |   Y                    |
        *    +1     --------------------------
        *   A+n     |   Y+1                  |
        *    +2     --------------------------
        *               .................
        *           --------------------------
        *   A+2n    |   Y+n                  | <-- sp
        *    +1     --------------------------
        *
        *           Variable stack (after)
        *           --------------------------
        *   A       | 0 if(X=Y) - 1 if (X!=Y)| <-- sp
        *           --------------------------
        *   A+1     |   X1                   |
        *           --------------------------
        *               ..................
        *           --------------------------
        *   A+n     |   Xn                   |
        *           --------------------------
        *   A+n     |   Y                    |
        *    +1     --------------------------
        *   A+n     |   Y+1                  |
        *    +2     --------------------------
        *                 .................
        *           --------------------------
        *   A+2n    |   Y+n                  | <-- sp
        *    +1     --------------------------
        *
        *           n = length - 1
        */
        void NotEqual() {
            int16_t length = memory[ip];
            int16_t y = sp - length + 1;
            int16_t x = sp - length;
            sp = x;

            memory[sp] = 0;
            for (int16_t i=0; i<length; i++) {
                if(memory[x+i] != memory[y+i]) {
                    memory[sp] = 1;
                    break;
                }
            }
            ip = ip + 1;
        }

        // TestForLessThan: r = v1 < v2    before: [v1, v2, ...    after:  [r, ...
        void TestForLessThan() {
            v2 = memory[sp--];
            memory[sp] = (memory[sp] < v2) ? 1 : 0;
        }

        // TestForGreaterOrEqual: r = v1 >= v2    before: [v1, v2, ...    after:  [r, ...
        void TestForGreaterOrEqual() {
            v2 = memory[sp--];
            memory[sp] = (memory[sp] >= v2) ? 1 : 0;
        }

        // TestForGreater: r = v1 >= v2    before: [v1, v2, ...    after:  [r, ...
        void TestForGreater() {
            v2 = memory[sp--];
            memory[sp] = (memory[sp] > v2) ? 1 : 0;
        }

        // TestForGreaterOrEqual: r = v1 <= v2    before: [v1, v2, ...    after:  [r, ...
        void TestForLessOrEqual() {
            v2 = memory[sp--];
            memory[sp] = (memory[sp] <= v2) ? 1 : 0;
        }

    /** *========================================
        *   Assigment_statement: Variable_symbol Expression ASSIGN
        *========================================
        *
        *           Program
        *           --------------------------
        *           |   AssignOpcode         |
        *           --------------------------
        *           |   length               | <-- ip
        *           --------------------------
        *
        *           Variable stack (before)
        *           --------------------------
        *   A-1     |   ?                    |
        *           --------------------------
        *   A       |   X                    |
        *           --------------------------
        *   A+1     |   Y0                   |
        *           --------------------------
        *   A+2     |   Y1                   |
        *           --------------------------
        *               .................
        *           --------------------------
        *   A+n     |   Yn                   | <-- sp
        *           --------------------------
        *
        *           Variable stack (after)
        *           --------------------------
        *   A-1     |   ?                    | <-- sp
        *           --------------------------
        *   A       |   X                    |
        *           --------------------------
        *   A+1     |   Y0                   |
        *           --------------------------
        *   A+2     |   Y1                   |
        *           --------------------------
        *                .................
        *           --------------------------
        *   A+n     |   Yn                   |
        *           --------------------------
        *               n = length
        */
        void Assign() {
            int16_t length = memory[ip];
            sp = sp - length - 1;
            int16_t x = memory[sp + 1];
            int16_t y = sp + 2;
            for (int16_t i=0; i<length; i++) {
                memory[x+i] = memory[y+i];
            }
            ip = ip + 1;
        }

    /** ====================================================
        *   Argument      = Expression | Variable_symbol | Procedure_argument
        *   Argument_list = Argument {Argument}
        *   Procedure_call
        *       Standard_call |
        *       [Argument_list] Instance ProcCall
        *       [Argument_list] Instance ParamCall
        *=====================================================
        *           Program
        *           --------------------------
        *           |   ProcCallOpcode       |
        *           --------------------------
        *           |   displacement         | <-- ip
        *           --------------------------
        *               ip = ip + displ
        *
        *           Variable stack (before)
        *           --------------------------
        *   A       |   link                 | <-- sp
        *           --------------------------
        *   A+1     |                        |
        *           --------------------------
        *   A+2     |                        |
        *           --------------------------
        *   A+3     |                        |
        *           --------------------------
        *   A+4     |                        |
        *           --------------------------
        *
        *           Variable stack (after)
        *           --------------------------
        *   A       |   link                 | <-- bp
        *           --------------------------
        *   A+1     |   bp                   |
        *           --------------------------
        *   A+2     |                        |
        *           --------------------------
        *   A+3     |   pe                   |
        *           --------------------------
        *   A+4     |   ip+2(next inst)      | <-- sp
        *           --------------------------
        */

        void ProcCall() {
            int16_t displacement = memory[ip];
//t         printStack();

            memory[sp + 1] = bp;
            memory[sp+3] = pe;
            memory[sp+4] = ip + 1; // next instruction
            bp = sp;
            sp = sp + 4;
            ip = ip + displacement - 1;
//t         printStack();
        }

        void ProcArg() {
           int16_t displacement = memory[ip++];
//t        printStack();
           memory[++sp] = displacement;
//t        printStack();
        }

    void ParamCall() {
//t        printStack();
        int16_t displacement = memory[ip];
        int16_t addr = memory[sp] + displacement;
        int16_t dest = memory[addr + 1];

//t        printStack();

        memory[sp] = memory[addr];
        memory[sp + 1] = bp;
        memory[sp + 3] = pe;
        memory[sp + 4] = ip + 1; // next instruction
        bp = sp;
        sp = sp + 4;
        ip = dest;
//        printStack();
    }

       /** =====================================================
        *   Conditional_statement = [Expression Do] Statement_list [Else]
        *   Conditional_statement_list = Conditional_statement {Conditinal_statement}
        * =====================================================
        *           Program
        *           --------------------------
        *           |   DoOpcode             |
        *           --------------------------
        *           |   displacement         | <-- ip
        *           --------------------------
        *
        *           Variable stack (before)
        *           --------------------------
        *   A       |   X                    |
        *           --------------------------
        *   A+1     |   (0,1)                | <-- sp
        *           --------------------------
        *
        *           Variable stack (after)
        *           --------------------------
        *   A       |   X                    | <-- sp
        *           --------------------------
        *   A+1     |   (0,1)                |
        *           --------------------------
        */
        void Do() {
            int16_t displacement = memory[ip];
            if (memory[sp] == 1)
                ip++;
            else
                ip = ip + displacement - 1;
            sp--;
        }

    /**
        *           Program
        *           --------------------------
        *           |   ElseOpcode           |
        *           --------------------------
        *           |   displacement         | <-- ip
        *           --------------------------
        */
        void Else() {
            int16_t displacement = memory[ip];
            ip = ip + displacement - 1;
        }

    /** =====================================================
        *   if_statement = conditional_statement_list
        *   while_statement
        *           conditional_statement_list
        *   when_statement =
        *           When conditional_statement_list Wait Endwhen
      * =====================================================
        *           Program
        *           --------------------------
        *           |   WhenOpcode           |
        *           --------------------------
        *           |                        | <-- ip
        *           --------------------------
        */
        void When() {
            //do nothing
        }

    /**
        *           Program
        *           --------------------------
        *           |   WaitOpcode           |
        *           --------------------------
        *           |   displacement         | <-- ip
        *           --------------------------
        */
        void Wait() {
            int16_t displacement = memory[ip];
            ip = ip + displacement - 1;
            preempt();
            if (taskCurrent >= (numberOfTasks-1))
                taskCurrent = 0;
            else
                taskCurrent++;
            resume();
        }

    /**
        *           Program
        *           --------------------------
        *           |   EndWhenOpcode        |
        *           --------------------------
        *           |                        | <-- ip
        *           --------------------------
        */
        void EndWhen() {
            //do nothing
        }

    /** =====================================================
        *   Process_statement = Process statement_list Also
      * =====================================================
        *           Program
        *           --------------------------
        *           |   ProcessOpcode        |
        *           --------------------------
        *           |   tempLength           | <-- ip
        *           --------------------------
        *           |   lineNo               |
        *           --------------------------
        */
        void Process() {
            int16_t tempLength = memory[ip];
            lineNo = memory[ip+1];

            if ((sp + tempLength) > pe) {
                runError("Not enough memory to run a process sp");
            }
            ip += 2;
        }

    /**
        *           Program
        *           --------------------------
        *           |   AlsoOpcode           |
        *           --------------------------
        *           |   displacement         | <-- ip
        *           --------------------------
        */
        void Also() {
            int16_t displacement = memory[ip];
            if (numberOfTasks > 1) {
                while (taskCurrent < (numberOfTasks-1)) {
                    taskQueue[taskCurrent] = taskQueue[taskCurrent+1];
                    taskCurrent++;
                }
                numberOfTasks = numberOfTasks - 1;
                taskCurrent = 0;
                resume();
            } else { // numberOfTasks = 1
                sp = taskStackTop;
                pe = taskProgTop;
                ip = ip + displacement - 1;
            }
        }

    /** =====================================================
        *   Process_statement_list =  Process_statement {Process_statement}
        *   Concurent_statement    =  Goto Process_statement_list Cobegin
      * =====================================================
        *           Program
        *           --------------------------
        *           |   CoBeginOpcode        |
        *           --------------------------
        *           |   numOfTask            | <-- ip
        *           --------------------------
        *           |   lineNo               |
        *           --------------------------
        *           |   process constant     |
        *           --------------------------
        *           |   displacement         |
        *           --------------------------
        *           |   process constant     |
        *           --------------------------
        *           |   displacement         |
        *           --------------------------
        *           |   lineNo               |
        *           --------------------------
        *               .................
        *           --------------------------
        *           |   lineNo               |
        *           --------------------------
        *           |   displacement         |
        *           --------------------------
        */
        void Cobegin() {
            int16_t numOfTask = memory[ip];
            lineNo = memory[ip+1];

            numberOfTasks = numOfTask;
            if (numberOfTasks > MAX_QUEUE)
                runError("Exceeds maximum number of processes");
            taskStackTop = sp;
            taskProgTop = pe;

            int16_t length = (pe-sp)/numOfTask;
            for (int16_t i=0; i<numOfTask;i++) {
                pe = sp + length;
                taskQueue[i] = new Task();
                taskQueue[i]->sp = sp;
                taskQueue[i]->bp = bp;
                taskQueue[i]->pe = pe;
                taskQueue[i]->ip = ip + memory[ip+i*2+3] - 1;
                sp = pe;
            }
            taskCurrent = 0;
            resume();
        }

    /**         Program
        *           -----------------
        *           |   IndexOpcode |
        *           -----------------
        *           |   lower       | <-- ip
        *           -----------------
        *           | upper         |
        *           -----------------
        *           |   length      |
        *           -----------------
        *           |   lineNo      |
        *           -----------------
        *
        *           Variable stack (before)
        *           ------------------
        *   A       | VariableAddress|
        *           ------------------
        *   A+1     |   Index        | <-- VI
        *           ------------------
        *
        *           Variable stack (after)
        *           ------------------
        *   A       | IndexAddress   | <-- VI
        *           ------------------
        *   A+1     |                |
        *           ------------------
        */
    void Index() {
        int16_t index;

        int16_t lower  = memory[ip];
        int16_t upper  = memory[ip+1];
        int16_t length = memory[ip+2];

        lineNo  = memory[ip+3];

        index = memory[sp];
        sp = sp - 1;

        if ((index < lower) || (index > upper))
            runError("index exceeds array dimension");

        memory[sp] = memory[sp] + (index-lower)*length;
        ip += 4;
    }

    /**
        *           Program
        *           -----------------
        *           |   FieldOpcode |
        *           -----------------
        *           |   Displacement| <-- ip
        *           -----------------
        *
        *           Variable stack (before)
        *           ------------------
        *   A       | Link           | <-- VI
        *           ------------------
        *   A+1     |                |
        *           ------------------
        *
        *           Variable stack (after)
        *           ------------------
        *   A       | Link + disp    | <-- VI
        *           ------------------
        *   A+1     |                |
        *           ------------------
        *
        */
    void Field() {
        int16_t displ = memory[ip];
        memory[sp] = memory[sp] + displ;
        ip = ip + 1;
    }
    //------------------------------------------- Extras
    void PutInteger() {
        char integer[8];
        sprintf(integer, "%d", memory[sp]);
        PutS(integer);
        sp = sp - 1;
    }
    void PutCharacter() {
        PutC((char)memory[sp]);
        sp = sp - 1;
    }
    void PutBoolean() {
        PutS((memory[sp] == 0) ? "false" : "true");
        sp = sp - 1;
    }
    void PutLine() {
        PutN();
    }
    void EndCode() { 
//t     printf("The program terminates.\n");
        exit(0);
    }
};

int main() {
    bsl_Uart_Init();

    Kernel* kernel = new Kernel();
    kernel->load();
    kernel->run();

    return 0;
}
