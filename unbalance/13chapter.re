= 二値分類モデルの予測値

== 予測値と正例の割合

前章の二値分類モデルの評価では、ROCでもRPでも予測値の順序が正例である可能性の高さの順序になっているかを確認するものであることを見た。
しかし、予測値がそのままズバリ確率値になっていると何かと都合が良い。
例えば、あるユーザに対して二つのうち一つのサービスを訴求するとき、それぞれのサービス加入予測モデルを使って予測値の高い方を訴求するということを考える。
このとき、両方のモデルから得られる予測値がそのまま加入確率を示していると、単純に予測値の高い方を訴求すればそのまま加入確率の高いサービスを訴求することになる。
しかし、予測値の補正が不十分で予測値の大小と加入確率の大小で逆転している場合、予測値の大きい方を選択すると実は加入確率が低い方を選択してしまっていたということが起こりうる。
この状況を表したのが@<img>{corr}の相関図である。
@<img>{corr}は各サービスの予測値と、その予測値をもつデータ集合の正例の割合をプロットしたものである。
サービスAとサービスBでは傾きが異なっているため、あるユーザに対しては予測値の大小と正例割合の大小が逆転していることが起こりうる。
二値分類モデルごとに学習度合いが異なるとこういったことが起こりやすいため予測値の補正が必要になる。

//image[corr][予測値と正例割合の相関図][scale=0.7]{
//}

== 相関図の原理

二値分類モデルによる予測値の出力は多くの場合で0から1の範囲で与えられる。
もし予測値がこの範囲を超えていても、線形な変換で順序を変えずに0から1に限定することができるとする。
一方、目的変数は正例なら1、負例なら0であるとする。
これにより、データ全体の目的変数の平均は正例の割合と一致する。

@<img>{predict_hist}のように予測値の分布、つまりヒストグラムを考えると、正例か負例かで二つの分布を考えることができる。

//image[predict_hist][予測値の分布][scale=0.7]{
//}

ヒストグラムを重ねて表示することにより相関図の意味がより鮮明になる。
すなわち、同じ予測値における正例と負例の確率密度の比が正例の割合となる。
ここでは神様のように正例負例を0か1でずばり当てるのではなく、同じ予測値を持っているデータについては、ベルヌーイ試行のように確率的に正例か負例かに振り分けられると解釈できる。

@<img>{corr_hist}では、ヒストグラムから相関図が得られる様子を示している。
正例の予測値の最小値より小さい予測値に対しては、全てのデータは負例である。
すなわち、その範囲の予測値では正例割合は0である。
一方、負例の予測値の最大値よりも大きい予測値では正例割合は1である。
そして、予測値が正例の予測値の最小値と負例の予測値の最大値の間にある場合は確率密度の比に従って正例割合が決まる。

//image[corr_hist][予測値の分布と相関図][scale=0.7]{
//}

正例負例の予測値の分布がそれぞれパラメトリックに数式で得られる場合、Trainに含まれない説明変数でも正例負例の確率密度を計算してその割合を見積もることができる。

Validに対してもこのようなことを行えば、確率値に補正された予測値を得ることができる。

=== グラフィカルモデルによる説明
上記の説明をグラフィカルモデルに落とし込むと@<img>{graph}のようになる。
説明変数Xや目的変数Tはそれぞれ、TrainとValidに分割することができ、ValidをX'、T'などと表す。@<img>{graph}の他のシンボルは@<table>{params_graph}にまとめてある。

//image[graph][確率値に補正された予測値を得るグラフィカルモデル][scale=0.7]{
//}

//table[params_graph][グラフィカルモデルのパラメータ]{
シンボル	名称	説明
------------
X、X'	説明変数	二値分類モデルに投入する変数
T、T'	目的変数	二値分類の正解ラベル。正例：１、負例：０
M	二値分類モデル	LightGBMやLogistic回帰などの二値分類モデル
S、S'	二値分類予測値	二値分類モデルによって得られる予測値
C	予測値分布パラメータ	正例負例それぞれの予測値分布を記述するパラメータ
Y、Y'	正例割合予測	二値分類モデル予測値に応じた正例割合の予測値
//}

==== データセット
@<img>{graph}の中の@<img>{dataset}で表された部分は入力となるデータセットである。
X'、T'は二値分類モデルの学習前に分けたValidである。

//image[dataset][Train（左）、Valid（右）][scale=0.3]{
//}

==== 二値分類モデル学習
@<img>{graph}の中の@<img>{train_model}で表された部分は、説明変数Xと目的変数Tにより二値分類モデルMを学習することを示している。

//image[train_model][モデル学習][scale=0.3]{
//}

==== 予測値算出
@<img>{graph}の中の@<img>{cal_score}で表された部分は、説明変数Xと二値分類モデルMにより二値分類の予測値Sを算出することを示している。
モデルMを用いてテスト用データのX'からS'も算出できる。

