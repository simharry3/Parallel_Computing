////////////////////////////////////////////////////////
//					CLAYTON RAYMENT
//					01/20/2017
//	Code adapted from Computer Organization Homework #4
////////////////////////////////////////////////////////


//////////////////////////////////////
//
//
//	|	1	|	2	|	3	|	4	|	5	|	6	|	7	|	8	|
//	|		s1	|		s2	|		s3	|		s4	|
//	|			ss1				|			ss2				|
//	|							s31								|
//
//
//
//
//




//Block size: 8bit
//Input size: 512bit

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>

//262144
//512
#define n 512
#define blockSize 8
#define hexSize n/4
#define numRanks 8
#define numFiles 8

typedef struct binaryNumber binaryNumber;

//Struct to contain the binary number nicely:
struct binaryNumber{
	char name;
	char hex[hexSize/numFiles + 1];
  int data[n/numFiles + 1];
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

	for(int i = 0; i < n/numFiles; ++i){
		printf("%d", A->data[n/numFiles - 1 - i]);
	}
	printf("\n");

}

//Prints the hex number in a nice format (and in the correct orientation).
void printHex(binaryNumber* A){
	printf("Hex:\n");

	for(int i = 0; i < hexSize/numFiles; ++i){
		printf("%c", A->hex[hexSize/numFiles - 1 - i]);
	}
	printf("\n");

}

