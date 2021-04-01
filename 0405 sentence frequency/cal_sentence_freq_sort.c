#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TOTAL_EUC_KR 94
#define TOTAL_HANGUL 2350

/*
	String과 Freq를 저장할 수 있는 구조체를 생성
*/
typedef struct _STR {
	char *str;
	double freq;
} STR;

int compareWithFreq(const void *a, const void *b) {
	STR *aPointer = (STR *)a;
	STR *bPointer = (STR *)b;

	if (aPointer->freq < bPointer->freq)
		return 1;
	else if (aPointer->freq > bPointer->freq)
		return -1;
	else
		return 0;
}

int Cnt[TOTAL_EUC_KR][TOTAL_EUC_KR][TOTAL_EUC_KR][TOTAL_EUC_KR];	// valid for <A1~FE, A1~FE>
int CntBlank[TOTAL_EUC_KR][TOTAL_EUC_KR];	// freq -- syl+blank
int CntBlankStart[TOTAL_EUC_KR][TOTAL_EUC_KR];	// freq -- blank+syl

/*
	입력 데이터 파일을 이용해 바이그램을 생성
	CntBlankStart, CntBlank, Cnt를 이용해 저장해두고 getFreq 함수에서 이용할 것임.
*/
long freqCountBigram(char *fname) {
	int i, len;
	int i1, i2, j1, j2;
	char line[BUFSIZ];
	long n=0;
	FILE *fp;

	fp = fopen(fname, "r");

	while (!feof(fp)) {
		fgets(line, BUFSIZ-1, fp);
		len = strlen(line) - 1;

		for (i = 0; len - i >= 3; ) {
			if ((line[i + 0] & 0x80) == 0) {
				if (line[i + 0] == ' ') {
					j1 = (line[i + 1] & 0x00FF) - 0x00A1;
					j2 = (line[i + 2] & 0x00FF) - 0x00A1;
					CntBlankStart[j1][j2]++;	// blank + '음절'
				}
				i++;
				continue;	// ASCII char
			} else if ((line[i + 2] & 0x80) == 0) {
				if (line[i + 2] == ' ') {
					i1 = (line[i + 0] & 0x00FF) - 0x00A1;
					i2 = (line[i + 1] & 0x00FF) - 0x00A1;
					CntBlank[i1][i2]++;	// '음절' + blank
				}
				i += 2;
				continue;	// ASCII char
			} else {
				i1 = (line[i + 0] & 0x00FF) - 0x00A1;
				i2 = (line[i + 1] & 0x00FF) - 0x00A1;
				j1 = (line[i + 2] & 0x00FF) - 0x00A1;
				j2 = (line[i + 3] & 0x00FF) - 0x00A1;
				if (i1 < 0 || i2 < 0 || j1 < 0 || j2 < 0) {
					;	// non-KS C 5601 -- cp949 syllables
				} else {
					Cnt[i1][i2][j1][j2]++;
					n++;
				}
				i += 2;
			}
		}
	}
	fclose(fp);
	return n;
}

/*
	두 음절을 입력받아 P(j | i)를 계산해서 리턴
*/
double getFreq(int i1, int i2, int j1, int j2) {
	int pI = 0;

	if (i1 != '-' && i2 != '-') {
		for (int j11 = 15; j11 < 40; j11++) {	// 25: B0~C8 으로 축소 가능
			for (int j22 = 0; j22 < TOTAL_EUC_KR; j22++) {
				pI += Cnt[i1][i2][j11][j22];
			}
		}
	}
	
	if (j1 == '-' && j2 == '-') {
		pI += CntBlank[i1][i2];
	} else if (i1 == '-' && i2 == '-') {
		for (int j11 = 15; j11 < 40; j11++) {	// 25: B0~C8 으로 축소 가능
			for (int j22 = 0; j22 < TOTAL_EUC_KR; j22++) {
				pI += CntBlankStart[j11][j22];
			} 
		}
	}
	
	if (i1 == '-' && i2 == '-') {
		return (double) CntBlankStart[j1][j2] / pI;
	} else if (j1 == '-' && j2 == '-') {
		return (double) CntBlank[i1][i2] / pI;
	} else {
		return (double) Cnt[i1][i2][j1][j2] / pI;
	}
}

