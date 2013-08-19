//Maya ASCII 2009 scene
//Name: bird_animated1.ma
//Last modified: Mon, Aug 19, 2013 11:40:32 AM
//Codeset: 1252
requires maya "2009";
currentUnit -l centimeter -a degree -t film;
fileInfo "application" "maya";
fileInfo "product" "Maya Unlimited 2009";
fileInfo "version" "2009 Service Pack 1a";
fileInfo "cutIdentifier" "200904080023-749524";
fileInfo "osv" "Microsoft Windows XP Service Pack 3 (Build 2600)\n";
createNode transform -s -n "persp";
	setAttr ".t" -type "double3" 1.132289798505024 0.12533553107262338 1.4519335389595653 ;
	setAttr ".r" -type "double3" 4.4616472703625512 37.399999999999828 2.5022785640260851e-016 ;
createNode camera -s -n "perspShape" -p "persp";
	setAttr -k off ".v";
	setAttr ".fl" 34.999999999999986;
	setAttr ".fcp" 3000;
	setAttr ".coi" 2.0835459640971599;
	setAttr ".imn" -type "string" "persp";
	setAttr ".den" -type "string" "persp_depth";
	setAttr ".man" -type "string" "persp_mask";
	setAttr ".tp" -type "double3" 0.0032140837306451342 0.58474283021157336 -0.0090874391520679998 ;
	setAttr ".hc" -type "string" "viewSet -p %camera";
createNode transform -s -n "top";
	setAttr ".t" -type "double3" -0.073217992396798368 100.10056471874404 0.094583040349039971 ;
	setAttr ".r" -type "double3" -89.999999999999986 0 0 ;
createNode camera -s -n "topShape" -p "top";
	setAttr -k off ".v";
	setAttr ".rnd" no;
	setAttr ".fcp" 3000;
	setAttr ".coi" 99.742375383393906;
	setAttr ".ow" 1.7908165529050468;
	setAttr ".imn" -type "string" "top";
	setAttr ".den" -type "string" "top_depth";
	setAttr ".man" -type "string" "top_mask";
	setAttr ".tp" -type "double3" 0.072356000770563256 0.35762461660608835 0.038142939703309814 ;
	setAttr ".hc" -type "string" "viewSet -t %camera";
	setAttr ".o" yes;
createNode transform -s -n "front";
	setAttr ".t" -type "double3" -0.047365510368282904 0.39038458642502449 100.10077153933737 ;
createNode camera -s -n "frontShape" -p "front";
	setAttr -k off ".v";
	setAttr ".rnd" no;
	setAttr ".fcp" 3000;
	setAttr ".coi" 100.1;
	setAttr ".ow" 2.8336260670748366;
	setAttr ".imn" -type "string" "front";
	setAttr ".den" -type "string" "front_depth";
	setAttr ".man" -type "string" "front_mask";
	setAttr ".hc" -type "string" "viewSet -f %camera";
	setAttr ".o" yes;
createNode transform -s -n "side";
	setAttr ".t" -type "double3" 100.10082945212631 0.30391981013795949 -0.13449130510905608 ;
	setAttr ".r" -type "double3" 0 89.999999999999986 0 ;
createNode camera -s -n "sideShape" -p "side";
	setAttr -k off ".v";
	setAttr ".rnd" no;
	setAttr ".fcp" 3000;
	setAttr ".coi" 100.1;
	setAttr ".ow" 2.7115353843623597;
	setAttr ".imn" -type "string" "side";
	setAttr ".den" -type "string" "side_depth";
	setAttr ".man" -type "string" "side_mask";
	setAttr ".hc" -type "string" "viewSet -s %camera";
	setAttr ".o" yes;
createNode transform -n "m_bird";
	setAttr -l on ".tx";
	setAttr -l on ".ty";
	setAttr -l on ".tz";
	setAttr -l on ".rx";
	setAttr -l on ".ry";
	setAttr -l on ".rz";
	setAttr -l on ".sx";
	setAttr -l on ".sy";
	setAttr -l on ".sz";
createNode mesh -n "m_birdShape" -p "m_bird";
	addAttr -ci true -sn "mso" -ln "miShadingSamplesOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "msh" -ln "miShadingSamples" -min 0 -smx 8 -at "float";
	addAttr -ci true -sn "mdo" -ln "miMaxDisplaceOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "mmd" -ln "miMaxDisplace" -min 0 -smx 1 -at "float";
	setAttr -k off ".v";
	setAttr -s 4 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode mesh -n "m_birdShapeOrig" -p "m_bird";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode joint -n "root";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -bt "lock" -min 0 -max 1 
		-at "bool";
	setAttr ".uoc" yes;
	setAttr ".t" -type "double3" 0.0016038272517390779 0.2237275349598738 0.073059643295614818 ;
	setAttr ".r" -type "double3" 2.2967713873819848 -1.1778385028756484 -0.97919351428296786 ;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" -9.2313011099339697 -17.106186694450972 28.922298882026709 ;
	setAttr ".bps" -type "matrix" 0.8365552480583418 0.46222819223408562 0.29414352831577878 0
		 -0.43605820884141433 0.88676101532713381 -0.15332364526715458 0 -0.33170552519978264 1.3877787807814453e-015 0.94338297872705779 0
		 0.0016038272517390779 0.2237275349598738 0.073059643295614818 1;
	setAttr ".radi" 0.14281454949670896;
createNode joint -n "tail_joint" -p "root";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -bt "lock" -min 0 -max 1 
		-at "bool";
	setAttr ".uoc" yes;
	setAttr ".oc" 1;
	setAttr ".t" -type "double3" -0.12803686544781803 -0.010370690163625402 -0.30233168188405651 ;
	setAttr ".r" -type "double3" -35.718132939517076 15.338855068485127 4.4762128042101645 ;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" 0.2572375662194436 0.62890310680232719 -5.2846187854334881 ;
	setAttr ".bps" -type "matrix" 0.87675009663600267 0.37856706895201159 0.2966416059049079 0
		 -0.35859776065196791 0.92557386215530246 -0.1213287754422296 0 -0.32049479575205847 -8.6138779195960403e-016 0.94725027627119029 0
		 -0.00069877058248852923 0.15534896236547002 -0.24822606266121822 1;
	setAttr ".radi" 0.14281454949670896;
createNode joint -n "tail" -p "tail_joint";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -bt "lock" -min 0 -max 1 
		-at "bool";
	setAttr ".uoc" yes;
	setAttr ".oc" 2;
	setAttr ".t" -type "double3" -0.091738647420243916 0.051178505091510784 -0.30882281733808647 ;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" -5.2883316572091414e-014 18.692850666535755 -22.244951820100876 ;
	setAttr ".bps" -type "matrix" 0.99999999999999989 -1.1250775417139479e-016 -5.5511151231257827e-017 0
		 5.7735030615735775e-017 1 9.5099448957170501e-016 0 5.5511151231257827e-017 -9.1309433856506677e-016 1 0
		 -0.00050703012642938465 0.16798921811885098 -0.57418148674957015 1;
	setAttr ".radi" 0.1;
createNode ikEffector -n "effector6" -p "tail_joint";
createNode joint -n "left_wing_joint" -p "root";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -bt "lock" -min 0 -max 1 
		-at "bool";
	setAttr ".uoc" yes;
	setAttr ".oc" 1;
	setAttr ".t" -type "double3" 0.22644391445877424 0.0062685295213627812 -0.018135766100818573 ;
	setAttr ".r" -type "double3" -10.120941426531427 -4.2336961647116871 22.620974897492005 ;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" 8.260016499449823e-014 19.372326660016611 -27.530982397135627 ;
	setAttr ".bps" -type "matrix" 1 -7.1755830721418492e-017 -5.5511151231257827e-017 0
		 1.711367925871757e-016 1 -1.4008708495817237e-015 0 -5.5511151231257827e-017 1.447984758105949e-015 1 0
		 0.19431896234731832 0.33395498378553468 0.12159656841778753 1;
	setAttr ".radi" 0.1202201305972303;
createNode joint -n "left_wing" -p "left_wing_joint";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -bt "lock" -min 0 -max 1 
		-at "bool";
	setAttr ".uoc" yes;
	setAttr ".oc" 2;
	setAttr ".t" -type "double3" 0.2535594696890856 0.13635603061383517 0.1083789356697686 ;
	setAttr ".r" -type "double3" -1.9461757421352537 5.7809910127460737 -23.388683235920528 ;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" 0 15.371927684568945 0 ;
	setAttr ".bps" -type "matrix" 0.96422539888122427 3.1820262504589121e-008 -0.26508372291097559 0
		 -3.6975185275218747e-008 0.99999999999999922 -1.4456377431892941e-008 0 0.26508372291097482 2.3740726135495274e-008 0.96422539888122472 0
		 0.44787843203640393 0.47031101439936995 0.2299755040875559 1;
	setAttr ".radi" 0.12420427419457522;
createNode joint -n "left_tip" -p "left_wing";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -bt "lock" -min 0 -max 1 
		-at "bool";
	setAttr ".uoc" yes;
	setAttr ".oc" 3;
	setAttr ".t" -type "double3" 0.28526957363589228 -0.096177927593718682 -0.38235365254075671 ;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" 0 -15.371927684568961 0 ;
	setAttr ".bps" -type "matrix" 0.99999999999999922 3.697518537459972e-008 3.8857805861880479e-016 0
		 -3.6975185275218747e-008 0.99999999999999922 -1.4456377431892941e-008 0 -9.9920072216264089e-016 1.4456377479006828e-008 0.99999999999999989 0
		 0.6215868743362325 0.37413308680565072 -0.21431991826949304 1;
	setAttr ".radi" 0.12420427419457522;
createNode ikEffector -n "effector2" -p "left_wing";
createNode ikEffector -n "effector1" -p "left_wing_joint";
createNode joint -n "right_wing_joint" -p "root";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -bt "lock" -min 0 -max 1 
		-at "bool";
	setAttr ".uoc" yes;
	setAttr ".oc" 1;
	setAttr ".t" -type "double3" -0.098673078095677422 0.17573725147690916 0.11077800045948516 ;
	setAttr ".r" -type "double3" -17.52715704772293 -13.266058466100342 37.934172403339993 ;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" -179.99999999999991 19.372326660016611 -27.530982397135627 ;
	setAttr ".bps" -type "matrix" 1 -7.1755830721418492e-017 -5.5511151231257827e-017 0
		 -1.0398430075915929e-016 -1.0000000000000002 -6.2662641083186521e-018 0 5.5511151231257827e-017 -4.6948260932454516e-018 -1 0
		 -0.19431899999999996 0.33395500000000006 0.121597 1;
	setAttr ".radi" 0.1202201305972303;
createNode joint -n "right_wing" -p "right_wing_joint";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -bt "lock" -min 0 -max 1 
		-at "bool";
	setAttr ".uoc" yes;
	setAttr ".oc" 2;
	setAttr ".t" -type "double3" -0.253559 -0.13635600000000003 -0.108379 ;
	setAttr ".r" -type "double3" -2.1074669093312561 13.063798938658971 -29.251961319806586 ;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" 8.5377364599126052e-007 15.371927684568936 -1.9290256496994082e-015 ;
	setAttr ".bps" -type "matrix" 0.96422539888122449 2.5919348095771279e-008 0.26508372291097537 0
		 2.4992093717915662e-008 -0.99999999999999989 6.8707973003972079e-009 0 0.26508372291097559 -2.3548173724231252e-017 -0.96422539888122483 0
		 -0.44787799999999994 0.47031100000000015 0.22997600000000001 1;
	setAttr ".radi" 0.12420427419457522;
createNode joint -n "right_tip" -p "right_wing";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -bt "lock" -min 0 -max 1 
		-at "bool";
	setAttr ".uoc" yes;
	setAttr ".oc" 3;
	setAttr ".t" -type "double3" -0.28527026756871343 0.096178000000000041 0.38235405939819 ;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" 8.5377364625159387e-007 -15.371927684568936 2.7033800320058785e-022 ;
	setAttr ".bps" -type "matrix" 0.99999999999999967 2.4992093750144137e-008 -1.6653345369377348e-016 0
		 2.4992093717915662e-008 -0.99999999999999989 6.8707973003972079e-009 0 1.6653345369377348e-016 -6.8707973113583022e-009 -1 0
		 -0.6215869975963102 0.37413299260598076 -0.21431999933918039 1;
	setAttr ".radi" 0.12420427419457522;
createNode ikEffector -n "effector4" -p "right_wing";
createNode ikEffector -n "effector3" -p "right_wing_joint";
createNode ikEffector -n "effector5" -p "root";
createNode ikHandle -n "left_wing_ik";
	setAttr ".roc" yes;
createNode ikHandle -n "left_wing_tip_ik";
	setAttr ".roc" yes;
createNode ikHandle -n "right_wing_ik";
	setAttr ".roc" yes;
createNode ikHandle -n "right_wing_tip_ik";
	setAttr ".roc" yes;
createNode ikHandle -n "spine_ik";
	setAttr ".roc" yes;
createNode ikHandle -n "tail_ik";
	setAttr ".roc" yes;
createNode lightLinker -n "lightLinker1";
	setAttr -s 4 ".lnk";
	setAttr -s 4 ".slnk";
createNode displayLayerManager -n "layerManager";
	setAttr ".cdl" 2;
	setAttr -s 3 ".dli[1:2]"  1 2;
	setAttr -s 3 ".dli";
createNode displayLayer -n "defaultLayer";
createNode renderLayerManager -n "renderLayerManager";
createNode renderLayer -n "defaultRenderLayer";
	setAttr ".g" yes;
createNode polySphere -n "polySphere1";
	setAttr ".r" 0.8;
	setAttr ".sa" 10;
	setAttr ".sh" 6;
createNode polyExtrudeFace -n "polyExtrudeFace1";
	setAttr ".ics" -type "componentList" 2 "f[26]" "f[36]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
	setAttr ".ws" yes;
	setAttr ".pvt" -type "float3" 0 0.25000155 0.76175004 ;
	setAttr ".rs" 36009;
	setAttr ".lt" -type "double3" 0 -2.7755575615628914e-017 0.22250706507221668 ;
	setAttr ".c[0]"  0 1 1;
createNode polyTweak -n "polyTweak1";
	setAttr ".uopa" yes;
	setAttr -s 52 ".tk[0:51]" -type "float3"  -0.066681445 0.1928172 -0.078744166 
		-0.025470046 0.1928172 -0.12741072 0.025470037 0.1928172 -0.12741071 0.066681415 
		0.1928172 -0.078744143 0.082422778 0.1928172 1.796645e-008 0.066681407 0.1928172 
		0.078744173 0.02547002 0.1928172 0.12741074 -0.025470054 0.1928172 0.12741072 -0.066681445 
		0.1928172 0.078744166 -0.082422785 0.1928172 9.9813597e-009 -0.11549563 0.11132307 
		-0.13638891 -0.044115409 0.11132307 -0.22068189 0.044115413 0.11132307 -0.22068189 
		0.11549563 0.11132307 -0.13638888 0.14276044 0.11132307 2.3811941e-008 0.11549561 
		0.11132307 0.13638891 0.044115383 0.11132307 0.22068189 -0.044115417 0.11132307 0.22068189 
		-0.11549563 0.11132307 0.1363889 -0.14276046 0.11132307 9.9813597e-009 -0.13336286 
		0 -0.15748833 -0.050940074 0 -0.25482148 0.050940093 0 -0.25482142 0.13336286 0 -0.1574883 
		0.16484557 0 2.5951536e-008 0.13336283 0 0.15748833 0.050940055 0 0.25482148 -0.0509401 
		0 0.25482142 -0.13336286 0 0.15748832 -0.16484557 0 9.9813597e-009 -0.11549563 -0.11132307 
		-0.13638891 -0.044115409 -0.11132307 -0.22068189 0.044115413 -0.11132307 -0.22068189 
		0.11549563 -0.11132307 -0.13638888 0.14276044 -0.11132307 2.3811941e-008 0.11549561 
		-0.11132307 0.13638891 0.044115383 -0.11132307 0.22068189 -0.044115417 -0.11132307 
		0.22068189 -0.11549563 -0.11132307 0.1363889 -0.14276046 -0.11132307 9.9813597e-009 
		-0.066681445 -0.1928172 -0.078744166 -0.025470046 -0.1928172 -0.12741072 0.025470037 
		-0.1928172 -0.12741071 0.066681415 -0.1928172 -0.078744143 0.082422778 -0.1928172 
		1.796645e-008 0.066681407 -0.1928172 0.078744173 0.02547002 -0.1928172 0.12741074 
		-0.025470054 -0.1928172 0.12741072 -0.066681445 -0.1928172 0.078744166 -0.082422785 
		-0.1928172 9.9813597e-009 -1.228195e-008 0.22264615 9.9813597e-009 -1.228195e-008 
		-0.22264615 9.9813597e-009;
