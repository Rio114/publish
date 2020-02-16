
= 二値分類の評価

== 二値分類とは

二値分類とは、それぞれのデータ点のある属性が正であるかか負であるかを分類もしくは予測するものである。
例えば、以下のような状況である。

//quote{
ある疾患に罹患しているかを推定するため、血液検査によって数値Aを測定する。数値Aが予め決められた閾値より大きければ罹患していると推定、小さければ罹患していないと推定する。
//}

この検査を二値分類の用語に当てはめると以下のようになる。

 * 正と予測：罹患していると推定
 * 負と予測：罹患していないと推定

多くの場合、二値分類では正負を誤り無しで分類することができない。
疾患の例では体質や環境のばらつきのため、特に閾値近辺では誤りが発生しやすい。
二値分類ではある程度の誤りが発生してしまうことは許容して、それが全体に対してどの程度なのかを評価して実務に適用してくことが求められる。

二値分類においては予測の誤りは二種類にわけられる。
冒頭の疾患の例で言うと、以下のような状況である。

 * 誤検知：罹患だと推定したにもかかわらず罹患していなかった
 * 見逃し：罹患していないと推定したにもかかわらず罹患していた

当然ながら、誤検知も見逃しも可能な限り減らしたい。
これらの誤りはトレードオフであり、検査を緩くすることで誤検知は減るが見逃しが増え、検査を厳しくすることで見逃しは減るが誤検知が増えると考えられる。
極端な例では、全て罹患していると診断すると見逃しはなくなり、全て罹患していないとすれば誤検知はなくなる。
しかし、これらがナンセンスであることは誰の目にも明らかだろう。

もし、先ほどの検査で以下の条件が付いたら、どのように数値Aの閾値を決めたら良いだろうか。

//quote{
その疾患は潜伏期間が長く、すぐには症状が現れない。
しかし、症状が現れた時には既に治療が難しい状況になっていることが多い。一方で、罹患していると推定された人には高額な精密検査を受けて適切な治療を施すことにより発症を防ぐことができる。
//}

もし疾患が後から重篤化することがわかっていて早期発見でそれが防げるとしたら、見逃しは可能な限り避けなくてはならない。
一方、検査を厳しくすることで見逃しを減らすことができるが、誤検知を増やしてしまい、精密検査という不便を強いられることになる。
この場合、見逃しや誤検知に対する損失を定量的に評価できれば、ある閾値に対する全体の損失を予測できるようになり、その損失を最小にする閾値を設定することができる。

前置きが長くなったので話をまとめると以下のようになる。
以降ではこのことを前提に話を進めていきたい。

 * 通常の二値分類では誤検知や見逃しが発生する。
 * モデルを調節することにより、誤検知や見逃しの割合を調節できる。
 * 誤検知や見逃しは可能な限り減らしたいが、トレードオフなので両方を0にすることはできない。
 * 誤検知や見逃しによる損失を定量的に評価できれば、全体を最適化できる可能性がある。

== 混合行列

=== データの分割

二値分類を考えるとき、正負について二つの見方がある。
@<img>{split}のように、真実について分類する場合と予測について分類する場合である。

 * 真実で分類
 ** 正例：真実が正（Positive）
 ** 負例：真実が負（Negative）
 * 予測で分類
 ** 正予測：予測が正（Positive）
 ** 負予測：予測が負（Negative

//image[split][母集団の分割][scale=0.4]{
//}

=== 組み合わせによる分類

正例負例、予測の正負の組み合わせの2x2の4マスにデータ数を振り分けることで@<img>{conf_mat}のような表を作ることができ、これを混合行列（Confusion Matrix）という。
真実でも予測でも、正としたものをPositive、負としたものをNegativeとする。
さらに、予測が真実と同じであればTrue、予測が真実と異なればFalseとする。

//image[conf_mat][混合行列][scale=0.4]{
//}

