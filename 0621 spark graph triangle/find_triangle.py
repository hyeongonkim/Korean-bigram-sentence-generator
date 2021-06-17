from pyspark import SparkConf, SparkContext

conf = SparkConf().setAppName("FindTriangle")
sc = SparkContext(conf=conf)

file = sc.textFile('amazon.txt', 100)
edges = file.map(lambda x: tuple(map(int, x.split('\t'))))

edges_for_join = edges.map(lambda x: (x, None)).union(edges.map(lambda x: ((x[1], x[0]), None)))

grouped_edges = edges.groupByKey().mapValues(list)
wedges_values = grouped_edges\
    .filter(lambda x: len(x[1]) >= 2)\
    .map(lambda x: (x[0], [(a, b) for idx, a in enumerate(x[1]) for b in x[1][idx + 1:]]))\
    .flatMapValues(lambda x: x)\
    .map(lambda x: (x[1], x[0]))

answer = edges_for_join.join(wedges_values)\
    .flatMapValues(lambda x: x)\
    .filter(lambda x: x[1] is not None)\
    .count()

print(answer)