createNode polyExtrudeFace -n "polyExtrudeFace2";
	setAttr ".ics" -type "componentList" 2 "f[26]" "f[36]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
	setAttr ".ws" yes;
	setAttr ".pvt" -type "float3" 0 0.49089006 1.0558406 ;
	setAttr ".rs" 41648;
	setAttr ".lt" -type "double3" 0 3.7790489029743135e-008 0.19170665884938792 ;
	setAttr ".ls" -type "double3" 1.4521521549027265 1.5511955553120325 0.019643170792250815 ;
	setAttr ".c[0]"  0 1 1;
createNode polyTweak -n "polyTweak2";
	setAttr ".uopa" yes;
	setAttr -s 7 ".tk";
	setAttr ".tk[52:57]" -type "float3" 0.11520029 0.30607352 -0.091730528  
		-0.11520029 0.30607352 -0.091730528  -0.04100943 0.061261095 0.020446304  0.04100943 
		0.061261095 0.020446304  -0.023676798 -0.11260631 0.36868417  0.023676809 -0.11260631 
		0.36868417 ;
createNode polyExtrudeFace -n "polyExtrudeFace3";
	setAttr ".ics" -type "componentList" 2 "f[26]" "f[36]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
	setAttr ".ws" yes;
	setAttr ".pvt" -type "float3" 0 0.55559814 1.2344059 ;
	setAttr ".rs" 59084;
	setAttr ".lt" -type "double3" 0 -0.055410605769732391 0.24074628257038408 ;
	setAttr ".ls" -type "double3" 0.72962713361136911 0.22037037911881149 1 ;
	setAttr ".c[0]"  0 1 1;
createNode polyTweak -n "polyTweak3";
	setAttr ".uopa" yes;
	setAttr -s 6 ".tk[58:63]" -type "float3"  0 -0.07861378 -0.028047821 
		0 -0.07861378 -0.028047821 0 -0.051968772 0.028965442 0 -0.051968772 0.028965442 
		0 -0.024752889 0.082027718 0 -0.024752889 0.082027718;
createNode polyExtrudeFace -n "polyExtrudeFace4";
	setAttr ".ics" -type "componentList" 2 "f[26]" "f[36]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
	setAttr ".ws" yes;
	setAttr ".pvt" -type "float3" 0 0.55662549 1.5259409 ;
	setAttr ".rs" 40004;
	setAttr ".lt" -type "double3" 0 3.4694469519536142e-018 0.14176113573575772 ;
	setAttr ".ls" -type "double3" -0.026218665339866772 -0.0067217271283469453 1 ;
	setAttr ".c[0]"  0 1 1;
createNode polyTweak -n "polyTweak4";
	setAttr ".uopa" yes;
	setAttr -s 6 ".tk[64:69]" -type "float3"  0.025850484 -0.0012239639 
		0.013450627 -0.025850484 -0.0012239639 0.013450627 -0.041122038 -0.0074106613 0.044010878 
		0.041122038 -0.0074106613 0.044010878 -0.023741823 -0.013069981 0.073916666 0.023741826 
		-0.013069981 0.073916666;
createNode polyMergeVert -n "polyMergeVert1";
	setAttr ".ics" -type "componentList" 1 "vtx[70:75]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
createNode polyTweak -n "polyTweak5";
	setAttr ".uopa" yes;
	setAttr -s 7 ".tk";
	setAttr ".tk[70:75]" -type "float3" -0.0015744045 0.034382761 0.0039378405  
		0.0015744045 0.034382761 0.0039378405  0.0025045052 5.1483976e-006 3.2186508e-006  
		-0.0025045052 5.1483976e-006 3.2186508e-006  0.0014459789 -0.034382824 -0.0039409399  
		-0.0014459789 -0.034382824 -0.0039409399 ;
createNode polyExtrudeFace -n "polyExtrudeFace5";
	setAttr ".ics" -type "componentList" 1 "f[21]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
	setAttr ".ws" yes;
	setAttr ".pvt" -type "float3" -7.4505806e-008 0.33796665 -1.4043181 ;
	setAttr ".rs" 47705;
	setAttr ".lt" -type "double3" -1.9852334701272664e-023 5.5511151231257827e-017 0.2277712775874482 ;
	setAttr ".ls" -type "double3" 2.6604509670253926 0.16896629462829729 1 ;
	setAttr ".c[0]"  0 1 1;
createNode polyTweak -n "polyTweak6";
	setAttr ".uopa" yes;
	setAttr -s 29 ".tk";
	setAttr ".tk[10:13]" -type "float3" 0 0.027987333 -0.11345585  0 0.027987333 
		-0.11345585  0 0.027987333 -0.11345585  0 0.027987333 -0.11345585 ;
	setAttr ".tk[20:23]" -type "float3" 0 0.027987333 -0.11345585  0 0.19362818 
		-0.45668799  0 0.19362818 -0.45668799  0 0.027987333 -0.11345585 ;
	setAttr ".tk[30:33]" -type "float3" 0 0.027987333 -0.11345585  0 0.19362818 
		-0.45668799  0 0.19362818 -0.45668799  0 0.027987333 -0.11345585 ;
	setAttr ".tk[40:43]" -type "float3" 0 0.027987333 -0.11345585  0 0.027987333 
		-0.11345585  0 0.027987333 -0.11345585  0 0.027987333 -0.11345585 ;
	setAttr ".tk[58:70]" -type "float3" 0 0.0033646235 0.084605709  0 0.0033646235 
		0.084605709  0 0.01520509 0.084605709  0 0.01520509 0.084605709  0 0.026318807 0.084605709  
		0 0.026318807 0.084605709  0.030781303 0.053931966 0  -0.030781303 0.053931966 0  
		-0.048965801 -0.00051949959 0  0.048965801 -0.00051949959 0  -0.028270412 -0.053931966 
		0  0.028270416 -0.053931966 0  0 0 0.070266217 ;
createNode polyMergeVert -n "polyMergeVert2";
	setAttr ".ics" -type "componentList" 2 "vtx[71]" "vtx[74]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
createNode polyTweak -n "polyTweak7";
	setAttr ".uopa" yes;
	setAttr -s 2 ".tk";
	setAttr ".tk[71]" -type "float3" -0.034979165 0.024388313 0.011496067 ;
	setAttr ".tk[74]" -type "float3" 0.034979165 -0.024388313 -0.011496067 ;
createNode polyMergeVert -n "polyMergeVert3";
	setAttr ".ics" -type "componentList" 1 "vtx[72:73]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
createNode polyTweak -n "polyTweak8";
	setAttr ".uopa" yes;
	setAttr -s 2 ".tk[72:73]" -type "float3"  0.034979165 0.024388343 0.011495829 
		-0.034979165 -0.024388373 -0.011495829;
createNode polyExtrudeFace -n "polyExtrudeFace6";
	setAttr ".ics" -type "componentList" 2 "f[23]" "f[26]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
	setAttr ".ws" yes;
	setAttr ".pvt" -type "float3" -5.9604645e-008 0.14433846 0.3138583 ;
	setAttr ".rs" 42332;
	setAttr ".lt" -type "double3" 4.163336342344337e-017 4.163336342344337e-017 0.35012754557435855 ;
	setAttr ".c[0]"  0 1 1;
createNode polyTweak -n "polyTweak9";
	setAttr ".uopa" yes;
	setAttr -s 14 ".tk";
	setAttr ".tk[21:22]" -type "float3" 0 0.10052275 0.13731651  0 0.10052275 
		0.13731639 ;
	setAttr ".tk[24:25]" -type "float3" 0.13864399 0.070232078 0.20673822  0.11216532 
		0.070232078 -0.20673825 ;
	setAttr ".tk[28:29]" -type "float3" -0.11216536 0.070232078 -0.20673822  
		-0.13864399 0.070232078 0.20673823 ;
	setAttr ".tk[31:32]" -type "float3" 0 -0.10052275 0.073392466  0 -0.10052275 
		0.073392466 ;
	setAttr ".tk[34:35]" -type "float3" 0.12006921 -0.070232078 0.20673822  
		0.097138025 -0.070232078 -0.15134293 ;
	setAttr ".tk[38:39]" -type "float3" -0.097138055 -0.070232078 -0.1513429  
		-0.12006923 -0.070232078 0.20673823 ;
	setAttr ".tk[71:72]" -type "float3" 0.12229601 0 -0.12551492  -0.12229601 
		0 -0.12551492 ;
createNode polyExtrudeFace -n "polyExtrudeFace7";
	setAttr ".ics" -type "componentList" 2 "f[23]" "f[26]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
	setAttr ".ws" yes;
	setAttr ".pvt" -type "float3" -5.9604645e-008 0.27727389 0.4449569 ;
	setAttr ".rs" 55899;
	setAttr ".lt" -type "double3" 5.5511151231257827e-017 5.5511151231257827e-017 1.1547081095743623 ;
	setAttr ".ls" -type "double3" 1.6864630574091111 1.8149026789330254 1 ;
	setAttr ".c[0]"  0 1 1;
createNode polyExtrudeFace -n "polyExtrudeFace8";
	setAttr ".ics" -type "componentList" 2 "f[23]" "f[26]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
	setAttr ".ws" yes;
	setAttr ".pvt" -type "float3" 2.3841858e-007 0.71752495 0.88250506 ;
	setAttr ".rs" 47107;
	setAttr ".lt" -type "double3" 4.3021142204224816e-016 1.3877787807814457e-017 0.38367669149485267 ;
	setAttr ".ls" -type "double3" 0.65569640998181167 0.60795318139005905 1 ;
	setAttr ".c[0]"  0 1 1;
createNode polyTweak -n "polyTweak10";
	setAttr ".uopa" yes;
	setAttr -s 58 ".tk";
	setAttr ".tk[0:4]" -type "float3" -0.075058013 0.050796293 0  -0.038483586 
		0.050796293 0  0.038483594 0.050796293 0  0.075058013 0.050796293 0  0.092776805 
		-0.0037716622 0 ;
	setAttr ".tk[9:14]" -type "float3" -0.09277679 -0.0037716622 0  -0.17450646 
		-0.015665492 0  -0.042947061 0.024610171 0  0.042947076 0.024610171 0  0.17450647 
		-0.015665492 0  0.16069412 -0.041005749 0 ;
	setAttr ".tk[19:24]" -type "float3" -0.16069412 -0.041005749 0  -0.20150273 
		-0.095836371 0  -0.049590986 -0.043768279 0  0.049591005 -0.043768279 0  0.20150274 
		-0.095836371 0  0.14505026 0.031841312 0 ;
	setAttr ".tk[29:34]" -type "float3" -0.14505024 0.031841312 0  -0.17450646 
		-0.17600726 0  -0.042947061 -0.054567963 0  0.042947076 -0.054567963 0  0.17450647 
		-0.17600726 0  0.12561719 -0.04045862 0 ;
	setAttr ".tk[39:51]" -type "float3" -0.12561719 -0.04045862 0  -0.10075136 
		-0.23469645 0  -0.038483586 -0.23469645 0  0.038483594 -0.23469645 0  0.10075136 
		-0.23469645 0  0.092776805 -0.17996527 0  0 -0.18335713 0  0 -0.10290289 0  0 -0.10290289 
		0  0 -0.18335713 0  -0.09277679 -0.17996527 0  -5.1183897e-009 0.0098569607 0  -5.1183897e-009 
		-0.29649672 0 ;
	setAttr ".tk[71:81]" -type "float3" 0 -7.4505806e-009 0  0 -7.4505806e-009 
		0  0 0.20711784 0  0 0 0.34089881  0 0 0.34089881  0 0.11413845 0  0 0 0.34089881  
		0 0.20711793 0  0 0.11413854 0  0 0 0.34089881  0 -0.038525876 -1.1296732 ;
	setAttr ".tk[84]" -type "float3" 0 -0.20122021 -1.1296732 ;
	setAttr ".tk[86:87]" -type "float3" 0 -0.038525537 -1.1296732  0 -0.20121992 
		-1.1296732 ;
	setAttr ".tk[89:96]" -type "float3" -0.10509223 -0.17426142 -1.3252714  
		-0.53337479 -0.15471905 -0.98570085  -0.51748228 -0.25740299 -0.98570085  -0.10141474 
		-0.27696723 -1.3252714  0.53337479 -0.15471867 -0.98570085  0.10509221 -0.17426108 
		-1.3252714  0.10141476 -0.27696684 -1.3252714  0.51748228 -0.25740266 -0.98570085 ;
createNode deleteComponent -n "deleteComponent1";
	setAttr ".dc" -type "componentList" 0;
createNode phong -n "mat_bird";
	setAttr ".dc" 1;
	setAttr ".c" -type "float3" 1 1 1 ;
	setAttr ".sc" -type "float3" 0.2314 0.2314 0.2314 ;
	setAttr ".cp" 11.722000122070313;
