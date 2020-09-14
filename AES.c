// AES 구현
// 

#include<stdio.h>
#include<stdlib.h>
// in : 암호화 혹은 복호화될 문장을 저장하는 배열
// out : 암호화 혹은 복호화된 문장을 저장하는 배열
// state : 암호화 혹은 복호화 라운드 중간값을 저장하는 배열
unsigned char in[16], out[16], state[4][4];
// 확장된 키를 저장하는 배열
unsigned char RoundKey[180];
// Key
unsigned char Key[16];

unsigned char sbox[256];

//Inverse S-box
unsigned char Isbox[256];

unsigned char RC[10] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x4D,0x9A};


//Make sbox
#define ROTL8(x,shift) ((unsigned char) ((x) << (shift)) | ((x) >> (8 - (shift))))
void MakeSbox() {
	unsigned char p = 1, q = 1;

	//loop invariant: p * q == 1 in the Galois field 
	do {
		// multiply p by 3 
		p = p ^ (p << 1) ^ (p & 0x80 ? 0x4d : 0);

		//divide q by 3 (equals multiplication by 0xf6) 
		q ^= q << 1;
		q ^= q << 2;
		q ^= q << 4;
		q ^= q & 0x80 ? 0x3b : 0;//9

		// compute the affine transformation 
		unsigned char xformed = q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);

		sbox[p] = xformed ^ 0x15;
	} while (p != 1);

	// 0 is a special case since it has no inverse 
	sbox[0] = 0x15;
}

//S-box를 이용하여 inverse S-box 생성
void MakeInverseSbox() {
	int temp, row, col;
	for (int i = 0; i < 256; i++) {
		temp = sbox[i];
		col = temp & 0x0F;
		row = temp >> 4;
		Isbox[row * 16 + col] = i;
	}
	return;
}

//key 확장 함수
void KeyExpansion()
{
	printf("KEY EXPANSION\n");
	int i, j;
	unsigned char temp[4], k;
	// 처음 라운드키 생성
	printf("ROUND 0: ");
	for (i = 0; i < 4; i++)
	{
		RoundKey[i * 4] = Key[i * 4];
		RoundKey[i * 4 + 1] = Key[i * 4 + 1];
		RoundKey[i * 4 + 2] = Key[i * 4 + 2];
		RoundKey[i * 4 + 3] = Key[i * 4 + 3];
		printf("%.2X %.2X %.2X %.2X ", RoundKey[i * 4], RoundKey[i * 4 + 1], RoundKey[i * 4 + 2], RoundKey[i * 4 + 3]);
	}
	while (i < 44 )
	{
		for (j = 0; j < 4; j++)
			temp[j] = RoundKey[(i - 1) * 4 + j];
		
		//R function
		if (i % 4 == 0)
		{
			printf("\nROUND %d: ", i / 4);
			k = temp[0];
			temp[0] = temp[1];
			temp[1] = temp[2];
			temp[2] = temp[3];
			temp[3] = k;

			temp[0] = sbox[temp[0]];
			temp[1] = sbox[temp[1]];
			temp[2] = sbox[temp[2]];
			temp[3] = sbox[temp[3]];

			temp[0] = temp[0] ^ RC[(i / 4) - 1];
		}
		RoundKey[i * 4 + 0] = RoundKey[(i - 4) * 4 + 0] ^ temp[0];
		RoundKey[i * 4 + 1] = RoundKey[(i - 4) * 4 + 1] ^ temp[1];
		RoundKey[i * 4 + 2] = RoundKey[(i - 4) * 4 + 2] ^ temp[2];
		RoundKey[i * 4 + 3] = RoundKey[(i - 4) * 4 + 3] ^ temp[3];
		printf("%.2X %.2X %.2X %.2X ", RoundKey[i * 4], RoundKey[i * 4 + 1], RoundKey[i * 4 + 2], RoundKey[i * 4 + 3]);
		i++;
	}
}
// round key를 state에 더하는(XOR) 함수
void AddRoundKey(int round)
{
	printf("AR: ");
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			state[j][i] ^= RoundKey[round * 16 + i * 4+ j];

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			printf("%.2X ", state[j][i]);
	printf("\n");
}
// state 의 값을 sbox를 이용하여 치환하는 함수
void SubBytes()
{
	printf("SB: ");
	int i, j;
	for (i = 0; i < 4; i++) 
		for (j = 0; j < 4; j++) 
			state[i][j] = sbox[state[i][j]];
	
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			printf("%.2X ", state[j][i]);
	printf("\n");
}
// state 의 값을 Inverse sbox를 이용하여 치환하는 함수
void InvSubBytes()
{
	printf("SB: ");
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			state[i][j] = Isbox[state[i][j]];
	
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			printf("%.2X ", state[j][i]);
	printf("\n");
}

