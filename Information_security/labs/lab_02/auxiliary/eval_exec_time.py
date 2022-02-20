import time
from functools import wraps

def eval_time(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        time_start = time.time()
        res = func(*args, **kwargs)
        time_end = time.time()
        print(f" /// This is execution time of {func.__name__}: {time_end-time_start} sec")
        return res
    return wrapper
