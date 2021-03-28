/*
	Freq. count for Korean syllable bigram.
		Input file -- lines of Korean syllable bigram
		Char. code -- KS C 5601-1987 Hangul/Hanja/2byte-symbol
	Usage: C> a.exe bigram.txt
		bigram.txt -- 한 라인에 음절 bigram 1개씩 (KS 완성형 한글코드)
		C> get-ngram -2 < corpus.txt > bigram.txt
	Written by Kang Seungshik, nlpkang@gmail.com
*/
#include <stdio.h>

unsigned int Cnt[94][94][94][94];	// valid for <A1~FE, A1~FE>
double P[94][94][94][94];	// P(j|i) = freq(i,j) / freq(i)

// Print bigram freq. count for KS C 5601-1987 char.s
int put_freq(int min)
{
	int i1, i2, j1, j2;
	int n=0;

	for (i1 = 0; i1 < 94; i1++) {	// 25: B0~C8 으로 축소 가능
	for (i2 = 0; i2 < 94; i2++) {
	for (j1 = 0; j1 < 94; j1++) {	// 25: B0~C8 으로 축소 가능
	for (j2 = 0; j2 < 94; j2++) {
		if (Cnt[i1][i2][j1][j2] >= min) {
			printf("%c%c%c%c : %5d\n",
				i1+0xA1, i2+0xA1, j1+0xA1, j2+0xA1,
				Cnt[i1][i2][j1][j2]);
			n++;
		}
	} } } }

	return n;
}

int maxfreq(int i1, int i2, int *j1max, int *j2max)
{
	int j1, j2, maxCnt=0;
	int hbyte, lbyte;	// 2nd syllable

	for (j1 = 0; j1 < 94; j1++) {	// 25: B0~C8 으로 축소 가능
		for (j2 = 0; j2 < 94; j2++) {
			if (Cnt[i1][i2][j1][j2] > maxCnt) {
				hbyte = j1; lbyte = j2;
				maxCnt = Cnt[i1][i2][j1][j2];
			}
		}
	}

	*j1max = hbyte; *j2max = lbyte;
	return maxCnt;
}

int put_freq_max(int min)
{
	int i1, i2, maxCnt;
	int j1max, j2max;
	int n=0;

	for (i1 = 0; i1 < 94; i1++) {	// 25: B0~C8 으로 축소 가능
		for (i2 = 0; i2 < 94; i2++) {
			maxCnt = maxfreq(i1, i2, &j1max, &j2max);
			if (maxCnt >= min) {
				printf("%c%c%c%c : %5d\n",
					i1+0xA1, i2+0xA1, j1max+0xA1, j2max+0xA1,
					maxCnt);
				n++;
			}
		}
	}
	return n;
}

int main(int argc, char *argv[]) {
	char line[BUFSIZ];
	int i1, i2, j1, j2, n;
	FILE *fp;

	if (argc < 2) { puts("C> a.exe bigram.txt"); return 0; }
	fp = fopen(argv[1], "r");

	while (!feof(fp)) {
		fgets(line, BUFSIZ-1, fp);
		if ((line[0] & 0x80) == 0) continue;	// ASCII char
		if ((line[2] & 0x80) == 0) continue;	// ASCII char

		i1 = (line[0] & 0x00FF) - 0x00A1;
		i2 = (line[1] & 0x00FF) - 0x00A1;
		j1 = (line[2] & 0x00FF) - 0x00A1;
		j2 = (line[3] & 0x00FF) - 0x00A1;
		if (i1 < 0 || i2 < 0 || j1 < 0 || j2 < 0) continue;
		if (i1 > 93 || i2 > 93 || j1 > 93 || j2 > 93) continue;

		Cnt[i1][i2][j1][j2]++;
		//puts(line); getchar();
	}

	//n = put_freq(1);	// arg: min. frequency for output
	n = put_freq_max(1);	// max. frequency only
	fprintf(stderr, "n = %d\n", n);

	fclose(fp);
	return 0;
}
