#include "unistd.h"
int main(void)
{
	char *str = "Hello, World!";
	write(2, str, sizeof(str));
	return 0;
}