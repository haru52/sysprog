# UNIX System Programs

## Overview

大学の UNIX システムプログラミングの講義課題です。  
C 言語で書かれた UNIX システムプログラムです。

- myFTP：簡易FTPサーバ／クライアント
- chat：1対1のチャットプログラム
- mycp：自作cp
- udpecho：UDPを用いたechoサーバ／クライアント
- Dijkstra's Shortest Path First Algorithm：ダイクストラ法による最短経路探索アルゴリズム
- mysh：自作シェル（実装中）

## Description

### myFTP

- 簡易的なFTPサーバ／クライアント
- 機能
  - ファイルの送受信
  - クライアント側でpwd・cd・lsに相当するコマンドを実行可能（サーバ／クライアント両方を対象に実行可能）
  - IPv4/v6対応

### chat

- 1対1のチャットプログラム
- 機能
  - 1つのプログラムで動作
  - UDP，select()を使用
  - IPv4/v6対応

### mycp

- 自作cp
- 機能
  - ファイルのコピー
  - 上書き確認

### udpecho

- UDPを用いたechoサーバ／クライアント
- クライアントがサーバに送信した文字列をサーバがそのまま返し，クライアントは受信した文字列を表示
- udpechod: UDP echo server
- udpecho: UDP echo client

### Dijkstra's Shortest Path First Algorithm

- ダイクストラ法による最短経路探索アルゴリズム

### mysh (WIP)

- 自作シェル
- 以前作ったものを再実装中
- 機能
  - コマンド実行
  - リダイレクト
  - パイプ（多段パイプ可能）（実装中）
  - Ctr+Cでフォアグラウンドプロセスを終了（実装中）
  - バックグラウンド実行（実装中）

## Author

[haru](https://haru52.com/)
