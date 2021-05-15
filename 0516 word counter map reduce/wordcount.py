from emit import *

def map_func(key, value):
    for x in value.split():
        emit(x, 1)
    

def reduce_func(key, values):
    emit(key, sum(values))
