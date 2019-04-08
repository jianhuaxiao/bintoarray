#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

unsigned int GetFileSize(char* filename);
void read_src(char* path, unsigned char* buf, unsigned int size);
void make_dstfile(char* outpath, unsigned char* buf, unsigned int size);
static void printf_usage(char* name) {
  printf("Usage: %s [-i file name, -o output file name]\n", name);
}

int main(int argc, char* argv[]) {
  unsigned char* buf = NULL;

  unsigned int size;
  char srcbmp[100] = {0};
  char dstfile[100] = {0};
  int opt;

  if (argc < 5) {
    printf_usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  while ((opt = getopt(argc, argv, "i:o:")) != -1) {
    switch (opt) {
      case 'i':
        memcpy(srcbmp, optarg, sizeof(srcbmp));
        break;
      case 'o':
        memcpy(dstfile, optarg, sizeof(srcbmp));
        break;

      default:
        printf_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
  }
  size = GetFileSize(srcbmp);
  buf = (unsigned char*)calloc(size, sizeof(unsigned char));
  read_src(srcbmp, buf, size);

  make_dstfile(dstfile, buf, size);
  return 0;
}

unsigned int GetFileSize(char* filename) {
  unsigned int siz = 0;
  FILE* fp = fopen(filename, "rb");
  if (fp) {
    fseek(fp, 0, SEEK_END);
    siz = ftell(fp);
    fclose(fp);
  }
  return siz;
}

void read_src(char* path, unsigned char* buf, unsigned int size) {
  FILE* infile;

  if ((infile = fopen(path, "rb")) == NULL) {
    printf("\nCan not open the path: %s \n", path);
    exit(-1);
  }

  fread(buf, sizeof(unsigned char), size,
        infile);  // printf("\n打开的图为 %d",img->bfType);

  fclose(infile);
}
void make_dstfile(char* outpath, unsigned char* buf, unsigned int size) {
  FILE* infile;
  int i, j, k, n;
  char pbuf[10] = {0};
  char annotate[32] = {0};
  if ((infile = fopen(outpath, "wa+")) == NULL) {
    printf("\nCan not open the path: %s \n", outpath);
    exit(-1);
  }
  k = 0;

  fwrite("u8 temp_array[] = {\n", strlen("u8 temp_array[] = {\n"), 1, infile);
  for (i = 0; i < size; i++) {
    k++;
    if (!(i % 8)) {
      sprintf(pbuf, "    0x%02x", buf[i]);
    } else {
      sprintf(pbuf, "0x%02x", buf[i]);
    }

    fwrite(pbuf, strlen(pbuf), 1, infile);
    if (k != 8) fwrite(", ", strlen(", "), 1, infile);

    if ((8 == k) || (i == size - 1)) {
      k = 0;
      fwrite(",", strlen(","), 1, infile);
      sprintf(annotate, " //%d-%d", i - 7, i + 1);
      fwrite(annotate, strlen(annotate), 1, infile);
      fwrite("\n", strlen("\n"), 1, infile);
    }
  }

  fseek(infile, 0, SEEK_END);
  if (k == 0)
    fwrite("};", strlen("};"), 1, infile);
  else
    fwrite("\n};", strlen("\n};"), 1, infile);

  fclose(infile);
}