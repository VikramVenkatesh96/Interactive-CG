#include "Application.h"

#define WIDTH 1280
#define HEIGHT 720

int main()
{
	Application myApp(WIDTH,HEIGHT, "MyApp");
	myApp.Run();
	return 0;
}