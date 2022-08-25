#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>


int getTag(long long int addy, double bSize, double sSize){
	return (addy >> ((int)(bSize + sSize)));
}

int getIndex(long long int addy, double bSize, double sSize){
	addy = addy & (((1 << (int)sSize)-1) << (int)bSize);
	return (addy >> (int)bSize);
}

int isValidNumber(char* input){
	
	if (input[0] == '-'){
		return -1;
	}
	
	for (int w = 0; w < strlen(input); w++){
		if (isdigit(input[w]) == 0) return -1;
	}
	
	if (ceil(log2((double)strtol(input, NULL, 10))) != floor(log2((double)strtol(input, NULL, 10)))){
		return -1;
	}
	
	return 0;
} 


int main(int argc, char* argv[argc + 1]){

	if (isValidNumber(argv[1]) == -1 || isValidNumber(argv[3]) == -1){
		printf("error");
		return EXIT_SUCCESS;
	}
	
	int cacheSize = strtol(argv[1], NULL, 10);
	int setSize = 0;
	char *assoc = argv[2];
	int blockSize = strtol(argv[3], NULL, 10);
	
	FILE * fp = fopen(argv[4], "r");
	
	int tag;
	int set;
	
	int lines = 0;

	int reads = 0;
	int writes = 0;
	int hits = 0;
	int misses = 0;

	double offBitSize = 0;
	double setBitSize = 0;
	

	int isValid = 0;
	if (strcmp(assoc, "direct") == 0){
		lines = 1;
		setSize = cacheSize/blockSize;
		isValid = 1;
	}
	else if (strcmp(assoc, "assoc") == 0){
		setSize = 1;
		lines = cacheSize/blockSize;
		isValid = 1;
	}
	else {
		sscanf(assoc, "assoc:%d", &lines);
		setSize = cacheSize/(lines*blockSize);
		isValid = 1;	
	}

	if (isValid == 0) {
		printf("error");
		return EXIT_SUCCESS;
	}
	
	
	long long int cache[setSize][lines+1];
	//memset(cache, -1, sizeof(cache));
	for (int q = 0; q < setSize; q++){
		for (int m = 0; m < lines + 1; m++){
			cache[q][m] = -1;
		}
	}
	offBitSize = log2((double)blockSize);
	setBitSize = log2((double)setSize);
	long long int memAdd;
	char command;
	
	while (fscanf(fp, "%c %llx", &command, &memAdd) != EOF){
		tag = getTag(memAdd, offBitSize, setBitSize);
		set = getIndex(memAdd, offBitSize, setBitSize);
		if (cache[set][0] == -1) cache[set][lines] = 0;
		
		if (command == 'R'){
			int wasHit = 0;
			for (int y = 0; y < lines; y++){
				if (cache[set][y] == tag){
					hits++;
					wasHit = 1;
					break;	
				}
			}
			if (wasHit == 0){
				reads++;
				misses++;
				cache[set][cache[set][lines]] = tag;
				cache[set][lines] = (cache[set][lines] + 1) % lines;
			}
		}
		
		if (command == 'W'){
			int wasHit2 = 0;
			for (int z = 0; z < lines; z++){
				if (cache[set][z] == tag){
					writes++;
					hits++;
					wasHit2 = 1;
					break;
				}
			}
			if (wasHit2 == 0){
				misses++;
				cache[set][cache[set][lines]] = tag;
				cache[set][lines] = (cache[set][lines] + 1) % lines;
				reads++;
				writes++;
			}
		}
	}
  	
  	printf("memread:%d\n", reads);
  	printf("memwrite:%d\n", writes);
  	printf("cachehit:%d\n", hits);
  	printf("cachemiss:%d\n", misses);
  	fclose(fp);
  	return EXIT_SUCCESS;
}
