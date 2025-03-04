# Espresso Archiver: RLE Variant Obfuscation Program

## 📌 Introduction (소개)

While studying C programming at university, I realized that we rarely get to use it in real-world applications. 
Many courses focus on calling C system functions from C++ or other languages rather than writing pure C programs.

One day, I thought: *What if I split a binary data into two different datas: bit count, and bit order.*
(as a result, bit order would be: 010101010... or 10101010...)
Interestingly, I later discovered that my approach was essentially a form of Run-Length Encoding (RLE). 
Despite this, common algorithms like BFS are often included in portfolios, so I believe this project effectively demonstrates fundamental knowledge of my major.
Also basic obfuscations are provided, so it wouldn't be bad.

I hope this program helps beginners like me get a better grasp of programming concepts.

---

대학에서 C 언어를 배우면서, 실제로 활용할 기회가 적고 C 시스템 함수를 C++ 등에서 호출하는 실습이 많다는 점을 느꼈습니다.

그러던 중, *파일의 데이터를 비트 수와 나열 순서로 나누면 어떨까* 라는 아이디어에서 출발하여 Espresso Zip을 개발했습니다.
(아마 결과는 010101010...아니면 10101010...이겠죠.)

나중에 보니 이 방식이 사실상 Run-Length Encoding (RLE)과 동일하다는 걸 깨달았지만, 
흔한 알고리즘인 BFS 같은 것도 포트폴리오에 포함하는 걸 보면, 이 프로그램도 전공 지식을 보여주는 데 충분히 가치가 있다고 생각했습니다.
기초적인 난독화 또한 있으니 나쁘지 않은 선택이겠죠.

이 프로그램이 저와 같은 프로그래밍 입문자들에게 도움이 되었으면 좋겠습니다.

---

## 🛠 Installation & Usage | 설치 및 사용법


### 🔹 Build | 빌드

요구 라이브러리(required library): zlib, gperftools

```sh
make
```

## 🛠 How to Use (사용법)

### 🔹 Encode & Compress | 변형 및 압축


```sh
./eszip myfile.bin
```

This will generate an archive:

- `myfile.bin.ezip`

압축파일은 다음과 같습니다:

- `myfile.bin.ezip`

### 🔹 Extract | 압축 해제

```sh
./esunzip testfile.extension
```

---

## 🔍 How It Works (작동 방식)

Espresso Zip follows a simple RLE-like approach:

1. XOR encryption → XOR conversion with key A
2. apply bit negation (NOT operation)
3. apply Bit Shift operation → Shuffle data
4. encrypt XOR with key B
4. finally compress (zlib Deflate) and save with *.ezip extension

Espresso Zip은 단순한 RLE 변형 방식으로 동작합니다:


1. XOR 암호화 → 키 A를 이용해 XOR 변환
2. 비트 부정 (NOT 연산) 적용
3. 비트 쉬프트 (Bit Shift) 연산 → 데이터 흐트리기
4. 키 B로 XOR 암호화
4. 최종적으로 압축 (zlib Deflate) 후 *.ezip 확장자로 저장


---

## 📌 Why Use Essosplit? (Essosplit의 특징)

✅ **Simple yet effective obfuscation** - Unarchived data appears broken, making casual inspection difficult. Combining XOR, NOT, and Bit Shift provides adequate security.

✅ **Lightweight and fast** - Since it follows RLE principles, the processing is quick.

✅ **Easy to understand** - Great for beginners learning about file manipulation.

✅ **간단하지만 효과적인 난독화** - 압축 해제된 파일은 깨진 것처럼 보이므로 단순한 확인으로는 원본을 알기 어렵습니다. XOR, NOT, Bit Shift가 합리적인 보안을 제공합니다.


✅ **가볍고 빠름** - RLE 원리를 따르므로 처리 속도가 빠릅니다.

✅ **이해하기 쉬움** - 파일 조작을 배우는 입문자에게 적합한 프로그램입니다.

---

## 🔧 Limitations (한계)

- 여러 파일/디렉토리 구조를 압축하지 못함(Cannot archive multiple files, or directories)
- zlib를 활용해 압축 속도나 효율이 평이함(Compression is not enough efficient: identical to zlib's performance)
- 프로그램 구조가 지나치게 단순함.(Program structure is way-too-simple)

Frankly, its size and quality is similar to program examples in Highschool(or Secondary School) IT textbooks.
Actual development period was less than a week.
사실 이 프로그램의 크기나 질로 봤을 때 고등학교 정보 책 정도에 있을 수준입니다.
실제로 개발 자체도 1주일 정도 걸렸고요.


## 🔧 Future Improvements (향후 개선점)

- Support for different file types and formats (다양한 파일 포맷 지원)
- Support for multiple files/directory archive (여러 파일, 디렉토리 압축 지원)
- Customizable obfuscation methods (사용자 정의 난독화 기법 추가)
- Performance optimizations (성능 최적화)

---

## 📜 License (라이선스)

This project is open-source under the MIT License.

이 프로젝트는 MIT 라이선스 하에 오픈소스로 제공됩니다.
