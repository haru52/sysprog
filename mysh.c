#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

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


void printargs(int argc, char *argv[])
{
  int i;

  // fprintf(stderr, "argc: %d\n", argc);
  for (i = 0; i < argc; i++) {
    fprintf(stderr, "argv[%d]: %s\n", i, argv[i]);
  }
}


int count_pipe(int argc, char *argv[])
{
  int i;
  int pipe_num;

  for (i = 0, pipe_num = 0; i < argc; i++) {
    if (strcmp(argv[i], "|") == 0) {
      pipe_num++;
    }
  }

  return pipe_num;
}


// パイプ時に分割
void split_proc(int proc_id, int pipe_num, int argc, char *argv[], int *pargc, char *pargv[])
{
	*pargc = 0;
	int argv_i, pargv_i;
  int passed_pipe_num = 0;

  if (proc_id == 1) {
    for (argv_i = 0, pargv_i = 0; argv_i < argc; argv_i++, pargv_i++) {
      if (strcmp(argv[argv_i], "|") == 0) {
        break;
      }
      pargv[pargv_i] = argv[argv_i];
      *pargc += 1;
    }
  } else if (proc_id == (pipe_num + 1)) {
    for (argv_i = 0; passed_pipe_num < (proc_id - 1); argv_i++) {
      if (strcmp(argv[argv_i], "|") == 0) {
        passed_pipe_num++;
      }
    }
    for (pargv_i = 0; argv_i < argc; argv_i++, pargv_i++) {
      pargv[pargv_i] = argv[argv_i];
      *pargc += 1;
    }
  } else {
    for (argv_i = 0; passed_pipe_num < (proc_id - 1); argv_i++) {
      if (strcmp(argv[argv_i], "|") == 0) {
        passed_pipe_num++;
      }
    }
    for (pargv_i = 0; argv_i < argc; argv_i++, pargv_i++) {
      if (strcmp(argv[argv_i], "|") == 0) {
        break;
      }
      pargv[pargv_i] = argv[argv_i];
      *pargc += 1;
    }
  }
  pargv[pargv_i] = NULL;
}

void redirect(int pargc, char *pargv[])
{
  int i;
  int fd;

  for (i = 0; i < pargc; i++) {
    if (strcmp(pargv[i], "<") == 0) {
      fd = open(pargv[i + 1], O_RDONLY);
      close(0);
      dup(fd);
      close(fd);
      pargv[i] = NULL;
      i++;
    } else if (strcmp(pargv[i], ">") == 0) {
      fd = open(pargv[i + 1], O_WRONLY|O_CREAT|O_TRUNC, 0644);
      close(1);
      dup(fd);
      close(fd);
      pargv[i] = NULL;
      i++;
    }
  }
}

int main(void)
{
  int argc;
  char *argv[SIZE];
  int pipe_num;
  int i;
  int pargc;
  char *pargv[SIZE];
  int pid;
  int status;
  int proc_id;
  
  while(1) {
    printf("mysh$ ");

    char cp[BUFSIZE];
    memset(cp, 0, sizeof(cp));

    if (fgets(cp, BUFSIZE, stdin) == NULL) {
      perror("fgets");
      exit(1);
    }

    getargs(cp, &argc, argv);

    if (strcmp(argv[0], "cd") == 0) {
      if (argc == 1) {
        chdir(getenv("HOME"));
      } else {
        chdir(argv[1]);
      }
    } else if (strcmp(argv[0], "exit") == 0) {
      exit(0);
    }
    pipe_num = count_pipe(argc, argv);
    // printf("pipe_num: %d\n", pipe_num);
    
    for (proc_id = 1; proc_id <= pipe_num  + 1; proc_id++) {
      split_proc(proc_id, pipe_num, argc, argv, &pargc, pargv);
      // printf("proc_id: %d\n", proc_id);
      // printargs(pargc, pargv);

      if ((pid = fork()) < 0) {
        /* エラー処理 */
        perror("fork");
      } else if (pid == 0) {
        /* 子プロセスの処理 */
        int fd;
        int i;
        redirect(pargc, pargv);
        execvp(pargv[0], pargv);
      } else {
        /* 親プロセスの処理 */
        wait(&status);
      }
    }
  }
  
  return 0;
}