例えば、負例に正の予測をしてしまうことを、False Positiveという。False Positiveは偽陽性や誤検知と言ったりもする。以降では用語に慣れるため、「誤検知（FP）」のようにTP、FN、FP、TNを適宜かっこ書きで示していく。

=== 混合行列の例

検査の例で考えると、閾値の設定次第で予測の分類が変わってくる。
例えば、以下のような架空のパターン@<table>{confusion_matrix_A}、@<table>{confusion_matrix_B}が考えられる。

 * パターンA：正と予測される数が少ない。
 * パターンB：正と予測される数が多い。

//table[confusion_matrix_A][パターンAの混合行列]{
..	正の予測	負の予測	合計
------------
正例	 90	10	   100
負例	100	9900	10000
合計	190	9910	10100
//}

//table[confusion_matrix_B][パターンBの混合行列]{
..	正の予測	負の予測	合計
------------
正例	99	1	100
負例	1000	9000	10000
合計	1099	9001	10100
//}

両方の混合行列では合計列が共に100、10000、10100という数字の並びになっていることを確認してほしい。
真実はいつも一つだからである。
一方、横方向の数字を見比べると、合計行の合計10100だが(190, 9910)、(1099, 9001)というように割合が異なる。
これは検査の設定次第で正と予測する数を調節できるからである。
パターンAは正例の正解が少ないものの、負例の正解は多い。
パターンBはその逆である。
この時点ではパターンAとパターンBのどちらが良いかはまだ判断できない。

== 評価指標

一般的に精度というと、正例は正、負例は負とどれだけ正確に予測できているかという指標を思い浮かべることが多いかもしれない。
しかし、不均衡なデータがあった場合、正負にかかわらず全て多数側であると予測を行うことで見かけ上高い正解率を達成できてしまう。
二値分類では何か特別なものをその他大勢から選別するという目的が多く、全てをその他大勢と言ってしまっては意味が無い。
そのため、二値分類では以下に示すように様々な指標を用いて評価を行う。
なお、日本語では分野によって異なる名称を使うことがあるので英語で覚えるのが良いと思う。

 * Accuracy
 * Precision
 * Recall
 * FPR
 * F1スコア

これ以降では先に挙げた二つのパターン@<table>{confusion_matrix_A}、@<table>{confusion_matrix_B}について評価指標がどのように計算されるかを示す。
それぞれ似たような数式で計算されるが、完全理解のコツは何を分母にして、何を無視しているかを意識することである。
F1スコア以外は全て、着目しているものの中の何割がどうであるか、という形になっていることを確認してほしい。

=== Accuracy

Arrucary（正解率）は一般的に言う精度であり、機械学習の文脈では正解率という。
Accuracyの定義は以下の通りである。

//texequation{
Accuracy = \frac{TP+TN}{TP+FN+FP+TN}
//}

//image[Accuracy][Accuracy（正解率）][scale=0.5]{
//}

正例は正、負例は負とということをどれだけ正確に言い当てているかという指標である。
前述の通り、不均衡なデータに対しては使うときには正負どちらかに引っ張られていないか注意が必要である。
Accuracyの計算例は以下の通りである。

//texequation{
Accuracy_A = \frac{90 + 9900}{10100} = 0.9891
//}

//texequation{
Accuracy_B = \frac{99 + 9000}{10100} = 0.9009
//}

正の予測が少ないパターンAでは、パターンBよりも負例を言い当てる割合が大きくなるため正解率が上がっている。
正例を90当てるか99当てるかよりも、負例を9900当てるか9000当てるかの方が正解率に対する影響が大きい。

=== Precision

Precisionは正予測の何割が正解であったかという、誤検知の小ささを示す指標である。
精度や適合率とも言う。
Precisionの定義は以下の通りである。

//texequation{
Precision = \frac{TP}{TP+FP}
//}

//image[Precision][Precision（正予測の言い当て率）][scale=0.5]{
//}

