# AviUtl-RPC 
AviUtlで編集時などに、Discordにステータスを表示する(RPC)プラグイン

# 機能
アイドル時、編集時、エンコード時にDiscordのユーザーの欄のところに現在のステータスを表示します。
それに加え、現在のファイル名、経過時間も表示します。

# 使い方
GitHubのReleases（パソコンなら右カラム）から最新リリースを選択して、`Assets`のうちのaviutl_rpc_[バージョン名].zipを選択してダウンロードして展開します。  
AviUtlの`aviutl.exe`と同じディレクトリに`aviutl_rpc.auf`と`discord_game_sdk.dll`を移動してください。  
インストール時にはAviUtlの再起動が必要です。  
インストールされるとAviUtlの`表示`メニューに`AviUtl Discord RPCの表示`が追加されていますので、そこでプラグインの有効、無効を設定できます。  
有効、無効の状態はプロジェクトファイルごと（本当はおそらくプロファイルごと）に保存されます。以前プラグインをインストールせずに編集していたファイルは既定で無効になっています。  
何も編集ファイルを開いていない状態で`AviUtl Discord RPCの表示`から`有効にする`のチェックボックスにチェックを入れると、今後作成するファイルは既定で有効になります。（既定で無効にしたい場合はその逆です。）

# エラー等が発生したら
何か問題が発生したとき、あるいはAviUtlでエラーが発生したと表示されたときのうち本プラグインに起因すると思われる問題はGitHubのIssueに投稿をお願いします。  
それが出来ない場合は、[こちら](http://me.scrpg.tyanoyu.net)の連絡手段のいずれかを使ってご連絡いただければ幸いです。

# 機能要望
機能要望も上のIssueにお願いします。

# 自前でビルド
## Visual Studioを使う場合
`AviUtl-RPC.vcxproj`をVisual Studioで開いてビルドを実行するだけ  
ただしプロジェクトディレクトリの直下に`Binaries`ディレクトリを作成してDiscord Developer Portalから入手できる`Discord Game SDK`に含まれている`discord_game_sdk.dll`と`discord_game_sdk.dll.lib`が必要です。個人でダウンロードしてください。

## 使わない場合
マルチバイト文字セットを使用してください。  
モジュール定義ファイルは`main.def`を指定し、依存ライブラリに上で説明した`discord_game_sdk.dll.lib`を指定してください。

# ライセンス・免責事項
リポジトリの[LICENSE.md](LICENSE.md)を参照  
（リリースパッケージにも同封）

# 著作権表示
(c) mtripg6666tdr
