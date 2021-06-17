from pyspark import SparkConf, SparkContext

conf = SparkConf().setAppName("FindNodeDegree")
sc = SparkContext(conf=conf)

file = sc.textFile('amazon.txt', 100)
edges = file.map(lambda x: tuple(map(int, x.split('\t'))))

whole_edges = edges.union(edges.map(lambda x: (x[1], x[0])))

answer = whole_edges.groupByKey().mapValues(len).sortByKey().collect()

print(answer)