TP+FPという分母は正の予測の合計であり、分子は正の予測の中の正例の数なので、正予測の言い当て率だといえる。
パターンAのように厳選して正の予測を行うなら間違いは少なくなりPrecisionは1に近くなる。逆に多くのものを正だと予測すると、Precisionは元の集団に含まれる正例の割合に近づいていく。
Precisionの計算例は以下の通りである。

//texequation{
Precision_A = \frac{90}{90+100} = 0.4738
//}

//texequation{
Precision_B = \frac{99}{99+1000} = 0.0901
//}

正の予測が少ないパターンAでは正例を言い当てた数（TP）がパターンBよりも少ないが、Precisionで比較するとパターンAの方が大きくなっている。
負例を誤って正と予測してしまう例（FP）が少ないからである。

負と予測したものの中に正例（FN）がたくさんあったとしても、定義式にFNは含まれていないので、それはPrecisionからはわからない。

=== Recall

Recallは正例の何割を集めることができたという、見逃しの小ささを示す指標である。
再現率や感度とも言う。
Recallの定義は以下の通りである。

//texequation{
Recall = \frac{TP}{TP+FN}
//}

//image[Recall][Recall（正例の捕捉率）][scale=0.5]{
//}

TPR（True Positive Rate）や再現率とも言う。
TP+FNという分母は正例の合計であり、分子は正例の中で正と予測されたものであるため、正例の捕捉率だと言える。
見逃しをしたくないがために、何でもかんでも正の予測を行えばRecallは1に近づくが、誤検知（FP）が増えてPrecisionが下がる。
一方で、誤検知を減らすために、正の予測が少なくなるようにすると、見逃しが増えるためRecallが下がる。
Recallの計算例は以下の通りである。

//texequation{
Recall_A = \frac{90}{100} = 0.9000
//}

//texequation{
Recall_B = \frac{99}{100} = 0.9900
//}

Recallは分母が正例全体であるため閾値に依らず、この場合では100である。
正の予測が少ないパターンAでは、正例の言い当て（TP）も少なくなるためRecallはパターンBよりも小さくなっている。
また、正の予測の中には間違って正と予測した負例（FP）がたくさんあるかもしれないが、Recallの式には含まれていないので、FPはRecallからはわからない。

=== FPR
FPRは簡単に言うと負例の取りこぼし率である。
FPRの定義は以下の通りである。

//texequation{
FPR = \frac{FP}{FP+TN}
//}

//image[FPR][FPR（負例の取りこぼし率）][scale=0.5]{
//}

単独ではあまり使うことは無いが、モデルを評価するためROC曲線を描くために算出される。
Recallでは正例のうちの正の予測の割合であったが、こちらは負例のうちの正の予測の割合である。
見逃しを減らすようにRecallを上げる（＝正例の捕捉率を上げる）と負例を誤って正と予測してしまう割合が増える。
つまりFPRが上がる。

=== F1スコア

F1スコアはPrecisionとRecallの調和平均である。
F1スコアの定義は以下の通りである。

//texequation{
F1 = \frac{2\times Precision \times Recall}{Precision + Recall}
//}

逆数で表すと、

//texequation{
\frac{1}{F1} = \frac{1}{2}\times (\frac{1}{Precision} + \frac{1}{Recall})
//}

F1の定義式に対してPrecision、Recallの定義式を代入すると、以下のようになる。

//texequation{
\frac{1}{F1} = 1 + \frac{FP}{2\times TP} + \frac{FN}{2\times TP}
//}

誤検知（FP）と見逃し（FN）を減らすとF1スコアの逆数が小さくなる、すなわちF1スコアが大きくなる。
加えて、誤検知が1件増えるのと見逃しが1件増えるのはF1スコアに対して同等に作用するのが式から読み取ることができる。
従って、誤検知と見逃しの重要性に偏りがある場合にはF1スコアを用いてはいけない。
また、負例に偏っている不均衡データの場合、誤検知（FP）が見逃し（FN）よりも大きな数となるためPrecisionによる影響が大きいとも言える。

