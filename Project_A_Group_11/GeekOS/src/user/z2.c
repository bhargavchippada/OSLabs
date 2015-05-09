#include <conio.h>
#include <process.h>

int main(int argc, char **argv) 
{
  int c = 0;
  while (c < 1000000){
    c++;
    if (c % 500000 == 0){
      char ch = Get_Key();
      Print("Key - %c\n", ch);
      ch = Get_Key();
      Print("Key - %c\n", ch);
    }
  }
  return 1;
}
