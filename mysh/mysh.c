#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include<signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SIZE 80
#define BUFSIZE 256


void printargs(int argc, char *argv[])
{
  int i;
  // fprintf(stderr, "argc: %d\n", argc);
  for (i = 0; i <= argc; i++) {
    fprintf(stderr, "argv[%d]: %s\n", i, argv[i]);
  }
}


// void getargs_new(char *lbuf, int *argc, char *argv[])
// {
//   char *p;
//   *argc = 0;
//   char buf[BUFSIZE] = {};
//   char *buf_p = buf;
//
//   for(p = lbuf; *p != '\n' ;p++) {
//     switch(*p) {
//       case ' ':
//       case '\t':
//         // fprintf(stderr, "space or tab\n");
//         // fprintf(stderr, "buf: %s\n", buf);
//         break;
//       case '<':
//       case '>':
//       case '|':
//         // fprintf(stderr, "redirect or pipe\n");
//         *buf_p = '\0';
//         // fprintf(stderr, "buf: %s\n", buf);
//         strncpy(argv[*argc], buf, sizeof(buf));
//         *argc += 1;
//         *buf_p = *p;
//         buf_p++;
//         *buf_p = '\0';
//         strncpy(argv[*argc], buf, sizeof(buf));
//         *argc += 1;
//         // fprintf(stderr, "buf: %s\n", buf);
//         break;
//       default:
//         // fprintf(stderr, "normal\n");
//         *buf_p = *p;
//         buf_p++;
//         // fprintf(stderr, "buf: %s\n", buf);
//         break;
//     }
//   }
//   *buf_p = '\0';
//   fprintf(stderr, "buf: %s\n", buf);
//   fprintf(stderr, "argc: %d\n", *argc);
//   // strncpy(argv[*argc], buf, strlen(buf) + 1);
//   // 下のstrcpyでセグフォ
//   fprintf(stderr, "hoge\n");
//   strncpy(argv[*argc], buf, strlen(buf));
//   fprintf(stderr, "fuga\n");
//   // fprintf(stderr, "hoge\n");
//   // fprintf(stderr, "argv[i]: %s\n",argv[*argc]);
//   *argc += 1;
//   printargs(*argc, argv);
//   fprintf(stderr, "getargs ends\n");
// }


void getargs(char *cp, int *argc, char *argv[])
{
  char *p = cp;
  *argc = 0;

  while(1) {
    if (*p == ' ' || *p == '\t') {
      p++;
    } else if (*p == '\n') {
      break;
    } else {
      argv[*argc] = p;
      *argc = *argc + 1;
      while(*p != ' ' && *p != '\t' && *p != '\n') {
        p++;
      }
      if (*p == '\n') {
        break;
      }
      *p = '\0';
      p++;
    }
  }
  *p = '\0';
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


void split_proc(int proc_id, int pipe_num, int argc, char *argv[], int *pargc, char *pargv[], int *bg_flag)
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
  
  if (strcmp(pargv[pargv_i - 1], "&") == 0) {
    *bg_flag = 1;
    pargv[pargv_i - 1] = NULL;
    *pargc -= 1;
  } else {
    pargv[pargv_i] = NULL;
  }
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


void wait_bg_proc(int sig) {
  if (sig == SIGCHLD) {
    int status;
    wait(&status);
  }
  return;
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
  int bg_flag = 0;
  int pfd[2];
  int fd;
  
  while(1) {
    printf("mysh$ ");

    char cp[BUFSIZE];
    memset(cp, 0, sizeof(cp));

    if (fgets(cp, BUFSIZE, stdin) == NULL) {
      perror("fgets");
      exit(1);
    }

    getargs(cp, &argc, argv);
    if (argc == 0) {
      continue;
    }

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
    
    for (proc_id = 1; proc_id <= pipe_num  + 1; proc_id++) {

      bg_flag = 0;
      split_proc(proc_id, pipe_num, argc, argv, &pargc, pargv, &bg_flag);
      
      if (pipe_num > 0 && proc_id == 1) {
        pipe(pfd);
      }

      if ((pid = fork()) < 0) {
        /* エラー処理 */
        perror("fork");
      } else if (pid == 0) {
        /* 子プロセスの処理 */
        redirect(pargc, pargv);

        if (pipe_num > 0) {
          if (proc_id == 1) {
            close(1);
            dup(pfd[1]);
            close(pfd[0]);
            close(pfd[1]);
          } else if (proc_id == pipe_num + 1) {
            close(0);
            dup(pfd[0]);
            close(pfd[0]);
            close(pfd[1]);
          } else if (proc_id % 2 == 0) {
            // 偶数
          } else {
            // 奇数
          }
        }
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        // redirect(pargc, pargv);
        execvp(pargv[0], pargv);
      } else {
        /* 親プロセスの処理 */
        signal(SIGINT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        if (bg_flag == 1) {
          // fprintf(stderr, "waiting for bg proc...\n");
          signal(SIGCHLD, wait_bg_proc);
        } else {
          // fprintf(stderr, "waiting for fg prog...\n");
          if (pipe_num > 0 && proc_id == pipe_num + 1) {
            close(pfd[0]);
            close(pfd[1]);
          }
          for (i = 0; i < pipe_num + 1; i++) {
            wait(&status);
          }
        }
      }
    }
  }

  return 0;
}