F1スコアの計算例は以下の通りである。

//texequation{
F1_A = \frac{2\times 0.4737\times 0.9000}{0.4737 + 0.9000} = 0.6406
//}

//texequation{
F1_B = \frac{2\times 0.0901\times 0.9900}{0.0901 + 0.9900} = 0.1638
//}

パターンAとパターンBを比較すると、Recallは10%程度の差異だがPrecisionは5倍を超える差異になっている。
その結果、Precisionの差異に引っ張られる形でF1スコアにも差異が見られる。

=== 評価指標の数値例
これまで見てきた数字を並べて見比べて、どういった特徴があるのかを見てみたい。
パターンA、パターンBはそれぞれ以下のような状況を想定していた。

 * パターンA：正と予測される数が少ない。
 * パターンB：正と予測される数が多い。

それぞれの評価指標をまとめたのが@<table>{compare}である。

//table[compare][パターンA・パターンBの評価指標比較]{
..	パターンA	パターンB
---------
Accuracy（正解率）	0.9891	0.9009
Precision（精度、正例の言い当て率）	0.4738	0.0901
Recall（再現率、正例の捕捉率）	0.9000	0.9900
FPR	0.0100	0.1000
F1	0.6406	0.1638
//}

 * パターンAでは正と予測する数を少なくしたにもかかわらず、AccuracyはパターンAの方が大きくなっている。
 負例の正解が大きい影響である。
 * PrecisionとRecallがトレードオフになっている。
 誤検知と見逃しのトレードオフを示している。
 * F1スコアはPrecisionとRecallから計算されるが、Precisionに引っ張られている。

これまで見てきた数値は二値分類の設定だけで機械的に得られる数値に過ぎず、大小に実務的な意味はない。
それを議論するためにはドメイン知識を使って何を重視するかを決めなくてはならない。

== 損失行列
これまで見てきたように、評価指標がいくつもあると結局何を見たらよいかという問題に直面する。
ここでは具体例を用いて何を重視するかを導き出す方法を考えたい。

損失行列は、混合行列の4つの事象に対してどれだけの損失（もしくは利益）が得られるかを表したものである。
それぞれの損失がどの程度になるかはドメイン知識が必要であるが、考え方は@<img>{loss_mat}のようにシンプルである。
TP、FN、FP、TNそれぞれに1データ当たりのコストや利益を先に見積もっておき、それぞれの数をかけて4つの数字を足しわせることで全体のコストもしくは利益が得られる。
ここではコストとしてマイナスの場合は損失を表し、プラスの場合は利益を表す。

//image[loss_mat][損失行列]{
//}

図のようにデータ点一つ当たりの損失を混合行列のセル一つ一つに定義してあれば、
セルごとにかけ合わせることにより、全体の損失がどれくらいになるかを簡単に計算できる。

=== 損失行列の例
==== 疾患の検査

とある疾患では治療が遅れることにより、治療のコストが飛躍的に増大する。
一方で、早期発見できれば治療のコストは小さい。
この疾患において検査という二値分類を考えた場合の損失行列を設計する。

この検査による一人あたりのコストを以下のように仮定する。

 * 検査で陽性となった場合は、真実がどうであれ再検査を受ける。再検査のコストは-10とする。
 * 再検査はコストをかけるので、確実な診断ができる。すなわち、精密検査ではFPもFNも0で正解率100％とする。
 * 再検査で罹患が確かめられた場合、治療に-90のコストを要する。
 * 真実は罹患しているにもかかわらず最初の検査で発見できなかった場合（FNの場合）、将来的なコストは-2000とする。
 * 罹患しておらず、検査でも陰性となった場合（TNの場合）、追加のコストは発生しない。

