#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 256

// Fisher-Yates Shuffle을 이용해 배열을 섞음
void shuffle(unsigned char *array, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        unsigned char temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

int main() {
    unsigned char c_table[SIZE];       // 원본을 섞은 테이블

    // 0~255 초기화
    for (int i = 0; i < SIZE; i++) {
        c_table[i] = (unsigned char)i;
    }

    // 랜덤 시드 설정
    srand((unsigned int)time(NULL));

    // 배열을 섞음
    shuffle(c_table, SIZE);

    // 역매핑 테이블 생성


    // 결과 출력
    printf("unsigned char XOR_KEYS[512] = {");
    for (int i = 0; i < SIZE; i++) {
        printf("%d%s", c_table[i], (i < SIZE - 1) ? ", " : "");
    }
    printf("};\n\n");

    return 0;
}

