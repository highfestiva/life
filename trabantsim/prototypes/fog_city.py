#!/usr/bin/env python3
# Looks a bit like a simplified scene from Limbo.

# NOTE:
#   Prototyping looks is *NOT* what Trabant is about.
#   Trabant is about prototyping controls and game mechanic.
#   Delete this file and forget it ever existed. Thanks.


from trabant import *
from trabant.objects import nophys
from trabant.packstr import unpack


bg(col='#fff')
fg(col='#222')
fog(200,600)
gravity((0,0,0))
cam(distance=200, pos=(0,0,-10), fov=25, light_angle=(-1.1,0,-0.5))


# City's vertices and triangles as generated by some 3D editor, compressed using the simple trabant.packstr module.
city_vertices = '''Jv|&c?ndYr(YD&SG"SyZ7j%N|iaKo4k7;[Q0] H:C:dB8i(}*Gv:@^Zus-G>Tpg5e}/cHg%4mr9$/Dt"SBpQi?jQn@GKm~:7?j\\Q'CFZCP9At5N""M$!(PJ
^scD\\p6yQXOiU3&7`JG-s+}>?bktC`px{wwJ}[6B;0/ZQCR?LFS@JX;%xQ5=JF:eg#M8|gb=6KS1Bs`tZ_JN"t])=:Zmpn@0e325 U-w+#_k%|kr75=AeS0;MG{ $S>f:E@fPH;l.n
QJr{,/4rF>7<$NH,(b46d{KW1GNGcC?X +'p{h|7-o"tez_zqVQv$W)cDA-CLZOG6?xJ%vE|WdA:O#w NeFt/KTn*'NWs\\p]_f6NxwNi6Cd4@bPmr@T-o/WS9hGG4T1SYB.w'P~,Qw
o%o43+W$o{DexXo}b6\\'J%uu<x='EHEyV@I6#{PZ3^QD=wwF'1NuZW+98~4M\\y]ZE*/ YpYJ64ixgB$3?&gED/ka 1m!8B5h(^+;(I[D@EAkV{%Mz2]TJ}:dknV{I)8YSe^+\\PID
y0 X8'GPJ>9OAP3'Hd[z193XPM*a5E9juw@#OYd%3aaR\\Z<h3ii+ts4REK9FWhfjman-&,Zdn4V&EXU_z.n}A&9Ki,3EZ}NXjr<"(X`(Yx#,lpjrI_DSV{Wfxp=<jyw`CI@TW$Cjgt
No'wKtP9sv&2If]eL+dSe-%4@vGNW(g [MK2:x{Gq'`JJ3N+0mt_Rv=tZ_1@V#tq{<gO4;e$8:TO;m4XML,y$*7!qjz;.td+{:A[~zx+94D}RRT7=Wp\\nFcZqV[muMHv*za"!ra\\Y
Xl2yo+Y`Q^|Yr,*/}Fr;4ljC}YobRzPjbJfC(pA<O\\'P^a6a&NnDwRzTQ~]IyvLlMl,-p&>@?X_rQ#hcVU[;5GP&&hCB]D[T!]0d3`fbNN/,te~2]%P(d(/Og-ygazvr2Bh7A2[tQM
W$RT}8H[l_s:D?wl57hEsDSQ|Fde$>?'(|KA<U\\)~B"48j`'I#l}Nvy2GRPnkv%Ah4&Rukhz}>AN`g^yrEWN9cp_QG/]XOnp9J<?T:$]|3m=U(E?C9}ej/f`sw4`I_fvq\\+B.\\H!
qa0|S5jF_A=]f:di[Md=e6&cxBgVevh@)rtBX>xJ&[1&bfka[w/Ek)#:UX|bh[=<hq7c@8$fvsfs#f6=*eazx+Y-niP3~hzK%;6#uHr.v)=K/N0fwQUkPL@}895tI-bn0a>M)SSJ-n>
eR6_,;7`~`&k)mrw9m^[IJt_p6Fk'yf=w0L&j\\8"6uXm^c2ABbgwjxYNmm"I)_WcLS3D5@|':=2N!PA X=Z9W/t;DIoQ&=%#fR|Gg.L]4vOAh.2\\2^TL(UeA0IaT_7o5/'kE%X\\S
$hN`\\{1_CM#JS^EdxG<UfPA6#2w0^m0U;8|B%64>>0:=ONf!BiyVg6n^.]'Qoei4sXcij$PaF3PvG](?z>uUzlEO2:Z%E`wpASKLB!/\\qd_5mPr^K/wv8:2(w!<W0=x?.U>T*fo{)
h0KjD<SGFBybouzF=&b3>q7E> 3}84$paA;[d0~b)apa:?Xx3kN&_u[{)r8/aK#NX+@ct.w|`^OaW{@[D>zC)N_3"KyIBB4Dr-1*Agwvn`n5Ng=Ja=r$7@2U&,Z,!jbpOXk1tfk@!,A
;NAC}(-#o&H0q<T(9,J3;G0,L{SeCHKg(<_r_Gno~(*}wQIIEy{r1]]\\-.[--9M[wTmw\\k)(3l8_cqO\\1\\ZeoSwPNXZ]g*mZEf`eCg DSB3J93GI/<zC7SK\\-;<;i1vL|#CI90
kU+#6oZ8E:?Hh>p*;)(]Hh%Pchi*!ASV[yoFZV3b)Xz:VsHzI~_;&xyj\\L=s&|z!nm^?L\\u*T.11?#e^B|X?:J1YIt\\cqXF=T&yV|A+x=.x.{dh=^50J\\_2s<86C CTl@32T^R@
#?*h%Mh^b-E]FuQweQP/;>|8NA^Bi LBcnE'Q0^`uAw66imc*jqvZ6hjRVD=G{KJtlO0gp)j7yl`z,WOzK*tzRJwB8%*i=5t5-ILxPxY+R4(2J:-tQ+_Nwu(KICh@&6}PV209x=%bi~
'1QC^r!6w/Z}Qui~^os[P?$ "zopw$xQec;0v.^!CGm_)jL;j-#4%>HvQj4zJ0"^qy0C>1WT*98A"I)hdg1 *h$_nrQc}4x`n#(Cj^9}S_k$d@cPpIv:L3}{y&v;|V?Y.A47lK-]]ra
=pc[H'/`XCpw?M9r(X?64d?4`cSXVLgJUXI`?ByPEfXhLytl0ay!XZxj_KcwO',#Xn^4.D>|0Ien@$dG1*"%e?^q'>}+l=6L`IBKT{@UpgQ}r+d&YMTM/, o k~EXsppT^Pkf^\\D\\
H"'MQ#{Qi>7Od!`i]y1-Fn[R!K3Oy{MAa{D\\WRf,(W'Rj6ajj%o9.B4I(=Yn6N|Wwn$iXIT;lbZ!?#:kS2`x8[Qi4!`w\\4/D6$ip*?eOF-s4;#VS~H`bJem=e6]XX453o'}.j^7-?
bZf'CTfh!U>f:'"H(c2[<'l"Tm3Cp"ucVD%H"IP$}: mv4v5]Oa0)Vy;Vn-J2fzd`}+p"P^Za-c R&O-ghE~op5\\|YkSJH*CsmN)Kc:Q?Sm#Ci RGEwJ3S]?zVo*H'S%R(hZVY ~oc
09rp%W~pmlruMiAaHgV[('N)x.9Xo*De.PewiE2ZJ]vhDtaJGIt[~OYyo]"sjLSlW{hg:t$uI]Eq^oZWe}qRJ.}G)9MzTI,h=L5NrkDd6yC} xEb/,q%qgj2=Te-0R{,^Rn1>P]:Qu@
GguXb@='c>^{{>g)0j^SV<+H4rqZ8a'^9d}ABJ2AH\\Ns[&:H{DdrJOi'76jkYgM*)>02cjln-;<V41u9*\\v`K~oxcxIHc_J3a]R/fF@-i\\-Y(,%GmY:5tpiv2AL}m?kR#obT0zzp
J3<QA*yxP!"fe@^|hy![o(9x{pV8z,=Wvw+AYA\\4_z7{[X4#^@mJ&Bpy_C7S'XJ-u,:cSz'>@kgzUV2z]o7)/.\\P$vJWg0TV'N&)u5bt`~`t150f+9o&]G[q_NEZ~Cy/PiwAhWKQ!
}!Ws5_+`GgGdc-?+fG-{L*{nq[a7z3@dv!>d_vhQ4Ffh\\/BbBgj[.m9Q:/G}vKjidY,;4QZ'#ROvdh#")M<#[mVrTNkA^V[w*nEL46i\\5){)TXb>``8,I_'#_ljq6}YJ$@y!(sP*2
Hp?XEODk_|@DPoI~.LoB|<%TZ5hS4y~xc6AqO$_|o<-SF0pCv_^BB`4M_/pu`>+F=SBJIB'!H<{Ja@!JM?|GAUQ#2?3}Ta[T8K(/V)MA]5>ky5l&"Sv-uzH!lm/w_BOML8!8!PZMlo9
Q<@P4^$_cn qP__,5EN$uz%cP&p1%eo#K1L2 &NAsb*jR~}7,MU91OwuXYvHJa$AxA\\7SOgZ7"/^jgQl^V9xU:<7c9o*S#e;'^./PYRsewC=W'zMwDfm,{-J>`g(FR)~,OV]GAI0~d
Kv@$r!-:wm.sAb@2W'@!B|7_i/x1JTa58FE#~6@5klr3#=`'C@]U$m VXMmK$}b)o-vC88U8/*=AS6WA_rmUBQ62;Z?,/dxZ%Ec;G,%MGd|}NDzg<8.2]/5A&!AY[v}l;E`*wo RD%o
v7M;g_k{3rXFGei+rep`p^0[LBa]9+ayM7pG&]3"?u~w^mxrD<q+Gi5yH:V,:*7uc[UIpZMAo9q^]c@}n(Ik+@rT|pz:hkWi"{s+s|H4ti5wOpIDa#-B/M/ns{jHF~IuAWby&T>~fX6
D TZ]Z7W)#6ypE?!ARi!n^@HOsKUmd5-0J6^i Ic6?;/l6kvk+$~Fw`cKdX~IZ'/]wj*f4%v-&yD_KSZCWyxxWljLpae.k*i269Lff}ux87$ND3oJe3(bjK=bqH#y=u9/`Y*K~kX_WC
}Zy-j*ZU~.]xr5o gK\\#~f33,(LY&g;GL~0<?_s~Y0.P=dk82kE"bW2 K-aV@g;dg)SFIs<-Ay,uE}2E4}fawv$s,_&!X3[o5AEe1WZe6*:\\UBuTk6.4QI2/$skfZ<u1>1sY`j.y>
!##<rNI(mb.Ee|e-'h2k_-kDA7.aFn E;-_FKIjAP*3hwxM`le/t<!_KsZbU2`!%2jt?p6,L<XC-@C3(n&,[N7}w4\\6j1o;nE,s1jFm3q5d5RpY_mN!{3F8R>kjoT6!HX?^PEb_H6c
*w8>VkQ/e=S2xQWkydwTn$5U-/T{YRvPO\\Iy+0>J?!Rnv7*A/)2^1io(H5=GdNp#[6-p%i206c%V?#W\\wC0.?f%(w`2>V9j;0fEFfC~A] "raIn7E.bD_&3U"o9qz3SmujgT$$N#Q
S9]HAi2WlLDA:b%!CJD}#k#Q{E$.|fAlC;O`P2nhEPp?H\\rj|!+%0-\\KR9ir^oZWjx={9Ig7MWhY6)gY% <Gc\\!gd\\!DegvYsdzkYMr})LX:h)C`S#yV0ta-Epdfrbs&'v[8gBq
.wy\\o5-vYFpK1`~G`YT:lNV7,p(RAW]cVF-9|6A9yO7u@{#GWuGCa1'nsz<&A7|5~$ys3${-tg)7Od.\\SaCw:k{gURE1A:I.YA5]ywu&0?gybW-G#zw}Q>o`w\\A- 5M4[ckGIpHf
QY|LxbQx(O6=+cQSXXYnao$Z<t?X{GiU<*R6Ch#fnU8ez_:ABna1)+>Z2O&O@z&)l.)Q,J!~X1dyfiW?=<O?$MGgV231]'%G"K>wMm(\\%Zyk4j>v,umi^7>oy q(lL/QN^}b&a@-]G
.e~ZSQH+ajlSD,vX"U=,o?hjt{ZzGI{:[W*Abx"Hx/azL4-ypo=H$^JB-.(Q)73Q)B<lT>WU"XA$2H%JR+uP,2#'''
ci = iter(float(f) for f in unpack(city_vertices).split())
city_vertices = [vec3(x,-z,y) for x,y,z in zip(ci,ci,ci)]
city_triangles = '''+s7}=:mT}UKL4gR2t"?(2{{L+!TQF+<Lk7Rd&<Xx*qG-e.=7gC0~fF<<-)TTo+<xz49:+"AbeuoG[<i:~]2L=R~aOFO}BH(-y?uUQ2q1c:&(|JsLZlKOuh@
hS[>^C;uGvnqjdmxrMyN0M8egVO>M!mH3~R>tc3*s2~tazZ799Aj<IV/h5w$oKsmp7Wx*(1sK$P:BOqaiL9Aivop$NdojLta}N\\*b$4qkIb^dUGQi[8Q+{iJwH<AUcd|2^"at*4_(R
b4hjh:3owbgkavvHV.Rh07qta_j_JwCUOVu.0ql|A3DSJ9|~[anh0&X'D~Q/`-NYc]\\('piau`>3U[FUj%5'4#41vngl5SZ`|)A{j0ynB0'}]w>$av9?.d[(EjB6q~T=6g4:jx/7dU
~WNcaM/tb"\\4NcFVC}iLd:y&.)Wq0e95q]ZX/nga!uAun4.a_KhC)$s3qjP)ILj_2?>c`(qud(yQh$ c@LST4CMbNZCkv`9b,crFC)&'jNN>[E3_4;:c<{>zQ))cPNY*&tBH2vC b{
EWPCehgsVu^xa/4pG}K*x1DJxLwT5/;0/^%v8d:kB~*"*hd0@09T[U^[Fq8jSe!u=E.IzWw+~<e?.L1K$wWP5hAh'ZywXgs*OR~_e9F8l"R9-hI5e8Ti&$<pe2)0OBpm18"}e}J.C>8
Cw/wz(7GiMZ/yN9d-a5Ny4k!oldMYk&;+=bDo[%J(UEJ6K1^yj_|6`0YL SY>,r{]-8qcX)z#8PU=nqnloL3f|#g-5\\pdB)q>;k_]@d'+9x]4TZqM3"u|ssGky0+EbV(fp}Yt'.L, 
@|Gh=`0Xo:InD*$[bE/2lead@y&"Sz$pMx/Z}xaC25fK/Ul'3{8 am]sz#LSWJym/kpi\\Cy{'V"fqH{"s?:QhUfv~(;s{QLs%9;vr A7(J9v|`6NT)@JV;+^B+$N3K-,;%>A*g_`ny
Qd5Xv':jXbmm'Q-/UtIQ[ine=%F-,i3vNwW8Y_5RRWKg#rj`]|-RR,Yv^]]0BQi4~q@(zMP6,KD1lVAo`&<Kb0#fUXeXU8}XDh==rn5tXNo'U"+bsGZ"+eVp>9R?w$V,qPC*5q-vc[D
>H8)ZTRkdR@s0?h`K:(SACfl%UI?(W55_!&guK:<C'C~wUBP0[uDXKg_!UPr~-Ju-UcN&yM55T@;J1[Dg>Vcf?.WtS@GNZwV/S4RzLo{)13|C\\S@#hAG"!JzW1F[K"l{&$c:a|iDaF
;*isdY,ytK7%pL"(\\VzOVS4?^bx\\4*Xq`X;/Ydsn='Dq'koyIyY4e5AMN!M)~BOn5BnrbOkIP;]hB/YeIXpHAY,J5spjh[[l>:cO3)p[Gtr)d;o65&vTXD|_hAxVkICEsWHVef,Xd
^/SuO|ENp.S 9cQjjr|C)4K:>\\1{]|UR7gL![Ufra d90W!*-r(2|VH}9I!+Y+lgK{.3\\lm`1gfM`?`_@|`(Q]y.A.%:!mu55bR/G 9-P"8)BUcdEdh\\po9>q!Dx+@NT^.@S7t 3
D6lqXH[6lZXigb]nP)7#QU5MxPj`F~F]_N%uU^~Hzr+By$Fa S#(b3)lIPWzZhvmlu/u-uaFI@|>m1~~"c(ycRT4D/u#Z-"!GPk6yUGh:NHVHXi\\fAg:v[vy/48*j9lp>V~?|l ,$k
K/6n[J-^PfVmV+f>K)WFGzE.K#nbh$d[mn".:<s`qTpXf,5q2loC>vHpx)](:p 8(^A\\|rj+(hrekh9qW5fd+'WBFfY98wq7*hkMms# LL'q=)'de7RDn6z0k[?^fc/7 9akpYx1EF
~6@]B0)`;-"M;{arQ5vk,Ev3B=WwTSXxbOBX/Uue$5a@j|k,~>p_Y@Ugx~hLE80<x$gL- BO9?79T#B^77ek;5T>,Kv>PS2P?\\*YK5J:*7!k(E=cS'! C?G_6P71nS^.W,rnrYxGz,
XQ1Us0n80=w8fmjlpD*Y\\#%*/ue#(a{v]G5U[vpzH_bNfhJR?sme[>iRpY#bJ#6Ls~~P']*,j\\+d{!~*J3#hOTld={FVbS/w8tuj27?zy|9L=zF HIo##Y2mMC(YOrr][pNi**zBc
yt,{5!lqSE%{oR+=0E_CQPX@cNYRPv~5Rk5ASRMp\\0V+n:(Ta(xf-Ow$SX7cla8e/jT@E%z3  sco>{]a.=i \\\\N<gRr@Ziv^5I:2B]7Tm*1if[hxQ7B^X$V{q_%u/;WF|?!_ 2g
Lc*v#[?7_0Ts5$By{8y6c2Vu%fk(P/p]M%n>ItN7Ct6s`:~9|8bH6"'SSg}[#&K$ vpXC(LRi=rjxtR%{RiEFYNSgX"5;xK4<bBgjd5_5/FGk=&0yg%+2%pwY"_j1UfLPPR2gi?JuTs
}6;{ge/H#La0/lDa+P}[T$~]N3I*|wv}@<TVynAK-" $hjhU&9x3ndPAj,k|1/}cP)o`T[Y-[~e&kWf{#;0Mc#Z'VU}Dwj.pPRsV9ed($py{9H4agkKAnrJf9.n-FHFj0>WAdyQT;h&
NCc$R)7EO96NJN`Cd\\P">5BUr8){/mr/k3~K:GsK,udRa6JT_o6Gi,Lzpom 5T#9}&T0-`_S%eGfUQyBG)\\k4Ie Fx/~v${2wg#E]W;Mgb/:+\\wIw{tzY%.u1QKCio3Tk't?XaE5
)@:\\+yKI{k0/U|%&mZWzsi Uj<kV>^|>I 2Qg8EQv34.''cG1hI.g=0TL4VfMBs5w1=w8uie61'JPD)zbEeFq/^g|c<*eTff|AEK?:'c<PP?-y9\\ PP5a6o/a\\[f3;>?@RqAqZ`c
T0eT)yzGmf^Ge8Z0Prb`KZ<O-6B92w\\~&Zf$zcm9Xq$uz^q+(.SJSTX<@r\\3v`),86JBQ.6(24&2!>~~hq-V51b,\\UkipdiP~'NKCet]`QU\\v\\L/<8C#5ve&Z%44nq,ScV,nn;
w%Nch63cH7oLuJB)rnAGkBlhwycuy,w,ecJ`waT6fbeYte/z}bGDo;~&@>E1XB?9!B"f'vM_JT<GQ"l)Y*M`hP\\>G2#p>[cHFy\\2(EG$ZNYU02tXan}Mv-m38vr1z6]&o5oP)-:m>
vytRp,D%"0 [YLjwB<"iuVrji=gv@9H:k.gF`[YB`cMXYfjZDd09>vk?W{GFiiv"OG)m3z&IL&v'$aSu1^P4[N)U@W\\<b>d4?h[@2e"$}-]sUE&{&h-1Bh|!I5q|/7r_@mWh7C/,1v
-%QNrm]#/fhf|f3)-v'qN`^4=#'e?<4rW]7DE\\{VA'Lb[JFQD|#0jRp("9.3aMmGC)<;x?DYi:,u\\4+RUDSm=scAii|^"}`{ReXf[UaShB7jb35E^4M#q)YU{J*"~]s94>^=0@s<2
5/:r[_siD}_:P2L/@ZB#7*#Gjq2~B#hjT"7F^AD"$t/:150y8|>e|!bM?dA2[(L=XT9n=g}+AhCD:;QF]W3u'%K]r6]]6B04e3e}-Bel_Td{?!k0s'QM-".pvbt*w_ZPRMnOO8EAo$R
mom';%41* i9'o7 9,.m*oy`\\*i/ :.03yopYXWS3r`BW>xOL%lnqx,HGh,)BHaQg'1Ayz+=1b5w/Th1%b5W[LzQXWR)J ,o:/]GG!H<fu)"`QD"aj$?(wcS\\kbU`nw>{vCje+"A0
m%b4vTIYZEb)Nmy2vY*&&!e+|d;S8>cFsG^+D*a O5#IJdZ-<ip]O[D9z$`kS;5s4G0V$?=l0VO<Iw,3;}94"MO0!#I,"kj: Hmf&@bW0$^Qwq(=vNFaRfc}*W^ GO1^Yd[/I#.7[X.
5a`PJC[zNPP:?/P8Ek-G.I^J":uq)OXE\\=5P,Btj+F}dIUF*waoLCa#qE%O@IryCY?:E,8Mr{j@;(f#0<P%[,"!A9L2(~FfTal]Wi?Y8:)gylEHfS[5hPT9P-p>0[oth,C_;iDm?;N
-TH3lLv-L_lQ4Nf FWFfbVfcV!gPKk7L)>#UvS`t=KC,*!(zM\\JB<b8 v-}@Fa^#!Let,C_:FxHM0q,&"^Z~m6e7T=,T?U^LDig("J4x"}KPo_IP3W`#:IVr0|/8b%p-l,fzQ #E`W
p;eEyW7m4) e#WKWrZ/w^I|'-Bdu6<M)pw\\r2;ePxnMrY[F+3o[D8YVmaA"|!&%nif@6d,>SKsD.p"T\\@JxeB<bV ~LAx[]p*|BTl<fFNYQSXj`A^IAow$Ec NR`LA!S;R^5(;9aO
$J9Z7l :)[M@"cBLYUNMM(JZOy\\SqaH 3(!6c:lcp2ffC5N2N2Yqj0hh3^o`NhhtG6yI-@}v{i.&^CETOBoe5M>_<N .Xv*!qCk"pJp_d3[3|`0>;N3Fn2S[ROkRX)Mn!iE:NI'"y|
}ZI"2P+J03wW-v9TW6{`ID"rl\\BL8VEp$8= sbF"#8r-XWwJG#m:Ed%kHJ;l#Q=cF*g\\>$80Rwii>$~Y)-3+kaM%?F&%iQ*3a{FYYT[Px5K[^a70?nd28^a+ApFL5*/~"+W5io<.*
.PMcQ5>31u~C,vOh7>n5*FV<\\%Rpoz+_Sq2\\(k6F[T1xxWy\\/Dr)*BqP;7kk"[Q|#{quKM8&uqI0Tr~T"8q9.,t:|SiA95nPS63)V04Z7EbfkoEhj#(Et`;>1%((BrI&_)h3WHQl
uh7*6u9.JOHcdhMr@BZb0qq;_z_4U`9&9'-iK1NS4Y-9ERs{92r|`6;!GCb\\Wb-SV[p[l)s"-M.~!%3ly3e~!3TfD\\Eb_<9u8scbz_J@p3;.QnA~m(pW.2}]Cf8j@3lN|OMSPB~!K
)80D7@dURtA_+Jf\\yp)"E{I&B:UyL!~&]|R6c%s||c;#Y3RfEx07T9N/t$9"l)LG!.U3H~zv?:$LB+|SzU+"C)mC6Lcz,<O*h5Xzh[1qX[V#s,b52C6+7$|,RDUIf<mb|:ZfGT>?[_
5x=tVk14kTLQx4W;9dJ?uoyqy'zH1.X-K]C)udx^5V((lTMk`EQ8t-53)]&AQ(0l+>Xju'u6w69X^Cx<4l3b}*rE)P|"5$M`;1|jku}92YU-'k94d6FHq7X<$IV5bNiqS-<2D4_Br0R
5~>rqN"&D-w`l:!Br=J_v$t#q\\(uS@0~k(n#;fY<'C/Vu Y$IgQd^%SzZv!qaokr1]@; _7mvW7%&"\\TT1|4+hy0M@&/ERt5iL1N#TFkb$g|'^bWC5u9P6rRX8$LdLBkP1PX M.#n
;*(hj:gm68ovN?np7*grj1{}X 0 8B3<\\S9xZNZgGpW!MNH806Z6n~Ug.!uU{2f6;%g:NO(mP-^[9m|?29{Bsxd?C3.'^J7LE?m.61smReKx*~%-@!TL)cyAC]~vcPkqrY/~zr#!:I
;u",@!s0d|9_zAoH,;y%epE.fABnB#T(,4:'Ou6]N^Pfgvw#|L79OIOIt.l [6{%K)i6}(X;+?'(s'8e,1"szrh`R{?4M:Eu)8bnI]|jl!&o8,sphJ;Aim[_7}^O>$&=$hA/!(aA,5P
R8M{3seFP?87#kjs sA?uigo$l>>["e::*P/b~Q!6PhqtlrQh,^W?']>k&LnbEZ>S:^T]?/4%EQwW7dW(-v~MH]~'n-7 cifjkX\\3aI}'8M4w{#WP17(</sfspxB0pu/$(^>w^-@e*
IXI'LH(&QIBI*w"'9(};'fu#fJ[rN2NYj_0r`+Eo-ACU6.,=;*FBw6^u=Ik6QR1M3Vp$ZR^qm,9QhH%owvJ4.N'QYRLR+{ta<hN[}JVI@2=+nmOVGq:1Gu8!k$~uUH^D{re6B6L@E_P
Y"wh H#3^{R2eBz\\SJ%=4dy\\>bW$mER`sY"eQrC;264@op(s}cyC3=jBJdNa~co%l-pSm)G #@zDyteE@gI]68F_52v->fqO5QnONasf9fNq5t2[<s89SYrv"u;v~5InSVir[:Pmh
80$9As \\{j?1#I4:;-Tw1``NC%/J3k]NB,<kj/ZeX\\tW]CIwEZ/CZxVV]9Nw"VVv.T9o@5j=b!Y\\+ve31Ui\\T3%~\\5$$MI1K}4V&ZWIJW`PRW7!`FylIL1l9U$} ;{~%QCZi>D
Hc@:$KCXow'}oY'COTTm-^`]ahy#ONP'l:#spv:v<DVV#:w|7~eIM3>P-" EZZS/!RT!7"UPHvr?*%NRi&W(S\\eq%(>!!a\\O&=\\,;0n'tM%Wn:`OS.<em;wIe7 v]utND84OahZG
=nW#JkZh?^W8(k=AS'"Uu8B=b4&5o${t*R$@8zP]"u;M,0ov? A-zDR$0NRyXL$Q)j'w|wF"VLVI~(>];VUDht^B`],&q)T!4C7f: 9<g^uOx=t7x"dLZ?R5K6O}03+!W(f77J?i\\@
0rO^;*<_Eu>\\BTx}`bLzleF3jyt-DNAD59^+rTP8-133(*q:)pW:SS~?tM|.&<A>m$&7N5Y=F'pbN]SUW`bRtlY3GA93eBik-*f6I!Bj-,Ln~rE,*_NcRQz:t#%kLYd3HCh13hF'g/
(t82)Cnt4a4P#NVXcYX}B4:C|3OuFAzAdnO&>]_aYSt$vortp]*(5&-]014n{>c( 5@&fVJUm,o&aNHFq6W<KUGlVM9JLe)]:Xk"(G$8|nk)cC,WHAxDU:IR8K9Xz|<r`yX?);RY^=y
|wC9kV*CmpG/cRC6xFK+zk:JF%HI(cr`z<dbiYDh]UMev>zq uP;Rt1g'}V%*v1my["I"6tRhpzQ!/Rr9ovPXT|=tQS1JS0v5XxDuZ9S?Tp81X@fpqht3P[PnYCLoii=U/qv}JBof[U
HI"+ka^9t'/ -(.)Gn;{{d$c}n+8fV(H.u&^{{Quja63 crLSNBVBl^oM.)P{DQt=`;`_<OGMOl"~x!:|]F< _Ok}pJL)xJ&<k;UpZ+o$d$t=I~-w2;p#v{bcFh<h0m[r03TI <En3g
P!. &h9X[]"sS<^2&_`.z}N88SFVa5~K((H1J&lzFt9yp&';])^zIFWN@J];XigS ,dsE{[3Lf`LHX68k,MhD2qla#~vE]A^s#tAY09xaTC<;JRWQ#:E%gH))pv),A"iew;EJB^F"``
/m "?)W4ewdZIwtKLzQ<+\\X~G)fbZ:Jxwu#5Bs^X3$`9I}%FE@`!BBi~:h!Ij,)@(IBS81X]Ju:*bwK^&zV84Vy#'";/aipguZI}MrCOc\\1(cs}'J`~FyN/F~w2OtTnskQDKZG!n$
A|4W3o$|wK/3S$QkUt$C=?jUc/heESIV#]ic@H)>2+CNmga(1:(pS}`+!<3Hk.EkC[J~Bnl1$/t2A1/z(L8y^NG'<yc<(dFVv<(Z{VIRL*P-P->tgQld7w|$NtTB[u=A&6YG;',_Qcw
9]c7rP"&OtYErC5"z{ZR35l+>H39zg>TY`HqavNt,j 3f{7&"TTKOm5KZ2N?AWqmW~ZGAG.w=UAUvm!1[~p=@*@*9HS~00(zd3Ueqlcllg>q69IA^TxG\\#AE<pKREh?FKWiJ|',{=}
H*nzd!CWK*&mat8}zn*F&$o;%(ex/G#8r'IRzHMYgZ[9pRt*4G};&B0x$%AI-Y@;qkc#dJ\\X3SN+?eG~dv{`WbQ#YP@l't-b-CP*D(L8H8vy<!mb#bRIn\\-3u\\RL?v]/yaHa\\\\
`.oN:!&^|L);*;>*;[I~QbsTQ<gK%)*q#iH/~,4FWm_$S>z}r/ #kJsD =Z)&mU$s\\o`CrK{X^f;b_}aA @1t|0`qC>'NW2-bT}o8:oLRv*dri?fJPXYDr]>dS0C9zGqI-AS2I6}h\\
$|$3tGph!Xg9N$ eWVjDWThY\\X|,I"pq_Kpi5eR^o~0c6et_f3 `UCZ3`a5+#J?$gf.xM.bLvWX+MJ-`dpOj+\\HD6U_}z1wL;.{F=H{0CZLyiN?"j:z@r]$jOE|~{C49C1z+c50jm
:dhlf9uAaP4"Z.4J\\^L(98-}]NQL]&SSxWb>ds/H5Hk.tW9$m]Ro7XuNacjKoaAqt)uI0h}I6'gY~G3>-;bhQbm]OSqh! v)-+$.r3Ywzr[f;U)Rf,AF^(eU*$ A13~OBj*sY)KkhL
Tcs@N@_HZ<e]V8u y<5(|oEthkOH 0x W{7;h]Y9-*BrqE7'N9)^{3+OUreoGHiAY.#TC=DJ5)=f{zJGl"af84T5S&V*s3<V8-0Ln242,31}%}mq'UM4\\k&x%~#xnrA5+8f>*X^!}X
`I#8n|2dBmZl+?%Q,9Gs^OV09pSTW8E21Zb!z2JA\\)^axV4l*SXzk~wm&Bm]eaU"VYW7Exm"V8'''
city_triangles = [int(i) for i in unpack(city_triangles).split()]
create_mesh(city_vertices, city_triangles, mat='flat', process=nophys)


