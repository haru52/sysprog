#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define BUF_SIZE 1024
#define MAX_LEN 256
#define USAGE "usage: ./mycp [source file] [destination file]\n"

int main(int argc, char *argv[])
{
  char *src_filepath, *dst_filepath;
  FILE *src_fp, *dst_fp;
  char buf[BUF_SIZE];
  char answer[MAX_LEN];

  if (argc != 3) {
    printf(USAGE);
    exit(1);
  }

  src_filepath = argv[1];
  dst_filepath = argv[2];

  if ((src_fp = fopen(src_filepath, "r")) == NULL) {
    perror("fopen");
    exit(1);
  }

  if ((dst_fp = fopen(dst_filepath, "r")) == NULL) {
    if (errno != ENOENT) {
      perror("fopen");
      exit(1);
    }
  } else {
    while(1) {
      printf("%sは既に存在します。上書きしますか？（y/n）\n", dst_filepath);
      scanf("%255s", answer);
      if (strlen(answer) != 1) {
        continue;
      }
      if (strcmp(answer, "y") == 0) {
        break;
      } else if (strcmp(answer, "n") == 0) {
        exit(0);
      }
    }
  }
  if ((dst_fp = fopen(dst_filepath, "w")) == NULL) {
    perror("fopen");
    exit(1);
  }

  while(1) {
    int size = fread(buf, sizeof(char), BUF_SIZE, src_fp);
    if (size) {
      if (fwrite(buf, sizeof(char), size, dst_fp) < size) {
        perror("fwrite");
        exit(1);
      }
    } else {
      if (ferror(src_fp)) {
        perror("ferror");
        exit(1);
      } else if (feof(src_fp)) {
        break;
      }
    }
  }

  fclose(src_fp);
  fclose(dst_fp);

  return 0;
}
