#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Doom prototype.

from trabant import *
import trabant
from trabant.packstr import unpack
from math import sin,fmod
from time import time

userinfo('Press space to open doors.')

# E1M1 Doom 1 map in buggy short format.
#from e1m1 import compressed_E1M1
compressed_E1M1 = '''3{n*rzYfv%PA]50)A8<V L2/+h9R\\APGc6;]-.\\LR$Pw{p5._UTY:Rs^+Jfr>Rd-R4w{B0*;{k:![vUN[~B<l>x}lKJ`:Hg?b&xH~Is'8)p<RIK5(@=B8;xCx|0Yth+%'Z\\$WidUNG&+Ztq}NF)2z0T4@\\l
h&*%KPb6$3-W4DWa,*?vZ#gYEgCNNtx3gr_lxyW=&SuLCsU}dP;{h](m*fp.qE)2q$H6BsH>5<N\\uAx\\"lzL_YOxn)-aF[rIWMgZQfOk<ABer],snJ=z5F=0%-}zic^7Ex&Lb_nd6.[?-fuI^kk+6{dJ2+Pk#Y%-gC-`~6i'Mt-9,Ca#L:
MDCd$SWx*Wyn=}IGqbu)AwuS+_|]\\zpzz*ta^O<6leGL[`JOI(Y"8-NW:^~Er~0D=zgt;!~z+KV+'[cqatf@Lxs^Vp+6^r?%-l:yYc_"ca4O1rJMx(iDh~n|i]5gauDaMc7m58!1`,Ml5v\\ZwMjN;`&K`)d/CdlQ)PJy.d]%SE#oE<:No6
8Mbub'w58V0[Vv3K0%uO`#8RG"K4P4B4q`sw~'\\Eu5q|.\\mfb,<MTa|rK8k0kz?zQP&j$L"nRFk F"[YiOe2*Olel<sr@lfzzRLwh:8srd=/\\dU#3eja/R!eLMQ-r,<:fv[N_yb_-0!8?W%Lc8J~^*cB}_T[TQ6@~yZm&=DXlJ[I|p7q-
c*fJCyR)[,j]"+l?{x-=+DxDk9:~|q3$I"g];Wqah`a-?}IZ*9y6HMW"W8JlJ5b{.Pta>n_L#.Q*4J?hig0U=NJ3?*g<2<Jr^qYHdEr/%+pO}7?jVs{"U65g5N;Sz~ DnY54+9w?/aN<OZz)E7SOBN#vH@nb_,7;fe:B1LtO,  2_C2ZNe][
H-b5yt9jT|4`E}$=fzF#sa"1*Saa+U3,A<@F~|A)n zh,DEWgK7#90yB&r"5x+)ksbumsDzaI:sKGyl{pQ>Sgb_O@SZi"n$EV zg.eK:[AK<zs3MQG'2X/qulJGk%,8~vr?j@fXLEu!c(uVrN.aT]][gae,$[LlbI86Z]W"msBo!]9NEYPw/
D~IbP)5>+#"?Kmb=DQH.H.3wx_G3D*zFA@M+_PvWLZkelWaY!/`P\\pYO28|s&n{s%!G/(3^?@{)yST(n/ssK{p[%-[~u$V_;T_GE!+jK&z26hTfSbN'>abs\\!{}pYVyV3o?=IXZN}^2'jm6Hjx6.)@@ZRN\\og)9%0=^t|JhB4q1.ihs%>
lK\\'g2k%?[&WQi{vmZe{;+vFETWqv*{2RK"&G_n"$w2ine?]/Q-[##5;o0YJYb~ta&W=(.vr?)C<blag;8Yk"1vVL8mElzVZ-"9YW=pLg5xB.zP^5fj~])X]HWNU#d/1:ki3'fu\\\\Otpp;(!9;/swNgt]QuoSkXza9*K?tLrLd=.m!u8x
1Aj$KPaV>=R;[=\\bY>`t!WY!):}?\\uH$KkW?IY2U_!V#qpK0'+i44Xy$h4a9ww7MNp^F)Jg=^C:y<[n<{>T~'z>ha,p 67`KSOzNA(^?kG@(jm;,+VU?O5HYvu=sk:RE@%uEsQ2O1(+%a8VOQW mvHotk+6RNne'S+mxLvK_mI?p3t+,M3
)i'|O]o}ky&I%eoJd>*1O`%DE=y>7o1;Iu\\6=3)63>f4bwe:P#!UHa\\M{?Kk_\\rQS=$WR_oP8l*s4.7P)\\T.Pe1oz1nPpyz md;s@LUt>?j9^Sz_Y;<363%s]#K{u|\\{J4a49Y\\}tCBcz@22yQFe_4N1Oc^DBa{Wz.^I8}%UDgMR[y
WB_P*.<;Bog~sLtpqX4 @Qv.R`"{7]{%=Eo_rPB=\\fFZbZh\\*~^1aCMU'O@}nw0#ef<p|8@_d1)7vfF7p/zPt?<D*Vk!xmJGquq2EMT<UI/)mif>kEs65SV{cv'X2T]rKM{(9&tr^8Yqu }[X.E;:E*+[xt'11(0RuNCl#(>>u1U2/GS*g
Z;~;7v)3-W&66|Kf-+E'fgc{^OhbE UxBkE!:+1bgSNnfV3yzB{e``GcJy$@;llu([*~;[Z!l8.7}SYX$?ILQ/E=ENCiM^j0;%-yLKXtAeGUeMZ`92C9~mbdZvyKr uvwi>$T7?_iBWm{r5O4"5}+y_H031=``_c}C?`V]}uv22Vh9X2j;MU
LE<`O;).&F[o<XM#WS1JFak$-Aq_>]QngMmMQ<y){%)zaLnE/68:MvPk]k1OuFX_7?`ugD*{PV<G v1w4)fL?Sn9&Y,=z/H'm[nhZz'CvP wn?0C+Zn)@AA"Nn`C]ei7jjE{fW%80Kzj]XV#lCxLk:%F)B}'[FkZEpX+.w0.AVyAJkEm/w_S
X%i%*EHl/Wu :ee![qaF=:%W;HVM9cPMW%RL(jJ~*R[ckCq!Mvs>}T?c1s2seME@kCH+(2mP.zl8~P{R9iy}b-3@jfYh)bdq}SU>l[xp}}hyZn1lax(_*Oe3Eghk\\g9GiNzB=Vp<h9<t(o$9t.HU<9q2)QA>yR(.*Qx!92!D3*A383cLMxN
Mx?W!JKkSm*oK^_~_Bw&TPj.h:U"hdiiHl}kv`HebdDQncG9dF,?A\\1Z9I+9pYKaG@hPmOYzovm2gv ,WPLXKJNPs!QYB?t16he;=%<+rP6KvKi$0eoX%[4;\\R@uAmT:E]zH@K:,zU!<Vvk+1GUw5t5mq~Bsu]7,WBoL$'G}AV:B{U$;^K
jto<~QqKE2ZmL>{iEHF BdiiRO-~2WyJ[YHWqO<B&^i9fC"01.jCzPyOH'VZ:l8J>/fN!5:qi paa4A,6_$57}t%Ki"ih.gBXw8ZZ?BhP*TmGCRh1xqx>8hdzV2'wb |a2\\^Nj0|7Ahq#iPk7_]y?\\-zT3rD'`o-bOJf)ue*"$dnA(]Qlu
GQ&]{Q@Tm4Os\\zv'#W$@C?k4Bhl2*!cmsWB}"]Qas]zVRs]83bp6/g-qfV'I?!GE'E/`o2$B=,i3%ptJL8oBv3|">y8;mD-# f[=4If3_ieHZHcdwr#4v%*yvf~a~$,J#IAqpVB~hmw.f7W:}9 *28l2=?CPz{cpp~H3+E)S [ v:yK[1P+
WmxsWQFWvPV\\yUF5t4Y3ZCCg]xk~;_ouA)Lu xp^eU%n""9P#tpM^'WCoOu&Z0gVqn]@}slh4Z@S}^9v!v928HjmbRWDEdp`s0~ei0ew{:jOyQ/=62|4mK}MrWL.@HLw0++<?JI0T/CO%Q<FMA`>Na]0UsYKn\\cE=0|1V4YHfHV"]S!Z9v
vqynt$kcfW@e!,e&|%ah01?6"q|vwskbbk97hP+}2($raM@<I9f7C}z1iCPqAn|z1830R^FhU@mT:Cf^3j+_$cf^DaI\\%eDTaG8o'79uz.L_,{K,_2C<'00=\\aS /'epE>Y7yZxeuq Hv!mqs'0n2-J}MrfYgNN91&6?~B"-@>~0b5}:2<
1mZ6bcblJs,V3#n2qSFeoyK4G)6g]/::F:W=NlkX{E3?-$k@il36xoKM7L_8SLj0.33c1bWv4rpWNohFO"p[}Cjbs*N!Iq1T@#{C!tdzDTz!%uEJvWix;?)$1ZC|E|^>33'$&t]TYduLfl$\\5f&;d|C$s-;Blmc&G":wRNYtocwp2@2O&/F
PQb0iea[_|Z:=,Si\\xnu5YG'"M='cs"U{. 2P2x5girKGCF\\82!uKkOl"|DaW7`#4nOihUHjCE]t`xIE9$_(/[C1h%z@f~ZuPRG*vl3&;A\\*EpH-*I=:XMAl|Yq#/(1=X~T%!OMe37K5`+JMph9WIomBAtvvqmZ9*6ay%Tcr3%HtktOMr
ZW9,,t89(;fni8!G*f6Z~@Xa65E*=wn>8?OlJo./vYU.SP}XXoBx:#*jTg3rT*J!uz%0O7L gPU\\tQ>?}&z2A7b^@H}D{`hdOyMj7J;"RB.JLVLDpgNAN`Fay'{BLHr7e0awR7n}Sk3502I:sqZeu%9wm*IQOA<x0-O89(Np1EM@Fx<ZV9M
ah$S-%6MzoDFz<4O!;~+UcExI9cC{"I;F!"H)xsDH)7g8E@v;KVi6L|NG=shoHd_mD`Y+JPz(uj:b[?MG5T37@_{e{;O88CXMI2v"]Kd<VZ%3c)_sdXz-8VquZm4 [VkkcOp`dL"P]`4/~h7O$RlrO}#3@=3Fq9E##,fJ-ce6;CtEBaaAKT]
s!:-n'<}a~LfNZy=Cw)e,B9E'*<v#.gg-ya4+g=b78uL@E'*eo#2s)aV\\[*WC]>My&(_1_s182\\tX*kJse+ppjU\\S:.MbU|b]S~$IG^<V>f-Aihs@'pZvG:3A:p91D?Gu~cX}D$?<yG_z30HCd-h'GE]OMPAaj$''Ec^Qm&FB5I4_ik"t
{xCrZV:0|\\x=*/.a [w?{=8l9JE!xD(99En0$vLUJ8:o_dj[uy[T`xFB3iv,Nc,v)B kH0"Tt;o[;hqIu6BFT%9i(c"<E3^7$C/{,*I3,aTT})&DI<,>t#[`/^}s)i QxnNl:4m0s|z5vEVf$@UIE(gJ$M*#ex)jJdgf''~4x2zo];`U}$*
EdvGR@cj?D~>;{d[P:?b(4b{Xq,+d1l|hn%Wiep>nBDXX5j\\EWzZgP,0wI8^?7`={v3;jEXF1?NTmtdG)B**ZzuG$6/*1<50qU%%lFQ>OM_Ese#\\vuPN^N50A^U_>1Z*yu}a~d72r:o<eZV` Fc7!G18M,;ov#@)f=3~'JG`8XeJsYHVf|
;5S c=DH<gDC&vv5,<]}<T3,QpxOyanr1c\\ m_"]0<7DL9C'.7wKd'hZN.GSJi-wNz6FNH_;IX%"h^2h2,;V6B3k64QhJ.FdA79cE]_cm\\HR){hPm4R2Sx]tAic6}m_oXetG\\=O!0L6Hx}9%9\\M7~(QyIyKk_VD#@r{q]&!L3- WP6N]
M^%.p<A8 FAzw{z)C0{MulI}Fh5-ql"yzLwQ%*#V6w0< Fo'oo>Nj@:=(dBi8b(qTF AtKw8@zbQXT{M;a9pYj*[WZDs4"DTmyr,vS3P=X5wzt;g$dF> MUF@{|WJDXo66h^SrWE881*'y7n]\\BV:+yE+lFOO{MzEJ`P):}n3FT?*QU'xQ)
h'YBHPyI/e #AgG7oCu<LFvz\\+|i62&<t0kM62}/:U63\\vt%$jIk#QCDs7u/SZqqq|yTeE<B/ddS6Xz7Z<bxR3"6$P2.a/U")f[\\+8iMUcGQXRm_"w-*47J9;!!,tkC_-ohKf695rP T1+DcQg@-s$!pe"8_i]d4z"oTg#ep IB3yc&C]
H8o=NjWK\\ G]kfDC~q{\\q_X9q|7Mwc%zCE%IX-z|hQ=se,~P:gL6Xifn(_)!^[g?\\=m22z#hg%hVY3jHD9y*D|vn?eo:0T,ca<hXu!nY+6)09JcMklJ*c{1=.| 7jN8=^7##0_A.cm~RknS>B2uF64pXxSkT!|)Z1^Qg"j:i 43\\bC'c
tchA(n]d>NX7FP^-[DpP>%JP~X>B1hsg !L.vrqLAx`4r9%M8l{'MC&(f{|sv^z+frUGuP25M.5nuVzYp6b=yTGB7A,I3ysSyX%sadf,0wN:oJm69!ad+CYzM0Z;@z0oc 3e0HyYv?=NXYw]yBgwTg-#Y,}*w;zOxjQ?)gJ_9V:tzg$SV2:b
&0>,!}@PPc0X}`aZDT0v![v)BG*G7.$b3)/tsO$Z: Y:c4((r$9UaY*r- WfRXd-G|yWq=ilqukjss7eWoJ;%j,rcPo7^kR%Mu&6h})vPAZL==w<Q[*@uQca.VZb;A5;!dn(b$M\\W6G1pT){aan}zoy%fhrjL>o!Q,/-x5Dve{B)qlN,Kb{
%"$7BeefGf:h>1@p*_ukgC~"Hda:2?gq~@,H-kpI<D_[XE9_qs=7HN?@8}P%7eqn<FPK$!p90tE9d$l>0eY([S~K^;q\\?"rc<5*Mg!B=-xjfD+DY}|TlN#L#&e&m7A<1c.DGv*fo{|eVUY?8n#~zwK"i 63~w*0;MC(5SR24xUi9tZhC|~x
CenkEMHF*'I{Wn,d+Y9uv#@hP%NC|S"hZ2?kh0tIOA"$GheKE8 nhGoff|6Y[MK:`$f*|$C]hr[HN,R[wM9! K'VmR.oot_IF)Z#YgB29vjh<w C$ YE*&49qx+Y8Gf"r5Ut>~Ogclqw](/O}SwThSv,QCu#^54434<]?x6U5M9IGrNM=Vc?
oXb^h1![Q`jrBoWcpCsdiZ#Kc*o@JNr1Q=BvQRv"Oxrjd?E1:V1K{zMU%&"6;S`+W}+Yx&z3,~U%*W;V3xB`dK{c ~X9nmDR.\\7HaD@caiTL{y1R1sC1+-)urbHu#w!<_Rh^*P\\N..z>6:Py_\\C5fc(2{T ?*zg`GmBnM\\nN?'CyF&fc
uZ|D3myfE5&#j2,^B|nGHQo-tnQa=NJVL[{%n(r;i}SD<kk`VH8:m1SL]6>g'3~DcZwH_|("*c}eku$J^&ht-#PYE_&G^CcE}z-MQ#f{oO_PZF%7!a% b*[d0i.7D@gGI(1rVEfyVqsd]! <vY|cfnrPAg|/hW"QWoaYwR=r]e#5#!N-mQ9e
}gmF0U|J87b<syP8$v{q?"ZI=t!t&!ylup7AZf";x!uVg>B$8;_9zz=<Z$DaC1} cN0W3PvV"t[-m/6ihkgbq*R/8zRCc+\\Z$e,$qcmO(X_j69;~q1/(79)C7=u\\WT"'*DXC`NbcN7Y%-IeHy':v5{ecFzK`?rQL6L Yh2?2.DCL+}K.^i
deaRp/LFdGd*;_EznS0U]N0^+V@j]ys%i"y:mg2$8s)[]OC&$ 4Sy|jJ2MmUO-ej$_}((X6QUfuv9aU<vDBM*gqA3h9S#cx1VDv@,HcD"[z;r\\ZwiD,lmLKYD1t75}B tuS'wt(T%&vJ$d{Y]ieV'a}-Jjc73mP8"G^{lGXpVZ1S:=-RFu$
tXDQs_]u]W!`)JtGHVpp|<AG>08~0zTXu&Py}w(2!BnVuNSm-R85.U4!TJ-{{%dzgDF;Silt;X 78i/<D5g<2S9$W-8%?!|+LI~40i&@O1Y%m095>r`#rez%UvtZojOUX!n1k`)/1m=7/FO0Cq@O]1],J"!ddwQBOQUf7j@v:TC#AsU#JE0U
-nc?tpa_Q25.rvIUfef02.kj)]97FN1MA#>a 0U;:&/]{8+X20HDb(1&31W`tOQOn1t|0eM(=JRf@T,^8Jwpx&-\\R/j@7RZl1_Kx2Gc1alpH`'^QDfaLoQc:ZW`a.z]1;wt}q9p9F^Bco{Y8eTb/d>4r.=]-6kV;A*hEC7V8^iP('\\-WdZ
7gE&3F.e]8fES jm5VWv&b'1=Sy(c;Er'sV!`j;Q/VN>N\\q+r[wb1>KH9J_G_Qm@'):Ga>Fz>!o3==@abc$-dIy"EnylmqQDd)BO'#amWn4fH{ev[-m:^u(6nE=|k8d0uPCm"UydkLgHz>HOYT<z,3o_VS@)*3(x~;**-36:G;un''UX g8
llge9l;U_CPN SuMK?bUkN/dj|%/G{!M@~I7aa0v!{!3%u):/oDl6iLV9jfRi]HM0&![TV]25w=|h(E'js[lkoy/t4Y^[l!Q'`YmUZ<S^kI#x'tkU&6%jFayI &;BjNyoIo(OcQs0!e1k{VIu+$5S#j`9SG(eD`h}>UYCWF^1!Rk=#xy>*rb
[=0jk|2d^q,.55@f`K[)K<BLriJ%'I}KF4D&:`jSxSsch2PBsmW x)S^X]-3R&8t&4/@!M^JGi@p!}De9I\\lr9Bv0+I{g00td#:+RDz(tdJ=W;_7_F.ErzBs*m8L#\\9ZkfXBb.|<??><$1!5lT_B{%&P,/DTk}[sC{A"N X;/f ez@dv&V
FKoNOWC!kON;}$~veg3,^on&,/Io+;bzK&0t[bdN:"#XBpbpuuI,Edl4ofpj]yz*6E0J*h#'`M#=CGo)8'}R-pX8mO9s>Yis{8XDzc6bZ+d<L"AW@K\\H5aN)@|;Bky(wl&MzEHV f;g(>mcC;mkL&1am;p96a&xfM;>y:<#ttykr\\J@BN<
P#.]x2;]=j^Mw@JI@<*.PKx>n6!-!ARE aL3r/<`y*nt=B|mu(W @bXW|tGmf*crD{L|.Gfm|!#,R(ZJhO<R#8|xBP>L[)bhk3U(9gVb5{TR9lRSkYx<;ch(0$:5U)&tQGlHfamzbu\\P=pSv71wrx}dJ`&8)YKs3C`RDZLC4*77?|=6E8hw
>@R)".)vD+V)"uRoor @.ScwKd /TCKf*|QP5gIL-@Xuu/o"Z3-# y?,jY&}L0*9!_1=7<1%!].(aDJ_C>[0u*K9O1L+uim3 2uW]IH%pKmc+16Ca8ljx.z\\CZXb7UfE%916l-O+3z1xtPJ8H3-L&7{wT}BWz(Ji`s+e=jHrodl:amUGU#a
jOIQYR"D'ba_^pDuEh;<b(jV M&RY:dZkg^~d?BT$$`TXt8gN\\$jgKkzy$.Qn&TOcmqX9'6\\PZ*N\\DZF}n<PN|-U6z1)|&+oNfo^!50<C~>+QNuv"bbabc~_D1;<+2Ot$]Kjdlbu;CY$/InBwA^=o|n{0)*=Eqsa{-7U(0Aq^,B+H#o%{
'Q(0i YAf -?7xT9OnK_hxkX*k~sD{]VbJO(T`&j;4Lux$)^pr ?bxq'^;Cu$NcLKHlf}h?t;bLG%,vZXxY$7CKTWp}Y"tuw@XC^0j7rh]' y;_`l`!b3~}8{JrbjB';6~MO4 0;0gRN*{:{iJ!c^~;X[|px^[j^Yr~m^0<[b8< 1$1Ny&Uy
[O"T;MYM BK'b:qBKv$QWu.Uh W^~E&Ex=w2[y.Bl_%mDs{92sjRi`/a5Asnh&x+Gm=+6xho^KN1<7~zA3DPY=lPFkXpq\\Gq\\t2tKN-QIhbSX}Mq&w.e<pX}#exuTJsE!#nqQ9!Yp!Y3`~4rBs{,H+ 5[(a#jNKT2d`p1Jw1RM]'yQWSs1
]gE^9R9h(`Bfh&%?08r>v4#q%|{O3Viben7lqo]\\s-54?J@[xZ2_+TaCUx4W^xV;Im`pj(k<{(*1_@|l/G0K\\&jN23(YINdAzPm*C#4VoD":&b{LgO#<1F6}VuE8AmNUil]1AuH@CDh<~gMjDiGbT;0oCA`]~5X+fqJC[ryg%7Znm0+O=f
G[%^>~8#6z<dgt~HxwQXvp_qFRnWyJ<^!Z&JB(ge+~l/uc^<g]R'A9HfQjJ^M:8>61E{ dmnedBfMep=x?5^XBN_%o<fx_]XFb</FAy<0+8VF2'T8vJTL-rk#Zh*ou3%k& r-gsQ=XLzZC)h~mAS|`)o'6N/(`oeyo0OpcH(l{}lKYYP#4:}
d_VtY.Kiv|ejEFj<;tT#'"2hFmB]5N#Pzmv?Nj}(O#n;hsm}%fN.zE5+8b+2#?G:P0*\\'_nF[MV/gT?=gs)})gpn*M5+`#RC~NP?|8%*fl9sDapg3 _*I)bX@,%`l6wy/uT~z[Lt PP,+%gf:h#c!R!K\\` -Bxn$%Fc#;>UnxGyTIIB!-P
do39If)^X*EI}NzJYj\\4L#dRZo7{uZhk1^<yqD_._o+E@q+uNdovKrPiamI>[|cq:lVJ7~>d|wW,lx>*$xM0Y0/?`,"twa3\\?}WI$rf^![AGY*|Er$a`g!Oa?#|QCjjL:YcE0K[Fv9TVHs:R0UqZM4^,=]jvsypZ%uNBNmx&$VJRCx6pv@
 =]"?t1oBKcfw\\.NcBhhP`-+ JrEWdJDl+jSkS}h?{jqSKg-DLU^{~)XYy8Gdv;cya&SV"tsOgwSaS36aV_W5Z)Dpb/_j7X|5Nga@%#8,PY(kJ4_<Y-&jF9CQ5xZ8j/%&K;zlB0XCDzdEsULgH>/uTh'0r{[@o2XJP>cq#AmY" IF"a$h\\
B7pQmh"A:'UKdPgZ?5s'M0,f2=h:um(>uA[eU>}v8_o;2@tMGy527i\\oQ]c7B7(6ohNv\\ ZB?#i2GT\\G,;u:,>/BQP<f<|O5S,_8sp_<Kj3Q!_&hT_>[N=qx<F>D&bLw8dz8pQObEl&I]b&)YZ =(~)G"@Rvjd,f.T!!LAT|-E's:pyS>
n:PbQp v9P<(08Q'`iCD):g+#.wtu25Wvz@g7m;Hx33jv*JA1?E%L'.%Tce34"[X ?92bib4\\VS*a+J2di;<s{&*iim\\!UBNhF=W_UKI5pB4ju2mn\\)*JwE3h|/l8|GCGKNu.Xg'eSS,|~E$`VU'VxX|c'ZE`j0Nh)Pfqhv_|U2Ex6;RR
'Bl;#qc!{&&}MwHr\\k8w54d2.S-]x+T7]z^/2wC[OT_C`u,WJq49O^tgzv@svhuwT<;6!EBBv:[57eRvO#/&:<~[.BC':A[633#`(qF`tmX}|%=Y9l3{Zzjtu{@yNnaqZKb(-%C{u~cJMvkE~n`??l+V{J=/8!WSUI7eC=yOT9*"Ky;|*9'
-KNJRmDRdAbkU<B3vNo-#4;I#$u1YH'5ro0PEgy7os=msJ=kY(|Y%E8PDz4,W_zoQL(5](10Riaij)&o'V(H]BuZl0F{%=83(o2vxqJk#PKbr(:P7XPdd$Ie8!OmiLL@A23-Cqb/ltMjHN>]qHn[jhzY:Fd2'"Fb5m-+L%U5TatP;UBc<|_t
 1g1A*k879@iF#M4n[Jzu\\r"?N8Y)fw'u}>2BeVT'Fm1Z hjz<N)9VKK<+^<vU8has]^60<]`tk`Lk|&ee0zL1ccs^e?Is|UP8#mb##U0}#n|\\I}*)j@`HIH~}Og/foD90zv)waV~}]Mo6IQGPCu2A!i*"9!@'lEm0|~G5+D[vnw0o>1lv
?l6@Yyd,o7ykg6IzBf8#^n$+/@u7([<?(?mT*zw&5~Y@hJE]N*et$G]$87i.i:c#jw(mzBbX<7h9S_[-f*yz$cTvVPilE~jd*KVHxv#T#b7xzNRxs@R2+tv{.ZZ$p`bu(=<gqr7%qu+cU&i2tzpx6mma7`-z5:jUDrjc`;!1T{,iY5D%tL&-
@c:N\\l9[z:JZFC_"n|ox5@U/IRqHz"P35h<i]fgGvJr:,;kvG#1jSv@~^)OWd;9LZ-'zz00k[N3JSh=tb:DCkp&8qR"*.#dUYPJ#DaC~mPG(r`!K;vhVc5Q#l19T]^:N} ^cc<sTmTby2";Jh1G;p&~kB(MDigCQjwquP)n/f/qNV|X-,yI
},n$t?7rqsAnkJE9a,4_@L.t`upG0$aRFQZz62vCf7<ZG=b{^4Z:sS@Q*<!w*~V>fl7Zv'0Y,.G~BDKQxD`<!]J"kpB-jNFqAb*G%VR"^Vge$l?B b\\:Z"es!Lcc!3&y;7-zZJOpEW_i L=UON>RwJP>+%cxs_1XGIiQizo^#^6tG(q,K2w
j0!'''
exec(unpack(compressed_E1M1)) # This creates three global arrays: boxes, box_angles and float_lookup.

