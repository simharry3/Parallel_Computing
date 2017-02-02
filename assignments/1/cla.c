////////////////////////////////////////////////////////
//					CLAYTON RAYMENT
//					01/20/2017
//	Code adapted from Computer Organization Homework #4
////////////////////////////////////////////////////////

//Block size: 8bit
//Input size: 512bit

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define n 512
#define blockSize 8

typedef struct binaryNumber binaryNumber;

//Struct to contain the binary number nicely:
struct binaryNumber{
	char name;
	char hex[n/4];
  int data[n];
};

//xor function implementation:
int xor(int X, int Y){
	if(X == Y){
		return 0;
	}
	else{
		return 1;
	}
}

//initialize binary number from unsigned long int and a name
void initBinary(binaryNumber* A, unsigned long long int uA, char name){
	A->name = name;
	//int n = 64;
	for(int i = 0; i < n; ++i){
		A->data[i] = uA & 1; 
		uA = uA >> 1;
	}
	for(int i = 0; i < n/4; ++i){
		A->hex[i] = 0;
	}

}

//Prints the binary number in a nice format
void printBinary(binaryNumber* A){
	printf("%c (bin): ", A->name);

	for(int i = 0; i < n; ++i){
		printf("%d", A->data[n - 1 -i]);
	}
	printf("\n");

}

//Prints the hex number in a nice format
void printHex(binaryNumber* A){
	printf("%c (hex): ", A->name);

	for(int i = 0; i < n/4; ++i){
		printf("%02x", A->hex[n/4 - 1 - i]);
	}
	printf("\n");

}

//make a hex number from binary:
void makeHex(binaryNumber* A){
	int index = 0;
	int mult = 1;
	char value[5];
	for(int i = 0; i < n/4; ++i){
		memcpy(value, &(A->data[i * 4]), 4);
		value[4] = '\0';
		printf("%d|", strtol(&value[0], NULL, 2));
	}
}

//messy debug function to print an array
void debugBinary(int* num, int len, char name){
	printf("%c (bin): ", name);

	for(int i = 0; i < len; ++i){
		printf("%d", num[(len-1)-i]);
	}
	printf("\n");

}


//Main CLA implementation according to the PDF
void sumCLA(binaryNumber* A, binaryNumber* B, binaryNumber* S){
	//TODO: Convert to run 512 bits with 8 bit block size.
	printf("Carry Lookahead Sum, CS:\n");
	//Calculate all G[i] and P[i]:
	int P[n] = {0};
	int G[n] = {0};
	for(int i = 0; i < n; ++i){
		P[i] = A->data[i] | B->data[i];
		G[i] = A->data[i] & B->data[i];
	}
	
	 // debugBinary(&G[0], n, 'G');
	 // debugBinary(&P[0], n, 'P');

	//Calculate all GG[j] PG[j];
	int PG[n/blockSize] = {0};
	int GG[n/blockSize] = {0};
	for(int j = 0; j < n/blockSize; ++j){
		int i = j * blockSize;
		int rollP = 1;
		GG[j] = G[i + blockSize - 1];
		PG[j] = P[i];
		for(int k = 0; k < blockSize; ++k){
			if(k > 0){
				PG[j] &= P[i + k];
				rollP &= P[i + (blockSize - k - 1)];
				GG[j] |= rollP & G[i + blockSize - k - 2];
			}
		}
		//GG[j] = G[i + 3] | (P[i + 3] & G[i + 2]) | (P[i + 3] & P[i + 2] & G[i + 1]) | (P[i + 3] & P[i + 2] & P[i + 1] & G[i + 0]);
	}

	// debugBinary(&GG[0], n/blockSize, 'g');
	// debugBinary(&PG[0], n/blockSize, 'p');

	//Calculate all SG[k] and SP[k]
	int SG[blockSize] = {0};
	int SP[blockSize] = {0};
	for(int k = 0; k < blockSize; ++k){
		int j = k * blockSize;

		int rollP = 1;
		SG[k] = GG[j + blockSize - 1];
		SP[k] = PG[j];
		for(int l = 0; l < blockSize; ++l){
			if(l > 0){
				SP[k] &= PG[j + l];
				rollP &= PG[j + (blockSize - l - 1)];
				SG[k] |= rollP & GG[j + blockSize - l - 2];
			}
		}
	}
	// debugBinary(&SG[0], blockSize, 'g');
	// debugBinary(&SP[0], blockSize, 'p');


	//Calculate all SC[k]
	int SC[blockSize] = {0};
	SC[0] = SG[0] | (SP[0] & 0);
	for(int k = 1; k < blockSize; ++k){
		SC[k] = SG[k] | (SP[k] & SC[k-1]);
	}
	// debugBinary(&SC[0], 4, 'C');

	//Calculate all GC[j]:
	int k = 0;
	int GC[n/blockSize] = {0};
	GC[0] = GG[0];
	for(int j = 1; j < n/blockSize; ++j){
		if(j % blockSize == 0){
			GC[j] = GG[k] | (PG[j] & SC[j/blockSize]);
			++k;
		}
		else{
			GC[j] = GG[k] | (PG[j] & GC[j-1]);
		}
		
	}
	// debugBinary(&GC[0], 16, 'C');
	//Calculate C[i]:
  k = 0;
	int C[n + 1] = {0};
	C[0] = GC[0];
	for(int j = 0; j < n; ++j){
		if((j) % blockSize == 0){
			C[j + 1] = G[j] | (P[j] & GC[j/blockSize]);
			++k;
		}
		else{
			C[j + 1] = G[j] | (P[j] & C[j]);
		}
	}

	// debugBinary(&C[0], n+1, 'C');
	//Calculate S[i]:
	S->data[0] = xor(xor(A->data[0], B->data[0]), 0);
	for(int i = 1; i < n; ++i){
		S->data[i] = xor(xor(A->data[i], B->data[i]), C[i]);
	}

}

