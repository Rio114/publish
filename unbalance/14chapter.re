= バンディット問題への応用

== バンディット問題とは
//quote{
今、カジノにいるとする。目の前にはたくさんのスロットマシーンがあってどれかで遊ぼうとしている。おそらくこの中に一つ当たりの台があって、それを見つけたら大儲けできるに違いない。さて、どれで遊ぼうか。
//}

こんな状況をバンディット問題という。
カジノに限らず、複数の選択肢があってうまい具合に選ぶと利益を大きくすることができるような状況である。

広告の配信などでも同じような状況が起こる。
例えば、以下のような例である。

 * ある広告枠に最適な広告は何か。
 * あるユーザに最適な広告は何か。

このような問題をどうやって解くかは様々な手法が提案されているが、一つ一つの手法は触れず、これまで見てきた正例割合の予測値を使ってこの状況を乗り越えるかを考えたい。

== 正例割合の予測値による配信
=== ランダム配信
あるポータルサイトでは性別や年齢、課金状況など登録者の情報を持っている。ポータルサイトに新しい広告枠が出来たので登録者属性とは無関係にランダムに配信したが、登録者に応じた広告を配信したほうがCTR（Click Through Rate、クリック率）を上げられるのではという意見が挙げられた。

これまでのCTRの実績は@<table>{random_deliver}のようになっていた。ただし、alphaは広告Aが配信された集団であり、beta, gamma, deltaも同様である。そのため、まだ配信されていない広告についてはCTRがNullになっている。

//table[random_deliver][ランダム配信の結果]{
集団	CTR@<sub>{A}	CTR@<sub>{B}	CTR@<sub>{C}	CTR@<sub>{D}
------------
alpha	0.05	na	na	na
beta	na	0.04	na	na
gamma	na	na	0.06	na
delta	na	na	na	0.03
//}

=== 最大予測CTRによる最適化
登録者ごとによく見てみるとクリックしやすい人としにくい人がいるような仮説が浮かんだとする。
そこで、登録者属性をLightGBMに投入することでCTRを予測するモデルを広告ごとに作り、広告を属性ごとに出しわける方法を考える。
つまり、広告Aはalphaから学習し、広告Bはbetaから学習するということになる。
そして、例えばbetaのうち広告BをCTした人に似た特徴を持つ人はalphaやgammna等の中の人でも広告BをCTする可能性が高いのではないかという発想である。
もし予測値が前章で議論したように正例割合の予測値として補正されていれば、登録者ごとに予測値が最大となる広告を配信することで、全体のCTRを最大化できる。

@<table>{predicted_ctr}では登録者ごとに補正済みのCTR予測値を広告ごとに算出したものとする。
集団alphaやbetaの中での区別をするために、1，2，3、、、というように分けている。
例えば、alpha@<sub>{1}はalphaの中の>>1さんである。

//table[predicted_ctr][予測値CTR]{
集団	CTR@<sub>{A}	CTR@<sub>{B}	CTR@<sub>{C}	CTR@<sub>{D}
------------
alpha@<sub>{1}	0.05	0.02	0.02	0.03
alpha@<sub>{2}	0.03	0.02	0.06	0.01
alpha@<sub>{3}	0.04	0.05	0.02	0.01
：	：	：	：	：
beta@<sub>{1}	0.01	0.01	0.02	0.08
beta@<sub>{2}	0.02	0.01	0.02	0.07
：	：	：	：	：
//}

@<table>{max_deliver}ではまだ配信していない広告に対しても予測値を出したことを利用して、登録者ごとに平均予測値や最大予測値を算出している。
平均予測値は、その登録者にランダムに広告を配信したときに期待できるCTRである。
一方、最大予測値は最適な広告を配信したときに期待できるCTRである。

//table[max_deliver][最大予測値による配信]{
集団	平均予測値	最大予測値	配信広告
------------
alpha@<sub>{1}	0.03	0.05	A
alpha@<sub>{2}	0.03	0.06	C
alpha@<sub>{3}	0.03	0.05	B
：	：	：	：
beta@<sub>{1}	0.03	0.08	B
beta@<sub>{2}	0.03	0.07	D
：	：	：	：
平均	0.03	0.06	-
//}

これをさらに登録者全体で平均をとると、平均予測値の平均値はランダム配信の期待CTRであるし、最大予測値の平均は最適配信の期待CTRになる。

もし@<table>{max_deliver}のように、平均予測値の平均値が0.03であり、最大予測値の平均値が0.06であった場合、ランダム配信に対して最大予測値配信の効果はLiftが2倍だと期待できる。

//texequation{
最大予測値配信の効果 = \frac{最大予測値の平均}{平均予測値の平均}
//}

//texequation{
= \frac{0.06}{0.03} = 2
//}

以上のように、二値分類モデルの予測値を正例割合の予測値として補正すると、バンディット問題に対して非常に見通しの良い解答を得ることができる。