# Helper functions.
rotz = lambda a: quat().rotate_z(a)
is_shoot_tap = lambda: (fasttap() or check_reset_time_tap()) if is_touch_device() else click(left=True)

# Setup without gravity so things don't move around until everything loaded.
fg(outline=False)
bg(col='#421')
gravity((0,0,0))
collisions(False)

# Load map, i.e. place the pre-calculated boxes.
imp_elevator_downed = secret_room_found = walked_back_after_secret = False
trabant.wait_until_loaded = False   # Optimization: don't wait for each box to load before placing the next.
map_scale = 35
map_offset = vec3(-1060,3620,-45)
l = float_lookup
elevators = 420,56,67,481,489,494
hall_door,imp_elevator,hidden_elevator,bigroom_door,exithall_door,exit_door = elevators
blocks = []
for i in range(0,len(boxes),6):
    p = [l[j] for j in boxes[i:i+6]]
    pos,size = (vec3(*p[:3])+map_offset)/map_scale,vec3(*p[3:])/map_scale
    idx = i//6
    if idx >= floor_count and idx < floor_count+wall_count:
        col = '#888' # Wall color.
    else:
        s = +1 if idx < floor_count else -1
        col = int(((pos.z+s*size.z/2)+3)*219)
        col = ((col&0xf)<<8) + (col>>4)
        col = (col+0xeee)/2
        col = '#%3.3x' % col
    q = rotz(l[box_angles[idx]])
    b = create_box(pos, orientation=q, side=size, mat='flat', col=col, static=idx not in elevators)
    blocks.append(b)
