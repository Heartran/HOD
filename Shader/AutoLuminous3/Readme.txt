
--------------------------------------------------------

　　　　　　　　　　 発光エフェクト
　　　　　　　　　AutoLuminous Ver.3.6

　　　　　　　　　　　製作：そぼろ

--------------------------------------------------------

KeyLuminous、ObjectLuminous に続く発光エフェクト第３弾です。
指定した箇所が発光しているように描画することができます。


■ 動作環境　-------------------------------------------

シェーダモデル3.0および
浮動小数点テクスチャのミップマップをサポートする
グラフィックボード（DX10世代以降を推奨）

MikuMikuEffect 0.27 およびそれが動作する MikuMikuDance
MikuMikuEffect x64 0.32 および MikuMikuDance x64
MikuMikuMoving 1.1.8.1 以降

テスト環境：
　Win7 x64 + Phenom II X6 + GTS450
　Win7 x64 + Core i7 2677M + Intel HD Garphic


■ 使い方　----------------------------------------------

□ 発光部分の指定　----------------------------

このエフェクトではモデルまたはアクセサリ改造によって
発光部分を指定します。

指定方法として、光らせたい素材のスペキュラ色（反射色）を黒(0,0,0)に、
スペキュラ強度（反射強度）を100以上に設定します。
この組み合わせは通常使用されることはないので
意図しない発光が起こる可能性は低いです。

スペキュラ強度の標準値は110です。
とりあえず光らせたい場合はこの値を使ってください。
これより上下すると、スペキュラ強度から100をマイナスした値に比例して
発光強度も上下します。

発光色は、ディフューズ色とアンビエント色から自動的に決定されます。


□ MMDへの読み込み　----------------------------

上のように改造したモデルを読み込んだのち、
AutoLuminous.xを読み込んでエフェクトをロードすると、
指定部分が発光します。

MMMの場合、読み込むのはAutoLuminous.fxになります。


□ パラメータ操作　----------------------------

AutoLuminous.xのアクセサリとしての値を変更することで、
エフェクトのパラメータを操作できます。

動画内で常に一定の設定を使う場合はそのまま値を入力し、
動的に変化させる場合はダミーボーンへの取り付けを推奨します。


・Si: 発光強度

発光部分の光の強さを指定します。

・Tr: ぼかし強度

発光部分のぼかしの強さを指定します。
大きくすることはできません。
これは、もともとのぼかし強度がかなり大きく設定されているためです。
それでも大きくしたい場合はエフェクトの直接編集が必要です。

--------------------------

・X: 光芒方向数

グレアの伸びる方向の数を指定します。
1方向あたり、反対方向にも光が伸びることに注意してください。
例えば、3を指定すると6本の光芒が伸びることになります。
適正値は 0〜9 程度です。
数を増やすほど負荷が増えることに注意してください。
また、増やすと意図したものより明るくなる場合があります。

・Y: 元画面輝度

元スクリーン画面の明るさを増減させます。
適正範囲は -100〜100 程度です。
-100にすると元スクリーンは完全に真っ黒になります。

・Z: 発光強度

値の指定方法が違うだけで、効果はSiと同じです。
適正範囲は -100〜500 程度です。
-100でSi=0と、100でSi=2と同じ効果があります。
ダミーボーンから発光強度を操作したい場合を想定しています。

--------------------------

・Rx: 光芒角度

指定した角度だけ、グレアの角度をずらします。

・Ry: 光芒長さ

グレアの長さを増減させます。
適正範囲は -100〜100 程度です。
あまり長くすると縞が現れてしまいます。

・Rz: 点滅周期

発光を点滅させます。入力した数値がそのまま秒単位での点滅周期になります。
正の数だとサイン波で、負の数だと矩形波で点滅します。
0にすると点滅は止まります。



■ 使い方の拡張　----------------------------------------------

□ ObjectLuminous互換機能　--------------------------

AutoLuminousはObjectLuminousと同様の、
以下のような使用方法が適用できます。

・セレクタエフェクトの使用
・個別素材への単色エフェクトの適用
・MMEタブからの材質の「解除」

それぞれの詳細はObjectLuminousのReadmeおよび各種解説をご覧ください。
ObjectLuminous用のセレクタエフェクトはそのまま使うことができます。


□ KeyLuminousとの互換機能　-------------------------

KeyLuminousと同じように、画面上のキーカラーを指定して
発光させることができます。
エフェクトの編集が必須なので、詳細は上級編にて解説します。


□ エフェクトの直接編集　----------------------------

いくつかのパラメータは変更するためにエフェクトを
直接編集する必要があります。
これにはある程度のエフェクトの知識が必要です。

それらのパラメータには、
ぼかし範囲、サンプリング数、背景色などがあります。
各項目の説明はエフェクトファイル中に記載されています。

特にサンプリング数については、
フルHD級の大解像度で出力する場合、
標準のサンプリング数では全く足りずに縞が出てしまうので、
値を大きくすることが必須となります。
ただしサンプリング数を増やせば当然重くなります。


□ 追加UVによる頂点ごとの発光制御　-------------------

PMXに限り、追加UV機能を用いることで、より柔軟に
頂点ごとの発光制御を行うことができます。
詳細は上級編にて解説します。


■ サンプル　--------------------------------------------------

Sampleフォルダに入っています。

・サンプル１
4色に塗り分けられたボールです。
このボールのうち、青とピンクの部分が発光部分として設定されています。

・サンプル２
PMD版サンプルです。
こちらは緑と黄色が発光色に指定されています。
モーフによる表情制御を試すことができます。

・サンプル３
PMX版サンプルです。灰色の立方体です。
頂点発光などの2.0からの新機能を試せます。

