# Essosplit: RLE Variant File Split & Obfuscation Program

## 📌 Introduction (소개)

While studying C programming at university, I realized that we rarely get to use it in real-world applications. Many courses focus on calling C system functions from C++ or other languages rather than writing pure C programs.

One day, I thought: *What if I split a file in such a way that it appears broken at first glance?* This led me to develop Essosplit. Interestingly, I later discovered that my approach was essentially a form of Run-Length Encoding (RLE). Despite this, common algorithms like BFS are often included in portfolios, so I believe this project effectively demonstrates fundamental knowledge of my major.

I hope this program helps beginners like me get a better grasp of programming concepts.

---

대학에서 C 언어를 배우면서, 실제로 활용할 기회가 적고 C 시스템 함수를 C++ 등에서 호출하는 실습이 많다는 점을 느꼈습니다.

그러던 중, *파일을 분할하면 마치 깨진 것처럼 보이지 않을까?* 라는 아이디어에서 출발하여 Essosplit을 개발했습니다.

나중에 보니 이 방식이 사실상 Run-Length Encoding (RLE)과 동일하다는 걸 깨달았지만, 흔한 알고리즘인 BFS 같은 것도 포트폴리오에 포함하는 걸 보면, 이 프로그램도 전공 지식을 보여주는 데 충분히 가치가 있다고 생각했습니다.

이 프로그램이 저와 같은 프로그래밍 입문자들에게 도움이 되었으면 좋겠습니다.

---

## 🛠 How to Use (사용법)

### 🔹 Split a file (파일 분할)

```sh
./brewer testfile.extension
```

This will generate two output files:

- `testfile.extension.water`
- `testfile.extension.bean`

파일을 두 개로 분할하여 저장합니다:

- `testfile.extension.water`
- `testfile.extension.bean`

### 🔹 Recover a file (파일 복원)

```sh
./recover testfile.extension
```

This will restore the original file as:

- `testfile.extension.recover`

복원된 파일은 다음과 같이 저장됩니다:

- `testfile.extension.recover`

---

## 🔍 How It Works (작동 방식)

Essosplit follows a simple RLE-like approach:

1. It scans the file and detects consecutive identical bytes.
2. The byte values are stored in the `*.water` file.
3. The repetition counts are stored in the `*.bean` file.
4. During recovery, both files are read and reconstructed into the original format.

Essosplit은 단순한 RLE 변형 방식으로 동작합니다:

1. 파일을 읽으며 연속된 동일한 바이트를 찾습니다.
2. 바이트 값은 `*.water` 파일에 저장됩니다.
3. 반복 횟수는 `*.bean` 파일에 저장됩니다.
4. 복원 과정에서는 두 파일을 읽어 원본 파일을 재구성합니다.

---

## 📌 Why Use Essosplit? (Essosplit의 특징)

✅ **Simple yet effective obfuscation** - The split files appear broken, making casual inspection difficult.

✅ **Lightweight and fast** - Since it follows RLE principles, the processing is quick.

✅ **Easy to understand** - Great for beginners learning about file manipulation.

✅ **간단하지만 효과적인 난독화** - 분할된 파일은 깨진 것처럼 보이므로 단순한 확인으로는 원본을 알기 어렵습니다.

✅ **가볍고 빠름** - RLE 원리를 따르므로 처리 속도가 빠릅니다.

✅ **이해하기 쉬움** - 파일 조작을 배우는 입문자에게 적합한 프로그램입니다.

---

## 🔧 Future Improvements (향후 개선점)

- Support for different file types and formats (다양한 파일 포맷 지원)
- Customizable obfuscation methods (사용자 정의 난독화 기법 추가)
- Performance optimizations (성능 최적화)

---

## 📜 License (라이선스)

This project is open-source under the MIT License.

이 프로젝트는 MIT 라이선스 하에 오픈소스로 제공됩니다.
