/*
	Freq. count for Korean syllable bigram.
		Input file -- lines of Korean syllable bigram
		Char. code -- KS C 5601-1987 Hangul/Hanja/2byte-symbol
	Written by Kang Seungshik, nlpkang@gmail.com
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int Cnt[94][94][94][94];	// valid for <A1~FE, A1~FE>
int CntBlank[94][94];	// freq -- syl+blank
int CntBlankStart[94][94];	// freq -- blank+syl

// Print bigram freq. count for KS C 5601-1987 char.s
int put_freq(int min)
{
	int i1, i2, j1, j2;
	int n=0;

	for (i1 = 0; i1 < 94; i1++) {	// 25: B0~C8 으로 축소 가능
	for (i2 = 0; i2 < 94; i2++) {
	for (j1 = 0; j1 < 94; j1++) {	// 25: B0~C8 으로 축소 가능
	for (j2 = 0; j2 < 94; j2++) {
		if (Cnt[i1][i2][j1][j2] > min) {
			printf("%c%c%c%c: %5d\n",
				i1+0xA1, i2+0xA1, j1+0xA1, j2+0xA1,
				Cnt[i1][i2][j1][j2]);
			n++;
		}
	} } } }
	return n;
}

int maxfreq(int i1, int i2, int *j1max, int *j2max)
{
	int j1, j2, maxcnt=0;
	int hbyte=0xB0-0xA1, lbyte=0;	// 2nd syllable -- default '가'

	for (j1 = 15; j1 < 40; j1++) {	// 25: B0~C8 으로 축소 가능
		for (j2 = 0; j2 < 94; j2++) {
			if (Cnt[i1][i2][j1][j2] > maxcnt) {
				hbyte = j1; lbyte = j2;
				maxcnt = Cnt[i1][i2][j1][j2];
			}
		}
	}

	*j1max = hbyte; *j2max = lbyte;
	return maxcnt;
}

int put_freq_max(int min)
{
	int i1, i2, freq;
	int j1max, j2max;
	int n=0;

	for (i1 = 0; i1 < 94; i1++) {	// 25: B0~C8 으로 축소 가능
		for (i2 = 0; i2 < 94; i2++) {
			freq = maxfreq(i1, i2, &j1max, &j2max);
			if (freq > min) {
				printf("%c%c%c%c: %5d\n",
					i1+0xA1, i2+0xA1, j1max+0xA1, j2max+0xA1,
					freq);
				n++;
			}
		}
	}
	return n;
}

void next_syl_generation()
{
	char seed[BUFSIZ];
	int i1, i2, j1, j2;
	int i, n, freq, k=0;

	printf("Start syllable(ex. '가') = ");
	scanf("%s", seed);
	printf("Size(ex. 20) = ");
	scanf("%d", &n); if (n < 0) n = 20;

	i1 = (seed[0] & 0x00FF) - 0x00A1;
	if (i1 < 0 || i1 >= 94) i1 = rand() % 25 + 15;
	i2 = (seed[1] & 0x00FF) - 0x00A1;
	if (i2 < 0 || i2 >= 94) i2 = rand() % 94;

	printf("Sentence = \n\t%c%c\n", i1+0xA1, i2+0xA1);
	for (i=0; i < n; i++) {
		freq = maxfreq(i1, i2, &j1, &j2);
		if (k > 0 && (CntBlank[i1][i2] * k/5) > freq) {
			printf("\t \n");
			k = 0;
		} else {
			printf("\t%c%c\n", j1+0xA1, j2+0xA1);
			k++;
			i1 = j1; i2 = j2;
		}
	}
}

long freq_count_bigram(char *fname)
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

		for (i=0; len-i > 3; ) {
			if ((line[i+0] & 0x80) == 0) {
				if (line[i+0] == ' ') CntBlankStart[i1][i2]++;	// blank + '음절'
				i++;
				continue;	// ASCII char
			} else if ((line[i+2] & 0x80) == 0) {
				if (line[i+2] == ' ') CntBlank[i1][i2]++;	// '음절' + blank
				i += 3;
				continue;	// ASCII char
			} else {
				i1 = (line[i+0] & 0x00FF) - 0x00A1;
				i2 = (line[i+1] & 0x00FF) - 0x00A1;
				j1 = (line[i+2] & 0x00FF) - 0x00A1;
				j2 = (line[i+3] & 0x00FF) - 0x00A1;
				if (i1 < 0 || i2 < 0 || j1 < 0 || j2 < 0) {
					;	// non-KS C 5601 -- cp949 syllables
				} else {
					Cnt[i1][i2][j1][j2]++;
					n++;
				}
				i += 4;
			}
		}
	}
	fclose(fp);
	return n;
}

int main(int argc, char *argv[]) {
	int n;

	if (argc < 2) { puts("C> a.exe corpus.txt"); return 0; }
	n = freq_count_bigram(argv[1]);
	printf("Total %ld bigrams are found!\n", n);

	//n = put_freq(0);	// arg: min. frequency for output
	//n = put_freq_max(0);	// max. frequency only
	//printf("n = %d\n", n);

	next_syl_generation();

	return 0;
}
