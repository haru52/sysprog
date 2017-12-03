# UNIX System Programs

## Overview
C言語のUNIXシステムプログラムです．

- mycp：自作cp
- mysh：自作シェル（再実装中）
- udpecho：UDPを用いたechoサーバ／クライアント


## Description
### mycp
- 自作cp
- 機能
  - ファイルのコピー
  - 上書き確認

### mysh
- 自作シェル
- 以前作ったものを再実装中
- 機能
  - コマンド実行
  - リダイレクト
  - パイプ（多段パイプ可能）（実装中）
  - Ctr+Cでフォアグラウンドプロセスを終了（実装中）
  - バックグラウンド実行（実装中）

### udpecho
- UDPを用いたechoサーバ／クライアント
- クライアントがサーバに送信した文字列をサーバがそのまま返し，クライアントは受信した文字列を表示します．
- udpechod: UDP echo server
- udpecho: UDP echo client


## Author
[haru90](https://github.com/haru90)