# Create some ships to liven up the stiff, gray city.
shipascii = r'''
  /\
 /XX\
/XXXX\
'''
def create_ship(pos, d):
    pos = vec3(*pos) + rndvec()*20
    za = +pi/2 if d<0 else -pi/2
    vel = vec3(+10 if d<0 else -10,0,0) + rndvec()*1
    return create_ascii_object(shipascii, pos=pos, orientation=quat().rotate_z(za).rotate_x(pi/2), vel=vel)

ships = []
for x in range(-2,3):
    ships += [create_ship((x* 40,40,20), -1)]
    ships += [create_ship((x*250,250,45), -1)]
    ships += [create_ship((x*220,220,25), +1)]
    ships += [create_ship((x*300,300,60), +1)]


# Simplistic semi-transparent triangles to create an illusion of smoke.
# Create the particles as clones to improve speed.
smoke_template = create_mesh([vec3(0,0,2),vec3(-2,0,0),vec3(2,0,0)], [0,1,2], pos=(0,-500,-500), col='#fff1', static=True, process=nophys)
smokes = create_clones(smoke_template, [(vec3(30,115,19)+rndvec()*2,quat().rotate_y(random()*pi*2)) for _ in range(30)], static=True)


while loop():
    try:
        ship = next(shipiter)
        p,v = ship.pos(),ship.vel()
        if abs(p.x)>(abs(p.y)*2+50) and abs(p.x+v.x)>abs(p.x):
            p = p-v.normalize()*abs(p.x)*2
            ship.pos(p)
    except:
        shipiter = iter(ships)

    for s in smokes:
        p = s.pos()
        if p.z < 26:
            p += vec3(0.1,0,0.3)+rndvec()*0.3
        else:
            p = vec3(30,115,19)+rndvec()*2
        s.pos(p)