trabant.wait_until_loaded = True

# Setup doors.
def create_door(idx):
    # Bolt the doors to any fixed block.
    blocks[0].joint(slider_joint, blocks[idx], axis=vec3(0,0,1), stop=(-80/map_scale,0))
    e = blocks[0].create_engine(slider_engine, strength=5, friction=1, sound=sound_engine_hizz)
    def trig():
        e.force(-1)
        timer_callback(5, lambda: e.force(0))
    blocks[idx].trigger_door = trig
[create_door(d) for d in (hall_door,bigroom_door,exithall_door,exit_door)]

# Setup elevators. Each have some trivial control logic.
def create_elevator(idx, stop, trig):
    # Bolt the elevator to any fixed block.
    blocks[0].joint(slider_joint, blocks[idx], axis=vec3(0,0,1), stop=(0, stop/map_scale))
    e = blocks[0].create_engine(slider_engine, strength=10, friction=1, sound=sound_engine_hizz)
    e.force(-1) # Keep elevator up.
    return lambda pos: trig(pos, e)

def imp_elevator_trigger(pos, engine):
    global imp_elevator_downed
    if not imp_elevator_downed and pos.y < -16:    # Entering exit hall.
        imp_elevator_downed = True
        engine.force(1) # Lower elevator.
        sound(sound_clank, pos)