// state의 행마다 각각 left shift를 한다.
void ShiftRows()
{
	printf("SR: ");
	unsigned char temp;
	//  first row 1 columns to left
	temp = state[1][0];
	state[1][0] = state[1][1];
	state[1][1] = state[1][2];
	state[1][2] = state[1][3];
	state[1][3] = temp;
	//  second row 2 columns to left
	temp = state[2][0];
	state[2][0] = state[2][2];
	state[2][2] = temp;
	temp = state[2][1];
	state[2][1] = state[2][3];
	state[2][3] = temp;
	//  third row 3 columns to left
	temp = state[3][0];
	state[3][0] = state[3][3];
	state[3][3] = state[3][2];
	state[3][2] = state[3][1];
	state[3][1] = temp;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			printf("%.2X ", state[j][i]);
	printf("\n");
}

// 역의 계산을 위해 state의 행마다 각각 right shift를 한다.
void InvShiftRows()
{
	printf("SR: ");
	unsigned char temp;
	// first row 1 columns to right
	temp = state[1][3];
	state[1][3] = state[1][2];
	state[1][2] = state[1][1];
	state[1][1] = state[1][0];
	state[1][0] = temp;

	// second row 2 columns to right
	temp = state[2][0];
	state[2][0] = state[2][2];
	state[2][2] = temp;

	temp = state[2][1];
	state[2][1] = state[2][3];
	state[2][3] = temp;

	// third row 3 columns to right
	temp = state[3][0];
	state[3][0] = state[3][1];
	state[3][1] = state[3][2];
	state[3][2] = state[3][3];
	state[3][3] = temp;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			printf("%.2X ", state[j][i]);
	printf("\n");
}
// xtime 은 입력값을 1회 left shift하여 modular 연산을 한다.
#define xtime(x)   ((x<<1) ^ (((x>>7) & 1) * 0x4d))
// Multiply 는 xtime을 이용하여 x의 y배를 modular 연산을 한다. 
#define Multiply(x,y) \
( ((y & 1) * x) \
^ ((y>>1 & 1) * xtime(x)) \
^ ((y>>2 & 1) * xtime(xtime(x))) \
^ ((y>>3 & 1) * xtime(xtime(xtime(x)))) \
^ ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))))