これらの条件を損失行列に落とし込むと@<table>{loss_matrix_deseace}のようになる。
例えばTPのコストは再検査と治療で-10+(-90)=100となる。

//table[loss_matrix_deseace][検査の損失行列（1データ当たり）]{
..	正の予測	負の予測
---------
正例	-100	-2000
負例	 -10	0
//}

損失行列の各数値は一人あたりの損失なので混合行列の各数値と掛け合わせることで@<table>{loss_matrix_deseace_A}、@<table>{loss_matrix_deseace_B}のように合計損失を求めることができる。

//table[loss_matrix_deseace_A][パターンAの検査の損失行列（全体）]{
パターンA	正の予測	負の予測	合計
------------
正例	-9000	-20000	-29000
負例	-1000	     0	 -1000
合計	-9000	-21000	-30000
//}

//table[loss_matrix_deseace_B][パターンBの検査の損失行列（全体）]{
パターンB	正の予測	負の予測	合計
------------
正例	 -9900	-2000	-11900
負例	-10000	    0	-10000
合計	-19900	-2000	-21900
//}

合計損失はパターンAで-30000、パターンBで-21900となる。
従って、損失（の絶対値）が小さいパターンBのほうが好ましいと言える。
この場合、見逃し（FN）に大きな損失が設定されているため、できるだけ見逃しをなくすという戦略が機能するためである。
言い換えると、見逃しの損失が大きい損失行列であるため、Recallが大きいパターンBが有利になる。

==== 広告の配信

あるサービス利用者に新サービスに関する広告の配信を考えていて、利用者ごとに新サービスの加入申込みの予測を行いたい。
広告を配信したにもかかわらず申込みがなければ広告配信の費用が無駄になるが、申込みがあれば利益が見込まれる。

この広告配信による一人あたりのコストを以下のように仮定する。

 * 新サービスに加入すると予測した利用者に対しては、広告配信を行うので-10のコストが発生する。
 * 実際に新サービスに加入した利用者からは110の利益が得られる。
 * 加入しないと予測したユーザーに対しては利益も損失も発生しない。

これらの条件を損失行列に落とし込むと@<table>{loss_matrix_deliver}のようになる。

//table[loss_matrix_deliver][配信の損失行列（1データ当たり）]{
..	正の予測	負の予測
---------
正例	100	0
負例	-10	0
//}

また、この場合の合計損失は@<table>{loss_matrix_deliver_A}、@<table>{loss_matrix_deliver_B}の通りである。

//table[loss_matrix_deliver_A][パターンAの配信の損失行列（全体）]{
パターンA	正の予測	負の予測	合計
------------
正例	9000	0	9000
負例	1000	0	-1000
合計	8000	0	8000
//}

//table[loss_matrix_deliver_B][パターンBの配信の損失行列（全体）]{
パターンB	正の予測	負の予測	合計
------------
正例	9900	0	9900
負例	-10000	0	-10000
合計	-100	0	-100
//}

合計損失はパターンAで8000（利益）、パターンBで-100（損失）となった。
当然、利益のほうが目的に適うので今度はパターンAのほうが好ましいという結果になった。
この場合、見逃し（FN）には損失が無く、誤検知（FP）にのみ損失が設定されているため、Precisionが高いパターンAが有利になる。

== 多クラス分類の評価指標
クラス分類の問題で、YesかNoの二値ではなく、A, B, C,,,といった多クラスの分類を行う必要がある場合もある。
しかし、可能であればA(yes)とA以外(No)といった二値分類に帰着させると課題設定がシンプルになる。
そうはいかない場合、混合行列を多クラスに拡張させて考察していくことになる。

=== 多クラスの混合行列
真実も予測もクラスの数だけあるので、クラス数xクラス数の大きさの行列になる。
@<img>{conf_mat_mult}にクラスがA, B, Cの3クラスである予測の混合行列を示す。
予測の間違いについて、例えばFB@<sub>{A}としたときに、真実はAであるがBと予測してしまったFalseと表記する。

