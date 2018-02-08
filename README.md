# UNIX System Programs

## Overview
C言語のUNIXシステムプログラムです．

- mycp：自作cp
- chat：1対1のチャットプログラム
- udpecho：UDPを用いたechoサーバ／クライアント
- mysh：自作シェル（実装中）


## Description
### mycp
- 自作cp
- 機能
  - ファイルのコピー
  - 上書き確認

### chat
- 1対1のチャットプログラム
- 1つのプログラムで動作
- UDP，select()を使用
- IPv4/v6の両方をサポート

### udpecho
- UDPを用いたechoサーバ／クライアント
- クライアントがサーバに送信した文字列をサーバがそのまま返し，クライアントは受信した文字列を表示
- udpechod: UDP echo server
- udpecho: UDP echo client

### mysh（実装中）
- 自作シェル
- 以前作ったものを再実装中
- 機能
  - コマンド実行
  - リダイレクト
  - パイプ（多段パイプ可能）（実装中）
  - Ctr+Cでフォアグラウンドプロセスを終了（実装中）
  - バックグラウンド実行（実装中）


## Author
[haru90](https://github.com/haru90)