def hidden_elevator_trigger(pos, engine):
    global secret_room_found, walked_back_after_secret
    if not secret_room_found and pos.x > 60 and pos.x < 64 and pos.y < -4:
        secret_room_found = True
        sound(sound_clank, pos)
    elif not walked_back_after_secret and secret_room_found and pos.y > 16: # Walking back out between the light-posts.
        walked_back_after_secret = True
        engine.force(1) # Lower elevator.
        sound(sound_clank, pos)
    if walked_back_after_secret and pos.y < -6 and pos.x > 70.5:
        walked_back_after_secret = secret_room_found = False
        timer_callback(2, lambda: engine.force(-1)) # Raise elevator after a couple of seconds.

imp_trig = create_elevator(imp_elevator, 140, imp_elevator_trigger)
hidden_trig = create_elevator(hidden_elevator, 156, hidden_elevator_trigger)

# Create avatars.
def create_player(isplayer, pos, col):
    # Create a man-like capsule.
    avatar = create_capsule(pos, col=col, length=0.3, radius=0.7)
    avatar.create_engine(walk_abs_engine, strength=70, max_velocity=4)
    return avatar
player = create_player(True, (0,0,0), '#00f0')    # Alpha=0 means invisible. We hide in case we use some rendering mode which displays backfacing polygons.
# Create monsters. NPC = Non-Playing Character.
npcs = [create_capsule(p, col='#975', static=True) for p in [(-31,10,2.7), (-29,9,2.7), (-24, 6, -0.5), (-19,15,-0.5), (20,26,-0.4), (20,34,-0.4),
                                                             (34,35,0.3), (34,31,0.3), (67,4,2), (65,2.5,2), (63,-7,-1.5), (41,-8,-4.3), (38,-10,-4.3),
                                                             (63,-19,-1), (54,-24,-1), (49,-22,-1), (55,-33,-1)]]
