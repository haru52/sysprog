# myFTP

## Overview

簡易的なFTPサーバ／クライアントプログラムです．

## Description

- ファイルの送受信
- クライアント側でpwd・cd・lsに相当するコマンドを実行可能（サーバ／クライアント両方を対象に実行可能）
- IPv4/v6対応

## Requirements

- gcc

## Installation

`$ make`

## Usage

### Server (myftpd)

`$ ./chat [current directory (option)]`

### Client (myftpc)

`$ ./myftpc [server hostname]`

- myftpc実行中，`myFTP% help` で，ヘルプを閲覧できます．

## Author

[haru](https://haru52.com/)