createNode shadingEngine -n "phong1SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo1";
createNode script -n "uiConfigurationScriptNode";
	setAttr ".b" -type "string" (
		"// Maya Mel UI Configuration File.\n//\n//  This script is machine generated.  Edit at your own risk.\n//\n//\n\nglobal string $gMainPane;\nif (`paneLayout -exists $gMainPane`) {\n\n\tglobal int $gUseScenePanelConfig;\n\tint    $useSceneConfig = $gUseScenePanelConfig;\n\tint    $menusOkayInPanels = `optionVar -q allowMenusInPanels`;\tint    $nVisPanes = `paneLayout -q -nvp $gMainPane`;\n\tint    $nPanes = 0;\n\tstring $editorName;\n\tstring $panelName;\n\tstring $itemFilterName;\n\tstring $panelConfig;\n\n\t//\n\t//  get current state of the UI\n\t//\n\tsceneUIReplacement -update $gMainPane;\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"modelPanel\" (localizedPanelLabel(\"Top View\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `modelPanel -unParent -l (localizedPanelLabel(\"Top View\")) -mbv $menusOkayInPanels `;\n\t\t\t$editorName = $panelName;\n            modelEditor -e \n                -camera \"top\" \n                -useInteractiveMode 0\n                -displayLights \"default\" \n                -displayAppearance \"wireframe\" \n"
		+ "                -activeOnly 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n                -activeComponentsXray 0\n                -displayTextures 0\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n                -rendererName \"base_OpenGL_Renderer\" \n                -colorResolution 256 256 \n"
		+ "                -bumpResolution 512 512 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n                -lowQualityLighting 0\n                -maximumNumHardwareLights 1\n                -occlusionCulling 0\n                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n"
		+ "                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -dimensions 1\n                -handles 1\n                -pivots 1\n                -textures 1\n                -strokes 1\n                -shadows 0\n                $editorName;\nmodelEditor -e -viewSelected 0 $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tmodelPanel -edit -l (localizedPanelLabel(\"Top View\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        modelEditor -e \n            -camera \"top\" \n            -useInteractiveMode 0\n            -displayLights \"default\" \n            -displayAppearance \"wireframe\" \n            -activeOnly 0\n            -wireframeOnShaded 0\n            -headsUpDisplay 1\n            -selectionHiliteDisplay 1\n            -useDefaultMaterial 0\n            -bufferMode \"double\" \n"
		+ "            -twoSidedLighting 1\n            -backfaceCulling 0\n            -xray 0\n            -jointXray 0\n            -activeComponentsXray 0\n            -displayTextures 0\n            -smoothWireframe 0\n            -lineWidth 1\n            -textureAnisotropic 0\n            -textureHilight 1\n            -textureSampling 2\n            -textureDisplay \"modulate\" \n            -textureMaxSize 8192\n            -fogging 0\n            -fogSource \"fragment\" \n            -fogMode \"linear\" \n            -fogStart 0\n            -fogEnd 100\n            -fogDensity 0.1\n            -fogColor 0.5 0.5 0.5 1 \n            -maxConstantTransparency 1\n            -rendererName \"base_OpenGL_Renderer\" \n            -colorResolution 256 256 \n            -bumpResolution 512 512 \n            -textureCompression 0\n            -transparencyAlgorithm \"frontAndBackCull\" \n            -transpInShadows 0\n            -cullingOverride \"none\" \n            -lowQualityLighting 0\n            -maximumNumHardwareLights 1\n            -occlusionCulling 0\n"
		+ "            -shadingModel 0\n            -useBaseRenderer 0\n            -useReducedRenderer 0\n            -smallObjectCulling 0\n            -smallObjectThreshold -1 \n            -interactiveDisableShadows 0\n            -interactiveBackFaceCull 0\n            -sortTransparent 1\n            -nurbsCurves 1\n            -nurbsSurfaces 1\n            -polymeshes 1\n            -subdivSurfaces 1\n            -planes 1\n            -lights 1\n            -cameras 1\n            -controlVertices 1\n            -hulls 1\n            -grid 1\n            -joints 1\n            -ikHandles 1\n            -deformers 1\n            -dynamics 1\n            -fluids 1\n            -hairSystems 1\n            -follicles 1\n            -nCloths 1\n            -nParticles 1\n            -nRigids 1\n            -dynamicConstraints 1\n            -locators 1\n            -manipulators 1\n            -dimensions 1\n            -handles 1\n            -pivots 1\n            -textures 1\n            -strokes 1\n            -shadows 0\n            $editorName;\nmodelEditor -e -viewSelected 0 $editorName;\n"
		+ "\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"modelPanel\" (localizedPanelLabel(\"Side View\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `modelPanel -unParent -l (localizedPanelLabel(\"Side View\")) -mbv $menusOkayInPanels `;\n\t\t\t$editorName = $panelName;\n            modelEditor -e \n                -camera \"side\" \n                -useInteractiveMode 0\n                -displayLights \"default\" \n                -displayAppearance \"wireframe\" \n                -activeOnly 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n                -activeComponentsXray 0\n                -displayTextures 0\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n"
		+ "                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n                -rendererName \"base_OpenGL_Renderer\" \n                -colorResolution 256 256 \n                -bumpResolution 512 512 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n                -lowQualityLighting 0\n                -maximumNumHardwareLights 1\n                -occlusionCulling 0\n                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n"
		+ "                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -dimensions 1\n                -handles 1\n                -pivots 1\n                -textures 1\n                -strokes 1\n                -shadows 0\n                $editorName;\nmodelEditor -e -viewSelected 0 $editorName;\n"
		+ "\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tmodelPanel -edit -l (localizedPanelLabel(\"Side View\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        modelEditor -e \n            -camera \"side\" \n            -useInteractiveMode 0\n            -displayLights \"default\" \n            -displayAppearance \"wireframe\" \n            -activeOnly 0\n            -wireframeOnShaded 0\n            -headsUpDisplay 1\n            -selectionHiliteDisplay 1\n            -useDefaultMaterial 0\n            -bufferMode \"double\" \n            -twoSidedLighting 1\n            -backfaceCulling 0\n            -xray 0\n            -jointXray 0\n            -activeComponentsXray 0\n            -displayTextures 0\n            -smoothWireframe 0\n            -lineWidth 1\n            -textureAnisotropic 0\n            -textureHilight 1\n            -textureSampling 2\n            -textureDisplay \"modulate\" \n            -textureMaxSize 8192\n            -fogging 0\n            -fogSource \"fragment\" \n            -fogMode \"linear\" \n"
		+ "            -fogStart 0\n            -fogEnd 100\n            -fogDensity 0.1\n            -fogColor 0.5 0.5 0.5 1 \n            -maxConstantTransparency 1\n            -rendererName \"base_OpenGL_Renderer\" \n            -colorResolution 256 256 \n            -bumpResolution 512 512 \n            -textureCompression 0\n            -transparencyAlgorithm \"frontAndBackCull\" \n            -transpInShadows 0\n            -cullingOverride \"none\" \n            -lowQualityLighting 0\n            -maximumNumHardwareLights 1\n            -occlusionCulling 0\n            -shadingModel 0\n            -useBaseRenderer 0\n            -useReducedRenderer 0\n            -smallObjectCulling 0\n            -smallObjectThreshold -1 \n            -interactiveDisableShadows 0\n            -interactiveBackFaceCull 0\n            -sortTransparent 1\n            -nurbsCurves 1\n            -nurbsSurfaces 1\n            -polymeshes 1\n            -subdivSurfaces 1\n            -planes 1\n            -lights 1\n            -cameras 1\n            -controlVertices 1\n"
		+ "            -hulls 1\n            -grid 1\n            -joints 1\n            -ikHandles 1\n            -deformers 1\n            -dynamics 1\n            -fluids 1\n            -hairSystems 1\n            -follicles 1\n            -nCloths 1\n            -nParticles 1\n            -nRigids 1\n            -dynamicConstraints 1\n            -locators 1\n            -manipulators 1\n            -dimensions 1\n            -handles 1\n            -pivots 1\n            -textures 1\n            -strokes 1\n            -shadows 0\n            $editorName;\nmodelEditor -e -viewSelected 0 $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"modelPanel\" (localizedPanelLabel(\"Front View\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `modelPanel -unParent -l (localizedPanelLabel(\"Front View\")) -mbv $menusOkayInPanels `;\n\t\t\t$editorName = $panelName;\n            modelEditor -e \n                -camera \"front\" \n                -useInteractiveMode 0\n"
		+ "                -displayLights \"default\" \n                -displayAppearance \"wireframe\" \n                -activeOnly 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n                -activeComponentsXray 0\n                -displayTextures 0\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n"
		+ "                -rendererName \"base_OpenGL_Renderer\" \n                -colorResolution 256 256 \n                -bumpResolution 512 512 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n                -lowQualityLighting 0\n                -maximumNumHardwareLights 1\n                -occlusionCulling 0\n                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n"
		+ "                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -dimensions 1\n                -handles 1\n                -pivots 1\n                -textures 1\n                -strokes 1\n                -shadows 0\n                $editorName;\nmodelEditor -e -viewSelected 0 $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tmodelPanel -edit -l (localizedPanelLabel(\"Front View\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        modelEditor -e \n            -camera \"front\" \n            -useInteractiveMode 0\n            -displayLights \"default\" \n            -displayAppearance \"wireframe\" \n            -activeOnly 0\n            -wireframeOnShaded 0\n            -headsUpDisplay 1\n"
		+ "            -selectionHiliteDisplay 1\n            -useDefaultMaterial 0\n            -bufferMode \"double\" \n            -twoSidedLighting 1\n            -backfaceCulling 0\n            -xray 0\n            -jointXray 0\n            -activeComponentsXray 0\n            -displayTextures 0\n            -smoothWireframe 0\n            -lineWidth 1\n            -textureAnisotropic 0\n            -textureHilight 1\n            -textureSampling 2\n            -textureDisplay \"modulate\" \n            -textureMaxSize 8192\n            -fogging 0\n            -fogSource \"fragment\" \n            -fogMode \"linear\" \n            -fogStart 0\n            -fogEnd 100\n            -fogDensity 0.1\n            -fogColor 0.5 0.5 0.5 1 \n            -maxConstantTransparency 1\n            -rendererName \"base_OpenGL_Renderer\" \n            -colorResolution 256 256 \n            -bumpResolution 512 512 \n            -textureCompression 0\n            -transparencyAlgorithm \"frontAndBackCull\" \n            -transpInShadows 0\n            -cullingOverride \"none\" \n"
		+ "            -lowQualityLighting 0\n            -maximumNumHardwareLights 1\n            -occlusionCulling 0\n            -shadingModel 0\n            -useBaseRenderer 0\n            -useReducedRenderer 0\n            -smallObjectCulling 0\n            -smallObjectThreshold -1 \n            -interactiveDisableShadows 0\n            -interactiveBackFaceCull 0\n            -sortTransparent 1\n            -nurbsCurves 1\n            -nurbsSurfaces 1\n            -polymeshes 1\n            -subdivSurfaces 1\n            -planes 1\n            -lights 1\n            -cameras 1\n            -controlVertices 1\n            -hulls 1\n            -grid 1\n            -joints 1\n            -ikHandles 1\n            -deformers 1\n            -dynamics 1\n            -fluids 1\n            -hairSystems 1\n            -follicles 1\n            -nCloths 1\n            -nParticles 1\n            -nRigids 1\n            -dynamicConstraints 1\n            -locators 1\n            -manipulators 1\n            -dimensions 1\n            -handles 1\n            -pivots 1\n"
		+ "            -textures 1\n            -strokes 1\n            -shadows 0\n            $editorName;\nmodelEditor -e -viewSelected 0 $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"modelPanel\" (localizedPanelLabel(\"Persp View\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `modelPanel -unParent -l (localizedPanelLabel(\"Persp View\")) -mbv $menusOkayInPanels `;\n\t\t\t$editorName = $panelName;\n            modelEditor -e \n                -camera \"persp\" \n                -useInteractiveMode 0\n                -displayLights \"default\" \n                -displayAppearance \"smoothShaded\" \n                -activeOnly 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n"
		+ "                -activeComponentsXray 0\n                -displayTextures 0\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n                -rendererName \"base_OpenGL_Renderer\" \n                -colorResolution 256 256 \n                -bumpResolution 512 512 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n                -lowQualityLighting 0\n                -maximumNumHardwareLights 1\n                -occlusionCulling 0\n"
		+ "                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -dimensions 1\n                -handles 1\n"
		+ "                -pivots 1\n                -textures 1\n                -strokes 1\n                -shadows 0\n                $editorName;\nmodelEditor -e -viewSelected 0 $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tmodelPanel -edit -l (localizedPanelLabel(\"Persp View\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        modelEditor -e \n            -camera \"persp\" \n            -useInteractiveMode 0\n            -displayLights \"default\" \n            -displayAppearance \"smoothShaded\" \n            -activeOnly 0\n            -wireframeOnShaded 0\n            -headsUpDisplay 1\n            -selectionHiliteDisplay 1\n            -useDefaultMaterial 0\n            -bufferMode \"double\" \n            -twoSidedLighting 1\n            -backfaceCulling 0\n            -xray 0\n            -jointXray 0\n            -activeComponentsXray 0\n            -displayTextures 0\n            -smoothWireframe 0\n            -lineWidth 1\n            -textureAnisotropic 0\n            -textureHilight 1\n            -textureSampling 2\n"
		+ "            -textureDisplay \"modulate\" \n            -textureMaxSize 8192\n            -fogging 0\n            -fogSource \"fragment\" \n            -fogMode \"linear\" \n            -fogStart 0\n            -fogEnd 100\n            -fogDensity 0.1\n            -fogColor 0.5 0.5 0.5 1 \n            -maxConstantTransparency 1\n            -rendererName \"base_OpenGL_Renderer\" \n            -colorResolution 256 256 \n            -bumpResolution 512 512 \n            -textureCompression 0\n            -transparencyAlgorithm \"frontAndBackCull\" \n            -transpInShadows 0\n            -cullingOverride \"none\" \n            -lowQualityLighting 0\n            -maximumNumHardwareLights 1\n            -occlusionCulling 0\n            -shadingModel 0\n            -useBaseRenderer 0\n            -useReducedRenderer 0\n            -smallObjectCulling 0\n            -smallObjectThreshold -1 \n            -interactiveDisableShadows 0\n            -interactiveBackFaceCull 0\n            -sortTransparent 1\n            -nurbsCurves 1\n            -nurbsSurfaces 1\n"
		+ "            -polymeshes 1\n            -subdivSurfaces 1\n            -planes 1\n            -lights 1\n            -cameras 1\n            -controlVertices 1\n            -hulls 1\n            -grid 1\n            -joints 1\n            -ikHandles 1\n            -deformers 1\n            -dynamics 1\n            -fluids 1\n            -hairSystems 1\n            -follicles 1\n            -nCloths 1\n            -nParticles 1\n            -nRigids 1\n            -dynamicConstraints 1\n            -locators 1\n            -manipulators 1\n            -dimensions 1\n            -handles 1\n            -pivots 1\n            -textures 1\n            -strokes 1\n            -shadows 0\n            $editorName;\nmodelEditor -e -viewSelected 0 $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"outlinerPanel\" (localizedPanelLabel(\"Outliner\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `outlinerPanel -unParent -l (localizedPanelLabel(\"Outliner\")) -mbv $menusOkayInPanels `;\n"
		+ "\t\t\t$editorName = $panelName;\n            outlinerEditor -e \n                -showShapes 0\n                -showAttributes 0\n                -showConnected 0\n                -showAnimCurvesOnly 0\n                -showMuteInfo 0\n                -organizeByLayer 1\n                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n                -autoExpand 0\n                -showDagOnly 1\n                -showAssets 1\n                -showContainedOnly 1\n                -showPublishedAsConnected 0\n                -showContainerContents 1\n                -ignoreDagHierarchy 0\n                -expandConnections 0\n                -showUnitlessCurves 1\n                -showCompounds 1\n                -showLeafs 1\n                -showNumericAttrsOnly 0\n                -highlightActive 1\n                -autoSelectNewObjects 0\n                -doNotSelectNewObjects 0\n                -dropIsParent 1\n                -transmitFilters 0\n                -setFilter \"defaultSetFilter\" \n                -showSetMembers 1\n"
		+ "                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -displayMode \"DAG\" \n                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n                -showAttrValues 0\n                -highlightSecondary 0\n                -showUVAttrsOnly 0\n                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\toutlinerPanel -edit -l (localizedPanelLabel(\"Outliner\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        outlinerEditor -e \n            -showShapes 0\n            -showAttributes 0\n            -showConnected 0\n            -showAnimCurvesOnly 0\n            -showMuteInfo 0\n"
		+ "            -organizeByLayer 1\n            -showAnimLayerWeight 1\n            -autoExpandLayers 1\n            -autoExpand 0\n            -showDagOnly 1\n            -showAssets 1\n            -showContainedOnly 1\n            -showPublishedAsConnected 0\n            -showContainerContents 1\n            -ignoreDagHierarchy 0\n            -expandConnections 0\n            -showUnitlessCurves 1\n            -showCompounds 1\n            -showLeafs 1\n            -showNumericAttrsOnly 0\n            -highlightActive 1\n            -autoSelectNewObjects 0\n            -doNotSelectNewObjects 0\n            -dropIsParent 1\n            -transmitFilters 0\n            -setFilter \"defaultSetFilter\" \n            -showSetMembers 1\n            -allowMultiSelection 1\n            -alwaysToggleSelect 0\n            -directSelect 0\n            -displayMode \"DAG\" \n            -expandObjects 0\n            -setsIgnoreFilters 1\n            -containersIgnoreFilters 0\n            -editAttrName 0\n            -showAttrValues 0\n            -highlightSecondary 0\n"
		+ "            -showUVAttrsOnly 0\n            -showTextureNodesOnly 0\n            -attrAlphaOrder \"default\" \n            -animLayerFilterOptions \"allAffecting\" \n            -sortOrder \"none\" \n            -longNames 0\n            -niceNames 1\n            -showNamespace 1\n            $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"graphEditor\" (localizedPanelLabel(\"Graph Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"graphEditor\" -l (localizedPanelLabel(\"Graph Editor\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = ($panelName+\"OutlineEd\");\n            outlinerEditor -e \n                -showShapes 1\n                -showAttributes 1\n                -showConnected 1\n                -showAnimCurvesOnly 1\n                -showMuteInfo 0\n                -organizeByLayer 1\n                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n                -autoExpand 1\n"
		+ "                -showDagOnly 0\n                -showAssets 1\n                -showContainedOnly 0\n                -showPublishedAsConnected 0\n                -showContainerContents 0\n                -ignoreDagHierarchy 0\n                -expandConnections 1\n                -showUnitlessCurves 1\n                -showCompounds 0\n                -showLeafs 1\n                -showNumericAttrsOnly 1\n                -highlightActive 0\n                -autoSelectNewObjects 1\n                -doNotSelectNewObjects 0\n                -dropIsParent 1\n                -transmitFilters 1\n                -setFilter \"0\" \n                -showSetMembers 0\n                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -displayMode \"DAG\" \n                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n                -showAttrValues 0\n                -highlightSecondary 0\n                -showUVAttrsOnly 0\n"
		+ "                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                $editorName;\n\n\t\t\t$editorName = ($panelName+\"GraphEd\");\n            animCurveEditor -e \n                -displayKeys 1\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 1\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"integer\" \n                -snapValue \"none\" \n                -showResults \"off\" \n                -showBufferCurves \"off\" \n                -smoothness \"fine\" \n                -resultSamples 1\n                -resultScreenSamples 0\n                -resultUpdate \"delayed\" \n                -constrainDrag 0\n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Graph Editor\")) -mbv $menusOkayInPanels  $panelName;\n"
		+ "\t\t\t$editorName = ($panelName+\"OutlineEd\");\n            outlinerEditor -e \n                -showShapes 1\n                -showAttributes 1\n                -showConnected 1\n                -showAnimCurvesOnly 1\n                -showMuteInfo 0\n                -organizeByLayer 1\n                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n                -autoExpand 1\n                -showDagOnly 0\n                -showAssets 1\n                -showContainedOnly 0\n                -showPublishedAsConnected 0\n                -showContainerContents 0\n                -ignoreDagHierarchy 0\n                -expandConnections 1\n                -showUnitlessCurves 1\n                -showCompounds 0\n                -showLeafs 1\n                -showNumericAttrsOnly 1\n                -highlightActive 0\n                -autoSelectNewObjects 1\n                -doNotSelectNewObjects 0\n                -dropIsParent 1\n                -transmitFilters 1\n                -setFilter \"0\" \n                -showSetMembers 0\n"
		+ "                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -displayMode \"DAG\" \n                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n                -showAttrValues 0\n                -highlightSecondary 0\n                -showUVAttrsOnly 0\n                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                $editorName;\n\n\t\t\t$editorName = ($panelName+\"GraphEd\");\n            animCurveEditor -e \n                -displayKeys 1\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 1\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"integer\" \n                -snapValue \"none\" \n"
		+ "                -showResults \"off\" \n                -showBufferCurves \"off\" \n                -smoothness \"fine\" \n                -resultSamples 1\n                -resultScreenSamples 0\n                -resultUpdate \"delayed\" \n                -constrainDrag 0\n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"dopeSheetPanel\" (localizedPanelLabel(\"Dope Sheet\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"dopeSheetPanel\" -l (localizedPanelLabel(\"Dope Sheet\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = ($panelName+\"OutlineEd\");\n            outlinerEditor -e \n                -showShapes 1\n                -showAttributes 1\n                -showConnected 1\n                -showAnimCurvesOnly 1\n                -showMuteInfo 0\n                -organizeByLayer 1\n                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n                -autoExpand 0\n"
		+ "                -showDagOnly 0\n                -showAssets 1\n                -showContainedOnly 0\n                -showPublishedAsConnected 0\n                -showContainerContents 0\n                -ignoreDagHierarchy 0\n                -expandConnections 1\n                -showUnitlessCurves 0\n                -showCompounds 1\n                -showLeafs 1\n                -showNumericAttrsOnly 1\n                -highlightActive 0\n                -autoSelectNewObjects 0\n                -doNotSelectNewObjects 1\n                -dropIsParent 1\n                -transmitFilters 0\n                -setFilter \"0\" \n                -showSetMembers 0\n                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -displayMode \"DAG\" \n                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n                -showAttrValues 0\n                -highlightSecondary 0\n                -showUVAttrsOnly 0\n"
		+ "                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                $editorName;\n\n\t\t\t$editorName = ($panelName+\"DopeSheetEd\");\n            dopeSheetEditor -e \n                -displayKeys 1\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"integer\" \n                -snapValue \"none\" \n                -outliner \"dopeSheetPanel1OutlineEd\" \n                -showSummary 1\n                -showScene 0\n                -hierarchyBelow 0\n                -showTicks 1\n                -selectionWindow 0 0 0 0 \n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Dope Sheet\")) -mbv $menusOkayInPanels  $panelName;\n"
		+ "\t\t\t$editorName = ($panelName+\"OutlineEd\");\n            outlinerEditor -e \n                -showShapes 1\n                -showAttributes 1\n                -showConnected 1\n                -showAnimCurvesOnly 1\n                -showMuteInfo 0\n                -organizeByLayer 1\n                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n                -autoExpand 0\n                -showDagOnly 0\n                -showAssets 1\n                -showContainedOnly 0\n                -showPublishedAsConnected 0\n                -showContainerContents 0\n                -ignoreDagHierarchy 0\n                -expandConnections 1\n                -showUnitlessCurves 0\n                -showCompounds 1\n                -showLeafs 1\n                -showNumericAttrsOnly 1\n                -highlightActive 0\n                -autoSelectNewObjects 0\n                -doNotSelectNewObjects 1\n                -dropIsParent 1\n                -transmitFilters 0\n                -setFilter \"0\" \n                -showSetMembers 0\n"
		+ "                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -displayMode \"DAG\" \n                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n                -showAttrValues 0\n                -highlightSecondary 0\n                -showUVAttrsOnly 0\n                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                $editorName;\n\n\t\t\t$editorName = ($panelName+\"DopeSheetEd\");\n            dopeSheetEditor -e \n                -displayKeys 1\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"integer\" \n                -snapValue \"none\" \n"
		+ "                -outliner \"dopeSheetPanel1OutlineEd\" \n                -showSummary 1\n                -showScene 0\n                -hierarchyBelow 0\n                -showTicks 1\n                -selectionWindow 0 0 0 0 \n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"clipEditorPanel\" (localizedPanelLabel(\"Trax Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"clipEditorPanel\" -l (localizedPanelLabel(\"Trax Editor\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = clipEditorNameFromPanel($panelName);\n            clipEditor -e \n                -displayKeys 0\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"none\" \n                -snapValue \"none\" \n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n"
		+ "\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Trax Editor\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = clipEditorNameFromPanel($panelName);\n            clipEditor -e \n                -displayKeys 0\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"none\" \n                -snapValue \"none\" \n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"hyperGraphPanel\" (localizedPanelLabel(\"Hypergraph Hierarchy\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"hyperGraphPanel\" -l (localizedPanelLabel(\"Hypergraph Hierarchy\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = ($panelName+\"HyperGraphEd\");\n            hyperGraph -e \n                -graphLayoutStyle \"hierarchicalLayout\" \n                -orientation \"horiz\" \n"
		+ "                -mergeConnections 1\n                -zoom 0.324308\n                -animateTransition 0\n                -showRelationships 1\n                -showShapes 0\n                -showDeformers 0\n                -showExpressions 0\n                -showConstraints 0\n                -showUnderworld 0\n                -showInvisible 0\n                -transitionFrames 5\n                -currentNode \"m_bird\" \n                -opaqueContainers 0\n                -dropTargetNode \"tail_joint\" \n                -dropNode \"tail\" \n                -freeform 1\n                -imagePosition 0 0 \n                -imageScale 1\n                -imageEnabled 0\n                -graphType \"DAG\" \n                -heatMapDisplay 0\n                -updateSelection 1\n                -updateNodeAdded 1\n                -useDrawOverrideColor 0\n                -limitGraphTraversal -1\n                -range 0 0 \n                -iconSize \"largeIcons\" \n                -showCachedConnections 0\n                $editorName;\n\t\t}\n\t} else {\n"
		+ "\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Hypergraph Hierarchy\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = ($panelName+\"HyperGraphEd\");\n            hyperGraph -e \n                -graphLayoutStyle \"hierarchicalLayout\" \n                -orientation \"horiz\" \n                -mergeConnections 1\n                -zoom 0.324308\n                -animateTransition 0\n                -showRelationships 1\n                -showShapes 0\n                -showDeformers 0\n                -showExpressions 0\n                -showConstraints 0\n                -showUnderworld 0\n                -showInvisible 0\n                -transitionFrames 5\n                -currentNode \"m_bird\" \n                -opaqueContainers 0\n                -dropTargetNode \"tail_joint\" \n                -dropNode \"tail\" \n                -freeform 1\n                -imagePosition 0 0 \n                -imageScale 1\n                -imageEnabled 0\n                -graphType \"DAG\" \n                -heatMapDisplay 0\n"
		+ "                -updateSelection 1\n                -updateNodeAdded 1\n                -useDrawOverrideColor 0\n                -limitGraphTraversal -1\n                -range 0 0 \n                -iconSize \"largeIcons\" \n                -showCachedConnections 0\n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"hyperShadePanel\" (localizedPanelLabel(\"Hypershade\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"hyperShadePanel\" -l (localizedPanelLabel(\"Hypershade\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Hypershade\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"visorPanel\" (localizedPanelLabel(\"Visor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n"
		+ "\t\t\t$panelName = `scriptedPanel -unParent  -type \"visorPanel\" -l (localizedPanelLabel(\"Visor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Visor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"polyTexturePlacementPanel\" (localizedPanelLabel(\"UV Texture Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"polyTexturePlacementPanel\" -l (localizedPanelLabel(\"UV Texture Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"UV Texture Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"multiListerPanel\" (localizedPanelLabel(\"Multilister\")) `;\n\tif (\"\" == $panelName) {\n"
		+ "\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"multiListerPanel\" -l (localizedPanelLabel(\"Multilister\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Multilister\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"renderWindowPanel\" (localizedPanelLabel(\"Render View\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"renderWindowPanel\" -l (localizedPanelLabel(\"Render View\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Render View\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"blendShapePanel\" (localizedPanelLabel(\"Blend Shape\")) `;\n\tif (\"\" == $panelName) {\n"
		+ "\t\tif ($useSceneConfig) {\n\t\t\tblendShapePanel -unParent -l (localizedPanelLabel(\"Blend Shape\")) -mbv $menusOkayInPanels ;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tblendShapePanel -edit -l (localizedPanelLabel(\"Blend Shape\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"dynRelEdPanel\" (localizedPanelLabel(\"Dynamic Relationships\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"dynRelEdPanel\" -l (localizedPanelLabel(\"Dynamic Relationships\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Dynamic Relationships\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"devicePanel\" (localizedPanelLabel(\"Devices\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n"
		+ "\t\t\tdevicePanel -unParent -l (localizedPanelLabel(\"Devices\")) -mbv $menusOkayInPanels ;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tdevicePanel -edit -l (localizedPanelLabel(\"Devices\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"relationshipPanel\" (localizedPanelLabel(\"Relationship Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"relationshipPanel\" -l (localizedPanelLabel(\"Relationship Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Relationship Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"referenceEditorPanel\" (localizedPanelLabel(\"Reference Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n"
		+ "\t\t\t$panelName = `scriptedPanel -unParent  -type \"referenceEditorPanel\" -l (localizedPanelLabel(\"Reference Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Reference Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"componentEditorPanel\" (localizedPanelLabel(\"Component Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"componentEditorPanel\" -l (localizedPanelLabel(\"Component Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Component Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"dynPaintScriptedPanelType\" (localizedPanelLabel(\"Paint Effects\")) `;\n"
		+ "\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"dynPaintScriptedPanelType\" -l (localizedPanelLabel(\"Paint Effects\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Paint Effects\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"webBrowserPanel\" (localizedPanelLabel(\"Web Browser\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"webBrowserPanel\" -l (localizedPanelLabel(\"Web Browser\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Web Browser\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"scriptEditorPanel\" (localizedPanelLabel(\"Script Editor\")) `;\n"
		+ "\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"scriptEditorPanel\" -l (localizedPanelLabel(\"Script Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Script Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\tif ($useSceneConfig) {\n        string $configName = `getPanel -cwl (localizedPanelLabel(\"Current Layout\"))`;\n        if (\"\" != $configName) {\n\t\t\tpanelConfiguration -edit -label (localizedPanelLabel(\"Current Layout\")) \n\t\t\t\t-defaultImage \"\"\n\t\t\t\t-image \"\"\n\t\t\t\t-sc false\n\t\t\t\t-configString \"global string $gMainPane; paneLayout -e -cn \\\"single\\\" -ps 1 100 100 $gMainPane;\"\n\t\t\t\t-removeAllPanels\n\t\t\t\t-ap false\n\t\t\t\t\t(localizedPanelLabel(\"Persp View\")) \n\t\t\t\t\t\"modelPanel\"\n"
		+ "\t\t\t\t\t\"$panelName = `modelPanel -unParent -l (localizedPanelLabel(\\\"Persp View\\\")) -mbv $menusOkayInPanels `;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera persp` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"smoothShaded\\\" \\n    -activeOnly 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 0\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName\"\n"
		+ "\t\t\t\t\t\"modelPanel -edit -l (localizedPanelLabel(\\\"Persp View\\\")) -mbv $menusOkayInPanels  $panelName;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera persp` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"smoothShaded\\\" \\n    -activeOnly 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 0\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName\"\n"
		+ "\t\t\t\t$configName;\n\n            setNamedPanelLayout (localizedPanelLabel(\"Current Layout\"));\n        }\n\n        panelHistory -e -clear mainPanelHistory;\n        setFocus `paneLayout -q -p1 $gMainPane`;\n        sceneUIReplacement -deleteRemaining;\n        sceneUIReplacement -clear;\n\t}\n\n\ngrid -spacing 5 -size 12 -divisions 5 -displayAxes yes -displayGridLines yes -displayDivisionLines yes -displayPerspectiveLabels no -displayOrthographicLabels no -displayAxesBold yes -perspectiveLabelPosition axis -orthographicLabelPosition edge;\nviewManip -drawCompass 0 -compassAngle 0 -frontParameters \"\" -homeParameters \"\" -selectionLockParameters \"\";\n}\n");
	setAttr ".st" 3;
