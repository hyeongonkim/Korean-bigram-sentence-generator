import sys
from collections import Counter

# args가 적절하게 입력되었는지 확인
if len(sys.argv) < 3:
	print("C> sentenceSimilarityCalculator.py trigram.txt data.txt")
	exit()

# trigram 텍스트 파일을 읽으며 threshold 이상 등장 빈도를 가진 문자열만 정수 ID 부여
filename = sys.argv[1]
f = open(filename, 'r', encoding='utf-8')

vi_str = f.readline()

originTrigramList = {}
cnt = 0
threshold = 30

while vi_str:
	vi = vi_str.strip().split()
	if (int(vi[0]) < threshold):
		vi_str = f.readline()
		continue
	cnt += 1
	originTrigramList[vi[1]] = cnt
	vi_str = f.readline()

print('등장빈도가 {} 이상인 {} 개의 trigram을 feature로 사용합니다.'.format(threshold, cnt))

# 내적
# va, vb = [ [f1, v1], [f2, v2], ... [fn, vn] ]
def inner_product(va, vb):
	dot = 0
	for i in range(0, len(va)):
		for j in range(0, len(vb)):
			if va[i][0] == vb[j][0] and va[i][0] != -1:
				dot += (va[i][1] * vb[j][1])
	return dot

# 노멀라이즈
def norm(v):
	sum = 1
	for i in range(0, len(v)):
		sum += (v[i][1] * v[i][1])
	return sum

# 노멀라이즈와 내적을 이용해 두 문장 벡터의 코사인 유사도 계산
def cos_sim(va, vb):
	# Dot and norm
	dot = inner_product(va, vb)
	norm_a = norm(va)
	norm_b = norm(vb)

	# Cosine similarity
	if norm_a == 0 or norm_b == 0: return 0.
	return dot / (norm_a*norm_b)

# 평 문장을 받아 트라이그램 리스트로 변환해 리턴합니다.
def getTrigramFromSentence(sentence):
    n_sentence = sentence.strip()
    ngrams = [n_sentence[i:i+3].replace(' ', '_') for i in range(len(n_sentence)-2)]
    return ngrams

# 말뭉치 코퍼스를 읽으며 한 문장씩 유사도 검사를 수행합니다.
data_filename = sys.argv[2]
data_f = open(data_filename, 'r', encoding='utf-8')

# 유사한 문장을 찾기원하는 원시 문장을 입력 받습니다. 유저가 직접 타이핑합니다.
data_v1_str = input('유사 문장을 찾을 입력 문장을 입력하세요 >> ').strip()
print()
trigram_data_v1_str = getTrigramFromSentence(data_v1_str)

# TF를 계산합니다.
data_v1_str_cnt = Counter(trigram_data_v1_str)
data_v1_str_vec = []

# 계산한 TF와 Trigram ID를 기반으로 문장 벡터를 생성합니다.
for key in data_v1_str_cnt:
	data_v1_str_vec.append([originTrigramList.get(key, -1), data_v1_str_cnt[key]])

# 상위 2개의 유사 문장을 저장할 공간을 선언합니다.
first_str = ['', 0]
second_str = ['', 0]

# 본격적으로 말뭉치 코퍼스를 읽으며 문장벡터화 > 원시 문장 벡터와 비교를 수행합니다.
data_vi_str = data_f.readline().strip()

data_max_val = 0
data_max_str = ""

while data_vi_str:
	# 말뭉치 코퍼스의 한 문장을 문장 벡터로 변환합니다.
	trigram_data_vi_str = getTrigramFromSentence(data_vi_str)
	data_vi_str_cnt = Counter(trigram_data_vi_str)
	data_vi_str_vec = []

	for key in data_vi_str_cnt:
		data_vi_str_vec.append([originTrigramList.get(key, -1), data_vi_str_cnt[key]])

	# 두 문장 벡터로 코사인 유사도 검사를 진행합니다.
	data_k = cos_sim(data_v1_str_vec, data_vi_str_vec)

	# 검사한 유사도 값을 현재 상위 2개 값과 비교해 대치할 수 있다면 대치합니다.
	is_changed = False
	if data_k > first_str[1]:
		second_str[1] = first_str[1]
		second_str[0] = first_str[0]
		first_str[1] = data_k
		first_str[0] = data_vi_str
		is_changed = True
	elif data_k > second_str[1]:
		second_str[1] = data_k
		second_str[0] = data_vi_str
		is_changed = True
	
	# 이번 이터레이션의 문장이 상위 유사도를 가진 문장으로 판단되어 대체되었습니다.
	if is_changed:
		print('갱신! 첫번째 유사 문장: {}'.format(first_str[0]))
		print('갱신! 첫번째 유사도: {}'.format(first_str[1]))
		print('갱신! 두번째 유사 문장: {}'.format(second_str[0]))
		print('갱신! 두번째 유사도: {}\n'.format(second_str[1]))

	data_vi_str = data_f.readline().strip()

# 최종 결과를 출력합니다.
print('- - - - - - - - - - - - - - 최종 결과 - - - - - - - - - - - - - -')
print('입력 문장: ', data_v1_str)
print('첫번째 유사 문장: {}'.format(first_str[0]))
print('첫번째 유사도: {}'.format(first_str[1]))
print('두번째 유사 문장: {}'.format(second_str[0]))
print('두번째 유사도: {}\n'.format(second_str[1]))