// Encryotion 과정에서 state의 열을 섞는 함수
void MixColumns()
{
	printf("MC: ");
	int i;
	unsigned char a, b, c, d;
	for (i = 0; i < 4; i++)
	{
		a = state[0][i];
		b = state[1][i];
		c = state[2][i];
		d = state[3][i];

		state[0][i] = Multiply(a, 0x02) ^ Multiply(b, 0x03) ^ c ^ d ;
		state[1][i] = a ^ Multiply(b, 0x02) ^ Multiply(c, 0x03) ^ d ;
		state[2][i] = a ^  b ^ Multiply(c, 0x02) ^ Multiply(d, 0x03);
		state[3][i] = Multiply(a, 0x03) ^ b ^ c  ^ Multiply(d, 0x02);
	}

	for (i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			printf("%.2X ", state[j][i]);
	printf("\n");
}

//Decryption 과정에서 state의 열을 섞는 함수
void InvMixColumns()
{
	printf("MC: ");
	int i;
	unsigned char a, b, c, d;
	for (i = 0; i < 4; i++)
	{
		a = state[0][i];
		b = state[1][i];
		c = state[2][i];
		d = state[3][i];

		state[0][i] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
		state[1][i] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
		state[2][i] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
		state[3][i] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
	}
	for (i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			printf("%.2X ", state[j][i]);
	printf("\n");
};
// 암호화 함수
void Encrypt()
{
	printf("\n\n <------ENCRYPTION------>\n\n");
	int i, j, round = 0;
	//Copy the input PlainText to state array.
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			state[j][i] = in[i * 4 + j];

	// Add the First round key to the state.
	printf("ROUND 0\n");
	AddRoundKey(0);
	printf("\n");
	//rounds loop.
	for (round = 1; round < 10; round++)
	{
		printf("ROUND %d\n", round);
		SubBytes();
		ShiftRows();
		MixColumns();
		AddRoundKey(round);
		printf("\n");
	}
	// The last is given below.
	printf("ROUND 10\n");
	SubBytes();
	ShiftRows();
	AddRoundKey(10);
	
	// Copy the state array to output array.
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			out[i * 4 + j] = state[j][i];
}


//복호화 함수
void Decrypt()
{
	int i, j, round = 0;
	printf("\n\n <------DECRYPTION------>\n\n");
	//Copy the input CipherText to state array.
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			state[j][i] = in[i * 4 + j];

	// Add the First round key to the state.
	printf("ROUND 0\n");
	AddRoundKey(10);
	printf("\n");
	// rounds loop
	for (round = 9; round > 0; round--)
	{
		printf("ROUND %d\n", 10-round);
		InvShiftRows();
		InvSubBytes();
		AddRoundKey(round);
		InvMixColumns();
		printf("\n");
	}
	// The last round is given below.
	printf("ROUND 10\n");
	InvShiftRows();
	InvSubBytes();
	AddRoundKey(0);

	// Copy the state array to output array.
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			out[i * 4 + j] = state[j][i];
	
}
void main()
{
	int i;
	//make S-box
	MakeSbox();
	//make Inverse S- box;
	MakeInverseSbox();

	//read key.bin 
	FILE* stream;
	fopen_s(&stream, "key.bin", "rb");
	if (stream == NULL) {
		printf("No key file\n");
		system("pause");
		return;
	}
	fread(&Key, sizeof(unsigned char), 16, stream);
	fclose(stream);
	//read plain.bin
	fopen_s(&stream, "plain.bin", "rb");
	if (stream == NULL) {
		printf("No plain text file\n");
		system("pause");
		return;
	}
	fread(&in, sizeof(unsigned char), 16, stream);
	fclose(stream);
	
	//print initial condition
	printf("RC: ");
	for( i = 0 ; i < 10 ; i++)
		printf("%.2X ", RC[i]);
	printf("\nPLAIN: ");
	for (i = 0; i < 16; i++)
		printf("%.2X ", in[i]);
	printf("\nKEY: ");
	for (i = 0; i < 16; i++)
		printf("%.2X ", Key[i]);
	printf("\n\n");
	
	//excute KeyExpansion 
	KeyExpansion();
	// encrypts the plainText using AES
	Encrypt();
	
	// print encrypted text.
	printf("\nCIPHER: ");
	for (i = 0; i < 16; i++) 
		printf("%.2X ", out[i]);
	printf("\n\n");
	
	//write cipher.bin
	fopen_s(&stream, "cipher.bin", "wb");
	for (int i = 0; i < (sizeof(out) / sizeof(out[0])); i++) {
		if (fputc(out[i], stream) == EOF) {
			printf("err!");
			return;
		}
	}
	fclose(stream);

	
	for (i = 0; i < 16; i++)
		in[i] = out[i];
	// dencrypts the chiperText using AES
	Decrypt();

	//print decrypted text
	printf("\nDECRYPTED: ");
	for (i = 0; i < 16; i++)
		printf("%.2X ", out[i]);
	printf("\n");

	//write decrypt.bin
	fopen_s(&stream, "decrypt.bin", "wb");
	for (int i = 0; i < (sizeof(out) / sizeof(out[0])); i++) {
		if (fputc(out[i], stream) == EOF) {
			printf("err!");
			return;
		}
	}
	fclose(stream);

	//end
	system("pause");
	return;
}

