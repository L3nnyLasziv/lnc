#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

char* reverse(const char* inStr) {	
	const size_t l = strlen(inStr);	
	char* result = malloc( (l + 1) * sizeof(char) );	
	for (size_t index = 0;index<l;++index){
		result[index] = inStr[(l-1)-index];
	}
	result[l] = 0;
	return result;
}

unsigned char find(char c, const char* str) {
	for (size_t index = 0;index < strlen(str);++index) {
		if (c == str[index]) return index;
	}
	return 255;
}


unsigned long strToInt(const char* inStr, unsigned char inBase) {
	unsigned long number = 0;
	unsigned long multi = 1;
	
	char* revInStr = reverse(inStr);	
	for (size_t index = 0;index < strlen(revInStr);++index) {
		unsigned char digit = find(revInStr[index], digits);
		number += digit * multi;
		multi *= inBase;
	}	
	free(revInStr);	
	return number;
}


char* intToStr(unsigned long number, unsigned char outBase) {
	
	int _number = number;
	size_t l = 0;
	do {
		unsigned char digit = _number % outBase;
		l++;
		_number = _number / outBase;
	} while (_number > 0);
	
	char* revOutStr = malloc( (l + 1) * sizeof(char) );	
	
	size_t pos = 0;
	do {
		unsigned char digit = number % outBase;
		revOutStr[pos++] = digits[digit];
		number = number / outBase;
	} while (number > 0);
	revOutStr[pos] = 0;
	
	
	char* outStr = reverse(revOutStr);	
	free(revOutStr);	
	return outStr;
}

char* convert(const char* inStr, unsigned char inBase, unsigned char outBase) {
	return intToStr(strToInt(inStr, inBase), outBase);
}


int main(int argc, char **argv) {
	char* result = convert(argv[1], strToInt(argv[2],10), strToInt(argv[3],10));
	printf("%s -> %s \n", argv[1], result);	
	free(result);
	return 0;
}



/* 

char* reverse2(char* inStr) {	
	const size_t l = strlen(inStr);	
	char tmp;
	for (size_t index = 0;index<l/2;++index){
		tmp = inStr[index];
		inStr[index] = inStr[(l-1)-index];
		inStr[(l-1)-index] = tmp;
	}	
	return inStr;
}


*/