createNode script -n "sceneConfigurationScriptNode";
	setAttr ".b" -type "string" "playbackOptions -min 1 -max 13 -ast 1 -aet 13 ";
	setAttr ".st" 6;
createNode phong -n "mat_phys";
	setAttr ".dc" 1;
	setAttr ".c" -type "float3" 1 0 0.89416647 ;
	setAttr ".it" -type "float3" 0.69422001 0.69422001 0.69422001 ;
createNode shadingEngine -n "phong2SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo2";
createNode displayLayer -n "body";
	setAttr ".dt" 2;
	setAttr ".c" 4;
	setAttr ".do" 1;
createNode ikRPsolver -n "ikRPsolver";
createNode displayLayer -n "rig";
	setAttr ".c" 9;
	setAttr ".do" 2;
createNode skinCluster -n "skinCluster1";
	setAttr -s 97 ".wl";
	setAttr -s 3 ".wl[0].w";
	setAttr ".wl[0].w[0:1]" 0.87085946070138187 0.12694988497983056;
	setAttr ".wl[0].w[6]" 0.0021906543187876311;
	setAttr -s 2 ".wl[1].w[0:1]"  0.80141647218464307 0.19858352781535701;
	setAttr -s 4 ".wl[2].w[0:3]"  0.8014892841470509 0.19850474575600799 
		1.8984530489071732e-006 4.0716438920431662e-006;
	setAttr -s 4 ".wl[3].w[0:3]"  0.87084874368794174 0.12696388849966414 
		8.5369706926504268e-007 0.0021865141153249328;
	setAttr -s 2 ".wl[4].w[0:1]"  0.95011675437730447 0.049883245622695561;
	setAttr -s 2 ".wl[5].w[0:1]"  0.99145765958086374 0.008542340419136301;
	setAttr -s 2 ".wl[6].w[0:1]"  0.99513433044169042 0.0048656695583095995;
	setAttr -s 2 ".wl[7].w[0:1]"  0.99513433745439694 0.0048656625456030833;
	setAttr -s 2 ".wl[8].w[0:1]"  0.99145765958086374 0.008542340419136301;
	setAttr -s 2 ".wl[9].w[0:1]"  0.95012370549460856 0.049876294505391501;
	setAttr -s 3 ".wl[10].w[0:2]"  0.76844590617779285 0.23044148137292597 
		0.0011126124492811889;
	setAttr -s 3 ".wl[11].w[0:2]"  0.56908125437570833 0.4298163732946208 
		0.0011023723296707223;
	setAttr -s 3 ".wl[12].w[0:2]"  0.56917566835304778 0.42953665352453069 
		0.0012876781224217093;
	setAttr -s 3 ".wl[13].w[0:2]"  0.76837957865303763 0.23051071113955302 
		0.0011097102074093901;
	setAttr -s 3 ".wl[14].w[0:2]"  0.93132536689699219 0.068674623968497675 
		9.1272341957385936e-009;
	setAttr -s 2 ".wl[15].w[0:1]"  0.99811583948715465 0.0018841605128452666;
	setAttr -s 2 ".wl[16].w[0:1]"  0.99573562921182657 0.0042643707881734744;
	setAttr -s 2 ".wl[17].w[0:1]"  0.99573560796191163 0.0042643920380884548;
	setAttr -s 2 ".wl[18].w[0:1]"  0.99811580952010948 0.0018841904798904791;
	setAttr -s 2 ".wl[19].w[0:1]"  0.93130931457334665 0.068690685426653381;
	setAttr -s 3 ".wl[20].w[0:2]"  0.70673835049555145 0.29277122085903479 
		0.00049042864541363389;
	setAttr -s 3 ".wl[21].w[0:2]"  0.0016058286490302372 0.66097620912383093 
		0.33741796222713877;
	setAttr -s 3 ".wl[22].w[0:2]"  0.0016385567252640918 0.66138109462384409 
		0.33698034865089171;
	setAttr -s 3 ".wl[23].w[0:2]"  0.70677776729704167 0.29273294792426535 
		0.00048928477869291341;
	setAttr -s 2 ".wl[24].w[0:1]"  0.97289631641840291 0.027103683581597031;
	setAttr -s 2 ".wl[25].w[0:1]"  0.99999881922202849 1.1807779716059814e-006;
	setAttr -s 2 ".wl[26].w[0:1]"  0.9969522876478456 0.0030477123521544382;
	setAttr -s 2 ".wl[27].w[0:1]"  0.99695238070463388 0.0030476192953661678;
	setAttr -s 2 ".wl[28].w[0:1]"  0.99999881922202849 1.1807779716059814e-006;
	setAttr -s 2 ".wl[29].w[0:1]"  0.97288217745624961 0.027117822543750399;
	setAttr -s 3 ".wl[30].w[0:2]"  0.71497947402700968 0.28488237905777902 
		0.00013814691521126551;
	setAttr -s 3 ".wl[31].w[0:2]"  0.0028365187899005227 0.7004147815406645 
		0.29674869966943496;
	setAttr -s 3 ".wl[32].w[0:2]"  0.0032009571863484067 0.70020223674409909 
		0.29659680606954841;
	setAttr -s 3 ".wl[33].w[0:2]"  0.71504767128412072 0.28481448385983565 
		0.00013784485604354099;
	setAttr -s 2 ".wl[34].w[0:1]"  0.98377762374684719 0.016222376253152884;
	setAttr -s 2 ".wl[35].w[0:1]"  0.9999998408888211 1.5911117893279343e-007;
	setAttr -s 2 ".wl[36].w[0:1]"  0.99862183724359721 0.0013781627564028777;
	setAttr -s 2 ".wl[37].w[0:1]"  0.99862192916783943 0.0013780708321606272;
	setAttr -s 2 ".wl[38].w[0:1]"  0.9999998408888211 1.5911117893279343e-007;
	setAttr -s 2 ".wl[39].w[0:1]"  0.98377647734665896 0.016223522653341094;
	setAttr -s 2 ".wl[40].w[0:1]"  0.77510560271426532 0.22489439728573465;
	setAttr -s 3 ".wl[41].w[0:2]"  0.7147250252952454 0.28523295859339171 
		4.2016111362914135e-005;
	setAttr -s 3 ".wl[42].w[0:2]"  0.71474012465947612 0.28521786254403991 
		4.2012796483945127e-005;
	setAttr -s 3 ".wl[43].w[0:2]"  0.77513331106891648 0.22486653273486804 
		1.5619621540223444e-007;
	setAttr -s 2 ".wl[44].w[0:1]"  0.94806654435363025 0.051933455646369753;
	setAttr -s 2 ".wl[45].w[0:1]"  0.99991752419110502 8.2475808894833601e-005;
	setAttr -s 2 ".wl[46].w[0:1]"  0.99938484626936985 0.00061515373063003489;
	setAttr -s 2 ".wl[47].w[0:1]"  0.99938485206117522 0.00061514793882480308;
	setAttr -s 2 ".wl[48].w[0:1]"  0.99991780376146022 8.219623853973134e-005;
	setAttr -s 2 ".wl[49].w[0:1]"  0.94810147012154611 0.051898529878453885;
	setAttr -s 3 ".wl[50].w";
	setAttr ".wl[50].w[0:1]" 0.96526627171320567 0.034730978698389016;
	setAttr ".wl[50].w[6]" 2.7495884053502368e-006;
	setAttr -s 2 ".wl[51].w[0:1]"  0.90610955906651125 0.093890440933488736;
	setAttr -s 2 ".wl[52].w[0:1]"  0.99179979378360394 0.0082002062163960467;
	setAttr -s 2 ".wl[53].w[0:1]"  0.99179979378360394 0.0082002062163960467;
	setAttr -s 2 ".wl[54].w[0:1]"  0.99171421950070215 0.0082857804992977901;
	setAttr -s 2 ".wl[55].w[0:1]"  0.99171288673253788 0.0082871132674621761;
	setAttr -s 2 ".wl[56].w[0:1]"  0.98811853424994645 0.011881465750053548;
	setAttr -s 2 ".wl[57].w[0:1]"  0.98811849542809083 0.011881504571909172;
	setAttr -s 3 ".wl[58].w";
	setAttr ".wl[58].w[0:1]" 0.98842983120078609 0.011570154291941543;
	setAttr ".wl[58].w[6]" 1.4507272430689924e-008;
	setAttr -s 2 ".wl[59].w[0:1]"  0.98843009615015154 0.011569903849848498;
	setAttr -s 2 ".wl[60].w[0:1]"  0.9878756504439834 0.012124349556016621;
	setAttr -s 2 ".wl[61].w[0:1]"  0.98787056209150537 0.01212943790849471;
	setAttr -s 2 ".wl[62].w[0:1]"  0.98178487673129944 0.018215123268700617;
	setAttr -s 2 ".wl[63].w[0:1]"  0.9817848767993419 0.018215123200658139;
	setAttr -s 3 ".wl[64].w";
	setAttr ".wl[64].w[0:1]" 0.97862600263582611 0.021371982544160478;
	setAttr ".wl[64].w[6]" 2.0148200134328421e-006;
	setAttr -s 2 ".wl[65].w[0:1]"  0.97862797439505467 0.021372025604945395;
	setAttr -s 2 ".wl[66].w[0:1]"  0.97836668274446958 0.021633317255530422;
	setAttr -s 2 ".wl[67].w[0:1]"  0.97835984812444099 0.021640151875559014;
	setAttr -s 2 ".wl[68].w[0:1]"  0.9766671111955384 0.02333288880446166;
	setAttr -s 2 ".wl[69].w[0:1]"  0.97666711119264138 0.023332888807358777;
	setAttr -s 2 ".wl[70].w[0:1]"  0.96953550784207754 0.030464492157922452;
	setAttr -s 3 ".wl[71].w[0:2]"  0.025401335377507543 0.48729933231124628 
		0.48729933231124617;
	setAttr -s 3 ".wl[72].w[0:2]"  0.025389135635976879 0.48741455387250726 
		0.4871963104915159;
	setAttr -s 4 ".wl[73].w";
	setAttr ".wl[73].w[0:1]" 0.49994600404956424 5.4988102329835973e-005;
	setAttr ".wl[73].w[3]" 5.3003798541738973e-005;
	setAttr ".wl[73].w[6]" 0.49994600404956424;
	setAttr -s 4 ".wl[74].w";
	setAttr ".wl[74].w[0]" 0.42751952807542215;
	setAttr ".wl[74].w[6:8]" 0.57108431871239729 0.0013670751333046552 2.9078078875890877e-005;
	setAttr -s 4 ".wl[75].w";
	setAttr ".wl[75].w[0]" 0.42365202611519964;
	setAttr ".wl[75].w[6:8]" 0.57563953028351167 0.00069319862289738809 1.5244978391355981e-005;
	setAttr -s 4 ".wl[76].w";
	setAttr ".wl[76].w[0:1]" 0.49997184378459386 7.1872551417897108e-005;
	setAttr ".wl[76].w[3]" 7.95424880914277e-005;
	setAttr ".wl[76].w[6]" 0.49987674117589692;
	setAttr -s 4 ".wl[77].w";
	setAttr ".wl[77].w[0]" 0.42751954282882759;
	setAttr ".wl[77].w[3:5]" 0.57108430133557786 0.001367077703949343 2.9078131645182767e-005;
	setAttr -s 4 ".wl[78].w";
	setAttr ".wl[78].w[0:1]" 0.49994600404956424 5.4988102329835973e-005;
	setAttr ".wl[78].w[3]" 0.49994600404956424;
	setAttr ".wl[78].w[6]" 5.3003798541738973e-005;
	setAttr -s 4 ".wl[79].w";
	setAttr ".wl[79].w[0:1]" 0.49997184378459386 7.1872551417897108e-005;
	setAttr ".wl[79].w[3]" 0.49987674117589692;
	setAttr ".wl[79].w[6]" 7.95424880914277e-005;
	setAttr -s 4 ".wl[80].w";
	setAttr ".wl[80].w[0]" 0.42365202611519964;
	setAttr ".wl[80].w[3:5]" 0.57563953028351167 0.00069319862289738809 1.5244978391355981e-005;
	setAttr -s 4 ".wl[81].w";
	setAttr ".wl[81].w[0]" 0.00033769589965364498;
	setAttr ".wl[81].w[6:8]" 0.00052718911364849628 0.73381457779695791 0.26532053718973986;
	setAttr -s 4 ".wl[82].w";
	setAttr ".wl[82].w[0]" 1.8886521578971153e-006;
	setAttr ".wl[82].w[6:8]" 0.47508437732837588 0.52491354628583342 1.8773363285950524e-007;
	setAttr -s 4 ".wl[83].w";
	setAttr ".wl[83].w[0]" 4.3775655391842983e-005;
	setAttr ".wl[83].w[6:8]" 0.42403066118337962 0.57592085449144459 4.7086697840740214e-006;
	setAttr -s 4 ".wl[84].w";
	setAttr ".wl[84].w[0]" 0.001718916017549306;
	setAttr ".wl[84].w[6:8]" 0.0024787593425561853 0.66311166246571029 0.33269066217418425;
	setAttr -s 4 ".wl[85].w";
	setAttr ".wl[85].w[0]" 1.8886521578971153e-006;
	setAttr ".wl[85].w[3:5]" 0.47508437732837588 0.52491354628583342 1.8773363285950524e-007;
	setAttr -s 4 ".wl[86].w";
	setAttr ".wl[86].w[0]" 0.00033769589965364498;
	setAttr ".wl[86].w[3:5]" 0.00052718911364849628 0.73381457779695791 0.26532053718973986;
	setAttr -s 4 ".wl[87].w";
	setAttr ".wl[87].w[0]" 0.001718916017549306;
	setAttr ".wl[87].w[3:5]" 0.0024787593425561853 0.66311166246571029 0.33269066217418425;
	setAttr -s 4 ".wl[88].w";
	setAttr ".wl[88].w[0]" 4.3775655391842983e-005;
	setAttr ".wl[88].w[3:5]" 0.42403066118337962 0.57592085449144459 4.7086697840740214e-006;
	setAttr -s 4 ".wl[89].w";
	setAttr ".wl[89].w[0]" 7.8677347496918733e-007;
	setAttr ".wl[89].w[6:8]" 1.7280110486526707e-006 0.82406659156414941 0.17593089365132694;
	setAttr -s 4 ".wl[90].w";
	setAttr ".wl[90].w[0]" 0.0012466212036058395;
	setAttr ".wl[90].w[6:8]" 0.0087112728924133923 0.86588603385307228 0.1241560720509086;
	setAttr -s 4 ".wl[91].w";
	setAttr ".wl[91].w[0]" 0.0013051695648851429;
	setAttr ".wl[91].w[6:8]" 0.0095396432166371427 0.84829079410473351 0.14086439311374416;
	setAttr -s 4 ".wl[92].w";
	setAttr ".wl[92].w[0]" 7.6179965195910379e-006;
	setAttr ".wl[92].w[6:8]" 1.5975770865254833e-005 0.73327333583716969 0.26670307039544555;
	setAttr -s 4 ".wl[93].w";
	setAttr ".wl[93].w[0]" 0.0012466222891345693;
	setAttr ".wl[93].w[3:5]" 0.0087108661693762902 0.8658863312144861 0.12415618032700315;
	setAttr -s 4 ".wl[94].w";
	setAttr ".wl[94].w[0]" 7.8547564882343303e-007;
	setAttr ".wl[94].w[3:5]" 1.7259518059306904e-006 0.82406780748695307 0.17592968108559223;
	setAttr -s 4 ".wl[95].w";
	setAttr ".wl[95].w[0]" 7.6048723180058339e-006;
	setAttr ".wl[95].w[3:5]" 1.5956883408097516e-005 0.73327387391758203 0.26670256432669187;
	setAttr -s 4 ".wl[96].w";
	setAttr ".wl[96].w[0]" 0.001305171323580984;
	setAttr ".wl[96].w[3:5]" 0.0095390819708217507 0.84829113152831659 0.14086461517728069;
	setAttr -s 9 ".pm";
	setAttr ".pm[0]" -type "matrix" 0.8365552480583418 -0.43605820884141439 -0.33170552519978258 -0
		 0.46222819223408557 0.88676101532713369 1.3600232051658168e-015 -0 0.29414352831577883 -0.15332364526715464 0.94338297872705779 -0
		 -0.12624488539837633 -0.18649172318696838 -0.068391225556085899 1;
	setAttr ".pm[1]" -type "matrix" 0.87675009663600278 -0.35859776065196802 -0.32049479575205847 -0
		 0.37856706895201153 0.92557386215530235 -8.7430063189231078e-016 -0 0.2966416059049079 -0.12132877544222964 0.94725027627119007 -0
		 0.015436823683566952 -0.17415448086004845 0.23490825409843663 1;
	setAttr ".pm[2]" -type "matrix" 1.0000000000000002 1.1250775417139487e-016 5.5511151231257735e-017 -0
		 -5.7735030615735738e-017 1 -9.5099448957170501e-016 0 -5.5511151231257889e-017 9.1309433856506677e-016 1 -0
		 0.00050703012642936253 -0.16798921811885045 0.57418148674957026 1;
	setAttr ".pm[3]" -type "matrix" 1 7.1755830721418406e-017 5.5511151231257926e-017 -0
		 -1.7113679258717563e-016 1 1.4008708495817237e-015 0 5.5511151231258074e-017 -1.447984758105949e-015 1 -0
		 -0.19431896234731827 -0.33395498378553451 -0.12159656841778801 1;
	setAttr ".pm[4]" -type "matrix" 0.96422539888122449 -3.697518537459972e-008 0.26508372291097498 -0
		 3.1820262421252613e-008 0.99999999999999933 2.3740726063722579e-008 -0 -0.26508372291097554 -1.4456377479006854e-008 0.96422539888122494 -0
		 -0.3708930119441613 -0.47031099451436892 -0.34047351550299937 1;
	setAttr ".pm[5]" -type "matrix" 0.99999999999999956 -3.6975185374599713e-008 -9.2310529400822591e-016 -0
		 3.697518527521876e-008 0.99999999999999933 1.4456377431892918e-008 -0 4.6467348646786305e-016 -1.4456377479006856e-008 1 -0
		 -0.62158688816987229 -0.37413306692065018 0.21431991286088453 1;
	setAttr ".pm[6]" -type "matrix" 1 -7.175583072141848e-017 -5.5511151231257827e-017 -0
		 -1.0398430075915926e-016 -0.99999999999999978 6.2662641083186559e-018 0 5.5511151231257827e-017 4.694826093245447e-018 -1 -0
		 0.19431899999999999 0.333955 0.12159699999999998 1;
	setAttr ".pm[7]" -type "matrix" 0.96422539888122449 2.499209375014413e-008 0.26508372291097543 -0
		 2.5919348061790147e-008 -0.99999999999999944 -2.1522454544228461e-017 0 0.26508372291097548 6.8707973113583022e-009 -0.96422539888122483 -0
		 0.37089243674979594 0.47031100961329037 0.34047386798303042 1;
	setAttr ".pm[8]" -type "matrix" 0.99999999999999967 2.499209375014413e-008 5.1821565759908012e-018 -0
		 2.4992093717915655e-008 -0.99999999999999944 -6.8707973003972079e-009 0 -5.1821566284961751e-018 6.8707973113583022e-009 -1 -0
		 0.62158698824594316 0.37413300961329038 -0.2143199967685884 1;
	setAttr ".gm" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
	setAttr -s 9 ".ma";
	setAttr -s 9 ".dpf[0:8]"  5.2 5.2 5.2 5.2 5.2 5.2 5.2 5.2 5.2;
	setAttr -s 9 ".lw";
	setAttr -s 9 ".lw";
	setAttr ".mmi" yes;
	setAttr ".mi" 4;
	setAttr ".ptw" -type "doubleArray" 97 0 0 0 0 0 3.5699897125596181e-005 0.0025880839665381136
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
		 0 0 2.7507364160081003e-006 0 0 0 0 0 0 0 0 0 0 0 0 0 0.0005673464790199173 0 0 0
		 0 0 0 0 0 5.3003798541738973e-005 0 0 7.95424880914277e-005 0.57108430133557786 0.49994600404956424
		 0.49987674117589692 0.57563953028351167 0 0 0 0 0.47508437732837588 0.00052718911364849628
		 0.0024787593425561853 0.42403066118337962 0 0 0 0 0.0087108661693762902 1.7259518059306904e-006
		 1.5956883408097516e-005 0.0095390819708217507 ;
	setAttr ".ucm" yes;
