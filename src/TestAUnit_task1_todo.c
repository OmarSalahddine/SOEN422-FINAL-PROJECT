// TestAUnit_task1_todo.c - Test AUnit for Arduino Nano.

#include "COutForAUnit.h"

#include "bsl_Uart.h"

#include <stdint.h>

#include <stdbool.h>

#include <util/delay.h>

static void result(void){
if(Equals()){
PutC('.');
} 

else {PutC('F');}

}

static void Test1(void) {
  PutS("Test 1 - Test Number one\n");
  PutS("1\n"); // Expected output
  PutS("1\n"); // Current output
}

static void Test2(void) {
  PutS("Test 2 - Test Number two\n");
  PutS("23\n"); // Expected output
  PutS("24\n"); // Current output
}

typedef void( * TestEntry)(void);

#define TestMax 2 // Up to 9.

static TestEntry tests[TestMax] = {
  Test1,
  Test2,
  //  Test3,
  //  Test4,
  //  ...
  //  Test9
};

/*-------------------------------------------------------------------
 * main
 *-------------------------------------------------------------------*/
int main(void) {
    bsl_Uart_Init();

    bool testRun = true;

    PutS("Test AUnit on Arduino Nano v1.0\n");
    PutS("Usage: Enter <n> where n is the test number 1..");
    PutX4(TestMax); 
    PutS(" or '0' (zero) to quit.\n");
    while(testRun){
    PutS("$ ");
    char cmd = GetC();
    PutC(cmd);
    PutN();
    ResetBuffer();
    switch(cmd){
     case '1':
      Test1();
      break;
     case '2':
      Test2();
      break;
     case '0':
      testRun = false;
      break;
     case '\n':
      break;
     default:
      PutS("Test not provided");
      break;
    }
    result();
    PutN();
    ResetBuffer();
   }

    PutS("bye!\n");
    return 0;
}
