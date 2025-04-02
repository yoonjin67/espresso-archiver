import difflib
import sys

def compare_data_similarity(data1, data2):
    """
    길이가 다른 두 데이터의 일치도 및 패턴 유사도를 비교합니다.

    Args:
        data1 (bytes or str): 첫 번째 데이터.
        data2 (bytes or str): 두 번째 데이터.

    Returns:
        tuple: 일치도(float), 패턴 유사도(float).
    """

    if isinstance(data1, bytes):
        data1 = data1.decode('latin-1', errors='ignore')
    if isinstance(data2, bytes):
        data2 = data2.decode('latin-1', errors='ignore')

    # 1. 일치도 계산
    min_len = min(len(data1), len(data2))
    max_len = max(len(data1), len(data2))
    matching_chars = sum(1 for a, b in zip(data1[:min_len], data2[:min_len]) if a == b)
    match_ratio = matching_chars / max_len

    # 2. 패턴 유사도 계산 (SequenceMatcher 사용)
    seq_matcher = difflib.SequenceMatcher(None, data1, data2)
    pattern_ratio = seq_matcher.ratio()

    return match_ratio, pattern_ratio

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("사용법: python script.py <파일1> <파일2>")
        sys.exit(1)

    file1_path = sys.argv[1]
    file2_path = sys.argv[2]

    try:
        with open(file1_path, 'rb') as f1, open(file2_path, 'rb') as f2:
            data1 = f1.read()
            data2 = f2.read()

        # 데이터 비교 및 결과 출력
        match_ratio, pattern_ratio = compare_data_similarity(data1, data2)

        print(f"일치도: {match_ratio:.4f}")
        print(f"패턴 유사도: {pattern_ratio:.4f}")

    except FileNotFoundError:
        print("파일을 찾을 수 없습니다.")
        sys.exit(1)
    except Exception as e:
        print(f"오류 발생: {e}")
        sys.exit(1)