/*
	각 Bigram의 빈도를 계산하고 토탈에 곱해 문장의 생성 확률을 계산, 빈도가 높은 문장부터 출력
*/
void calSentenceFreq(char *fname) {
	int i, len;
	char line[BUFSIZ];
	long n=0;
	FILE *fp;

	fp = fopen(fname, "r");
	
	int i1, i2, j1, j2;

	int totalLine = 0;
	
	while (!feof(fp)) {
		fgets(line, BUFSIZ-1, fp);
		totalLine++;
	}

	STR StrStructs[totalLine];

	fclose(fp);

	fp = fopen(fname, "r");

	int idx = 0;
	while (!feof(fp)) {
		fgets(line, BUFSIZ-1, fp);
		len = strlen(line) - 1;

		StrStructs[idx].str = malloc(sizeof(char) * BUFSIZ);

		double totalFreq = 100.0;
		double freq;
		
		for (i = 0; len - i >= 3; ) {
			if ((line[i + 0] & 0x80) == 0) {
				if (line[i + 0] == ' ') {
					StrStructs[idx].str[i + 0] = line[i + 0];
					StrStructs[idx].str[i + 1] = line[i + 1];
					StrStructs[idx].str[i + 2] = line[i + 2];

					j1 = (line[i + 1] & 0x00FF) - 0x00A1;
					j2 = (line[i + 2] & 0x00FF) - 0x00A1;
					totalFreq *= getFreq('-', '-', j1, j2);
				}
				i++;
				continue;	// ASCII char
			} else if ((line[i + 2] & 0x80) == 0) {
				if (line[i + 2] == ' ') {
					StrStructs[idx].str[i + 0] = line[i + 0];
					StrStructs[idx].str[i + 1] = line[i + 1];
					StrStructs[idx].str[i + 2] = line[i + 2];

					i1 = (line[i + 0] & 0x00FF) - 0x00A1;
					i2 = (line[i + 1] & 0x00FF) - 0x00A1;
					totalFreq *= getFreq(i1, i2, '-', '-');
				}
				i += 2;
				continue;	// ASCII char
			} else {
				StrStructs[idx].str[i + 0] = line[i + 0];
				StrStructs[idx].str[i + 1] = line[i + 1];
				StrStructs[idx].str[i + 2] = line[i + 2];
				StrStructs[idx].str[i + 3] = line[i + 3];

				i1 = (line[i + 0] & 0x00FF) - 0x00A1;
				i2 = (line[i + 1] & 0x00FF) - 0x00A1;
				j1 = (line[i + 2] & 0x00FF) - 0x00A1;
				j2 = (line[i + 3] & 0x00FF) - 0x00A1;
				if (i1 < 0 || i2 < 0 || j1 < 0 || j2 < 0) {
					;	// non-KS C 5601 -- cp949 syllables
				} else {
					totalFreq *= getFreq(i1, i2, j1, j2);
				}
				i += 2;
			}
		}

		StrStructs[idx].freq = totalFreq;
		idx++;
	}
	fclose(fp);

	qsort(StrStructs, totalLine, sizeof(StrStructs[0]), compareWithFreq);

	for (i = 0; i < totalLine; i++) {
		len = strlen(StrStructs[i].str);
		for (int k = 0; len - k >= 1; ) {
			if ((StrStructs[i].str[k + 0] & 0x80) == 0) {
				if (StrStructs[i].str[k + 0] == ' ') {
					printf(" ");
				}
				k++;
			} else {
				i1 = (StrStructs[i].str[k + 0] & 0x00FF) - 0x00A1;
				i2 = (StrStructs[i].str[k + 1] & 0x00FF) - 0x00A1;
				printf("%c%c", i1+0xA1, i2+0xA1);
				k += 2;
			}
		}
		free(StrStructs[i]);
		printf("\nFreq: %.20f Percent\n\n", StrStructs[i].freq);
	}
	
}

int main(int argc, char *argv[]) {
	long biN;

	if (argc < 3) { puts("C> a.exe corpus.txt target.txt"); return 0; }
	biN = freqCountBigram(argv[1]); // count bigram freq
	printf("Total %ld bigrams are found!\n\n", biN);

	calSentenceFreq(argv[2]);

	return 0;
}