createNode tweak -n "tweak1";
	setAttr -s 97 ".vl[0].vt[0:96]" -type "float3"  -0.13153072 0.50597423 
		0.26827157 -0.043142412 0.4896847 0.41109008 0.043142948 0.4896847 0.41109005 0.13153137 
		0.50597423 0.26827154 0.16258164 0.57250816 0.041768733 0.18581517 0.59608859 -0.18963332 
		0.070975214 0.61237758 -0.33245155 -0.070974745 0.61237758 -0.33245155 -0.18581469 
		0.59608859 -0.18963327 -0.16258106 0.57250816 0.041768759 -0.19563325 0.34988591 
		0.50645655 -0.091871507 0.29201782 0.75044298 0.091872193 0.29201782 0.75044298 0.19563375 
		0.34988591 0.50645643 0.28159925 0.44432902 0.027148949 0.321841 0.45978934 -0.37654603 
		0.12293252 0.48800331 -0.6239149 -0.12293199 0.48800331 -0.62391484 -0.32184064 0.45978934 
		-0.376546 -0.28159875 0.44432902 0.027148968 -0.22589768 0.18930639 0.55086613 -0.10608422 
		-0.094870143 0.96138889 0.10608475 -0.094870098 0.96138877 0.22589837 0.18930642 
		0.55086595 0.25418505 0.14380023 -0.16132452 0.29050937 0.18523535 -0.31638968 0.14195023 
		0.28688565 -0.74834436 -0.14194974 0.28688565 -0.74834436 -0.29050887 0.18523535 
		-0.31638962 -0.25418454 0.14380023 -0.16132447 -0.19563325 0.042851474 0.47143671 
		-0.091871507 -0.14538015 0.90181434 0.091872193 -0.14538015 0.90181434 0.19563375 
		0.042851493 0.47143662 0.22013092 0.087907746 -0.16769953 0.25158834 0.0737001 -0.30770302 
		0.12293252 0.062913716 -0.67239946 -0.12293199 0.062913716 -0.67239946 -0.25158784 
		0.073700093 -0.30770299 -0.22013015 0.087907746 -0.16769952 -0.1129487 -0.050236329 
		0.28945264 -0.043142412 -0.06652578 0.43227115 0.043142948 -0.06652578 0.43227112 
		0.1129493 -0.050236322 0.28945261 0.16258164 -0.034042329 -0.027412856 0.18581517 
		-0.0051880311 -0.25821316 0.070975214 -0.048134759 -0.40778795 -0.070974745 -0.048134759 
		-0.40778795 -0.18581469 -0.0051880325 -0.2582131 -0.16258106 -0.034042332 -0.027412834 
		2.7122999e-007 0.61942476 0.047119997 2.7122999e-007 -0.0051946463 -0.024122538 0.058634471 
		0.019978674 -0.85680902 -0.058633965 0.019978674 -0.85680902 -0.093272932 -0.0434702 
		-0.79963493 0.093273416 -0.0434702 -0.79963493 -0.053851046 -0.10225819 -0.74316871 
		0.053851537 -0.10225819 -0.74316871 0.085146062 -0.0030935826 -1.0259061 -0.085145451 
		-0.0030935826 -1.0259061 -0.13544665 -0.12091628 -0.98804128 0.13544711 -0.12091628 
		-0.98804128 -0.07820002 -0.23112786 -0.94839346 0.078200527 -0.23112786 -0.94839346 
		0.021167414 -0.10146891 -1.134535 -0.021166895 -0.10146891 -1.134535 -0.03367167 
		-0.12120833 -1.1399651 0.03367221 -0.12120833 -1.1399651 -0.019440241 -0.14048941 
		-1.144405 0.019440774 -0.14048941 -1.144405 2.5864529e-007 -0.12346194 -1.2952586 
		-0.38384038 -0.26131853 1.1612939 0.38384113 -0.26131842 1.1612936 0.5733034 -0.072118498 
		-0.28373161 0.50472379 0.12699565 -0.67507178 0.46580288 0.015460417 -0.66638499 
		0.525195 -0.11278526 -0.28836989 -0.50472343 0.12699574 -0.67507166 -0.57330275 -0.072118469 
		-0.28373155 -0.52519453 -0.11278524 -0.28836983 -0.46580234 0.015460479 -0.66638488 
		1.3175703 -0.15329663 0.24048451 1.2019122 -0.13589737 -0.84073967 1.1362746 -0.33322126 
		-0.81864995 1.2364359 -0.22525442 0.23937565 -1.2019119 -0.13589694 -0.84073967 -1.3175702 
		-0.15329655 0.24048458 -1.2364359 -0.22525428 0.23937565 -1.1362743 -0.33322084 -0.81864983 
		1.5962892 -0.18884113 0.23025715 1.8301991 -0.24577674 -0.16676179 1.7452868 -0.2884531 
		-0.13886765 1.5404307 -0.23374704 0.22500086 -1.8301991 -0.24577659 -0.16676201 -1.596289 
		-0.18884099 0.23025694 -1.5404308 -0.2337469 0.22500063 -1.7452868 -0.28845298 -0.13886771;
