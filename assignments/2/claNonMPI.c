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
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>

//262144
#define n 262144/6
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
	for(int i = 0; i < n/4; ++i){
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
	int* s;
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
	s = calloc(n, sizeof(int));
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
	debugBinary(g[0], n/numFiles, 'G');
	debugBinary(p[0], n/numFiles, 'P');

	//Calculate all group generates and group propogates:
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
					g[i][j] |= rollP & g[i-1][ind + blockSize - k - 2];
				}
			}
		}
		debugBinary(g[i], (int)pow(blockSize, reqBlocks - i), 'G');
		debugBinary(p[i], (int)pow(blockSize, reqBlocks - i), 'P');
	}
	//Calculate C[0]:
	//TODO: Check carryin from other MPI threads?
	c[0][0] = g[reqBlocks - 1][0] | (p[reqBlocks - 1][0] & 0);
	c[reqBlocks - 1][0] = g[0][0] | (p[0][0] & 0);
	for(int i = 1; i <= blockSize; ++i){
		c[0][i] = g[reqBlocks - 1][i] | (p[reqBlocks - 1][i] & c[0][i-1]);
	}
	debugBinary(c[0], blockSize, 'C');
	//Collapse all group generates and group propogates:
	for(int i = 1; i < reqBlocks; ++i){
		for(int j = 0; j < (int)pow(blockSize, i + 1); ++j){
			int ind = j * blockSize;
			//c[reqBlocks - i][j] = 1;
			if(j % blockSize == 0 && j != 0 && i != reqBlocks - 1){
				//c[reqBlocks - i][j] = 1;
				// printf("%d|", c[reqBlocks - i - 1][j/blockSize]);
			 c[reqBlocks - i][j] = c[reqBlocks - i - 1][j/blockSize];
			}
			else{
			//c[reqBlocks - i][j] = 2;
			// printf("%d|", g[reqBlocks - i][j]);
			c[reqBlocks - i][j] = g[i - 1][j-1] | (p[i - 1][j-1] & c[reqBlocks - i][j - 1]);
			}
		}
		debugBinary(c[reqBlocks - i], (int)pow(blockSize, i + 1), 'C');
	}

	// Calculate S[i]:
	//TODO CHECK CARRY IN
	s[0] = xor(xor(A->data[0], B->data[0]), 0);
	for(int i = 1; i < n/numFiles; ++i){
		s[i] = xor(xor(A->data[i], B->data[i]), c[reqBlocks - 1][i]);
	}
	debugBinary(s, n/numFiles, 'S');



	//Compute sum:
	// //Calculate all GG[j] PG[j];
	// int PG[n/blockSize] = {0};
	// int GG[n/blockSize] = {0};
	// for(int j = 0; j < n/blockSize; ++j){
	// 	int i = j * blockSize;
	// 	int rollP = 1;
	// 	GG[j] = G[i + blockSize - 1];
	// 	PG[j] = P[i];
	// 	for(int k = 0; k < blockSize; ++k){
	// 		if(k > 0){
	// 			PG[j] &= P[i + k];
	// 			rollP &= P[i + (blockSize - k - 1)];
	// 			GG[j] |= rollP & G[i + blockSize - k - 2];
	// 		}
	// 	}
	// 	//GG[j] = G[i + 3] | (P[i + 3] & G[i + 2]) | (P[i + 3] & P[i + 2] & G[i + 1]) | (P[i + 3] & P[i + 2] & P[i + 1] & G[i + 0]);
	// }

	// // debugBinary(&GG[0], n/blockSize, 'g');
	// // debugBinary(&PG[0], n/blockSize, 'p');

	// //Calculate all SG[k] and SP[k]
	// int SG[blockSize] = {0};
	// int SP[blockSize] = {0};
	// for(int k = 0; k < blockSize; ++k){
	// 	int j = k * blockSize;

	// 	int rollP = 1;
	// 	SG[k] = GG[j + blockSize - 1];
	// 	SP[k] = PG[j];
	// 	for(int l = 0; l < blockSize; ++l){
	// 		if(l > 0){
	// 			SP[k] &= PG[j + l];
	// 			rollP &= PG[j + (blockSize - l - 1)];
	// 			SG[k] |= rollP & GG[j + blockSize - l - 2];
	// 		}
	// 	}
	// }
	// // debugBinary(&SG[0], blockSize, 'g');
	// // debugBinary(&SP[0], blockSize, 'p');


	// //Calculate all SC[k]
	// int SC[blockSize] = {0};
	// SC[0] = SG[0] | (SP[0] & 0);
	// for(int k = 1; k < blockSize; ++k){
	// 	SC[k] = SG[k] | (SP[k] & SC[k-1]);
	// }
	// // debugBinary(&SC[0], 4, 'C');

	// //Calculate all GC[j]:
	// int k = 0;
	// int GC[n/blockSize] = {0};
	// GC[0] = GG[0];
	// for(int j = 1; j < n/blockSize; ++j){
	// 	if(j % blockSize == 0){
	// 		GC[j] = GG[k] | (PG[j] & SC[j/blockSize]);
	// 		++k;
	// 	}
	// 	else{
	// 		GC[j] = GG[k] | (PG[j] & GC[j-1]);
	// 	}
		
	// }
	// // debugBinary(&GC[0], 16, 'C');
	// //Calculate C[i]:
 //  k = 0;
	// int C[n + 1] = {0};
	// C[0] = GC[0];
	// for(int j = 0; j < n; ++j){
	// 	if((j) % blockSize == 0){
	// 		C[j + 1] = G[j] | (P[j] & GC[j/blockSize]);
	// 		++k;
	// 	}
	// 	else{
	// 		C[j + 1] = G[j] | (P[j] & C[j]);
	// 	}
	// }

	// // debugBinary(&C[0], n+1, 'C');
	// //Calculate S[i]:
	// S->data[0] = xor(xor(A->data[0], B->data[0]), 0);
	// for(int i = 1; i < n; ++i){
	// 	S->data[i] = xor(xor(A->data[i], B->data[i]), C[i]);
	// }

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
	char* filename = calloc(14, sizeof(char));
	int filesPerRank = numFiles/numRanks;

	char* buffer1 = calloc(hexSize/numFiles + 1, sizeof(char));
	char* buffer2 = calloc(hexSize/numFiles + 1, sizeof(char));


	int index = 0;
	FILE* fp;
	for(int i = 0; i < filesPerRank; ++i){
		sprintf(filename, "inputs/input.%d\0", i);
		fp = fopen(filename, "r");
		fgets(buffer1, hexSize/numFiles + 1, fp);
		fseek(fp, 1, SEEK_CUR);
		fgets(buffer2, hexSize/numFiles + 1, fp);

		for(int j = 0; j < hexSize; ++j){
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
	char* splitInName = calloc(10, sizeof(char));
	for(int index = 0; index < numFiles; ++index){
		fgets(buffer1, hexSize/numFiles + 1, fp1);
		fgets(buffer2, hexSize/numFiles + 1, fp2);
		mkdir("inputs", 0700);
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
		// for(int i = j; i != -1; --i){
		// 	fputc(buffer1[i], fo1);
		// }
		// fputc('\n', fo1);
		// for(int i = j; i != -1; --i){
		// 	fputc(buffer2[i], fo1);
		// }
		fclose(fo1);
	}
	fclose(fp1);
	fclose(fp2);	
}

int main(int argc, char* argv[]){
	//Read input files and break into seperate files:
	//should look like the following:
	//
	//	F1		F2		F3		F4		FX
	//	xxxx	xxxx	xxxx	xxxx	...
	//	yyyy	yyyy	yyyy	yyyy	...
	//

	clock_t begin = clock();

	inputParse(argv[1], argv[2]);


	binaryNumber A;
	A.name = 'A';
	binaryNumber B;
	B.name = 'B';
	binaryNumber S;

	getInputFile(&A, &B);
	printf("============INPUTS=============\n");
	hexToBinary(&A, &B);
	printHex(&A);
	printBinary(&A);
	printHex(&B);
	printBinary(&B);

	// printf("\n\n");
	printf("============OUTPUTS=============\n");
	sumCLA(&A, &B, &S);
	

	// //Uncomment for debugging:
	// // printOutput(&S);
	// // sumRipple(&A, &B, &S);
	// // printOutput(&S);
	// // printf("\n");

	// makeHex(&S);
	// printHex(&S);
	clock_t end = clock();
	printf("TIME: %f\n", (double)(end - begin)/CLOCKS_PER_SEC);


	return EXIT_SUCCESS;
}