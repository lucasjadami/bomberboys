#include <cstdio>
#include <cstring>

int main()
{
	char line[1000];
	double totalAverage, totalDeviation;
	int readCount = 0;
	
	while (gets(line) != NULL)
	{
		char firstWord[100];
		sscanf(line, "%s", firstWord);
		
		if (strcmp(firstWord, "Player") == 0)
		{
			double average, deviation;
			sscanf(line, " %*s %*s %*s %*s %*s %*s %*s %lf %lf ", &average, &deviation);
			readCount++;
			totalAverage += average;
			totalDeviation += deviation;
		}
	}
	
	printf("%lf %lf\n", totalAverage / readCount, totalDeviation / readCount);
	
	return 0;
}