createNode objectSet -n "skinCluster1Set";
	setAttr ".ihi" 0;
	setAttr ".vo" yes;
createNode groupId -n "skinCluster1GroupId";
	setAttr ".ihi" 0;
createNode groupParts -n "skinCluster1GroupParts";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "vtx[*]";
createNode objectSet -n "tweakSet1";
	setAttr ".ihi" 0;
	setAttr ".vo" yes;
createNode groupId -n "groupId2";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts2";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "vtx[*]";
createNode dagPose -n "bindPose1";
	setAttr -s 9 ".wm";
	setAttr -s 9 ".xm";
	setAttr ".xm[0]" -type "matrix" "xform" 1 1 1 2.7984192343579076e-009 2.2666548055813374e-010
		 6.6078686837200506e-009 0 0.0016038272517390779 0.2237275349598738 0.073059643295614818 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 -0.040035153316024197 -0.16341877840515617 0.23455634469226269 0.95743136073983581 1
		 1 1 yes;
	setAttr ".xm[1]" -type "matrix" "xform" 1 1 1 2.0191433150761338e-009 -1.1264253415813831e-009
		 6.7971083701795477e-009 0 -0.12803686544781803 -0.010370690163625402 -0.30233168188405651 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0.0024954072847638807 0.0053788524560892956 -0.046112150182092138 0.99891867061053718 1
		 1 1 yes;
	setAttr ".xm[2]" -type "matrix" "xform" 1 1 1 0 0 0 0 -0.091738647420243916
		 0.051178505091510784 -0.30882281733808647 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 
		0.03132873365422751 0.15935298024206304 -0.19034594655513759 0.96819086897451367 1
		 1 1 yes;
	setAttr ".xm[3]" -type "matrix" "xform" 1 1 1 -5.9178191217985326e-009 -3.1824105357812919e-009
		 -2.5294536888791871e-009 0 0.22644391445877424 0.0062685295213627812 -0.018135766100818573 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0.040035153316023503 0.16341877840515634 -0.23455634469226261 0.9574313607398357 1
		 1 1 yes;
	setAttr ".xm[4]" -type "matrix" "xform" 1 1 1 -2.374072475833291e-008 8.0041267553584991e-009
		 3.1820262957615111e-008 0 0.2535594696890856 0.13635603061383517 0.1083789356697686 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0.13374341314392871 0 0.99101599353421754 1
		 1 1 yes;
	setAttr ".xm[5]" -type "matrix" "xform" 1 1 1 0 0 0 0 0.28526957363589228 -0.096177927593718682
		 -0.38235365254075671 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 -0.13374341314392885 0 0.99101599353421754 1
		 1 1 yes;
	setAttr ".xm[6]" -type "matrix" "xform" 1 1 1 5.9178154404336566e-009 3.1824137173171071e-009
		 2.5294583176703488e-009 0 -0.098673078095677422 0.17573725147690916 0.11077800045948516 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 -0.9574313607398357 0.23455634469226272 0.16341877840515617 0.040035153316024197 1
		 1 1 yes;
	setAttr ".xm[7]" -type "matrix" "xform" 1 1 1 1.933814802916153e-008 -6.5197973044312195e-009
		 -2.5919348163715544e-008 0 -0.25355899999999998 -0.13635600000000003 -0.108379 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0.13374341314392862 0 0.99101599353421754 1
		 1 1 yes;
	setAttr ".xm[8]" -type "matrix" "xform" 1 1 1 0 0 0 0 -0.28527026756871343 0.096178000000000041
		 0.38235405939819 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 -0.13374341314392862 0 0.99101599353421754 1
		 1 1 yes;
	setAttr -s 9 ".m";
	setAttr -s 9 ".p";
	setAttr ".bp" yes;
createNode animCurveTL -n "left_wing_ik_translateX";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0.47174547008848666 5 0.36266369904115209 
		9 0.35806445906493389 13 0.44787843203640393;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 0.94652473926544189;
	setAttr -s 4 ".kiy[1:3]"  0 0 -0.32263141870498657;
	setAttr -s 4 ".kox[1:3]"  1 1 0.94652473926544189;
	setAttr -s 4 ".koy[1:3]"  0 0 -0.32263141870498657;
createNode animCurveTL -n "left_wing_ik_translateY";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0.39399314748332781 5 0.086742915052220371 
		9 0.56051573974510294 13 0.47031101439936995;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  0.97735857963562012 0.76688355207443237 
		0.54603314399719238;
	setAttr -s 4 ".kiy[1:3]"  0.21158964931964874 0.64178627729415894 
		-0.83776360750198364;
	setAttr -s 4 ".kox[1:3]"  0.97735857963562012 0.76688355207443237 
		0.54603314399719238;
	setAttr -s 4 ".koy[1:3]"  0.21158964931964874 0.64178627729415894 
		-0.83776360750198364;
createNode animCurveTL -n "left_wing_ik_translateZ";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0.24017699324844738 5 0.19355216688491297 
		9 0.25000607591008189 13 0.2299755040875559;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTL -n "left_wing_tip_ik_translateX";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 0.68687435384886975 5 0.62321109201306046 
		7 0.31846454737308127 9 0.28227528729499829 11 0.81675883022293272 13 0.73295436141073678;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 0.84645718336105347 0.54331797361373901 
		1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0.53245675563812256 0.8395269513130188 
		0;
	setAttr -s 6 ".kox[1:5]"  1 1 0.84645718336105347 0.54331797361373901 
		1;
	setAttr -s 6 ".koy[1:5]"  0 0 0.53245675563812256 0.8395269513130188 
		0;
createNode animCurveTL -n "left_wing_tip_ik_translateY";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 0.40557436329720914 5 0.06472068350028147 
		7 0.038447840154455126 9 0.25856909808821932 11 0.52749948301151728 13 0.46711762796289674;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 0.81344950199127197 0.9049573540687561;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0.58163547515869141 -0.42550235986709595;
	setAttr -s 6 ".kox[1:5]"  1 1 1 0.81344950199127197 0.90495729446411133;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0.58163547515869141 -0.42550233006477356;
createNode animCurveTL -n "left_wing_tip_ik_translateZ";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 -0.19618143490685697 5 -0.21687848053666886 
		7 -0.10911058043912397 9 -0.12403598862870868 11 -0.12745724190552229 13 -0.16441612333918315;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTL -n "right_wing_ik_translateX";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 -0.47174505165735792 5 -0.36266328048244972 
		9 -0.24234689919249855 13 -0.44787799999999989;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  0.89682674407958984 0.98315191268920898 
		0.94652473926544189;
	setAttr -s 4 ".kiy[1:3]"  0.44238188862800598 -0.18279056251049042 
		0.3226313591003418;
	setAttr -s 4 ".kox[1:3]"  0.89682674407958984 0.98315191268920898 
		0.94652473926544189;
	setAttr -s 4 ".koy[1:3]"  0.44238188862800598 -0.18279056251049042 
		0.3226313591003418;
createNode animCurveTL -n "right_wing_ik_translateY";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0.39399314631777643 5 0.086743085501073125 
		9 0.59829825621400168 13 0.47031100000000009;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  0.95591932535171509 0.7668837308883667 
		0.54603332281112671;
	setAttr -s 4 ".kiy[1:3]"  0.29362916946411133 0.64178609848022461 
		-0.83776348829269409;
	setAttr -s 4 ".kox[1:3]"  0.95591932535171509 0.7668837308883667 
		0.54603332281112671;
	setAttr -s 4 ".koy[1:3]"  0.29362916946411133 0.64178609848022461 
		-0.83776348829269409;
createNode animCurveTL -n "right_wing_ik_translateZ";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0.24017751992858777 5 0.19355257946831878 
		9 0.27142055172446639 13 0.22997599999999999;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTL -n "right_wing_tip_ik_translateX";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 -0.6888477365245933 5 -0.62321119004745729 
		7 -0.32902630579391923 9 -0.33329107609040443 11 -0.72472506856848329 13 -0.69291958914264096;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 0.94299072027206421 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 -0.33281907439231873 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 0.94299072027206421 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 -0.33281907439231873 0 0;
createNode animCurveTL -n "right_wing_tip_ik_translateY";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 0.40088678170932723 5 0.064720744345524828 
		7 0.069885526642635754 9 0.28867028218190605 11 0.55648303342209293 13 0.46690674025496265;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 0.85328739881515503 0.90495741367340088;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0.52144086360931396 -0.42550215125083923;
	setAttr -s 6 ".kox[1:5]"  1 1 1 0.85328739881515503 0.90495741367340088;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0.52144086360931396 -0.42550215125083923;
createNode animCurveTL -n "right_wing_tip_ik_translateZ";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 -0.1953025163480781 5 -0.21687861244127909 
		7 -0.050879335047816554 9 -0.092837096604490799 11 -0.12412763069643129 13 -0.19046066655875471;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTL -n "spine_ik_translateX";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 -0.00069877058248852923 5 -0.00057614317042638079 
		7 -0.00061583495668281023 9 -0.00071472791773660553 13 -0.00073991686256528848;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTL -n "spine_ik_translateY";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 0.15534896236547002 5 0.099719131775641123 
		7 0.051765830276078617 9 0.16680762219939135 13 0.19285140914312354;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTL -n "spine_ik_translateZ";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 -0.24822606266121822 5 -0.23111563457500733 
		7 -0.2068147592869341 9 -0.25045261896391696 13 -0.25396727871095298;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTL -n "tail_ik_translateX";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 -0.00051170682563376861 5 -0.00052335504107729573 
		7 -0.0005077427082299784 9 -0.00049257834815248473 13 -0.00051047356918071211;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTL -n "tail_ik_translateY";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 0.23088513550391881 5 -0.14426588516625849 
		7 -0.23774456189831877 9 -0.095172753972736446 13 0.2769945172444418;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTL -n "tail_ik_translateZ";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 -0.56556027587700097 5 -0.44762905227424632 
		7 -0.35711603160897143 9 -0.4448053510589659 13 -0.56912856446374938;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTU -n "left_wing_ik_visibility";
	setAttr ".tan" 5;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 1 5 1 9 1 13 1;
	setAttr -s 4 ".kit[0:3]"  9 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
createNode animCurveTA -n "left_wing_ik_rotateX";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0 5 0 9 0 13 0;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTA -n "left_wing_ik_rotateY";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0 5 0 9 0 13 0;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTA -n "left_wing_ik_rotateZ";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0 5 0 9 0 13 0;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTU -n "left_wing_ik_scaleX";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 1 5 1 9 1 13 1;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTU -n "left_wing_ik_scaleY";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 1 5 1 9 1 13 1;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTU -n "left_wing_ik_scaleZ";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 1 5 1 9 1 13 1;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTU -n "left_wing_ik_poleVectorX";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0.98218467340535487 5 0.98218467238984619 
		9 0.98218464821855112 13 0.98218468279644611;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTU -n "left_wing_ik_poleVectorY";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 -1.7421502696305136 5 -1.7421502703366807 
		9 -1.742150283432816 13 -1.7421502645604008;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTU -n "left_wing_ik_poleVectorZ";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0.015023493400817682 5 0.015023477903007053 
		9 0.015023539489261063 13 0.015023467382375422;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTU -n "left_wing_ik_offset";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0 5 0 9 0 13 0;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTA -n "left_wing_ik_roll";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0 5 0 9 0 13 0;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTA -n "left_wing_ik_twist";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0 5 0 9 0 13 0;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTU -n "left_wing_ik_ikBlend";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 1 5 1 9 1 13 1;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTU -n "left_wing_tip_ik_visibility";
	setAttr ".tan" 5;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 1 5 1 7 1 9 1 11 1 13 1;
	setAttr -s 6 ".kit[0:5]"  9 1 9 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