//image[cal_score][予測値算出][scale=0.3]{
//}

==== 予測値分布パラメータ算出
@<img>{graph}の中の@<img>{cal_param}で表された部分は、二値分類予測値Sと目的変数Tにより予測値分布のパラメータCを算出することを示している。パラメータの中身は予測値の分布を何に当てはめるかによって変わる。
正規分布なら平均と分散、ベータ分布ならalphaとbetaである。

//image[cal_param][予測分布パラメータ算出][scale=0.3]{
//}

==== 正例割合算出
@<img>{graph}の中の@<img>{cal_ratio}で表された部分は、二値分類予測値SとパラメータCにより予測値Sに応じた正例割合Yを算出することを示している。
また、Validの予測値S'についても、パラメータCを用いて正例割合Y'を算出できる。

//image[cal_ratio][正例割合算出][scale=0.3]{
//}

予測値分布の確率密度が計算できれば後は次の式で正例割合を算出できる。

//texequation{
Y = \frac{N_p\times Dist_p}{N_p\times Dist_p + N_n\times Dist_n}
//}

//table[Dist_fit][正例割合算出での変数]{
シンボル	名称
------------
N@<sub>{p}	Trainの正例の数
N@<sub>{n}	Trainの負例の数
Dist@<sub>{p}	正例に対する予測値Sでの確率密度
Dist@<sub>{n}	負例に対する予測値Sでの確率密度
//}

正例負例それぞれの予測値分布について、パラメータCによる分布と実際の分布の当てはまりが良ければ、最終的にTとYは近い値になっているはずである。
また、Validの正解T'がわからなくても、X'からY'を算出し、TとYの関係からT'の予測を行うことができる。

上記の手法で、TrainのXから二値分類モデルと予測値分布の二段階で正例の割合を予測することができる。
もちろん、それぞれの段階で過学習の問題があるので、モデリングを多く挟む分精度や信頼性が失われていると考えられるが、予測値を確率値として補正した代償だと言える。
どの程度の精度低下が起こるかはTrainとValidの比較によって相対的に評価する。

== 誤差評価
=== 二乗誤差
どれだけ線形かを示す指標として、二乗誤差が便利である。
ここでは予測値=正例割合と仮定してそこからどれだけずれているかをRMSE（rooted mean squared error）で考える。
@<img>{pred_error}のように予測値から期待される正例割合Sと、実際の正例割合Yの差の二乗を平均して平方根をとるという計算である。
これにより、予測値と実際の正例割合がどの程度ずれているかを把握できる。

//texequation{
RMSE = \sqrt{\frac{(S_1-Y_1)^2+(S_2-Y_2)^2 ,,, (S_n-Y_n)^2}{N}}
//}

//image[pred_error][二乗誤差][scale=0.6]{
//}

=== Logloss
二値分類モデルを学習するときに損失関数としてよく用いられるのがLoglossである。
データ点が複数ある時には単純に和をとる。
ただし、0log(0)=0とする。
正例すなわちt=1では-tlog(y)だけが損失として算入され、負例すなわちt=0では-(1-t)log(1-y)だけが損失になることを確認してほしい。

//texequation{
Logloss=-t\times log(y)-(1-t)\times log(1-y)
//}

//table[Logloss_param][Loglossの変数]{
シンボル	名称
------------
t	正解ラベル。正例：1、負例：0
y	予測値。0から1の実数
//}

説明変数が同じであるために予測値が同一になると考えられる集団があったとすると、二項分布の最尤推定を行うことで予測値＝確率値となる。
これは形式的にはLoglossを最小化することと同等である。
小難しい言い方だと、二項分布の最尤推定である。
二値分類予測の損失関数としてLoglossが用いられることが多いのはこういった理由がある。

一方で、正例と負例で同じ重みでLoglossが計算されるので負例に大きく偏っていると、負例の予測値を0に近づけるだけでLoglossが改善されていく。
このため、Loglossを損失関数にしている二値分類モデルでは不均衡データの学習が難しい。
ある程度負例をしてしまうと、Loglossが小さくなってしまい学習の勾配がつかなくなるためである。
このようにLoglossで最終的な評価を行うには注意が必要である。

=== 二乗誤差とLoglossの比較
では、二乗誤差とLoglossのどちらを使えばよいか。
答えは以下のとおりである。

 * 二値分類の学習にはLogloss
 * 予測値の補正には二乗誤差を最小化

@<img>{corr_plot}ではLoglossとRMSEが0になるときのイメージを図示している。

//image[compare_error][RMSEとLoglossが0になるとき]{
//}

