#include <stdio.h>
#include <stdlib.h>     //exit
#include <string.h>     //strlen, memset
#include <sys/socket.h> //socket, bind, listen, send, recv
#include <arpa/inet.h>  //sockaddr_in, inet_addr, htonl, htons
#include <unistd.h>     //close
#include <time.h>

#define PORT 7778    //echoサービスのポート番号
#define BUFSIZE 256

int main(int argc, char *argv[]) {
    int servSd;//サーバソケット
    int clntSd;//クライアントソケット
    unsigned short servPort = PORT;  //サーバのポート番号
    struct sockaddr_in servAdrs;     //サーバのアドレス構造体
    struct sockaddr_in clntAdrs;     //クライアントのアドレス構造体
    unsigned int clntLen;            //clntAdrsのサイズ
    int on = 1;                      //ソケットオプション用
    char greeting1[] = "こんにちは";
    char greeting2[] = "無効な選択です";
    int select;
    time_t timer;        /* 時刻を取り出すための型（実際はunsigned long型） */
    struct tm *local;    /* tm構造体（時刻を扱う) */
    char year[5], month[3], day[3], hour[3], minute[3], second[3],message[20];
    int sendLen;
    int recvLen;
    int rint;
    int bufsize;
    
    //サーバソケットの作成
    if ((servSd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        puts("socket失敗。");  //exit(-1);
    }
    //ソケットオプションの設定
    setsockopt(servSd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    //アドレス構造体の設定
    memset(&servAdrs, 0, sizeof(servAdrs));
    servAdrs.sin_family = AF_INET;
    servAdrs.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdrs.sin_port = htons(servPort);

    //ソケットにアドレスとポートを割り当てる
    if (bind(servSd, (struct sockaddr *)&servAdrs, sizeof(servAdrs)) < 0) {
        puts("bind失敗。");  //exit(-5);
    }
    //ポートを有効化し接続を受け付ける
    if (listen(servSd, 10) < 0) {
        puts("listen失敗。");  //exit(-6);
    }
    //接続を受け入れてクライアントソケットを取得する
    clntLen = sizeof(clntAdrs);
    if((clntSd = accept(servSd, (struct sockaddr *)&clntAdrs, &clntLen)) < 0) {
        puts("accept失敗。");  //exit(-6);
    }
    printf("接続: %s\n", inet_ntoa(clntAdrs.sin_addr));
    sendLen = strlen(greeting1);
    if(send(clntSd, greeting1, sendLen, 0) != sendLen) {
        puts("送信失敗。");  exit(-3);
    }
    bufsize = sizeof(int);   //受信
    if (recv(clntSd, &select, bufsize, 0) != bufsize) {
      puts("受信失敗。");  exit(-4);
    }

    rint = ntohl(select);

    timer = time(NULL);        /* 現在時刻を取得 */
    local = localtime(&timer);

    if(rint == 1){
      snprintf(year,5,"%d",local->tm_year + 1900);        /* 1900年からの年数が取得されるため */
      snprintf(month,3,"%d",local->tm_mon + 1);        /* 0を1月としているため */
      snprintf(day,3,"%d",local->tm_mday);
      strcat(message,year);
      strcat(message,"/");
      strcat(message,month);
      strcat(message,"/");
      strcat(message,day);
      sendLen = strlen(message);
      if(send(clntSd, message, sendLen, 0) != sendLen) {
          puts("送信失敗。");  exit(-3);
      }
    }
    else if(rint == 2){
      snprintf(hour,3,"%d",local->tm_hour);        /* 1900年からの年数が取得されるため */
      snprintf(minute,3,"%d",local->tm_min);        /* 0を1月としているため */
      snprintf(second,3,"%d",local->tm_sec);
      strcat(message,hour);
      strcat(message,"時");
      strcat(message,minute);
      strcat(message,"分");
      strcat(message,second);
      strcat(message,"秒");
      sendLen = strlen(message);
      if(send(clntSd, message, sendLen, 0) != sendLen) {
          puts("送信失敗。");  exit(-3);
      }
    }
    else{
      sendLen = strlen(greeting2);
      if(send(clntSd, greeting2, sendLen, 0) != sendLen) {
          puts("送信失敗。");  exit(-3);
      }
    }
  
    close(clntSd);  //ソケットを閉じる
    close(servSd);
    return 0;

}