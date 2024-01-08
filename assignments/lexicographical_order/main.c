#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LINE_BUF_SIZE  200000

int main(void) {
    char S[LINE_BUF_SIZE];
    char c;
    char result[LINE_BUF_SIZE + 1]; // +1 for the inserted character

    // Read the string S and character c from stdin
    if (fgets(S, sizeof(S), stdin) == NULL) {
        return 1; // Error reading string
    }
    // Remove newline character from fgets
    S[strcspn(S, "\n")] = 0;

    // Read the character c
    c = getchar();
    getchar(); // Read the newline character after the single character input

    // Find the correct position to insert c
    int len = strlen(S);
    bool inserted = false;
    for (int i = 0; i < len; ++i) {
        if (c < S[i]) {
            // Insert c before S[i]
            strncpy(result, S, i); // Copy up to the ith character
            result[i] = c; // Insert c
            strcpy(result + i + 1, S + i); // Copy the rest of S
            inserted = true;
            break;
        }
    }

    // If c was not inserted, append it to the end of S
    if (!inserted) {
        strcpy(result, S);
        result[len] = c;
        result[len + 1] = '\0';
    }

    // Output the result
    printf("%s\n", result);
    
    return 0;
}

