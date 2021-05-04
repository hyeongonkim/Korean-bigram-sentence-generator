import sys
from collections import Counter

if len(sys.argv) < 2:
	print("C> wc_sort_cut_1000.py data.txt")
	exit()

filename = sys.argv[1]
f = open(filename, 'r', encoding='utf-8')

data = f.readline()
data_whole_str = ''
while data:
    data_whole_str += data
    data = f.readline()

print(Counter(data_whole_str.split()).most_common(1000))