#npcs = [create_capsule(p, col='#975', static=True) for p in [(-19,15,-0.5)]]
for idx,npc in enumerate(npcs):
    npc.walk_dir = vec3(0,-1,0) # Just any starting direction.
    npc.start_pos = npc.pos()   # We stay near to starting position.
    npc.shoot_timer = idx+10

def ai(npc):
    pos = npc.pos()
    # Place NPC on ground.
    picks = [p for p in pick_objects(pos, vec3(0,0,-1), 0, 2) if p[0]!=npc]
    if picks:
        pos.z = picks[0][1].z + 1
        npc.pos(pos)
    # Check if we drifted too far.
    if (npc.start_pos-pos).length() > 10:
        npc.walk_dir = (npc.start_pos-pos).with_z(0).normalize()   # Walk back.
    # Check if we're heading towards a wall/object.
    objs = pick_objects(pos, npc.walk_dir, 0.6, 1.5)
    if objs:
        npc.walk_dir = rotz(1)*npc.walk_dir # Try turning a bit.
    # Check if we're near a step or a cliff.
    picks = pick_objects(pos+npc.walk_dir*0.6, vec3(0,0,-1), 0, 2)
    if not picks:   # Oh-oh, standing by a cliff, go the other way.
        npc.walk_dir = -npc.walk_dir
    else:
        # Check if we're at a too big step up or down.
        pickpos = picks[0][1]
        if (pos.z-1)-pickpos.z > 1 or (pos.z-1)-pickpos.z < -0.5:
            npc.walk_dir = rotz(1)*npc.walk_dir # Try turning a bit.
    # Check if player seen.
    aim = player.pos() - pos
    naim = aim.normalize()
    picks = pick_objects(pos, naim, 0.6, aim.length())
    if len(picks) == 1 and picks[0][0] == player:
        # We're looking straight at the player - fire!
        angle = vec3(0,1,0).angle_z(naim)
        fire(npc.shoot_timer, pos+naim, vec3(0,0,0), rotz(angle), naim)

