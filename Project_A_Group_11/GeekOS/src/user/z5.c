#include <conio.h>
#include <process.h>

int count=0;

void *valueCalc(){
  count++;
  Print("OK it's me with count = %d\n",count);
  while(1);
  Exit(0);
  return 0;
}

int main(int argc, char **argv) 
{
  int pid = SpawnUserThread(valueCalc);
  Print("Hello World !!! %d\n", pid);
  pid = SpawnUserThread(valueCalc);
  Print("Hello World !!! %d\n", pid);
  
  Print("Answer is 42\n");
  while(1);
}
