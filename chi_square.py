from collections import Counter

with open('test.ezip', 'rb') as f:
    data = f.read()

counts = Counter(data)
expected = len(data) / 256
chi_square = sum(((counts.get(byte, 0) - expected) ** 2) / expected for byte in range(256))
print(f"Chi-Square statistic: {chi_square:.4f}")

