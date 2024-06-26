# ゲームコントローラーC++ライブラリ/インターフェース for Linux
## 概要
Pad.hpp/.cppはゲームコントローラー（ジョイスティック）のデータを取得し，
それをプログラムで使用するためのクラス（プログラミングインターフェース）を提供する．

ライブラリで提供される機能等はすべて名前空間***Pad***内に実装，
コントローラー関連のクラス（***PadReader***/***PadData***），加えてこれらがデータの取得に必要とする
変数等がすべて含まれる．

## クラス・その他データの概説
### PadReader クラス
***PadReader***クラスは，Linuxで認識されているコントローラーのデバイス名を
指定することで，それに対応するデバイスファイルを通してコントローラーのイベントデータを読み取り取得する．
イベントデータは以下の要素により構成されている．
- イベントの種類
  - ボタンのアクション
  - スティック等の可変データ関連のアクション  
- イベントの番号
  - 一意に割り当てられているボタン・スティックの識別番号 
- イベントの値
  - ボタンのON/OFF
  - スティックの座標値・ボタンの押し込み深度

デバイス名はコンストラクタで引数として与える．
これに加えてファイルの読み取りをブロッキング・ノンブロッキングで行うかを指定できるが，
引数で与えない場合はデフォルトでブロッキングを指定する．まだ機能のテストを十分行っていないため
この引数としてノンブロッキングモードを指定することは非推奨．
### PadData クラス
 ***PadData***クラスは，データを保持するコンテナ（配列）を持ち，
 コントローラーのデータの変更や取得を行える．ボタンのデータはbool，スティック関連の可変値データは
 符号付き16Bit整数の配列である．ボタンデータはPush, Releaseの状態変化を外部から扱えるようにするため，
 最新の状態とその一つ前の状態を保持する配列を実装している．

 データの更新と取得はsetter,getterで行う．
