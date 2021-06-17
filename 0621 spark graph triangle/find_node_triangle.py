from pyspark import SparkConf, SparkContext

conf = SparkConf().setAppName("FindNodeTriangle")
sc = SparkContext(conf=conf)

file = sc.textFile('amazon.txt', 100)
edges = file.map(lambda x: tuple(map(int, x.split('\t'))))
flatten_edges = edges.flatMap(lambda x: [x[0], x[1]]).distinct().map(lambda x: (x, None))

edges_for_join = edges.map(lambda x: (x, None)).union(edges.map(lambda x: ((x[1], x[0]), None)))

grouped_edges = edges.groupByKey().mapValues(list)
wedges_values = grouped_edges\
    .filter(lambda x: len(x[1]) >= 2)\
    .map(lambda x: (x[0], [(a, b) for idx, a in enumerate(x[1]) for b in x[1][idx + 1:]]))\
    .flatMapValues(lambda x: x)\
    .map(lambda x: (x[1], x[0]))

triangle_nodes = edges_for_join.join(wedges_values)\
    .flatMapValues(lambda x: x)\
    .filter(lambda x: x[1] is not None)\
    .flatMap(lambda x: [(x[0][0], 1), (x[0][1], 1), (x[1], 1)])\
    .groupByKey()\
    .mapValues(len)

answer = flatten_edges\
    .leftOuterJoin(triangle_nodes)\
    .map(lambda x: (x[0], x[1][1]))\
    .flatMap(lambda x: [(x[0], 0 if x[1] is None else x[1])])\
    .sortByKey()\
    .collect()

print(answer)
