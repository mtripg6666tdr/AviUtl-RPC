# AviUtl-RPC 
![Discordに表示される画面](https://me.myon.ga/assets/AviUtl-RPC.png)  
![設定画面](https://user-images.githubusercontent.com/56076195/148711806-cb1e30c2-f12c-46a1-a035-78ae4891bb48.png)  
AviUtlで編集時などに、Discordにステータスを表示する(RPC)プラグイン

## 機能
アイドル時、編集時、エンコード時にDiscordのユーザーの欄のところに現在のステータスを表示します。
それに加え、現在のファイル名、経過時間も表示します。こちらは設定で無効にすることもできます。

## インストール
1. GitHubのReleases（パソコンなら右カラム）から最新リリースを選択して、`Assets`のうちのaviutl_rpc_[バージョン名].zipを選択してダウンロードします。  
2. ダウンロードされたzipファイルを展開します。  
3. AviUtlの`aviutl.exe`と同じディレクトリに`aviutl_rpc.auf`と`discord_game_sdk.dll`を移動します。すでにAviUtlを起動している場合は再起動をします。  

## 使用方法
インストールされるとAviUtlの`表示`メニューに`AviUtl Discord RPCの表示`が追加されていますので、そこでプラグインの有効、無効を設定できます。  
有効、無効の状態はプロジェクトファイルごと（本当はおそらくプロファイルごと）に保存されます。以前プラグインをインストールせずに編集していたファイルは既定で無効になっています。  
何も編集ファイルを開いていない状態で`AviUtl Discord RPCの表示`から`有効にする`のチェックボックスにチェックを入れると、今後作成するファイルでは既定で有効になります。（既定で無効にしたい場合はチェックを外します。）  
ファイル名の表示/非表示も同じ要領で設定することができます。

## エラー等が発生したら
何か問題が発生したとき、あるいはAviUtlでエラーが発生したと表示されたときのうち本プラグインに起因すると思われる問題はGitHubのIssueに投稿をお願いします。  
それが出来ない場合は、[こちら](http://me.scrpg.tyanoyu.net)の連絡手段のいずれかを使ってご連絡いただければ幸いです。

## 機能要望
機能要望も上のIssueにお願いします。

## 自前でビルド
`AviUtl-RPC.vcxproj`をVisual Studioで開いてビルドを実行するだけ  
ただしプロジェクトディレクトリの直下に`Binaries`ディレクトリを作成してDiscord Developer Portalから入手できる`Discord Game SDK`に含まれている`discord_game_sdk.dll`と`discord_game_sdk.dll.lib`が必要です。個人でダウンロードしてください。

## ライセンス・免責事項
リポジトリの[LICENSE.md](LICENSE.md)を参照  
（リリースのzipファイルにも同梱）

## 著作権表示
(c) mtripg6666tdr