左は正例には1、負例には0という予測値を与えて誤りなくズバリ二値分類ができている状態である。この時、LoglossもRMSEも0になる。
中央は予測値と正例割合が等しいが、予測値にはばらつきがあるという状況である。
この時、Loglossは正の数になるが、RMSEは0である。
右は予測値と正例割合が等しくないという状況である。
この時、Logloss、RMSEは共に正の数になる。

二値分類の学習においては、正例と思われるデータにはなるべく高い予測値を与え、負例と思われるデータには低い予測値を与えるようにLoglossを用いて学習する。
予測値を補正する場合は、@<img>{corr_plot}のようにデータ点を対角線に乗せるのが目標となるためRMSEを使う。

予測値が確率値として欲しいのであれば、初めからRMSEで学習すればよいかと思う人もいるだろう。
今回は検証していないが良いアイディアかもしれない。

== 評価例
前章のSantanderの例で、予測値の補正を行ってみる。

=== 予測値と正例割合の相関図
まず初めに、LightGBMから出てきた予測値と正例の割合の相関を直接見てみる。
予測値を0.01刻みに分割し、その中の正例をカウントすることで@<img>{corr_plot}のような相関図を得ることができる。
前章で見たように、この予測値はAUCがTrainに対して0.9以上あり、かなり学習できているように思われるが、相関図では予測値=正例割合となる対角線からは大きく離れている。
この乖離を無くすのが本章での目的である。

//image[corr_plot][予測値と正例割合の相関図][scale=0.6]{
//}

=== 予測分布の視覚化
予測値を正例負例に分けて分布を視覚化すると@<img>{score_hist_train}のようになる。
この分布は確率変数が0から1の範囲に限定されていて、形状はベータ分布によく当てはまりそうに見える。

//image[score_hist_train][予測値分布][scale=0.6]{
//}

=== ベータ分布による予測分布の近似
ベータ分布は数式によりパラメトリックに算出できるので、ある予測値Sにおける正例の予測割合は次のように計算できる。
@<table>{beta_fit}に変数をまとめた。

数理統計の計算により、ベータ分布のパラメータと平均と分散の関係は以下のように表される。

//texequation{
e = \frac{a}{a+b}
//}

//texequation{
v = \frac{ab}{ (a+b)^2 (a+b+1)}
//}

これをalpha, betaについて解くことで、平均と分散からalpha, betaが簡単に得られる。

//texequation{
a = \frac{e^2 (1-e)}{v}-e
//}

//texequation{
b = \frac{(1-e)}{e}a
//}

//table[beta_fit][ベータ分布近似の変数]{
シンボル	名称
------------
e	予測分布の平均、正例負例それぞれにある
v	予測分布の分散、正例負例それぞれにある
a	ベータ分布の母数alpha、正例負例それぞれにある
b	ベータ分布の母数beta、正例負例それぞれにある
//}

alpha, betaと予測値Sがあればベータ分布の確率密度を算出することができる。

a、bを使ってベータ分布の乱数をTrainの数だけ発生させると@<img>{beta_random_hist}のようになる。
0.1未満や0.9以上の分布の形は若干異なっているものの、@<img>{score_hist_train}によく似ている。

//image[beta_random_hist][予測値の近似][scale=0.6]{
//}

=== ベータ分布による正例割合の補正

@<img>{beta_random_hist}のように得られた正例負例の確率密度から、次のように各々の予測値における正例の割合を計算することができる。

//texequation{
Y = \frac{N_p\times Beta_p}{N_p\times Beta_p + N_n\times Beta_n}
//}

//table[beta_cal][ベータ分布による補正の変数]{
変数	名称
------------
N@<sub>{p}	Trainの正例の数
N@<sub>{n}	Trainの負例の数
Beta@<sub>{p}	正例に対する、母数a,b、予測値Sでの確率密度
Beta@<sub>{n}	負例に対する、母数a,b、予測値Sでの確率密度
//}

上式により予測値Sは@<img>{score_cal_score_plot}のように正例の予測割合Yに変換される。

//image[score_cal_score_plot][元の予測値と正例割合の予測値の対応][scale=0.6]{
//}

結果として予測値を正例割合に補正した相関図が@<img>{cal_score_ratio_plot}である。
@<img>{corr_plot}と同様に予測値を0.01刻みでまとめてある。
補正前の@<img>{corr_plot}と比較してみると、補正した@<img>{cal_score_ratio_plot}では予測値が正例割合をうまく予測できていることがわかる。

//image[cal_score_ratio_plot][正例割合の予測値と正例割合の相関図][scale=0.6]{
//}

補正前後の指標を@<table>{loss_post}にまとめてある。
補正後にRMSEが大きく下がっていることがわかる。

//table[loss_post][補正前後のRMSE]{
データ	補正前RMSE	補正後RMSE
------------
Train	0.266	0.042
Valid	0.261	0.032
//}

長々とやって得られた正例割合の予測値がどのように役立つのかを、次章のバンディット問題で見てみたい。
