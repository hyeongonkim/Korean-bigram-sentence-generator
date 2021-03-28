#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_EUC_KR 94
#define TOTAL_HANGUL 2350

/*
	Unigram과 Bigram을 저장할 수 있는 구조체를 생성
	Unigram: 한글을 두 조각으로 나눈 first, second, 등장 빈도 카운트
	Bigram: 한글을 두 조각으로 나눈 first, second, 등장 빈도 카운트, P(j | i) 빈도
*/
typedef struct _UNI {
	int first, second, cnt;
} UNI;

typedef struct _BI {
	int first, second, cnt;
	double freq;
} BI;

/*
	C에서 제공하는 퀵소트의 구조체 정렬 기준으로 사용하기 위한 정렬 기준
	구조체의 cnt, freq를 이용해 내림차순 정렬
*/
int compareWithCnt(const void *a, const void *b) {
	UNI *aPointer = (UNI *)a;
	UNI *bPointer = (UNI *)b;

	if (aPointer->cnt < bPointer->cnt)
		return 1;
	else if (aPointer->cnt > bPointer->cnt)
		return -1;
	else
		return 0;
}

int compareWithFreq(const void *a, const void *b) {
	BI *aPointer = (BI *)a;
	BI *bPointer = (BI *)b;

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
int CntUni[TOTAL_EUC_KR][TOTAL_EUC_KR];	// valid for <A1~FE, A1~FE>

UNI UniCntStructs[TOTAL_HANGUL];
BI BiFreqStructs[TOTAL_HANGUL];

/*
	입력 데이터 파일을 이용해 바이그램을 생성
	CntBlankStart, CntBlank, Cnt를 이용해 저장해두고 maxFreq 함수에서 이용할 것임.
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

		for (i = 0; len - i > 3; ) {
			if ((line[i + 0] & 0x80) == 0) {
				if (line[i + 0] == ' ') CntBlankStart[i1][i2]++;	// blank + '음절'
				i++;
				continue;	// ASCII char
			} else if ((line[i + 2] & 0x80) == 0) {
				if (line[i + 2] == ' ') CntBlank[i1][i2]++;	// '음절' + blank
				i += 3;
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
				i += 4;
			}
		}
	}
	fclose(fp);
	return n;
}

/*
	입력 데이터 파일을 이용해 유니그램 생성
	2350자 음절의 등장 빈도를 카운트하며, 최초 1회만 호출됨.
	카운트 후 바로 정렬해둬서 사용하기 편리하도록 함.
*/
void countUnigram(char *fname)
{
	char line[BUFSIZ];
	FILE *fp;

	fp = fopen(fname, "r");

	while (!feof(fp)) {
		fgets(line, BUFSIZ-1, fp);
		int len = strlen(line) - 1;

		for (int i = 0; len - i > 0; ) {
			if ((line[i] & 0x80) == 0) {
				i++;
				continue;	// ASCII char
			} else {
				int i1 = (line[i+0] & 0x00FF) - 0x00A1;
				int i2 = (line[i+1] & 0x00FF) - 0x00A1;
				if (i1 < 0 || i2 < 0) {
					;	// non-KS C 5601 -- cp949 syllables
				} else {
					CntUni[i1][i2]++; // Count unigram
				}
				i += 2;
			}
		}
	}
	fclose(fp);

	int i = 0;
	for (int j1 = 15; j1 < 40; j1++) {
		for (int j2 = 0; j2 < TOTAL_EUC_KR; j2++) {
			UniCntStructs[i].first = j1;
			UniCntStructs[i].second = j2;
			UniCntStructs[i].cnt = CntUni[j1][j2];
			i++;
		}
	}
	qsort(UniCntStructs, TOTAL_HANGUL, sizeof(UniCntStructs[0]), compareWithCnt);
}

/*
	음절을 이용해 다음 음절을 예측할 때 사용
	현재 음절을 통해 다음 음절의 등장 빈도를 계산하고
	다음에 등장하는 음절이 아예 없을 경우 유니그램 상위 30개 중 하나를 고르고
	다음에 등장하는 음절이 존재할 경우 상위 3개 중 랜덤으로 선택
*/
int maxFreq(int i1, int i2, int *j1max, int *j2max)
{
	int j1, j2, i = 0;

	for (j1 = 15; j1 < 40; j1++) {	// 25: B0~C8 으로 축소 가능
		for (j2 = 0; j2 < TOTAL_EUC_KR; j2++) {
			BiFreqStructs[i].first = j1;
			BiFreqStructs[i].second = j2;
			BiFreqStructs[i].cnt = Cnt[i1][i2][j1][j2];
			BiFreqStructs[i].freq = (double) Cnt[i1][i2][j1][j2] / CntUni[i1][i2]; // P(j | i) 계산
			i++;
		}
	}

	qsort(BiFreqStructs, TOTAL_HANGUL, sizeof(BiFreqStructs[0]), compareWithFreq); // P(j | i) 정렬, 새로 계산했으므로...

	if (BiFreqStructs[0].cnt < 1) { // 가장 자주 등장하는 것조차 등장한 적 없음
		int exceptionHandleRand = rand() % 30; // 상위 30개에서 랜덤 (문제 조건)
		*j1max = UniCntStructs[exceptionHandleRand].first;
		*j2max = UniCntStructs[exceptionHandleRand].second;
		return UniCntStructs[exceptionHandleRand].cnt;
	} else if (BiFreqStructs[1].cnt < 1) { // 2순위 후보 등장한 적 없음
		*j1max = UniCntStructs[0].first; // 그냥 첫번째 것 선택
		*j2max = UniCntStructs[0].second;
		return UniCntStructs[0].cnt;
	} else if (BiFreqStructs[2].cnt < 1) { // 3순위 후보 등장한 적 없음
		int next_rand = rand() % 2; // 상위 2개에서 랜덤
		*j1max = BiFreqStructs[next_rand].first;
		*j2max = BiFreqStructs[next_rand].second;
		return BiFreqStructs[next_rand].cnt;
	} else {
		int next_rand = rand() % 3; // 상위 3개에서 랜덤 (문제 조건)
		*j1max = BiFreqStructs[next_rand].first;
		*j2max = BiFreqStructs[next_rand].second;
		return BiFreqStructs[next_rand].cnt;
	}
}

/*
	문장을 생성하기 위해 사용
	시작 음절의 빈도 스코프를 받고(3~10) 이 스코프 사이에서 정렬된 유니그램 값 중 랜덤으로 시작 음절을 선택
	이때 블랙리스트에 있는 음절은 제외됨(종결 어미, 워드 연결에 주로 사용되는 음절들)
*/
void nextSylGeneration()
{
	int startRandScope;
	printf("Input number between 3, 10 > ");
	scanf("%d", &startRandScope);
	if (startRandScope < 3) startRandScope = 3; // scope fallback logic
	if (startRandScope > 10) startRandScope = 10;

	int i1, i2, j1, j2;
	int n, freq, k=0;

	printf("\n\n");

	// 시작 후보군 검사
	int addedSymbol = 0; 
	int checkIdx = 0;
	int startSymbols[startRandScope][2];
	while (addedSymbol < startRandScope) {
		// 시작 음절로 적합하지 않은 음절 블랙리스트 검사 (의, 에, 은, 는, 이, 가, 다, 을, 를)
		int first = UniCntStructs[checkIdx].first;
		int second = UniCntStructs[checkIdx].second;
		checkIdx++;
		if (
			(first == 31 && second == 38) // 의
			|| (first == 30 && second == 0) // 에
			|| (first == 31 && second == 25) // 은
			|| (first == 19 && second == 33) // 는 
			|| (first == 31 && second == 43) // 이
			|| (first == 15 && second == 0) // 가
			|| (first == 19 && second == 56) // 다
			|| (first == 31 && second == 26) // 을
			|| (first == 23 && second == 5) // 를
		) {
			continue;
		} else {
			startSymbols[addedSymbol][0] = first;
			startSymbols[addedSymbol][1] = second;
			printf("Index %d: %c%c\n ", addedSymbol, startSymbols[addedSymbol][0]+0xA1, startSymbols[addedSymbol][1]+0xA1);
			addedSymbol++;
		}
	}

	int rand_n = rand() % startRandScope; // 랜덤하게 시작 심볼 선택
	printf("\nIndex %d selected.\n\n", rand_n);

	i1 = startSymbols[rand_n][0];
	i2 = startSymbols[rand_n][1];

	printf("Sentence = \n\t%c%c\n", i1+0xA1, i2+0xA1);
	int i = 0;
	while (1) {
		i++;
		freq = maxFreq(i1, i2, &j1, &j2);
		// 문장이 10글자 이상 형성되었고,
		// 다, 야, 어로 끝날 경우 문장 종결 (3개 음절 합산 종결 어미 비율: 28.67%)
		// (Ref. https://github.com/songys/spoken_ending_form)
		if(i > 10 && (
				j1 == 19 && j2 == 56) // 다
				|| (j1 == 29 && j2 == 62) // 야
				|| (j1 == 29 && j2 == 77) // 어
			) {
			printf("\t%c%c\n.", j1+0xA1, j2+0xA1);
			break;
		} else if (k > 0 && (CntBlank[i1][i2] * k/5) > freq) {
			printf("\t \n");
			k = 0;
		} else {
			printf("\t%c%c\n", j1+0xA1, j2+0xA1);
			k++;
			i1 = j1; i2 = j2;
		}
	}
}

int main(int argc, char *argv[]) {
	long biN;
	long uniN;

	srand(time(NULL)); // time을 시드로 넣어 랜덤값 형성

	if (argc < 2) { puts("C> a.exe corpus.txt"); return 0; }
	biN = freqCountBigram(argv[1]); // count bigram freq
	printf("Total %ld bigrams are found!\n", biN);

	countUnigram(argv[1]); // count unigram

	nextSylGeneration(); // gen syl by unigram freq

	return 0;
}
