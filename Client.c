#include <stdio.h>
#include <stdlib.h>     //exit
#include <string.h>     //strlen, memset
#include <sys/socket.h> //socket, connect, send, recv
#include <arpa/inet.h>  //sockaddr_in, inet_addr, htons
#include <unistd.h>     //close
#include <netdb.h>      //gethostbyname
#define PORT 7778
#define BUFSIZE 256
int main(int argc, char *argv[]) {
  int sd;                          //ソケット
  char *servName;                  //サーバのホスト名またはIPアドレスの文字列
  unsigned short servPort = PORT;  //サーバのポート番号
  struct sockaddr_in servAdrs;	//サーバのアドレス構造体
  struct hostent *servEnt;         //サーバのホスト情報を格納する構造体
  unsigned long servIP;            //サーバのIPアドレス
  int recvLen;
  int sendLen;
  char *id;                         //ログインID
  char buf[BUFSIZE];
  int bufsize;
  int sint;
  char message[20];  
  int select;                    //選択番号の格納変数
  if (argc != 3) {
    printf("使用方法: %s サーバ名 ID\n", argv[0]);
    exit(1);
  }
  servName = argv[1];    //コマンドライン引数[1]をサーバ名とする
  if(strlen(argv[2])>8){
    printf("IDは8文字未満にしてください");
    exit(-2);
  }
  id = argv[2];  //コマンドライン引数[2]をログインIDとする
  if ((servEnt = gethostbyname(servName)) == NULL) {
    puts("gethostbyname失敗。"); exit(-7);
  }
  servIP = *((unsigned long *)servEnt->h_addr_list[0]);
  //ソケットの作成
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    puts("socket失敗。");  exit(-1);
  }
  //アドレス構造体の設定
  memset(&servAdrs, 0, sizeof(servAdrs));
  servAdrs.sin_family = AF_INET;
  servAdrs.sin_addr.s_addr = servIP;
  servAdrs.sin_port = htons(servPort);
  //接続
  if (connect(sd, (struct sockaddr *)&servAdrs, sizeof(servAdrs)) < 0) {
    puts("接続失敗");  exit(-2);
  }
  if((recvLen = recv(sd,buf,BUFSIZE-1,0)) <= 0) {   /*受信*/
      puts("受信失敗");  exit(-4);
  }
  buf[recvLen] = '\0';
  printf("%s %sさん\n",buf,id);
  printf("欲しい情報はどれですか？\n");
  printf("1.今日の日付　2.現在の時刻\n");
  scanf("%d",&sint);
  select = htonl(sint);            //エンディアン変換
  bufsize = sizeof(int);
  if (send(sd, &select, bufsize, 0) != bufsize) {
    puts("send失敗。");  exit(-3);
  }

  if((recvLen = recv(sd,message,BUFSIZE-1,0)) <= 0) {   /*受信*/
      puts("受信失敗");  exit(-4);
  }
  printf("%s\n",message);
  close(sd);
}