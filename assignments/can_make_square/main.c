#include <stdio.h>
#include <stdlib.h>

// 問題の要求する1行のサイズが大きい場合はこの値を変更してください。
// If the problem requires more large line size, please modify following.
#define LINE_BUF_SIZE  1024

#define MAX_STICK_LENGTH 50

int main(void) {
  // このコードは標準入力と標準出力を用いたサンプルコードです。
  // このコードは好きなように編集・削除してもらって構いません。
  // ---
  // This is a sample code to use stdin and stdout.
  // Edit and remove this code as you like.

  int n;
  scanf("%d", &n);

  int lengths[n];
  int frequency[MAX_STICK_LENGTH + 1 ] = {0};

  // Read the lengths of the sticks and count the frequency
  for (int i = 0; i < n; i++) {
      scanf("%d", &lengths[i]);
      frequency[lengths[i]]++;
  }

  // Check if a square can be formed
  for (int i = 1; i <= MAX_STICK_LENGTH; i++) {
      if (frequency[i] >= 4) {
          printf("YES\n");
          return 0;
      }
  }
  printf("NO\n");
  return 0;
}