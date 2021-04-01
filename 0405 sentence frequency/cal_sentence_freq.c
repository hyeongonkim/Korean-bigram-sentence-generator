#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TOTAL_EUC_KR 94
#define TOTAL_HANGUL 2350

int Cnt[TOTAL_EUC_KR][TOTAL_EUC_KR][TOTAL_EUC_KR][TOTAL_EUC_KR];	// valid for <A1~FE, A1~FE>
int CntBlank[TOTAL_EUC_KR][TOTAL_EUC_KR];	// freq -- syl+blank
int CntBlankStart[TOTAL_EUC_KR][TOTAL_EUC_KR];	// freq -- blank+syl

/*
	입력 데이터 파일을 이용해 바이그램을 생성
	CntBlankStart, CntBlank, Cnt를 이용해 저장해두고 getFreq 함수에서 이용할 것임.
*/
long freqCountBigram(char *fname)
{
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
double getFreq(int i1, int i2, int j1, int j2)
{
	int pI = 0;

	if (i1 != -999 && i2 != -999) {
		for (int j11 = 15; j11 < 40; j11++) {	// 25: B0~C8 으로 축소 가능
			for (int j22 = 0; j22 < TOTAL_EUC_KR; j22++) {
				pI += Cnt[i1][i2][j11][j22];
			}
		}
	}
	
	if (j1 == -999 && j2 == -999) {
		pI += CntBlank[i1][i2];
	} else if (i1 == -999 && i2 == -999) {
		for (int j11 = 15; j11 < 40; j11++) {	// 25: B0~C8 으로 축소 가능
			for (int j22 = 0; j22 < TOTAL_EUC_KR; j22++) {
				pI += CntBlankStart[j11][j22];
			} 
		}
	}
	
	if (i1 == -999 && i2 == -999) {
		return (double) CntBlankStart[j1][j2] / pI;
	} else if (j1 == -999 && j2 == -999) {
		return (double) CntBlank[i1][i2] / pI;
	} else {
		return (double) Cnt[i1][i2][j1][j2] / pI;
	}
}

/*
	각 Bigram의 빈도를 계산하고 토탈에 곱해 문장의 생성 확률을 계산
*/
void calSentenceFreq(char *fname)
{
	int i, len;
	char line[BUFSIZ];
	long n=0;
	FILE *fp;

	fp = fopen(fname, "r");
	
	int i1, i2, j1, j2;

	while (!feof(fp)) {
		fgets(line, BUFSIZ-1, fp);
		len = strlen(line) - 1;

		double totalFreq = 100.0;
		double freq;
		
		for (i = 0; len - i >= 3; ) {
			if ((line[i + 0] & 0x80) == 0) {
				if (line[i + 0] == ' ') {
					j1 = (line[i + 1] & 0x00FF) - 0x00A1;
					j2 = (line[i + 2] & 0x00FF) - 0x00A1;
					freq = getFreq(-999, -999, j1, j2);
					printf("  %c%c Freq: %.10f Percent\n", j1+0xA1, j2+0xA1, freq * 100);
					totalFreq *= freq;
				}
				i++;
				continue;	// ASCII char
			} else if ((line[i + 2] & 0x80) == 0) {
				if (line[i + 2] == ' ') {
					i1 = (line[i + 0] & 0x00FF) - 0x00A1;
					i2 = (line[i + 1] & 0x00FF) - 0x00A1;
					freq = getFreq(i1, i2, -999, -999);
					printf("%c%c   Freq: %.10f Percent\n", i1+0xA1, i2+0xA1, freq * 100);
					totalFreq *= freq;
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
					freq = getFreq(i1, i2, j1, j2);
					printf("%c%c%c%c Freq: %.10f Percent\n", i1+0xA1, i2+0xA1, j1+0xA1, j2+0xA1, freq * 100);
					totalFreq *= freq;
				}
				i += 2;
			}
		}
		printf("Total Freq: %.20f Percent\n\n\n\n", totalFreq);
	}
	fclose(fp);
}

int main(int argc, char *argv[]) {
	long biN;

	if (argc < 3) { puts("C> a.exe corpus.txt target.txt"); return 0; }
	biN = freqCountBigram(argv[1]); // count bigram freq
	printf("Total %ld bigrams are found!\n\n", biN);

	calSentenceFreq(argv[2]);

	return 0;
}
