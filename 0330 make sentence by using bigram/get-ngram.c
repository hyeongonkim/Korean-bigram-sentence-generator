/*
 * 텍스트 파일에서 "character ngram" 추출 프로그램
 *	C> a.exe -n < input.txt > output.txt
 *		(n = 1, 2, 3, 4, ...)
 *	<참고> 공백문자는 '_'로 대치, 첫끝 문자 추가(SS='^', SE='$')
 *	<주의> 한글 텍스트는 "KS 완성형"이어야 함!
 * 국민대학교 소프트웨어학부 강승식 (nlpkang@gmail.com)
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX	8192

// space -> '_' && sent-ending -> '$'
int set_blank2underbar(char *p, int n) {
	int i;

	for (i=0; i < n; i++) {
		if (p[i] == 0) return i;
		if (p[i] == '\r' || p[i] == '\n') {
			p[i++] = '$'; p[i++] = '\0';	// 끝문자 '$' 추가
			return i+1;
		}

		if ((p[i] & 0x80) == 0) {
			if (p[i] == ' ' || p[i] == '\t')	// 공백문자, 탭문자
				p[i] = '_';
		} else if (p[i+1] == 0) return i;
		else i++;
	}
	return i+1;
}

// character ngram 출력
void put_ngram(char *p, int n) {
	int i, j, k, i2, len;
	char bi[100], *q;

	len = strlen(p);
	if (len < n) return;

	for (i=0; i < len; ) {
		q = bi;
		i2 = i;
		for (j=k=0; j < n; j++) {
			if (p[i] == 0) return;

			if ((p[i] & 0x80) == 0) {
				*q++ = p[i++];	// ASCII
				k = k ? k : 1;
			} else {
				if (p[i+2] == 0) return;
				*q++ = p[i++];	// H1
				*q++ = p[i++];	// H2
				k = k ? k : 2;
			}
		}
		*q = 0;

		if (bi[2] == 0 && bi[0] == '^' && bi[1] == '$') ;
		else printf("%s\n", bi);

		i = i2 + k;
	}
}

int main(int argc, char *argv[]) {
	char *p, line[MAX];
	int n, ngram=2;

	if (argc == 2) {
		if (argv[1][0] == '-')
			ngram = atoi(argv[1]+1);
		else ngram = atoi(argv[1]);
	}
		
	line[0] = '^';	// 첫문자 '^' 추가
	while (fgets(line+1, MAX-2, stdin)) {
		//fputs(line, stdout);
		n = strlen(line);
		n = set_blank2underbar(line, n);
		//printf("%s\n", line);
		put_ngram(line, ngram);
	}
	return 0;
}