createNode animCurveTA -n "left_wing_tip_ik_rotateX";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 0 5 0 7 0 9 0 11 0 13 0;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTA -n "left_wing_tip_ik_rotateY";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 0 5 0 7 0 9 0 11 0 13 0;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTA -n "left_wing_tip_ik_rotateZ";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 0 5 0 7 0 9 0 11 0 13 0;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTU -n "left_wing_tip_ik_scaleX";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 1 5 1 7 1 9 1 11 1 13 1;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTU -n "left_wing_tip_ik_scaleY";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 1 5 1 7 1 9 1 11 1 13 1;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTU -n "left_wing_tip_ik_scaleZ";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 1 5 1 7 1 9 1 11 1 13 1;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTU -n "left_wing_tip_ik_poleVectorX";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 1.9703154574789914 5 1.9703154506388616 
		7 1.9703154626344719 9 1.9703154401401171 11 1.9703154528155931 13 1.9703154483565695;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTU -n "left_wing_tip_ik_poleVectorY";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 -0.082053609430010083 5 -0.082053684596498866 
		7 -0.082053584643638849 9 -0.082053713291127622 11 -0.082053622981832192 13 -0.082053799642926126;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTU -n "left_wing_tip_ik_poleVectorZ";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 0.33335297088652921 5 0.33335299281382391 
		7 0.33335294651561886 9 0.33335304780458708 11 0.33335299511426564 13 0.333352977985238;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTU -n "left_wing_tip_ik_offset";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 0 5 0 7 0 9 0 11 0 13 0;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTA -n "left_wing_tip_ik_roll";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 0 5 0 7 0 9 0 11 0 13 0;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTA -n "left_wing_tip_ik_twist";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 0 5 0 7 0 9 0 11 0 13 0;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTU -n "left_wing_tip_ik_ikBlend";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 1 5 1 7 1 9 1 11 1 13 1;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTU -n "right_wing_ik_visibility";
	setAttr ".tan" 5;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 1 5 1 9 1 13 1;
	setAttr -s 4 ".kit[0:3]"  9 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
createNode animCurveTA -n "right_wing_ik_rotateX";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0 5 0 9 0 13 0;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTA -n "right_wing_ik_rotateY";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0 5 0 9 0 13 0;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTA -n "right_wing_ik_rotateZ";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0 5 0 9 0 13 0;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTU -n "right_wing_ik_scaleX";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 1 5 1 9 1 13 1;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTU -n "right_wing_ik_scaleY";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 1 5 1 9 1 13 1;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTU -n "right_wing_ik_scaleZ";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 1 5 1 9 1 13 1;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTU -n "right_wing_ik_poleVectorX";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 -0.96245932097705245 5 -0.96245932216133223 
		9 -0.96245939185166718 13 -0.96245931156326525;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTU -n "right_wing_ik_poleVectorY";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 -1.7531769990396688 5 -1.7531769983333727 
		9 -1.7531769608062635 13 -1.7531770041107402;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTU -n "right_wing_ik_poleVectorZ";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0.0065165560428971561 5 0.0065165711492227631 
		9 0.0065163743476313735 13 0.0065165821151442231;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTU -n "right_wing_ik_offset";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0 5 0 9 0 13 0;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTA -n "right_wing_ik_roll";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0 5 0 9 0 13 0;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTA -n "right_wing_ik_twist";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 0 5 0 9 0 13 0;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTU -n "right_wing_ik_ikBlend";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 4 ".ktv[0:3]"  1 1 5 1 9 1 13 1;
	setAttr -s 4 ".kit[0:3]"  10 1 1 1;
	setAttr -s 4 ".kot[0:3]"  10 1 1 1;
	setAttr -s 4 ".kix[1:3]"  1 1 1;
	setAttr -s 4 ".kiy[1:3]"  0 0 0;
	setAttr -s 4 ".kox[1:3]"  1 1 1;
	setAttr -s 4 ".koy[1:3]"  0 0 0;
createNode animCurveTU -n "right_wing_tip_ik_visibility";
	setAttr ".tan" 5;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 1 5 1 7 1 9 1 11 1 13 1;
	setAttr -s 6 ".kit[0:5]"  9 1 9 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
createNode animCurveTA -n "right_wing_tip_ik_rotateX";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 0 5 0 7 0 9 0 11 0 13 0;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTA -n "right_wing_tip_ik_rotateY";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 0 5 0 7 0 9 0 11 0 13 0;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTA -n "right_wing_tip_ik_rotateZ";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 0 5 0 7 0 9 0 11 0 13 0;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTU -n "right_wing_tip_ik_scaleX";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 1 5 1 7 1 9 1 11 1 13 1;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTU -n "right_wing_tip_ik_scaleY";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 1 5 1 7 1 9 1 11 1 13 1;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTU -n "right_wing_tip_ik_scaleZ";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 1 5 1 7 1 9 1 11 1 13 1;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTU -n "right_wing_tip_ik_poleVectorX";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 -1.9848806856587995 5 -1.9848806967965251 
		7 -1.9848806903545237 9 -1.9848806868136188 11 -1.9848806869284175 13 -1.9848806855444172;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTU -n "right_wing_tip_ik_poleVectorY";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 -0.17754239538664529 5 -0.177542245701099 
		7 -0.17754230591787049 9 -0.17754242653700258 11 -0.17754236095235451 13 -0.17754239571181715;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTU -n "right_wing_tip_ik_poleVectorZ";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 -0.16949147925208591 5 -0.16949150561602447 
		7 -0.16949151797995404 9 -0.16949143309820824 11 -0.16949150045377523 13 -0.16949148025098476;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTU -n "right_wing_tip_ik_offset";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 0 5 0 7 0 9 0 11 0 13 0;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTA -n "right_wing_tip_ik_roll";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 0 5 0 7 0 9 0 11 0 13 0;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTA -n "right_wing_tip_ik_twist";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 0 5 0 7 0 9 0 11 0 13 0;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTU -n "right_wing_tip_ik_ikBlend";
	setAttr ".tan" 1;
	setAttr ".wgt" no;
	setAttr -s 6 ".ktv[0:5]"  1 1 5 1 7 1 9 1 11 1 13 1;
	setAttr -s 6 ".kit[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kot[0:5]"  10 1 10 1 1 1;
	setAttr -s 6 ".kix[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".kiy[1:5]"  0 0 0 0 0;
	setAttr -s 6 ".kox[1:5]"  1 1 1 1 1;
	setAttr -s 6 ".koy[1:5]"  0 0 0 0 0;
createNode animCurveTU -n "spine_ik_visibility";
	setAttr ".tan" 5;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 1 5 1 7 1 9 1 13 1;
	setAttr -s 5 ".kit[0:4]"  9 9 9 9 1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
createNode animCurveTA -n "spine_ik_rotateX";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 0 5 0 7 0 9 0 13 0;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTA -n "spine_ik_rotateY";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 0 5 0 7 0 9 0 13 0;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTA -n "spine_ik_rotateZ";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 0 5 0 7 0 9 0 13 0;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTU -n "spine_ik_scaleX";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 1 5 1 7 1 9 1 13 1;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTU -n "spine_ik_scaleY";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 1 5 1 7 1 9 1 13 1;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTU -n "spine_ik_scaleZ";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 1 5 1 7 1 9 1 13 1;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTU -n "spine_ik_poleVectorX";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 1.7993567194697953 5 1.7993567132020578 
		7 1.7993566990454357 9 1.7993567100753669 13 1.7993567323699715;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTU -n "spine_ik_poleVectorY";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 0.85126536780755191 5 0.85126538045264266 
		7 0.85126540833856412 9 0.85126538686282194 13 0.85126534109380547;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTU -n "spine_ik_poleVectorZ";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 -0.19406872409116427 5 -0.194068726737473 
		7 -0.19406873567499039 9 -0.19406872760969185 13 -0.19406872166144906;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTU -n "spine_ik_offset";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 0 5 0 7 0 9 0 13 0;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTA -n "spine_ik_roll";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 0 5 0 7 0 9 0 13 0;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTA -n "spine_ik_twist";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 0 5 0 7 0 9 0 13 0;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTU -n "spine_ik_ikBlend";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 1 5 1 7 1 9 1 13 1;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTU -n "tail_ik_visibility";
	setAttr ".tan" 5;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 1 5 1 7 1 9 1 13 1;
	setAttr -s 5 ".kit[0:4]"  9 9 9 9 1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
createNode animCurveTA -n "tail_ik_rotateX";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 0 5 0 7 0 9 0 13 0;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTA -n "tail_ik_rotateY";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 0 5 0 7 0 9 0 13 0;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTA -n "tail_ik_rotateZ";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 0 5 0 7 0 9 0 13 0;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTU -n "tail_ik_scaleX";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 1 5 1 7 1 9 1 13 1;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTU -n "tail_ik_scaleY";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 1 5 1 7 1 9 1 13 1;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTU -n "tail_ik_scaleZ";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 1 5 1 7 1 9 1 13 1;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTU -n "tail_ik_poleVectorX";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 1.8666061782811976 5 1.8666062254013627 
		7 1.8666062207995613 9 1.8666062023813434 13 1.8666061840585455;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTU -n "tail_ik_poleVectorY";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 0.71807843045057218 5 0.71807830717185384 
		7 0.71807831903661823 9 0.71807836746851283 13 0.7180784153660964;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTU -n "tail_ik_poleVectorZ";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 0.012030915347401194 5 0.012030962641296106 
		7 0.012030968452871382 9 0.012030935343752547 13 0.012030919320998201;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTU -n "tail_ik_offset";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 0 5 0 7 0 9 0 13 0;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTA -n "tail_ik_roll";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 0 5 0 7 0 9 0 13 0;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTA -n "tail_ik_twist";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 0 5 0 7 0 9 0 13 0;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode animCurveTU -n "tail_ik_ikBlend";
	setAttr ".tan" 10;
	setAttr ".wgt" no;
	setAttr -s 5 ".ktv[0:4]"  1 1 5 1 7 1 9 1 13 1;
	setAttr -s 5 ".kit[4]"  1;
	setAttr -s 5 ".kot[4]"  1;
	setAttr -s 5 ".kix[4]"  1;
	setAttr -s 5 ".kiy[4]"  0;
	setAttr -s 5 ".kox[4]"  1;
	setAttr -s 5 ".koy[4]"  0;
createNode polySoftEdge -n "polySoftEdge1";
	setAttr ".uopa" yes;
	setAttr ".ics" -type "componentList" 1 "e[*]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
	setAttr ".a" 180;
select -ne :time1;
	setAttr ".o" 10;
select -ne :renderPartition;
	setAttr -s 4 ".st";
select -ne :renderGlobalsList1;
select -ne :defaultShaderList1;
	setAttr -s 4 ".s";
select -ne :postProcessList1;
	setAttr -s 2 ".p";
select -ne :lightList1;
select -ne :initialShadingGroup;
	setAttr ".ro" yes;
select -ne :initialParticleSE;
	setAttr ".ro" yes;
select -ne :defaultRenderGlobals;
	setAttr ".mcfr" 15;
	setAttr ".outf" 23;
	setAttr ".an" yes;
	setAttr ".ef" 120;
	setAttr ".bfs" 0.60000002384185791;
	setAttr ".ifp" -type "string" "bird";
select -ne :defaultResolution;
	setAttr ".pa" 1;
select -ne :hardwareRenderGlobals;
	setAttr ".ctrs" 256;
	setAttr ".btrs" 512;
	setAttr ".hwfr" 15;
select -ne :defaultHardwareRenderGlobals;
	setAttr ".fn" -type "string" "im";
	setAttr ".res" -type "string" "ntsc_4d 646 485 1.333";
select -ne :ikSystem;
	setAttr -s 4 ".sol";
select -ne :hyperGraphLayout;
	setAttr ".hyp[0].x" 1191.0853271484375;
	setAttr ".hyp[0].y" -12.669804573059082;
	setAttr ".hyp[0].isf" yes;