・サンプル４
PMX版サンプルです。複数の板ポリです。
頂点発光での3.0からの新機能を使用した色相モーフなどを試せます。
また、材質モーフによる発光強度変更もテストできます。

・サンプル５
PMX版サンプルです。螺旋状に並んだボールです。
AutoLuminouSetterを利用した複雑な頂点発光を試せます。


■ 軽量エフェクト「AutoLuminousBasic」　-----------------

AutoLuminousBasicは、AutoLuminousが重くて動かない場合に、
最小限の機能だけを使えるようにする軽量版エフェクトです。
大幅に軽くなっていますが機能、描画クオリティとも制限され、
他エフェクトとの連携もできません。
クオリティはおおよそObjectLuminous相当です。

できるのはAL対応モデルを簡単に発光させることだけと考えてください。


■ 対応モデルの配布　------------------------------------------

スペキュラを特殊仕様にしてAutoLuminousに対応させたモデルは、
Readmeなどに「AutoLuminous対応」と書いていただけると
ユーザビリティの向上になるかと思います。

また、その際は無改造のAutoLuminousを読み込んだデフォルト状態で
違和感のない発光強度に設定することを推奨します。

また、2.0、3.0からの新機能を使用した場合は、AutoLuminousの
バージョンアップを促す文面の追加を推奨します。

ただし、これらは一切強制するものではありません。


■ トラブルシューティング　------------------------------------

　上から順番に試してみましょう。

・エラーが出て動かない
　→MMEがVer.0.27以上であることを確認してください
　→ダウンロードし直してみてください
　　→残念ながらグラボが対応していない可能性が高いです

・エラーは出ないが正常に動かない
　→サンプルとAutoLuminousのみの状態で正常に動くか確認してください
　→AutoLuminous.xの位置やサイズなどが異常な値になっていないか確認してください
　→ダウンロードし直してみてください
　　→残念ながらグラボが対応していない可能性が高いです
　　→バグが疑われる場合はTwitterなどに報告を下さい

・材質モーフが反映されない
　→MMEの仕様でモーフ後の色が正常に取得できないためです。
　　Ver.3.5よりスペキュラ強度とアルファは反映されるようになりました。
　　MMMでは材質モーフが使えます。

・重すぎてまともに作業できない
　→エフェクトを編集してHALF_DRAWを1にしてみてください。
　　画質は落ちますが軽くなります。
　→エフェクトを編集してHDR_RENDERを0にしてみてください。
　　機能は制限されますが軽くなります。
　→サンプリング数を減らしてみてください。
　→AutoLuminousBasicを試してみてください。
　→ObjectLuminousやKeyLuminousでの代用を検討してください
　　→PCやグラボを買い替える

・光らせたくない部分が勝手に光る
　→加算スフィアが光る場合、そのモデルにOptionsフォルダの
　　full_saturate.fxを適用してください
　→MMEの割り当てタブでAL_EmitterRTを選び、
　　光らせたくないモデルに"AL_BlackMask.fx"を割り当てます。
　→エフェクトを編集してHDR_RENDERを0にしてみてください。
　→KEYCOLOR_NUMを0にしてキーカラー発光機能をオフにしてみます。

・アクセサリの編集はどうするの
　→Xファイルはメモ帳で開いて数値を直接いじれます
　→・・・というのはあんまりなので、Readme上級編に
　　PMDEでのアクセサリ編集について追加しました。

・Readme上級編が意味不明なんだけど
　→仕様です
　

■ 更新履歴　--------------------------------------------------

Ver.3.6
・LightSamplingのSiが効いてないバグ修正

Ver.3.5
・材質モーフによるアルファ変化に対応

Ver.3.4
・同梱プラグインアップデート
・サンプル４に材質モーフ例を追加
・パラメータ微調整
・テストモード機能強化

Ver.3.3
・テクスチャを使用したモデルの発光に関する重大なバグ修正

Ver.3.2
・MMMで一部パラメータが反映されなかった問題修正

Ver.3.1
・頂点発光設定用のPMDEプラグインを同梱
・環境によって正常に動作しないバグ修正
・光芒のぼかしおよび明るさ調整
・頂点発光機能のバグ修正
・LightSampling.fx対応
・少し軽量化
・色調調整
・XML同梱

Ver.3.0
・材質モーフによる発光強度変化に対応（色は未対応）
・頂点発光機能の大幅強化
・光の広がりを大きく
・軽量版AutoLuminousBasicを同梱
・色合いなどのパラメータ調整
・細かいバグ修正

Ver.2.0
・頂点発光機能の追加
・テクスチャ発光の異方性フィルタリング対応（初期値では無効）
・対応モーフの追加
・残光機能の追加（初期値では無効）
・加算スフィア対策版full.fx同梱
・細かいバグ修正
・MikuMikuMoving仮対応

Ver.1.2
・YとZの値を、ダミーボーンからの操作を受け付けるように

Ver.1.1
・"LightBlinkS"がMMD上の表示に収まらないため、"LightBS"に改定
・色を調整
・ドキュメント改訂

Ver.1.0
・正式公開
・キーカラー発光機能を追加
・点滅方式の切り替え可能に
・マスク機能追加
・表情による個別発光制御機能追加
・オプションスイッチの整備
・HDRのブルームもSiの影響下に変更

Ver.0.1
・仮公開


■ 利用条件　--------------------------------------------------

商用利用、改変、再配布を含むいかなる利用も自由です。
また、作者の許諾も不要です。
ただし、このエフェクトを利用したことによるどのような損害も
責任を負うことはできません。自己責任でご利用ください。

mylist:  http://www.nicovideo.jp/mylist/17392230
twitter: sovoro_mmd