def fire(timer, pos, vel, orientation, direction):
    pos = pos + vel*0.05 + direction
    vel = vel + orientation*vec3(0,12,8)
    def create_grenade():
        grenade = create_sphere(pos=pos, vel=vel, radius=0.1, col='#3a3')
        grenades.add(grenade)
        def boom():
            if grenade in grenades:
                explode(grenade.pos(), vel=(0,0,3), strength=0.5)
                sound(sound_bang, grenade.pos())
                grenade.release()
                grenades.remove(grenade)
        grenade.boom = boom
        timer_callback(3, boom) # Bang in 3 seconds.
    if timeout(1, timer=timer, first_hit=True): # Limit shooting frequency.
        create_grenade()

bob,bob_angle = 0,0
avatars = [player] + npcs
grenades = set()
cam(distance=0, fov=60, target=player, target_relative_angle=True)
yaw,pitch = 0,0

collisions(True)
gravity((0,0,-30), friction=1, bounce=4)    # Higer gravity to keep player against ground. Bounce is a state variable. We want bouncing grenades.
userinfo()

while loop():
    pos = player.pos()

    # Activate triggers when player is at the right position.
    imp_trig(pos)
    hidden_trig(pos)

    # Move about monsters.
    [npc.pos(npc.pos()+npc.walk_dir*0.01) for npc in npcs]
    # Every frame one NPC gets to do some AI calculations.
    if npcs:
        ai(npcs[0]) # Take some decisions on movement and shooting.
        npcs = npcs[1:] + npcs[:1]  # Put moved NPC last.

    # Update mouse look angles.
    yaw,pitch = yaw-mousemove().x*0.09, pitch-mousemove().y*0.05
    pitch = max(min(pitch,pi/2),-pi/2)    # Allowed to look straight up and straight down, but no further.

    # XY movement relative to the current yaw angle.
    xyrot = rotz(yaw)
    push_dir = xyrot * keydir().with_z(0)
    # Check if walking towards stairs or raised step, if so we push upwards.
    picks = [(obj,pos) for obj,pos in pick_objects(pos+push_dir, vec3(0,0,-1), 0,0.5) if obj != player]
    if picks and pos.z-picks[0][1].z > 0.1: # Something below player, not pushing against a wall.
        push_dir.z = 0.5
    player.engine[0].force(push_dir)

    # Look around and view bob.
    vxy = player.vel().with_z(0).length()
    if vxy < 3:
        bob_angle = 0
        bob *= 0.8
    else:
        bob_angle += vxy / 35
        bob = sin(bob_angle)/5
    cam(pos=vec3(0,0,bob+0.4), angle=(pitch,0,yaw))
    player.avel((0,0,0)) # Angular velocity. Makes sure the player doesn't start rotating for some reason.
    player.orientation(quat()) # Keep player straight at all times.

    # Open doors.
    if 'SPACE' in keys():
        direction = xyrot.rotate_x(pitch) * vec3(0,1,0)
        for obj,_ in pick_objects(pos, direction, 0,2):
            if hasattr(obj, 'trigger_door'):
                obj.trigger_door()

    # Throw grenades.
    if is_shoot_tap():
            orientation = xyrot.rotate_x(pitch)
            direction = orientation*vec3(0,1,0)
            fire(2, pos, player.vel(), orientation, direction)
            print('player pos (for debugging purposes)', pos)

    # Check if grenade exploded or if a player touched ground.
    for obj,obj2,force,pos in collisions():
        if obj in grenades and obj2 in avatars:
            obj.boom()
            if obj2 in npcs:
                avatars.remove(obj2)
                npcs.remove(obj2)
                obj2.release()