//make a hex number from binary:
void makeHex(binaryNumber* A){
	int index = 0;
	int mult = 1;
	int binValue;
	char value[5];
	for(int i = 0; i < hexSize/numFiles; ++i){
		binValue = 0;
		mult = 1;
		for(int j = 0; j < 4; ++j){
			binValue += mult * A->data[i * 4 + j];
			mult *= 2;
		}
		sprintf(&(A->hex[i]), "%X", binValue);
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
	int** p;
	int** g;
	int** c;
	//TODO: Convert to run with block size 8 and 262144 bits.
	//TODO: Convert to use only loops.
	printf("Carry Lookahead Sum, CS:\n");
	//Calculate all G[i] and P[i]:

	//TODO: get carry input from other MPI file
	//CALLOC REQUIRED ARRAYS IN FOR LOOP:
	int reqBlocks = (int)log(n/numFiles)/log(blockSize) + 1;
	p = calloc(reqBlocks, sizeof(int*));
	g = calloc(reqBlocks, sizeof(int*));
	c = calloc(reqBlocks, sizeof(int*));
	printf("REQUIRED BLOCKS FOR CALCULATION: %d\n", reqBlocks);
	for(int i = 0; i < reqBlocks; ++i){
		printf("CALLOC SIZE %d\n", (int)pow(blockSize, reqBlocks - i));
		p[i] = calloc((int)pow(blockSize, reqBlocks - i), sizeof(int));
		g[i] = calloc((int)pow(blockSize, reqBlocks - i), sizeof(int));
		c[i] = calloc((int)pow(blockSize, reqBlocks - i) + 1, sizeof(int));
	}
	for(int i = 0; i < n/numFiles; ++i){
		p[0][i] = A->data[i] | B->data[i];
		g[0][i] = A->data[i] & B->data[i];
	}
	// debugBinary(g[0], n/numFiles, 'G');
	// debugBinary(p[0], n/numFiles, 'P');

	// Calculate all group generates and group propogates:
	for(int i = 1; i < reqBlocks; ++i){
		for(int j = 0; j < (int)pow(blockSize, reqBlocks - i); ++j){
			//printf("%d|", j);
			int ind = j * blockSize;
			int rollP = 1;
			g[i][j] = g[i-1][ind + blockSize - 1];
			p[i][j] = p[i-1][ind];
			for(int k = 0; k < blockSize; ++k){
				if(k > 0){
					p[i][j] &= p[i-1][ind + k];
					rollP &= p[i-1][ind + (blockSize - k - 1)];
					g[i][j] |= rollP & g[i-1][ind + blockSize - k - 1];
				}
			}
		}
		// debugBinary(g[i], (int)pow(blockSize, reqBlocks - i), 'G');
		// debugBinary(p[i], (int)pow(blockSize, reqBlocks - i), 'P');
	}
	//Calculate C[0]:
	//TODO: Check carryin from other MPI threads?
	for(int i = 0; i < reqBlocks; ++i){
		c[i][0] = g[i][0] | (p[i][0] & 0);
	}
	for(int i = 1; i < blockSize; ++i){
		c[0][i] = g[0][i] | (p[0][i] & c[0][i-1]);
	}
	// debugBinary(c[reqBlocks - 1], blockSize, 'C');
	//Collapse all group generates and group propogates:
	for(int i = reqBlocks - 1; i > 0; --i){
		printf("Size: %d\n", (int)pow(blockSize, reqBlocks - i + 1));
		for(int j = 0; j < (int)pow(blockSize, reqBlocks - i + 1); ++j){
			if(j % blockSize == 0){
				c[i - 1][j + 1] = c[i][j/blockSize];
			}
			else{
				c[i - 1][j + 1] = g[reqBlocks - i - 1][j] | (p[reqBlocks - i - 1][j] & c[i - 1][j]);
			}
		}

		// debugBinary(c[i - 1], (int)pow(blockSize, reqBlocks - i + 1), 'C');
	}

	// Calculate S[i]:
	//TODO CHECK CARRY IN
	S->data[0] = xor(xor(A->data[0], B->data[0]), 0);
	for(int i = 1; i < n/numFiles; ++i){
		S->data[i] = xor(xor(A->data[i], B->data[i]), c[0][i]);
	}
	// debugBinary(S->data, n/numFiles, 'S');

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

void getInputFile(binaryNumber* A, binaryNumber* B){
	// int n = 64;
	char* filename = calloc(15, sizeof(char));
	int filesPerRank = numFiles/numRanks;

	char* buffer1 = calloc(hexSize/numFiles + 1, sizeof(char));
	char* buffer2 = calloc(hexSize/numFiles + 1, sizeof(char));


	int index = 0;
	FILE* fp;
	for(int i = 0; i < filesPerRank; ++i){
		sprintf(filename, "inputs/input.%d", i);
		fp = fopen(filename, "r");
		fgets(buffer1, hexSize/numFiles + 1, fp);
		fseek(fp, 1, SEEK_CUR);
		fgets(buffer2, hexSize/numFiles + 1, fp);

		for(int j = 0; j < hexSize/numFiles; ++j){
			index = (i * hexSize) + j;
			A->hex[index] = buffer1[j];
			B->hex[index] = buffer2[j];
		}
		fclose(fp);
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
void hexToBinary(binaryNumber* A, binaryNumber* B){
	// int n = 64;
	for(int i = 0; i < hexSize/numFiles; ++i){
		char temp = hexLookup(A->hex[i]);
		//Convert to binary number
		for(int j = 0; j < 4; ++j){
			A->data[(i*4 + (j))] = temp & 0x01;
			temp = temp >> 1;
			
		}
	}
	for(int i = 0; i < hexSize/numFiles; ++i){
		char temp = hexLookup(B->hex[i]);
		//Convert to binary number
		for(int j = 0; j < 4; ++j){
			B->data[(i*4 + (j))] = temp & 0x01;
			temp = temp >> 1;
		}
	}
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

//Parse input files, and split them into seperate files for MPI use.
// Math is as follows:
// in1 - in2
void inputParse(char* in1, char* in2){
	char* buffer1 = calloc(hexSize/numFiles + 1, sizeof(char));
	char* buffer2 = calloc(hexSize/numFiles + 1, sizeof(char));

	FILE* fp1;
	FILE* fp2;
	FILE* fo1;

	fp1 = fopen(in1, "r");
	fp2 = fopen(in2, "r");

	//splitInputName  = input.x
	mkdir("inputs", 0700);
	char* splitInName = calloc(17, sizeof(char));
	for(int index = 0; index < numFiles; ++index){
		fgets(buffer1, hexSize/numFiles + 1, fp1);
		fgets(buffer2, hexSize/numFiles + 1, fp2);
		sprintf(splitInName, "./inputs/input.%d", index);
		fo1 = fopen(splitInName, "w");
		int j = hexSize/numFiles;
		for(int i = 0; i < j; ++i){
			fputc(buffer1[i], fo1);
		}
		fputc('\n', fo1);
		for(int i = 0; i < j; ++i){
			fputc(buffer2[i], fo1);
		}
		fclose(fo1);
	}
	fclose(fp1);
	fclose(fp2);	
}

void writeOutputFile(binaryNumber* S){
	FILE* fo1;
	int fileNumber = 0;
	mkdir("output", 0700);
	char* filename = calloc(18, sizeof(char));
	sprintf(filename, "./output/result.%d", fileNumber);
	fo1 = fopen(filename, "w");
	for(int j = 0; j < hexSize/numFiles; ++j){
		fputc(S->hex[j], fo1);
	}
	fclose(fo1);
}

int main(int argc, char* argv[]){
	//Read input files and break into seperate files:
	//should look like the following:
	//
	//	F1		F2		F3		F4		FX
	//	xxxx	xxxx	xxxx	xxxx	...
	//	yyyy	yyyy	yyyy	yyyy	...
	//

	// MPI_Init(NULL, NULL);


	inputParse(argv[1], argv[2]);


	binaryNumber A;
	A.name = 'A';
	binaryNumber B;
	B.name = 'B';
	binaryNumber S;
	S.name = 'S';

	getInputFile(&A, &B);
	printf("============INPUTS=============\n");
	hexToBinary(&A, &B);
	// printHex(&A);
	// printBinary(&A);
	// printHex(&B);
	// printBinary(&B);

	// // printf("\n\n");
	printf("============OUTPUTS=============\n");
	sumCLA(&A, &B, &S);
	
	// // //Uncomment for debugging:
	// // // printOutput(&S);
	// // // sumRipple(&A, &B, &S);
	// // // printOutput(&S);
	// // // printf("\n");

	makeHex(&S);
	// printHex(&S);
	writeOutputFile(&S);



	return EXIT_SUCCESS;
}