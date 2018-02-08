# Chat

## Overview
1つのプログラム（chat.c）で，1対1のチャットができるプログラムです．  
お互いにメッセージの送受信ができます．  
C言語のselect()を使ったネットワークプログラミングの練習を兼ねています．


## Description
- キーボードとネットワークの両方からの入力待ち
- キーボードからの入力を画面に表示
- 受信したデータを表示
- キーボードからの入力が “FIN” だったら実行終了
- 受信データが “FIN” だったら実行終了
- IPv4/v6の両方をサポート


## Requirements
- gcc


## Installation
`$ make`


## Usage
`$ ./chat [Destination hostname]`


## License
ご自由にお使いください．


## Author
[haru90](https://github.com/haru90)
