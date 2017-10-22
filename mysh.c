#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define SIZE 80
#define BUFSIZE 256

void getargs(char *cp, int *argc, char *argv[])
{
  char *p = cp;
	*argc = 0;
	
	while(1) {
		if (*p == ' ' || *p == '\t')
			p++;
		else if (*p == '\n')
			break;
		else {
			argv[*argc] = p;
			*argc = *argc + 1;
			while(*p != ' ' && *p != '\t' && *p != '\n')
				p++;
			if (*p == '\n')
				break;
			*p = '\0';
			p++;
		}
	}
	*p = '\0';
}


void printArgv(int argc, char *argv[])
{
  int i;
  for (i = 0; i < argc; i++) {
    printf("argv[%d]: %s\n", i, argv[i]);
  }
}


int countPipe(int argc, char *argv[])
{
  int i;
  int pipeNum;

  for (i = 0, pipeNum = 0; i < argc; i++) {
    if (strcmp(argv[i], "|") == 0) {
      pipeNum++;
    }
  }

  return pipeNum;
}


// パイプ時に分割
// void splitProc(int procID, int *pargc, char *pargv[], int *ac, char *av[])
// {
// 	*pargc = 0;
// 	int argv_i, pargv_i;

//   if (procID == 1) {

//   } else {

//   }

//   for (argv_i = 0; argv_i < *ac; i++) {
//     if (strcmp(av[i], "|") == 0) {
//       i++;
//       break;
//     }
//     pargv[i] = 
//   }

// 	for (i = 0; j < *ac; i++, j++) {
// 		if (!(strcmp(av[j], "|"))) {
// 			j++;
// 			break;
// 		} else {
// 			pargv[i] = av[j];
// 			*pargc = *pargc + 1;
// 		}
// 	}
// }

int main(void)
{
  int argc;
  char *argv[SIZE];
  int pipeNum;
  int i;
  int *pargc;
  char *pargv[SIZE];
  int pid;
  int status;
  
  while(1) {
    printf("mysh$ ");

    char cp[BUFSIZE] = {0};

    if (fgets(cp, BUFSIZE, stdin) == NULL) {
      fprintf(stderr, "input error\n");
      exit(1);
    }

    getargs(cp, &argc, argv);
    
    if (strcmp(argv[0], "exit") == 0) {
      printf("EXIT!\n");
      exit(0);
    }
    // pipeNum = countPipe(*argc, argv);
    
    // for (i = 0; i < pipeNum; i++) {
    //   splitProc(pargc, pargv, argc, argv);
      
      argv[argc] = NULL;
      // execvp(argv[0], argv);

      printArgv(argc + 1, argv);
      if ((pid = fork()) < 0) {
        /* エラー処理 */
        perror("fork");
      } else if (pid == 0) {
        /* 子プロセスの処理 */
        execvp(argv[0], argv);
      } else {
        /* 親プロセスの処理 */
        printf("親プロセス\n");
        wait(&status);
      }
    // }
  }
  
  return 0;
}