connectAttr "body.di" "m_bird.do";
connectAttr "skinCluster1GroupId.id" "m_birdShape.iog.og[6].gid";
connectAttr "skinCluster1Set.mwc" "m_birdShape.iog.og[6].gco";
connectAttr "groupId2.id" "m_birdShape.iog.og[7].gid";
connectAttr "tweakSet1.mwc" "m_birdShape.iog.og[7].gco";
connectAttr "polySoftEdge1.out" "m_birdShape.i";
connectAttr "tweak1.vl[0].vt[0]" "m_birdShape.twl";
connectAttr "deleteComponent1.og" "m_birdShapeOrig.i";
connectAttr "rig.di" "root.do";
connectAttr "root.s" "tail_joint.is";
connectAttr "rig.di" "tail_joint.do";
connectAttr "tail_joint.s" "tail.is";
connectAttr "tail.tx" "effector6.tx";
connectAttr "tail.ty" "effector6.ty";
connectAttr "tail.tz" "effector6.tz";
connectAttr "root.s" "left_wing_joint.is";
connectAttr "rig.di" "left_wing_joint.do";
connectAttr "left_wing_joint.s" "left_wing.is";
connectAttr "rig.di" "left_wing.do";
connectAttr "left_wing.s" "left_tip.is";
connectAttr "rig.di" "left_tip.do";
connectAttr "left_tip.tx" "effector2.tx";
connectAttr "left_tip.ty" "effector2.ty";
connectAttr "left_tip.tz" "effector2.tz";
connectAttr "rig.di" "effector2.do";
connectAttr "left_wing.tx" "effector1.tx";
connectAttr "left_wing.ty" "effector1.ty";
connectAttr "left_wing.tz" "effector1.tz";
connectAttr "rig.di" "effector1.do";
connectAttr "root.s" "right_wing_joint.is";
connectAttr "rig.di" "right_wing_joint.do";
connectAttr "right_wing_joint.s" "right_wing.is";
connectAttr "rig.di" "right_wing.do";
connectAttr "right_wing.s" "right_tip.is";
connectAttr "rig.di" "right_tip.do";
connectAttr "right_tip.tx" "effector4.tx";
connectAttr "right_tip.ty" "effector4.ty";
connectAttr "right_tip.tz" "effector4.tz";
connectAttr "rig.di" "effector4.do";
connectAttr "right_wing.tx" "effector3.tx";
connectAttr "right_wing.ty" "effector3.ty";
connectAttr "right_wing.tz" "effector3.tz";
connectAttr "rig.di" "effector3.do";
connectAttr "tail_joint.tx" "effector5.tx";
connectAttr "tail_joint.ty" "effector5.ty";
connectAttr "tail_joint.tz" "effector5.tz";
connectAttr "left_wing_joint.msg" "left_wing_ik.hsj";
connectAttr "effector1.hp" "left_wing_ik.hee";
connectAttr "ikRPsolver.msg" "left_wing_ik.hsv";
connectAttr "rig.di" "left_wing_ik.do";
connectAttr "left_wing_ik_translateX.o" "left_wing_ik.tx";
connectAttr "left_wing_ik_translateY.o" "left_wing_ik.ty";
connectAttr "left_wing_ik_translateZ.o" "left_wing_ik.tz";
connectAttr "left_wing_ik_visibility.o" "left_wing_ik.v";
connectAttr "left_wing_ik_rotateX.o" "left_wing_ik.rx";
connectAttr "left_wing_ik_rotateY.o" "left_wing_ik.ry";
connectAttr "left_wing_ik_rotateZ.o" "left_wing_ik.rz";
connectAttr "left_wing_ik_scaleX.o" "left_wing_ik.sx";
connectAttr "left_wing_ik_scaleY.o" "left_wing_ik.sy";
connectAttr "left_wing_ik_scaleZ.o" "left_wing_ik.sz";
connectAttr "left_wing_ik_poleVectorX.o" "left_wing_ik.pvx";
connectAttr "left_wing_ik_poleVectorY.o" "left_wing_ik.pvy";
connectAttr "left_wing_ik_poleVectorZ.o" "left_wing_ik.pvz";
connectAttr "left_wing_ik_offset.o" "left_wing_ik.off";
connectAttr "left_wing_ik_roll.o" "left_wing_ik.rol";
connectAttr "left_wing_ik_twist.o" "left_wing_ik.twi";
connectAttr "left_wing_ik_ikBlend.o" "left_wing_ik.ikb";
connectAttr "left_wing.msg" "left_wing_tip_ik.hsj";
connectAttr "effector2.hp" "left_wing_tip_ik.hee";
connectAttr "ikRPsolver.msg" "left_wing_tip_ik.hsv";
connectAttr "rig.di" "left_wing_tip_ik.do";
connectAttr "left_wing_tip_ik_translateX.o" "left_wing_tip_ik.tx";
connectAttr "left_wing_tip_ik_translateY.o" "left_wing_tip_ik.ty";
connectAttr "left_wing_tip_ik_translateZ.o" "left_wing_tip_ik.tz";
connectAttr "left_wing_tip_ik_visibility.o" "left_wing_tip_ik.v";
connectAttr "left_wing_tip_ik_rotateX.o" "left_wing_tip_ik.rx";
connectAttr "left_wing_tip_ik_rotateY.o" "left_wing_tip_ik.ry";
connectAttr "left_wing_tip_ik_rotateZ.o" "left_wing_tip_ik.rz";
connectAttr "left_wing_tip_ik_scaleX.o" "left_wing_tip_ik.sx";
connectAttr "left_wing_tip_ik_scaleY.o" "left_wing_tip_ik.sy";
connectAttr "left_wing_tip_ik_scaleZ.o" "left_wing_tip_ik.sz";
connectAttr "left_wing_tip_ik_poleVectorX.o" "left_wing_tip_ik.pvx";
connectAttr "left_wing_tip_ik_poleVectorY.o" "left_wing_tip_ik.pvy";
connectAttr "left_wing_tip_ik_poleVectorZ.o" "left_wing_tip_ik.pvz";
connectAttr "left_wing_tip_ik_offset.o" "left_wing_tip_ik.off";
connectAttr "left_wing_tip_ik_roll.o" "left_wing_tip_ik.rol";
connectAttr "left_wing_tip_ik_twist.o" "left_wing_tip_ik.twi";
connectAttr "left_wing_tip_ik_ikBlend.o" "left_wing_tip_ik.ikb";
connectAttr "right_wing_joint.msg" "right_wing_ik.hsj";
connectAttr "effector3.hp" "right_wing_ik.hee";
connectAttr "ikRPsolver.msg" "right_wing_ik.hsv";
connectAttr "rig.di" "right_wing_ik.do";
connectAttr "right_wing_ik_translateX.o" "right_wing_ik.tx";
connectAttr "right_wing_ik_translateY.o" "right_wing_ik.ty";
connectAttr "right_wing_ik_translateZ.o" "right_wing_ik.tz";
connectAttr "right_wing_ik_visibility.o" "right_wing_ik.v";
connectAttr "right_wing_ik_rotateX.o" "right_wing_ik.rx";
connectAttr "right_wing_ik_rotateY.o" "right_wing_ik.ry";
connectAttr "right_wing_ik_rotateZ.o" "right_wing_ik.rz";
connectAttr "right_wing_ik_scaleX.o" "right_wing_ik.sx";
connectAttr "right_wing_ik_scaleY.o" "right_wing_ik.sy";
connectAttr "right_wing_ik_scaleZ.o" "right_wing_ik.sz";
connectAttr "right_wing_ik_poleVectorX.o" "right_wing_ik.pvx";
connectAttr "right_wing_ik_poleVectorY.o" "right_wing_ik.pvy";
connectAttr "right_wing_ik_poleVectorZ.o" "right_wing_ik.pvz";
connectAttr "right_wing_ik_offset.o" "right_wing_ik.off";
connectAttr "right_wing_ik_roll.o" "right_wing_ik.rol";
connectAttr "right_wing_ik_twist.o" "right_wing_ik.twi";
connectAttr "right_wing_ik_ikBlend.o" "right_wing_ik.ikb";
connectAttr "right_wing.msg" "right_wing_tip_ik.hsj";
connectAttr "effector4.hp" "right_wing_tip_ik.hee";
connectAttr "ikRPsolver.msg" "right_wing_tip_ik.hsv";
connectAttr "rig.di" "right_wing_tip_ik.do";
connectAttr "right_wing_tip_ik_translateX.o" "right_wing_tip_ik.tx";
connectAttr "right_wing_tip_ik_translateY.o" "right_wing_tip_ik.ty";
connectAttr "right_wing_tip_ik_translateZ.o" "right_wing_tip_ik.tz";
connectAttr "right_wing_tip_ik_visibility.o" "right_wing_tip_ik.v";
connectAttr "right_wing_tip_ik_rotateX.o" "right_wing_tip_ik.rx";
connectAttr "right_wing_tip_ik_rotateY.o" "right_wing_tip_ik.ry";
connectAttr "right_wing_tip_ik_rotateZ.o" "right_wing_tip_ik.rz";
connectAttr "right_wing_tip_ik_scaleX.o" "right_wing_tip_ik.sx";
connectAttr "right_wing_tip_ik_scaleY.o" "right_wing_tip_ik.sy";
connectAttr "right_wing_tip_ik_scaleZ.o" "right_wing_tip_ik.sz";
connectAttr "right_wing_tip_ik_poleVectorX.o" "right_wing_tip_ik.pvx";
connectAttr "right_wing_tip_ik_poleVectorY.o" "right_wing_tip_ik.pvy";
connectAttr "right_wing_tip_ik_poleVectorZ.o" "right_wing_tip_ik.pvz";
connectAttr "right_wing_tip_ik_offset.o" "right_wing_tip_ik.off";
connectAttr "right_wing_tip_ik_roll.o" "right_wing_tip_ik.rol";
connectAttr "right_wing_tip_ik_twist.o" "right_wing_tip_ik.twi";
connectAttr "right_wing_tip_ik_ikBlend.o" "right_wing_tip_ik.ikb";
connectAttr "root.msg" "spine_ik.hsj";
connectAttr "effector5.hp" "spine_ik.hee";
connectAttr "ikRPsolver.msg" "spine_ik.hsv";
connectAttr "spine_ik_translateX.o" "spine_ik.tx";
connectAttr "spine_ik_translateY.o" "spine_ik.ty";
connectAttr "spine_ik_translateZ.o" "spine_ik.tz";
connectAttr "spine_ik_visibility.o" "spine_ik.v";
connectAttr "spine_ik_rotateX.o" "spine_ik.rx";
connectAttr "spine_ik_rotateY.o" "spine_ik.ry";
connectAttr "spine_ik_rotateZ.o" "spine_ik.rz";
connectAttr "spine_ik_scaleX.o" "spine_ik.sx";
connectAttr "spine_ik_scaleY.o" "spine_ik.sy";
connectAttr "spine_ik_scaleZ.o" "spine_ik.sz";
connectAttr "spine_ik_poleVectorX.o" "spine_ik.pvx";
connectAttr "spine_ik_poleVectorY.o" "spine_ik.pvy";
connectAttr "spine_ik_poleVectorZ.o" "spine_ik.pvz";
connectAttr "spine_ik_offset.o" "spine_ik.off";
connectAttr "spine_ik_roll.o" "spine_ik.rol";
connectAttr "spine_ik_twist.o" "spine_ik.twi";
connectAttr "spine_ik_ikBlend.o" "spine_ik.ikb";
connectAttr "tail_joint.msg" "tail_ik.hsj";
connectAttr "effector6.hp" "tail_ik.hee";
connectAttr "ikRPsolver.msg" "tail_ik.hsv";
connectAttr "tail_ik_translateX.o" "tail_ik.tx";
connectAttr "tail_ik_translateY.o" "tail_ik.ty";
connectAttr "tail_ik_translateZ.o" "tail_ik.tz";
connectAttr "tail_ik_visibility.o" "tail_ik.v";
connectAttr "tail_ik_rotateX.o" "tail_ik.rx";
connectAttr "tail_ik_rotateY.o" "tail_ik.ry";
connectAttr "tail_ik_rotateZ.o" "tail_ik.rz";
connectAttr "tail_ik_scaleX.o" "tail_ik.sx";
connectAttr "tail_ik_scaleY.o" "tail_ik.sy";
connectAttr "tail_ik_scaleZ.o" "tail_ik.sz";
connectAttr "tail_ik_poleVectorX.o" "tail_ik.pvx";
connectAttr "tail_ik_poleVectorY.o" "tail_ik.pvy";
connectAttr "tail_ik_poleVectorZ.o" "tail_ik.pvz";
connectAttr "tail_ik_offset.o" "tail_ik.off";
connectAttr "tail_ik_roll.o" "tail_ik.rol";
connectAttr "tail_ik_twist.o" "tail_ik.twi";
connectAttr "tail_ik_ikBlend.o" "tail_ik.ikb";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[0].llnk";
connectAttr ":initialShadingGroup.msg" "lightLinker1.lnk[0].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[1].llnk";
connectAttr ":initialParticleSE.msg" "lightLinker1.lnk[1].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[2].llnk";
connectAttr "phong1SG.msg" "lightLinker1.lnk[2].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[3].llnk";
connectAttr "phong2SG.msg" "lightLinker1.lnk[3].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[0].sllk";
connectAttr ":initialShadingGroup.msg" "lightLinker1.slnk[0].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[1].sllk";
connectAttr ":initialParticleSE.msg" "lightLinker1.slnk[1].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[2].sllk";
connectAttr "phong1SG.msg" "lightLinker1.slnk[2].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[3].sllk";
connectAttr "phong2SG.msg" "lightLinker1.slnk[3].solk";
connectAttr "layerManager.dli[0]" "defaultLayer.id";
connectAttr "renderLayerManager.rlmi[0]" "defaultRenderLayer.rlid";
connectAttr "polyTweak1.out" "polyExtrudeFace1.ip";
connectAttr "m_birdShape.wm" "polyExtrudeFace1.mp";
connectAttr "polySphere1.out" "polyTweak1.ip";
connectAttr "polyTweak2.out" "polyExtrudeFace2.ip";
connectAttr "m_birdShape.wm" "polyExtrudeFace2.mp";
connectAttr "polyExtrudeFace1.out" "polyTweak2.ip";
connectAttr "polyTweak3.out" "polyExtrudeFace3.ip";
connectAttr "m_birdShape.wm" "polyExtrudeFace3.mp";
connectAttr "polyExtrudeFace2.out" "polyTweak3.ip";
connectAttr "polyTweak4.out" "polyExtrudeFace4.ip";
connectAttr "m_birdShape.wm" "polyExtrudeFace4.mp";
connectAttr "polyExtrudeFace3.out" "polyTweak4.ip";
connectAttr "polyTweak5.out" "polyMergeVert1.ip";
connectAttr "m_birdShape.wm" "polyMergeVert1.mp";
connectAttr "polyExtrudeFace4.out" "polyTweak5.ip";
connectAttr "polyTweak6.out" "polyExtrudeFace5.ip";
connectAttr "m_birdShape.wm" "polyExtrudeFace5.mp";
connectAttr "polyMergeVert1.out" "polyTweak6.ip";
connectAttr "polyTweak7.out" "polyMergeVert2.ip";
connectAttr "m_birdShape.wm" "polyMergeVert2.mp";
connectAttr "polyExtrudeFace5.out" "polyTweak7.ip";
connectAttr "polyTweak8.out" "polyMergeVert3.ip";
connectAttr "m_birdShape.wm" "polyMergeVert3.mp";
connectAttr "polyMergeVert2.out" "polyTweak8.ip";
connectAttr "polyTweak9.out" "polyExtrudeFace6.ip";
connectAttr "m_birdShape.wm" "polyExtrudeFace6.mp";
connectAttr "polyMergeVert3.out" "polyTweak9.ip";
connectAttr "polyExtrudeFace6.out" "polyExtrudeFace7.ip";
connectAttr "m_birdShape.wm" "polyExtrudeFace7.mp";
connectAttr "polyExtrudeFace7.out" "polyExtrudeFace8.ip";
connectAttr "m_birdShape.wm" "polyExtrudeFace8.mp";
connectAttr "polyExtrudeFace8.out" "polyTweak10.ip";
connectAttr "polyTweak10.out" "deleteComponent1.ig";
connectAttr "mat_bird.oc" "phong1SG.ss";
connectAttr "m_birdShape.iog" "phong1SG.dsm" -na;
connectAttr "phong1SG.msg" "materialInfo1.sg";
connectAttr "mat_bird.msg" "materialInfo1.m";
connectAttr "mat_phys.oc" "phong2SG.ss";
connectAttr "phong2SG.msg" "materialInfo2.sg";
connectAttr "mat_phys.msg" "materialInfo2.m";
connectAttr "layerManager.dli[1]" "body.id";
connectAttr "layerManager.dli[2]" "rig.id";
connectAttr "skinCluster1GroupParts.og" "skinCluster1.ip[0].ig";
connectAttr "skinCluster1GroupId.id" "skinCluster1.ip[0].gi";
connectAttr "bindPose1.msg" "skinCluster1.bp";
connectAttr "root.wm" "skinCluster1.ma[0]";
connectAttr "tail_joint.wm" "skinCluster1.ma[1]";
connectAttr "tail.wm" "skinCluster1.ma[2]";
connectAttr "left_wing_joint.wm" "skinCluster1.ma[3]";
connectAttr "left_wing.wm" "skinCluster1.ma[4]";
connectAttr "left_tip.wm" "skinCluster1.ma[5]";
connectAttr "right_wing_joint.wm" "skinCluster1.ma[6]";
connectAttr "right_wing.wm" "skinCluster1.ma[7]";
connectAttr "right_tip.wm" "skinCluster1.ma[8]";
connectAttr "root.liw" "skinCluster1.lw[0]";
connectAttr "tail_joint.liw" "skinCluster1.lw[1]";
connectAttr "tail.liw" "skinCluster1.lw[2]";
connectAttr "left_wing_joint.liw" "skinCluster1.lw[3]";
connectAttr "left_wing.liw" "skinCluster1.lw[4]";
connectAttr "left_tip.liw" "skinCluster1.lw[5]";
connectAttr "right_wing_joint.liw" "skinCluster1.lw[6]";
connectAttr "right_wing.liw" "skinCluster1.lw[7]";
connectAttr "right_tip.liw" "skinCluster1.lw[8]";
connectAttr "left_wing_joint.msg" "skinCluster1.ptt";
connectAttr "groupParts2.og" "tweak1.ip[0].ig";
connectAttr "groupId2.id" "tweak1.ip[0].gi";
connectAttr "skinCluster1GroupId.msg" "skinCluster1Set.gn" -na;
connectAttr "m_birdShape.iog.og[6]" "skinCluster1Set.dsm" -na;
connectAttr "skinCluster1.msg" "skinCluster1Set.ub[0]";
connectAttr "tweak1.og[0]" "skinCluster1GroupParts.ig";
connectAttr "skinCluster1GroupId.id" "skinCluster1GroupParts.gi";
connectAttr "groupId2.msg" "tweakSet1.gn" -na;
connectAttr "m_birdShape.iog.og[7]" "tweakSet1.dsm" -na;
connectAttr "tweak1.msg" "tweakSet1.ub[0]";
connectAttr "m_birdShapeOrig.w" "groupParts2.ig";
connectAttr "groupId2.id" "groupParts2.gi";
connectAttr "root.msg" "bindPose1.m[0]";
connectAttr "tail_joint.msg" "bindPose1.m[1]";
connectAttr "tail.msg" "bindPose1.m[2]";
connectAttr "left_wing_joint.msg" "bindPose1.m[3]";
connectAttr "left_wing.msg" "bindPose1.m[4]";
connectAttr "left_tip.msg" "bindPose1.m[5]";
connectAttr "right_wing_joint.msg" "bindPose1.m[6]";
connectAttr "right_wing.msg" "bindPose1.m[7]";
connectAttr "right_tip.msg" "bindPose1.m[8]";
connectAttr "bindPose1.w" "bindPose1.p[0]";
connectAttr "bindPose1.m[0]" "bindPose1.p[1]";
connectAttr "bindPose1.m[1]" "bindPose1.p[2]";
connectAttr "bindPose1.m[0]" "bindPose1.p[3]";
connectAttr "bindPose1.m[3]" "bindPose1.p[4]";
connectAttr "bindPose1.m[4]" "bindPose1.p[5]";
connectAttr "bindPose1.m[0]" "bindPose1.p[6]";
connectAttr "bindPose1.m[6]" "bindPose1.p[7]";
connectAttr "bindPose1.m[7]" "bindPose1.p[8]";
connectAttr "root.bps" "bindPose1.wm[0]";
connectAttr "tail_joint.bps" "bindPose1.wm[1]";
connectAttr "tail.bps" "bindPose1.wm[2]";
connectAttr "left_wing_joint.bps" "bindPose1.wm[3]";
connectAttr "left_wing.bps" "bindPose1.wm[4]";
connectAttr "left_tip.bps" "bindPose1.wm[5]";
connectAttr "right_wing_joint.bps" "bindPose1.wm[6]";
connectAttr "right_wing.bps" "bindPose1.wm[7]";
connectAttr "right_tip.bps" "bindPose1.wm[8]";
connectAttr "skinCluster1.og[0]" "polySoftEdge1.ip";
connectAttr "m_birdShape.wm" "polySoftEdge1.mp";
connectAttr "phong1SG.pa" ":renderPartition.st" -na;
connectAttr "phong2SG.pa" ":renderPartition.st" -na;
connectAttr "mat_bird.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_phys.msg" ":defaultShaderList1.s" -na;
connectAttr "lightLinker1.msg" ":lightList1.ln" -na;
connectAttr "ikRPsolver.msg" ":ikSystem.sol" -na;
connectAttr "tail_ik.msg" ":hyperGraphLayout.hyp[0].dn";
// End of bird_animated1.ma