//Ripple sum for testing:
void sumRipple(binaryNumber* A, binaryNumber* B, binaryNumber* S){
	printf("Ripple Sum, RS:\n");
	int G[n] = {0};
	int P[n] = {0};
	int C[n+1] = {0};
  C[0] = 0;
  //Generate all P[i] G[i] and C[i]
 	for(int i = 0; i < n; ++i){
		P[i] = xor(A->data[i], B->data[i]);
		G[i] = A->data[i] & B->data[i];
		C[i + 1] = G[i] | (P[i] & C[i]);
		//Calculate sum:
		S->data[i] = xor(P[i], C[i]);
	}
}

void getInputs(binaryNumber* A, binaryNumber* B){
	// int n = 64;
	char* buffer1 = calloc(n/4 + 1, sizeof(char));
	char* buffer2 = calloc(n/4 + 1, sizeof(char));
	printf("Enter A (hex):\n");
	scanf("%s", buffer1);
	printf("Enter B (hex):\n");
	scanf("%s", buffer2);
	printf("\n");
	for(int i = 0; i < n/4; ++i){
		//printf("%c", buffer1[i]);
		A->hex[i] = buffer1[i];
		B->hex[i] = buffer2[i];
	}
	free(buffer1);
	free(buffer2);
}

char hexLookup(char in){
	in = toupper(in);
	if(isdigit(in)){
		return in - '0';
	}
	else{
		return in - 'A' + 0x0A;
	}
}

//Print the inputs to the function
void printAndTranslateInputs(binaryNumber* A, binaryNumber* B){
	// int n = 64;
	printf("A is: \n");
	for(int i = 0; i < n/4; ++i){
		printf("%c", A->hex[i]);
		char temp = hexLookup(A->hex[i]);
		//Convert to binary number
		for(int j = 0; j < 4; ++j){
			A->data[n - (i*4 + (3 - j)) - 1] = temp & 0x01;
			temp = temp >> 1;
			
		}
	}
	printf("\nBinary: \n");
	for(int i = 0; i < n; ++i){
		printf("%d", A->data[n - i - 1]);
	}
	printf("\n\n");

	printf("B is: \n");
	for(int i = 0; i < n/4; ++i){
		printf("%c", B->hex[i]);
		char temp = hexLookup(B->hex[i]);
		//Convert to binary number
		for(int j = 0; j < 4; ++j){
			B->data[n - (i*4 + (3 - j)) - 1] = temp & 0x01;
			temp = temp >> 1;
		}
	}
	printf("\nBinary:\n");
	for(int i = 0; i < n; ++i){
		printf("%d", B->data[n - i - 1]);
	}
	printf("\n");
}

//Print the sum output
void printOutput(binaryNumber* A){
	// int n = 64;
	printf("Binary:\n");
	for(int i = 0; i < n; ++i){
		printf("%d", A->data[n - i - 1]);
	}
	printf("\n");
}


int main(int argc, char* argv[]){
	binaryNumber A;
	binaryNumber B;
	binaryNumber S;

	getInputs(&A, &B);
	printAndTranslateInputs(&A, &B);

	// initBinary(&A, hA, 'A');
	// initBinary(&B, hB, 'B');
	// initBinary(&S, 0, 'S');

	//sumRipple(&A, &B, &S);
	printf("\n\n");
	sumCLA(&A, &B, &S);
	printOutput(&S);
	sumRipple(&A, &B, &S);
	printOutput(&S);
	
	// printBinary(&A);
	// printBinary(&B);
	// printBinary(&S);

	makeHex(&S);
	//printHex(&S);



	return EXIT_SUCCESS;
}