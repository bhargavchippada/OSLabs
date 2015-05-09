#include <conio.h>
#include <fileio.h>

int main() 
{
  int fd = Open("output.txt", O_CREATE|O_WRITE);
  Print("fd is %d", fd);
  char buf[10] = {"aditya"};
  Write(fd, buf, 10);
  Close(fd);
  return 1;
}
