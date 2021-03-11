import numpy as np
import time

r = np.random.RandomState((int)(time.time()))

print(r.randint(19))

print(r.randint(0,2, 10))
print(r.permutation(19))
# 1 0 0 1
# 