//image[conf_mat_mult][多クラスの混合行列][scale=0.4]{
//}

もし、A, B, Cの分類においてB, Cの区別が重要ではなくAかA以外かの二値分類で十分な場合、
多クラスの混合行列から二値分類の混合行列は@<img>{conf_mat_mult_bi}のように直ちに得られる。

//image[conf_mat_mult_bi][多クラスの混合行列を二値に還元したもの][scale=0.4]{
//}

=== 多クラスの評価指標

評価指標も二値分類と同様に考えることができる。

==== Accuracy
多クラス分類でのAccuracyとはAのものはA、BのものはB、CのものはCとどれだけ正確に予測できているかという指標である。
このAccuracyの定義は以下の通りである。

//texequation{
Precision_{micro} =
//}

//texequation{
\frac{TA + TB + TC}
{TA + FA_B + FA_C + TB + FB_A + FB_C + TC + FC_A + FC_B}
//}
//image[Accuracy_mult][多クラスにおけるAccuracy][scale=0.5]{
//}

二値分類でのAccuracyと同様に不均衡なデータでは多数を占めるクラスの影響が大きい。
また、後に示すようにPrecisionやRecallのマイクロ平均に一致する。

==== Precision
多クラス分類でのPrecisionはそれぞれのクラスに着目して算出される。
例えばAについて着目したとき、Aと予測したもののうち真にAであるものの割合、という指標になる。
このPrecisionの定義は以下のの通りである。

//texequation{
Precision_A = \frac{TA}
{TA + FA_B + FA_C}
//}


==== Recall
Precisionと同様に、多クラス分類でのRecallもそれぞれのクラスに着目して算出される。
例えばAについて着目したとき、真実がAのものをどれだけ捕捉できたか、という指標になる。
このRecallの定義は以下の通りである。

//texequation{
Recall_A = \frac{TA}
{TA + FB_A + FC_A}
//}



=== 指標の平均

多クラス分類では、各指標はそれぞれのクラスに着目したものであり、クラスの数だけある。
しかし、比較という観点からは一つの数値で代表させたいことが多い。
そこで指標の代表値として平均を用いることが考えられるが、平均の算出にもいくつかの方法がある。

==== マクロ平均

クラスごとに計算した評価指標を単純平均することによって得られるのがマクロ平均である。
もしクラス間で数が不均衡である場合、数が少ないクラスの評価指標を、数が多いクラスと同等に扱ってよいかはそれぞれの分析で個別に判断しなくてはならない。
N個のデータ点にM個のクラスがある場合は以下の通りである。

//texequation{
Precision_{macro} = \frac{Precision_A + Precision_B + Precision_C ,,,}{M}
//}

==== 加重平均

クラスごとに計算した評価指標をクラスごとの数で加重平均したものである。

//texequation{
Precision_{macro} = w_A\times Precision_A + w_B\times Precision_B,,,
//}
ただし、w@<sub>{A}=N@<sub>{A}/N, w@<sub>{B}=N@<sub>{B}/N,,,

==== ミクロ平均

ミクロ平均は割り算を行うときに全て考慮してしまう。
以下はA, B, Cの3クラスの時のミクロ平均である。
分母に混合行列の数字がすべて入っていて、分子には正解がすべて入っていることを確認してほしい。
つまり、Precision、Recallをそれぞれミクロ平均はAccuracyと等しい。

//texequation{
Precision_{micro} =
//}

//texequation{
\frac{TA + TB + TC}
{TA + FA_B + FA_C + TB + FB_A + FB_C + TC + FC_A + FC_B}
//}

//image[micro_ave][Precisionのミクロ平均][scale=0.5]{
//}

式からわかるようにマクロ平均と異なり、クラス間で不均衡な場合は多数クラスの結果に引っ張られる。
