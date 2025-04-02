import math
import sys
from collections import Counter

def calculate_entropy(data):
    if not data:
        return 0
    counts = Counter(data)
    entropy = 0.0
    for count in counts.values():
        p_x = count / len(data)
        entropy -= p_x * math.log(p_x, 2)
    return entropy

with open(sys.argv[1], 'rb') as f:
    data = f.read()

entropy_value = calculate_entropy(data)
print(f"Shannon Entropy: {entropy_value:.4f} bits per byte")

