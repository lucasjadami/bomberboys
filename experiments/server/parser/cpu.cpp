#include <cstdio>
#include <cstring>

int main()
{
	char line[1000];
	double totalUser, totalKernel, totalIdle;
	int readCount = 0;
	
	while (gets(line) != NULL)
	{
		gets(line);
		gets(line);
		gets(line);
	
		int user, kernel, idle;
	
		sscanf(line, "%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %d %d %d", &user, &kernel, &idle);
		readCount++;
		totalUser += user;
		totalKernel += kernel;
		totalIdle += idle;
	}
	
	printf("%lf %lf %lf\n", totalUser / readCount, totalKernel / readCount, totalIdle / readCount);
	
	return 0;
}
