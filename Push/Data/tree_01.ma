//Maya ASCII 2009 scene
//Name: tree_01.ma
//Last modified: Fri, Apr 01, 2011 09:56:15 PM
//Codeset: 1252
requires maya "2009";
requires "stereoCamera" "10.0";
currentUnit -l centimeter -a degree -t film;
fileInfo "application" "maya";
fileInfo "product" "Maya Unlimited 2009";
fileInfo "version" "2009 Service Pack 1a";
fileInfo "cutIdentifier" "200904080023-749524";
fileInfo "osv" "Microsoft Windows XP Service Pack 3 (Build 2600)\n";
createNode transform -s -n "persp";
	setAttr ".v" no;
	setAttr ".t" -type "double3" -5.8797809889891823 15.565461056471587 28.604494758407057 ;
	setAttr ".r" -type "double3" -22.538352730029825 -1455.3999999996468 0 ;
createNode camera -s -n "perspShape" -p "persp";
	setAttr -k off ".v" no;
	setAttr ".fl" 34.999999999999993;
	setAttr ".coi" 34.376145501295383;
	setAttr ".imn" -type "string" "persp";
	setAttr ".den" -type "string" "persp_depth";
	setAttr ".man" -type "string" "persp_mask";
	setAttr ".hc" -type "string" "viewSet -p %camera";
createNode transform -s -n "top";
	setAttr ".v" no;
	setAttr ".t" -type "double3" 0 100.1 0 ;
	setAttr ".r" -type "double3" -89.999999999999986 0 0 ;
createNode camera -s -n "topShape" -p "top";
	setAttr -k off ".v" no;
	setAttr ".rnd" no;
	setAttr ".coi" 100.1;
	setAttr ".ow" 15.000068016812264;
	setAttr ".imn" -type "string" "top";
	setAttr ".den" -type "string" "top_depth";
	setAttr ".man" -type "string" "top_mask";
	setAttr ".hc" -type "string" "viewSet -t %camera";
	setAttr ".o" yes;
createNode transform -s -n "front";
	setAttr ".v" no;
	setAttr ".t" -type "double3" 0.5957214705506082 3.4394699618816107 100.1 ;
createNode camera -s -n "frontShape" -p "front";
	setAttr -k off ".v" no;
	setAttr ".rnd" no;
	setAttr ".coi" 100.1;
	setAttr ".ow" 5.5757610159327191;
	setAttr ".imn" -type "string" "front";
	setAttr ".den" -type "string" "front_depth";
	setAttr ".man" -type "string" "front_mask";
	setAttr ".hc" -type "string" "viewSet -f %camera";
	setAttr ".o" yes;
createNode transform -s -n "side";
	setAttr ".v" no;
	setAttr ".t" -type "double3" 100.1 5.7212418150892654 1.1900664176762008 ;
	setAttr ".r" -type "double3" 0 89.999999999999986 0 ;
createNode camera -s -n "sideShape" -p "side";
	setAttr -k off ".v" no;
	setAttr ".rnd" no;
	setAttr ".coi" 100.1;
	setAttr ".ow" 3.1444108606198053;
	setAttr ".imn" -type "string" "side";
	setAttr ".den" -type "string" "side_depth";
	setAttr ".man" -type "string" "side_mask";
	setAttr ".hc" -type "string" "viewSet -s %camera";
	setAttr ".o" yes;
createNode transform -n "m_tree1";
createNode mesh -n "m_tree1Shape" -p "m_tree1";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "string";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	addAttr -ci true -sn "rgn" -ln "rgn" -dt "string";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 42 ".pt[0:41]" -type "float3"  0 5.0033693 0 0 5.0033693 
		0 0 5.0033693 0 0 5.0033693 0 0 5.0033693 0 0 5.0033693 0 0 5.0033693 0 0 5.0033693 
		0 0 4.2621603 0 0 4.2621603 0 0 4.2621603 0 0 4.2621603 0 0 4.2621603 0 0 4.2621603 
		0 0 4.2621603 0 0 4.2621603 0 0 5.0033693 0 0 5.0033693 0 0 5.0033693 0 0 5.0033693 
		0 0 5.0033693 0 0 5.0033693 0 0 5.0033693 0 0 5.0033693 0 0 5.0033693 0 0 5.0033693 
		0 0 5.0033693 0 0 5.0033693 0 0 5.0033693 0 0 5.0033693 0 0 5.0033693 0 0 5.0033693 
		0 0 5.0033693 0 0 5.0033693 0 0 5.0033693 0 0 5.0033693 0 0 5.0033693 0 0 5.0033693 
		0 -0.024596613 5.0445433 -0.035162032 0.010423528 5.0477028 0.034649689 -0.010809956 
		4.9601479 -0.033754066 0.023983648 4.9620342 0.035221517;
	setAttr ".rgvtx" -type "string" "[0.28284263610839844, 0.0033693313598632813, -0.28284269571304321, 0, 0.0033693313598632813, -0.3999999463558197, -0.28284263610839844, 0.0033693313598632813, -0.28284269571304321, -0.40000009536743164, 0.0033693313598632813, 0, -0.28284263610839844, 0.0033693313598632813, 0.28284269571304321, 0, 0.0033693313598632813, 0.39999997615814209, 0.28284263610839844, 0.0033693313598632813, 0.2828427255153656, 0.40000009536743164, 0.0033693313598632813, 0, 0.1841888427734375, 7.6759662628173828, -0.1841888427734375, 0, 7.6759662628173828, -0.26048237085342407, -0.1841888427734375, 7.6759662628173828, -0.1841888427734375, -0.2604823112487793, 7.6759662628173828, 5.1974349268846254e-009, -0.1841888427734375, 7.6759662628173828, 0.18418887257575989, 0, 7.6759662628173828, 0.26048240065574646, 0.1841888427734375, 7.6759662628173828, 0.18418887257575989, 0.2604823112487793, 7.6759662628173828, 5.1974349268846254e-009, 0, 0.0033693313598632813, 0, 0, 11.296511650085449, 0, 4.76837158203125e-007, 4.5015764236450195, 0.35610631108283997, 0.28935050964355469, 4.4554638862609863, 0.26466137170791626, 0.40000009536743164, 4.5005154609680176, 0.043893672525882721, 4.76837158203125e-007, 4.6874995231628418, 0.35610631108283997, 0.30353307723999023, 4.7618508338928223, 0.26466137170791626, 0.40000009536743164, 4.6802115440368652, 0.043893672525882721, 2.632936954498291, 6.0453629493713379, 1.2570126056671143, 2.547966480255127, 6.0611605644226074, 1.2672590017318726, 2.5803036689758301, 6.1664924621582031, 1.2201321125030518, 2.5179429054260254, 6.1302552223205566, 1.2462217807769775, 2.6232905387878418, 6.0694494247436523, 1.1886454820632935, 2.5942726135253906, 6.1362299919128418, 1.1683127880096436, -0.30350923538208008, 5.7355003356933594, 3.5945713072749186e-009, -0.2146611213684082, 5.7314162254333496, 0.2146611213684082, -0.30727624893188477, 5.5173988342285156, 3.454241337408348e-009, -0.21728801727294922, 5.5164484977722168, 0.21728801727294922, -0.23662853240966797, 5.5542988777160645, 0.1690199077129364, -0.28665971755981445, 5.5557384490966797, 0.048172727227210999, -0.23516702651977539, 5.6933941841125488, 0.1667427122592926, -0.28456544876098633, 5.6968517303466797, 0.047344900667667389, -2.3905606269836426, 6.4283175468444824, 0.82867282629013062, -2.4055716991424561, 6.4329166412353516, 0.7776373028755188, -2.375312328338623, 6.4830174446105957, 0.82780355215072632, -2.3899171352386475, 6.4883613586425781, 0.77738136053085327]";
	setAttr ".rgf" -type "string" "[[0,1,9,8],[1,2,10,9],[2,3,32,30,11,10],[3,4,33,32],[4,5,18,21,13,12,31,33],[5,6,19,18],[6,7,20,19],[7,0,8,15,23,20],[1,0,16],[2,1,16],[3,2,16],[4,3,16],[5,4,16],[6,5,16],[7,6,16],[0,7,16],[8,9,17],[9,10,17],[10,11,17],[11,12,17],[12,13,17],[13,14,17],[14,15,17],[15,8,17],[25,24,26,27],[24,28,29,26],[21,22,14,13],[22,23,15,14],[18,19,24,25],[22,21,27,26],[21,18,25,27],[19,20,28,24],[20,23,29,28],[23,22,26,29],[30,31,12,11],[39,38,40,41],[32,33,34,35],[33,31,36,34],[31,30,37,36],[30,32,35,37],[35,34,38,39],[34,36,40,38],[36,37,41,40],[37,35,39,41]]";
	setAttr ".rgn" -type "string" "[0.38262942433357239, 0.016797341406345367, -0.92374914884567261, 0.38262942433357239, 0.016797341406345367, -0.92374914884567261, 0.38262942433357239, 0.016797341406345367, -0.92374914884567261, 0.38262942433357239, 0.016797341406345367, -0.92374914884567261, -0.38262942433357239, 0.016797345131635666, -0.92374914884567261, -0.38262942433357239, 0.016797345131635666, -0.92374914884567261, -0.38262942433357239, 0.016797345131635666, -0.92374914884567261, -0.38262942433357239, 0.016797345131635666, -0.92374914884567261, -0.91822648048400879, 0.016696931794285774, -0.39570346474647522, -0.91822648048400879, 0.016696931794285774, -0.39570346474647522, -0.91822648048400879, 0.016696931794285774, -0.39570346474647522, -0.91822648048400879, 0.016696931794285774, -0.39570346474647522, -0.91822648048400879, 0.016696931794285774, -0.39570346474647522, -0.91822648048400879, 0.016696931794285774, -0.39570346474647522, -0.9237675666809082, 0.01553410105407238, 0.38263845443725586, -0.9237675666809082, 0.01553410105407238, 0.38263845443725586, -0.92376267910003662, 0.015816977247595787, 0.38263890147209167, -0.9237634539604187, 0.015813007950782776, 0.38263696432113647, -0.43472355604171753, 0.016181837767362595, 0.90041857957839966, -0.43472355604171753, 0.016181837767362595, 0.90041857957839966, -0.43472355604171753, 0.016181837767362595, 0.90041857957839966, -0.43472355604171753, 0.016181837767362595, 0.90041857957839966, -0.43472355604171753, 0.016181837767362595, 0.90041857957839966, -0.43472355604171753, 0.016181837767362595, 0.90041857957839966, -0.43472355604171753, 0.016181837767362595, 0.90041857957839966, -0.43472355604171753, 0.016181837767362595, 0.90041857957839966, 0.34295356273651123, 0.006265370175242424, 0.93933147192001343, 0.34295356273651123, 0.006265370175242424, 0.93933147192001343, 0.34295356273651123, 0.006265370175242424, 0.93933147192001343, 0.34295356273651123, 0.006265370175242424, 0.93933147192001343, 0.91117972135543823, -0.0018463532906025648, 0.41200491786003113, 0.91117972135543823, -0.0018463532906025648, 0.41200491786003113, 0.91117972135543823, -0.0018463532906025648, 0.41200491786003113, 0.91117972135543823, -0.0018463532906025648, 0.41200491786003113, 0.87832653522491455, 0.015939328819513321, -0.47779527306556702, 0.87832653522491455, 0.015939328819513321, -0.47779527306556702, 0.87832653522491455, 0.015939328819513321, -0.47779527306556702, 0.87832653522491455, 0.015939328819513321, -0.47779527306556702, 0.87832653522491455, 0.015939328819513321, -0.47779527306556702, 0.87832653522491455, 0.015939328819513321, -0.47779527306556702, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -0.99999994039535522, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -0.99999994039535522, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0.7052837610244751, 0.071760125458240509, -0.70528370141983032, 0, 0.071760155260562897, -0.9974219799041748, -3.370147965142678e-007, 1, -1.685073982571339e-007, 0, 0.071760155260562897, -0.9974219799041748, -0.7052839994430542, 0.0717601478099823, -0.70528370141983032, -3.370147965142678e-007, 1, -1.685073982571339e-007, -0.7052839994430542, 0.0717601478099823, -0.70528370141983032, -0.99742186069488525, 0.071760118007659912, 1.2898252066406712e-007, -3.370147965142678e-007, 1, -1.685073982571339e-007, -0.99742186069488525, 0.071760118007659912, 1.2898252066406712e-007, -0.70528388023376465, 0.071760132908821106, 0.70528364181518555, -3.370147965142678e-007, 1, -1.685073982571339e-007, -0.70528388023376465, 0.071760132908821106, 0.70528364181518555, 0, 0.071760170161724091, 0.9974219799041748, -3.370147965142678e-007, 1, -1.685073982571339e-007, 0, 0.071760170161724091, 0.9974219799041748, 0.7052839994430542, 0.071760155260562897, 0.70528370141983032, -3.370147965142678e-007, 1, -1.685073982571339e-007, 0.7052839994430542, 0.071760155260562897, 0.70528370141983032, 0.99742192029953003, 0.071760125458240509, -1.7735101209837012e-007, -3.370147965142678e-007, 1, -1.685073982571339e-007, 0.99742192029953003, 0.071760125458240509, -1.7735101209837012e-007, 0.7052837610244751, 0.071760125458240509, -0.70528370141983032, -3.370147965142678e-007, 1, -1.685073982571339e-007, 0.17686781287193298, 0.35621005296707153, 0.91751408576965332, 0.17686781287193298, 0.35621005296707153, 0.91751408576965332, 0.17686781287193298, 0.35621005296707153, 0.91751408576965332, 0.17686781287193298, 0.35621005296707153, 0.91751408576965332, 0.91571390628814697, 0.40164268016815186, 0.012297466397285461, 0.91571390628814697, 0.40164268016815186, 0.012297466397285461, 0.91571390628814697, 0.40164268016815186, 0.012297466397285461, 0.91571390628814697, 0.40164268016815186, 0.012297466397285461, 0.32029274106025696, 0.034719143062829971, 0.94668221473693848, 0.32029274106025696, 0.034719143062829971, 0.94668221473693848, 0.32029274106025696, 0.034719143062829971, 0.94668221473693848, 0.32029274106025696, 0.034719143062829971, 0.94668221473693848, 0.92220377922058105, 0.04846760630607605, 0.38365483283996582, 0.92220377922058105, 0.04846760630607605, 0.38365483283996582, 0.92220377922058105, 0.04846760630607605, 0.38365483283996582, 0.92220377922058105, 0.04846760630607605, 0.38365483283996582, 0.10523033887147903, -0.62897664308547974, 0.77026945352554321, 0.37904691696166992, -0.81703472137451172, 0.43448558449745178, 0.37904691696166992, -0.81703472137451172, 0.43448558449745178, 0.10523033887147903, -0.62897664308547974, 0.77026945352554321, -0.34349119663238525, 0.84438222646713257, -0.41113555431365967, -0.37296739220619202, 0.85759305953979492, -0.35416024923324585, -0.37296739220619202, 0.85759305953979492, -0.35416024923324585, -0.34349119663238525, 0.84438222646713257, -0.41113555431365967, -0.3534221351146698, 0.035504896193742752, 0.93478983640670776, -0.3534221351146698, 0.035504896193742752, 0.93478983640670776, -0.3534221351146698, 0.035504896193742752, 0.93478983640670776, -0.3534221351146698, 0.035504896193742752, 0.93478983640670776, 0.37904691696166992, -0.81703472137451172, 0.43448558449745178, 0.56830114126205444, -0.8228188157081604, 0.0017713651759549975, 0.56830114126205444, -0.8228188157081604, 0.0017713651759549975, 0.37904691696166992, -0.81703472137451172, 0.43448558449745178, 0.46657648682594299, -0.018018433824181557, -0.88429731130599976, 0.46657648682594299, -0.018018433824181557, -0.88429731130599976, 0.46657648682594299, -0.018018433824181557, -0.88429731130599976, 0.46657648682594299, -0.018018433824181557, -0.88429731130599976, -0.31254163384437561, 0.82754939794540405, -0.46634724736213684, -0.34349119663238525, 0.84438222646713257, -0.41113555431365967, -0.34349119663238525, 0.84438222646713257, -0.41113555431365967, -0.31254163384437561, 0.82754939794540405, -0.46634724736213684, -0.92372894287109375, 0.017463112249970436, 0.38264837861061096, -0.92372810840606689, 0.017467081546783447, 0.38265028595924377, -0.92366701364517212, 0.020475834608078003, 0.38264864683151245, -0.92366701364517212, 0.020475834608078003, 0.38264864683151245, -0.91949164867401123, 0.2609807550907135, 0.29397988319396973, -0.91949164867401123, 0.2609807550907135, 0.29397988319396973, -0.91949164867401123, 0.2609807550907135, 0.29397988319396973, -0.91949164867401123, 0.2609807550907135, 0.29397988319396973, -0.9237634539604187, 0.015813007950782776, 0.38263696432113647, -0.92376267910003662, 0.015816977247595787, 0.38263890147209167, -0.92376011610031128, 0.015958415344357491, 0.38263911008834839, -0.92376136779785156, 0.015952460467815399, 0.38263624906539917, -0.92376267910003662, 0.015816977247595787, 0.38263890147209167, -0.92372810840606689, 0.017467081546783447, 0.38265028595924377, -0.92375552654266357, 0.015962645411491394, 0.38264983892440796, -0.92376011610031128, 0.015958415344357491, 0.38263911008834839, -0.92372810840606689, 0.017467081546783447, 0.38265028595924377, -0.92372894287109375, 0.017463112249970436, 0.38264837861061096, -0.92375683784484863, 0.015956692397594452, 0.38264703750610352, -0.92375552654266357, 0.015962645411491394, 0.38264983892440796, -0.92372894287109375, 0.017463112249970436, 0.38264837861061096, -0.9237634539604187, 0.015813007950782776, 0.38263696432113647, -0.92376136779785156, 0.015952460467815399, 0.38263624906539917, -0.92375683784484863, 0.015956692397594452, 0.38264703750610352, -0.34947618842124939, -0.93164533376693726, 0.099515542387962341, -0.34947618842124939, -0.93164533376693726, 0.099515542387962341, -0.34947618842124939, -0.93164533376693726, 0.099515542387962341, -0.34947618842124939, -0.93164533376693726, 0.099515542387962341, 0.29059088230133057, -0.0095213772729039192, 0.95680004358291626, 0.29059088230133057, -0.0095213772729039192, 0.95680004358291626, 0.29059088230133057, -0.0095213772729039192, 0.95680004358291626, 0.29059088230133057, -0.0095213772729039192, 0.95680004358291626, 0.32658699154853821, 0.94224631786346436, -0.074247971177101135, 0.32658699154853821, 0.94224631786346436, -0.074247971177101135, 0.32658699154853821, 0.94224631786346436, -0.074247971177101135, 0.32658699154853821, 0.94224631786346436, -0.074247971177101135, -0.31816413998603821, 0.023504309356212616, -0.94774425029754639, -0.31816413998603821, 0.023504309356212616, -0.94774425029754639, -0.31816413998603821, 0.023504309356212616, -0.94774425029754639, -0.31816413998603821, 0.023504309356212616, -0.94774425029754639]";
createNode transform -n "m_tree_leaves1" -p "m_tree1";
createNode mesh -n "m_tree_leaves1Shape" -p "m_tree_leaves1";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "string";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	addAttr -ci true -sn "rgn" -ln "rgn" -dt "string";
	addAttr -ci true -sn "rguv0" -ln "rguv0" -dt "string";
	setAttr -k off ".v";
	setAttr -s 2 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".pv" -type "double2" 0 1 ;
	setAttr ".uvst[0].uvsn" -type "string" "map";
	setAttr ".cuvs" -type "string" "map";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".rgvtx" -type "string" "[-1.2661492824554443, 8.828582763671875, -0.016122125089168549, 2.4830615520477295, 7.4345169067382812, -0.016122125089168549, 0.12791615724563599, 12.577793121337891, -0.016122125089168549, 3.877126932144165, 11.183727264404297, -0.016122125089168549, -4.7405519485473633, 4.902951717376709, 1.4746583700180054, -1.050886869430542, 3.8819429874420166, 0.31538605690002441, -3.7719805240631104, 8.7704095840454102, 1.1511765718460083, -0.082315415143966675, 7.7494006156921387, -0.0080957561731338501, -0.18807908892631531, 9.2982158660888672, 1.259941577911377, -0.18807908892631531, 7.6477060317993164, -2.3836581707000732, -0.18807908892631531, 12.941815376281738, -0.39056795835494995, -0.18807908892631531, 11.291305541992188, -4.0341677665710449, -0.43052667379379272, 6.5520987510681152, -0.096869438886642456, 1.466538667678833, 3.116818904876709, 0.67772477865219116, 2.6467387676239014, 8.5970211029052734, 1.4356861114501953, 4.5438041687011719, 5.1617417335510254, 2.2102804183959961, -0.23634658753871918, 1.0597437620162964, 3.5477938652038574, -0.23634658753871918, 1.5223590135574341, -0.42536437511444092, -0.23634658753871918, 5.0329022407531738, 4.0104093551635742, -0.23634658753871918, 5.4955172538757324, 0.037250876426696777, -0.19382977485656738, 3.5039188861846924, -4.0241317749023437, -0.21144622564315796, 3.2225441932678223, -0.034079179167747498, -0.2612469494342804, 7.4934639930725098, -3.7430906295776367, -0.27886340022087097, 7.2120895385742187, 0.24696193635463715, -0.61697280406951904, 7.1213235855102539, 1.9198253154754639, 0.33899369835853577, 9.2120885848999023, -1.3535261154174805, -0.71515190601348877, 10.504292488098145, 4.0519328117370605, 0.24081464111804962, 12.595057487487793, 0.77858138084411621, -2.5770981311798096, 6.7635836601257324, -0.016122125089168549, 1.1506726741790771, 8.2140064239501953, -0.016122125089168549, -4.0275206565856934, 10.491353988647461, -0.016122125089168549, -0.29974979162216187, 11.941777229309082, -0.016122125089168549]";
	setAttr ".rgf" -type "string" "[[0,1,3,2],[4,5,7,6],[8,9,11,10],[12,13,15,14],[16,17,19,18],[20,21,23,22],[24,25,27,26],[28,29,31,30]]";
	setAttr ".rgn" -type "string" "[-2.3283064365386963e-010, 0, 1, -2.3283064365386963e-010, 0, 1, -2.3283064365386963e-010, 0, 1, -2.3283064365386963e-010, 0, 1, 0.30085718631744385, 0.0044188355095684528, 0.95365893840789795, 0.30085718631744385, 0.0044188355095684528, 0.95365893840789795, 0.30085718631744385, 0.0044188355095684528, 0.95365893840789795, 0.30085718631744385, 0.0044188355095684528, 0.95365893840789795, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, -0.42804640531539917, -0.032732874155044556, 0.90316379070281982, -0.42804640531539917, -0.032732874155044556, 0.90316379070281982, -0.42804640531539917, -0.032732874155044556, 0.90316379070281982, -0.42804640531539917, -0.032732874155044556, 0.90316379070281982, 1, -1.3969838619232178e-009, 0, 1, -1.3969838619232178e-009, 0, 1, -1.3969838619232178e-009, 0, 1, -1.3969838619232178e-009, 0, -0.99984830617904663, -0.016502944752573967, -0.0055781938135623932, -0.99984830617904663, -0.016502944752573967, -0.0055781938135623932, -0.99984830617904663, -0.016502944752573967, -0.0055781938135623932, -0.99984830617904663, -0.016502944752573967, -0.0055781938135623932, 0.97071141004562378, -0.10730306059122086, 0.21495462954044342, 0.97071141004562378, -0.10730306059122086, 0.21495462954044342, 0.97071141004562378, -0.10730306059122086, 0.21495462954044342, 0.97071141004562378, -0.10730306059122086, 0.21495462954044342, 2.3283061589829401e-010, 0, 1, 2.3283061589829401e-010, 0, 1, 2.3283061589829401e-010, 0, 1, 2.3283061589829401e-010, 0, 1]";
	setAttr ".rguv0" -type "string" "[0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0]";
createNode transform -n "phys_tree1" -p "m_tree1";
	setAttr ".t" -type "double3" 0 5 0 ;
	setAttr ".rp" -type "double3" 0 -5 0 ;
	setAttr ".sp" -type "double3" 0 -5 0 ;
createNode mesh -n "phys_tree1Shape" -p "phys_tree1";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "i_leaves1";
	setAttr ".t" -type "double3" 1.3054887926197094 10.006155378162163 -0.016122125493037265 ;
	setAttr ".r" -type "double3" 90 -1.5902773407317584e-015 -20.39659650607976 ;
createNode transform -n "transform8" -p "i_leaves1";
	setAttr ".v" no;
createNode mesh -n "i_leavesShape3" -p "transform8";
	setAttr -s 8 ".wm";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 8 ".iog";
	setAttr -s 2 ".iog[0].og";
	setAttr -s 2 ".iog[1].og";
	setAttr -s 2 ".iog[2].og";
	setAttr -s 2 ".iog[3].og";
	setAttr -s 2 ".iog[4].og";
	setAttr -s 2 ".iog[5].og";
	setAttr -s 2 ".iog[6].og";
	setAttr -s 2 ".iog[7].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr -s 8 ".ciog";
	setAttr ".uvst[0].uvsn" -type "string" "map";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "i_leaves3";
	setAttr ".t" -type "double3" -2.4114338093465624 6.3261761521490545 0.73328130248528067 ;
	setAttr ".r" -type "double3" 85.152903877337224 16.84706494905782 -15.467896893106243 ;
createNode transform -n "transform7" -p "i_leaves3";
	setAttr ".v" no;
createNode transform -n "i_leaves4";
	setAttr ".t" -type "double3" -0.18807909176414173 10.29476028756779 -1.3871130586249669 ;
	setAttr ".r" -type "double3" 0 65.630010901041317 -89.999999999998948 ;
createNode transform -n "transform6" -p "i_leaves4";
	setAttr ".v" no;
createNode transform -n "i_leaves5";
	setAttr ".t" -type "double3" 2.056638598957083 5.8569200556198782 1.0567054667714408 ;
	setAttr ".r" -type "double3" 112.98756121713578 -11.165786713134686 -61.091194248168456 ;
createNode transform -n "transform5" -p "i_leaves5";
	setAttr ".v" no;
createNode transform -n "i_leaves6";
	setAttr ".t" -type "double3" -0.23634659256239532 3.2776306050325212 1.7925224050538451 ;
	setAttr ".r" -type "double3" 2.5444437451708134e-014 96.641337360473926 -89.999999999998579 ;
createNode transform -n "transform4" -p "i_leaves6";
	setAttr ".v" no;
createNode transform -n "i_leaves7";
	setAttr ".t" -type "double3" -0.23634659256239532 5.3580041552687518 -1.8885848119542834 ;
	setAttr ".r" -type "double3" 184.53937855553684 -85.958364256886611 -93.582529945162861 ;
createNode transform -n "transform3" -p "i_leaves7";
	setAttr ".v" no;
createNode transform -n "i_leaves8";
	setAttr ".t" -type "double3" -0.18807909176414173 9.8581904591964236 1.3492033823974139 ;
	setAttr ".r" -type "double3" -201.96282536217731 54.918751082678959 -294.57143811041493 ;
createNode transform -n "transform2" -p "i_leaves8";
	setAttr ".v" no;
createNode transform -n "i_leaves9";
	setAttr ".t" -type "double3" -1.4384240461927598 9.3526798025030722 -0.016122125493037265 ;
	setAttr ".r" -type "double3" 90 -1.5902773407317584e-015 21.260301363457202 ;
createNode transform -n "transform1" -p "i_leaves9";
	setAttr ".v" no;
parent -s -nc -r -add "|i_leaves1|transform8|i_leavesShape3" "transform1";
parent -s -nc -r -add "|i_leaves1|transform8|i_leavesShape3" "transform2";
parent -s -nc -r -add "|i_leaves1|transform8|i_leavesShape3" "transform3";
parent -s -nc -r -add "|i_leaves1|transform8|i_leavesShape3" "transform4";
parent -s -nc -r -add "|i_leaves1|transform8|i_leavesShape3" "transform5";
parent -s -nc -r -add "|i_leaves1|transform8|i_leavesShape3" "transform6";
parent -s -nc -r -add "|i_leaves1|transform8|i_leavesShape3" "transform7";
createNode lightLinker -n "lightLinker1";
	setAttr -s 5 ".lnk";
	setAttr -s 5 ".slnk";
createNode displayLayerManager -n "layerManager";
createNode displayLayer -n "defaultLayer";
createNode renderLayerManager -n "renderLayerManager";
createNode renderLayer -n "defaultRenderLayer";
	setAttr ".g" yes;
createNode phong -n "mat_tree";
	setAttr ".c" -type "float3" 0.308 0.14098617 0.034803994 ;
	setAttr ".sc" -type "float3" 0.16528 0.16528 0.16528 ;
	setAttr ".cp" 5.2399997711181641;
createNode shadingEngine -n "phong1SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo1";
createNode groupId -n "groupId2";
	setAttr ".ihi" 0;
createNode phong -n "mat_leaves";
	setAttr ".sc" -type "float3" 0.04958 0.04958 0.04958 ;
	setAttr ".cp" 11.722000122070313;
createNode shadingEngine -n "phong2SG";
	setAttr ".ihi" 0;
	setAttr -s 17 ".dsm";
	setAttr ".ro" yes;
	setAttr -s 16 ".gn";
createNode materialInfo -n "materialInfo2";
createNode file -n "file1";
	setAttr ".ftn" -type "string" "C:/RnD/RG/trunk/Data//leaves.png";
createNode place2dTexture -n "place2dTexture1";
createNode script -n "uiConfigurationScriptNode";
	setAttr ".b" -type "string" (
		"// Maya Mel UI Configuration File.\n//\n//  This script is machine generated.  Edit at your own risk.\n//\n//\n\nglobal string $gMainPane;\nif (`paneLayout -exists $gMainPane`) {\n\n\tglobal int $gUseScenePanelConfig;\n\tint    $useSceneConfig = $gUseScenePanelConfig;\n\tint    $menusOkayInPanels = `optionVar -q allowMenusInPanels`;\tint    $nVisPanes = `paneLayout -q -nvp $gMainPane`;\n\tint    $nPanes = 0;\n\tstring $editorName;\n\tstring $panelName;\n\tstring $itemFilterName;\n\tstring $panelConfig;\n\n\t//\n\t//  get current state of the UI\n\t//\n\tsceneUIReplacement -update $gMainPane;\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"modelPanel\" (localizedPanelLabel(\"Top View\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `modelPanel -unParent -l (localizedPanelLabel(\"Top View\")) -mbv $menusOkayInPanels `;\n\t\t\t$editorName = $panelName;\n            modelEditor -e \n                -camera \"top\" \n                -useInteractiveMode 0\n                -displayLights \"default\" \n                -displayAppearance \"wireframe\" \n"
		+ "                -activeOnly 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n                -activeComponentsXray 0\n                -displayTextures 0\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n                -rendererName \"base_OpenGL_Renderer\" \n                -colorResolution 256 256 \n"
		+ "                -bumpResolution 512 512 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n                -lowQualityLighting 0\n                -maximumNumHardwareLights 0\n                -occlusionCulling 0\n                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n"
		+ "                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -dimensions 1\n                -handles 1\n                -pivots 1\n                -textures 1\n                -strokes 1\n                -shadows 0\n                $editorName;\nmodelEditor -e -viewSelected 0 $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tmodelPanel -edit -l (localizedPanelLabel(\"Top View\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        modelEditor -e \n            -camera \"top\" \n            -useInteractiveMode 0\n            -displayLights \"default\" \n            -displayAppearance \"wireframe\" \n            -activeOnly 0\n            -wireframeOnShaded 0\n            -headsUpDisplay 1\n            -selectionHiliteDisplay 1\n            -useDefaultMaterial 0\n            -bufferMode \"double\" \n"
		+ "            -twoSidedLighting 1\n            -backfaceCulling 0\n            -xray 0\n            -jointXray 0\n            -activeComponentsXray 0\n            -displayTextures 0\n            -smoothWireframe 0\n            -lineWidth 1\n            -textureAnisotropic 0\n            -textureHilight 1\n            -textureSampling 2\n            -textureDisplay \"modulate\" \n            -textureMaxSize 8192\n            -fogging 0\n            -fogSource \"fragment\" \n            -fogMode \"linear\" \n            -fogStart 0\n            -fogEnd 100\n            -fogDensity 0.1\n            -fogColor 0.5 0.5 0.5 1 \n            -maxConstantTransparency 1\n            -rendererName \"base_OpenGL_Renderer\" \n            -colorResolution 256 256 \n            -bumpResolution 512 512 \n            -textureCompression 0\n            -transparencyAlgorithm \"frontAndBackCull\" \n            -transpInShadows 0\n            -cullingOverride \"none\" \n            -lowQualityLighting 0\n            -maximumNumHardwareLights 0\n            -occlusionCulling 0\n"
		+ "            -shadingModel 0\n            -useBaseRenderer 0\n            -useReducedRenderer 0\n            -smallObjectCulling 0\n            -smallObjectThreshold -1 \n            -interactiveDisableShadows 0\n            -interactiveBackFaceCull 0\n            -sortTransparent 1\n            -nurbsCurves 1\n            -nurbsSurfaces 1\n            -polymeshes 1\n            -subdivSurfaces 1\n            -planes 1\n            -lights 1\n            -cameras 1\n            -controlVertices 1\n            -hulls 1\n            -grid 1\n            -joints 1\n            -ikHandles 1\n            -deformers 1\n            -dynamics 1\n            -fluids 1\n            -hairSystems 1\n            -follicles 1\n            -nCloths 1\n            -nParticles 1\n            -nRigids 1\n            -dynamicConstraints 1\n            -locators 1\n            -manipulators 1\n            -dimensions 1\n            -handles 1\n            -pivots 1\n            -textures 1\n            -strokes 1\n            -shadows 0\n            $editorName;\nmodelEditor -e -viewSelected 0 $editorName;\n"
		+ "\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"modelPanel\" (localizedPanelLabel(\"Side View\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `modelPanel -unParent -l (localizedPanelLabel(\"Side View\")) -mbv $menusOkayInPanels `;\n\t\t\t$editorName = $panelName;\n            modelEditor -e \n                -camera \"side\" \n                -useInteractiveMode 0\n                -displayLights \"default\" \n                -displayAppearance \"wireframe\" \n                -activeOnly 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n                -activeComponentsXray 0\n                -displayTextures 0\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n"
		+ "                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n                -rendererName \"base_OpenGL_Renderer\" \n                -colorResolution 256 256 \n                -bumpResolution 512 512 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n                -lowQualityLighting 0\n                -maximumNumHardwareLights 0\n                -occlusionCulling 0\n                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n"
		+ "                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -dimensions 1\n                -handles 1\n                -pivots 1\n                -textures 1\n                -strokes 1\n                -shadows 0\n                $editorName;\nmodelEditor -e -viewSelected 0 $editorName;\n"
		+ "\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tmodelPanel -edit -l (localizedPanelLabel(\"Side View\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        modelEditor -e \n            -camera \"side\" \n            -useInteractiveMode 0\n            -displayLights \"default\" \n            -displayAppearance \"wireframe\" \n            -activeOnly 0\n            -wireframeOnShaded 0\n            -headsUpDisplay 1\n            -selectionHiliteDisplay 1\n            -useDefaultMaterial 0\n            -bufferMode \"double\" \n            -twoSidedLighting 1\n            -backfaceCulling 0\n            -xray 0\n            -jointXray 0\n            -activeComponentsXray 0\n            -displayTextures 0\n            -smoothWireframe 0\n            -lineWidth 1\n            -textureAnisotropic 0\n            -textureHilight 1\n            -textureSampling 2\n            -textureDisplay \"modulate\" \n            -textureMaxSize 8192\n            -fogging 0\n            -fogSource \"fragment\" \n            -fogMode \"linear\" \n"
		+ "            -fogStart 0\n            -fogEnd 100\n            -fogDensity 0.1\n            -fogColor 0.5 0.5 0.5 1 \n            -maxConstantTransparency 1\n            -rendererName \"base_OpenGL_Renderer\" \n            -colorResolution 256 256 \n            -bumpResolution 512 512 \n            -textureCompression 0\n            -transparencyAlgorithm \"frontAndBackCull\" \n            -transpInShadows 0\n            -cullingOverride \"none\" \n            -lowQualityLighting 0\n            -maximumNumHardwareLights 0\n            -occlusionCulling 0\n            -shadingModel 0\n            -useBaseRenderer 0\n            -useReducedRenderer 0\n            -smallObjectCulling 0\n            -smallObjectThreshold -1 \n            -interactiveDisableShadows 0\n            -interactiveBackFaceCull 0\n            -sortTransparent 1\n            -nurbsCurves 1\n            -nurbsSurfaces 1\n            -polymeshes 1\n            -subdivSurfaces 1\n            -planes 1\n            -lights 1\n            -cameras 1\n            -controlVertices 1\n"
		+ "            -hulls 1\n            -grid 1\n            -joints 1\n            -ikHandles 1\n            -deformers 1\n            -dynamics 1\n            -fluids 1\n            -hairSystems 1\n            -follicles 1\n            -nCloths 1\n            -nParticles 1\n            -nRigids 1\n            -dynamicConstraints 1\n            -locators 1\n            -manipulators 1\n            -dimensions 1\n            -handles 1\n            -pivots 1\n            -textures 1\n            -strokes 1\n            -shadows 0\n            $editorName;\nmodelEditor -e -viewSelected 0 $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"modelPanel\" (localizedPanelLabel(\"Front View\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `modelPanel -unParent -l (localizedPanelLabel(\"Front View\")) -mbv $menusOkayInPanels `;\n\t\t\t$editorName = $panelName;\n            modelEditor -e \n                -camera \"front\" \n                -useInteractiveMode 0\n"
		+ "                -displayLights \"default\" \n                -displayAppearance \"wireframe\" \n                -activeOnly 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n                -activeComponentsXray 0\n                -displayTextures 0\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n"
		+ "                -rendererName \"base_OpenGL_Renderer\" \n                -colorResolution 256 256 \n                -bumpResolution 512 512 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n                -lowQualityLighting 0\n                -maximumNumHardwareLights 0\n                -occlusionCulling 0\n                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n"
		+ "                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -dimensions 1\n                -handles 1\n                -pivots 1\n                -textures 1\n                -strokes 1\n                -shadows 0\n                $editorName;\nmodelEditor -e -viewSelected 0 $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tmodelPanel -edit -l (localizedPanelLabel(\"Front View\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        modelEditor -e \n            -camera \"front\" \n            -useInteractiveMode 0\n            -displayLights \"default\" \n            -displayAppearance \"wireframe\" \n            -activeOnly 0\n            -wireframeOnShaded 0\n            -headsUpDisplay 1\n"
		+ "            -selectionHiliteDisplay 1\n            -useDefaultMaterial 0\n            -bufferMode \"double\" \n            -twoSidedLighting 1\n            -backfaceCulling 0\n            -xray 0\n            -jointXray 0\n            -activeComponentsXray 0\n            -displayTextures 0\n            -smoothWireframe 0\n            -lineWidth 1\n            -textureAnisotropic 0\n            -textureHilight 1\n            -textureSampling 2\n            -textureDisplay \"modulate\" \n            -textureMaxSize 8192\n            -fogging 0\n            -fogSource \"fragment\" \n            -fogMode \"linear\" \n            -fogStart 0\n            -fogEnd 100\n            -fogDensity 0.1\n            -fogColor 0.5 0.5 0.5 1 \n            -maxConstantTransparency 1\n            -rendererName \"base_OpenGL_Renderer\" \n            -colorResolution 256 256 \n            -bumpResolution 512 512 \n            -textureCompression 0\n            -transparencyAlgorithm \"frontAndBackCull\" \n            -transpInShadows 0\n            -cullingOverride \"none\" \n"
		+ "            -lowQualityLighting 0\n            -maximumNumHardwareLights 0\n            -occlusionCulling 0\n            -shadingModel 0\n            -useBaseRenderer 0\n            -useReducedRenderer 0\n            -smallObjectCulling 0\n            -smallObjectThreshold -1 \n            -interactiveDisableShadows 0\n            -interactiveBackFaceCull 0\n            -sortTransparent 1\n            -nurbsCurves 1\n            -nurbsSurfaces 1\n            -polymeshes 1\n            -subdivSurfaces 1\n            -planes 1\n            -lights 1\n            -cameras 1\n            -controlVertices 1\n            -hulls 1\n            -grid 1\n            -joints 1\n            -ikHandles 1\n            -deformers 1\n            -dynamics 1\n            -fluids 1\n            -hairSystems 1\n            -follicles 1\n            -nCloths 1\n            -nParticles 1\n            -nRigids 1\n            -dynamicConstraints 1\n            -locators 1\n            -manipulators 1\n            -dimensions 1\n            -handles 1\n            -pivots 1\n"
		+ "            -textures 1\n            -strokes 1\n            -shadows 0\n            $editorName;\nmodelEditor -e -viewSelected 0 $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"modelPanel\" (localizedPanelLabel(\"Persp View\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `modelPanel -unParent -l (localizedPanelLabel(\"Persp View\")) -mbv $menusOkayInPanels `;\n\t\t\t$editorName = $panelName;\n            modelEditor -e \n                -camera \"persp\" \n                -useInteractiveMode 0\n                -displayLights \"default\" \n                -displayAppearance \"smoothShaded\" \n                -activeOnly 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n"
		+ "                -activeComponentsXray 0\n                -displayTextures 1\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n                -rendererName \"base_OpenGL_Renderer\" \n                -colorResolution 256 256 \n                -bumpResolution 512 512 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n                -lowQualityLighting 0\n                -maximumNumHardwareLights 1\n                -occlusionCulling 0\n"
		+ "                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -dimensions 1\n                -handles 1\n"
		+ "                -pivots 1\n                -textures 1\n                -strokes 1\n                -shadows 0\n                $editorName;\nmodelEditor -e -viewSelected 0 $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tmodelPanel -edit -l (localizedPanelLabel(\"Persp View\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        modelEditor -e \n            -camera \"persp\" \n            -useInteractiveMode 0\n            -displayLights \"default\" \n            -displayAppearance \"smoothShaded\" \n            -activeOnly 0\n            -wireframeOnShaded 0\n            -headsUpDisplay 1\n            -selectionHiliteDisplay 1\n            -useDefaultMaterial 0\n            -bufferMode \"double\" \n            -twoSidedLighting 1\n            -backfaceCulling 0\n            -xray 0\n            -jointXray 0\n            -activeComponentsXray 0\n            -displayTextures 1\n            -smoothWireframe 0\n            -lineWidth 1\n            -textureAnisotropic 0\n            -textureHilight 1\n            -textureSampling 2\n"
		+ "            -textureDisplay \"modulate\" \n            -textureMaxSize 8192\n            -fogging 0\n            -fogSource \"fragment\" \n            -fogMode \"linear\" \n            -fogStart 0\n            -fogEnd 100\n            -fogDensity 0.1\n            -fogColor 0.5 0.5 0.5 1 \n            -maxConstantTransparency 1\n            -rendererName \"base_OpenGL_Renderer\" \n            -colorResolution 256 256 \n            -bumpResolution 512 512 \n            -textureCompression 0\n            -transparencyAlgorithm \"frontAndBackCull\" \n            -transpInShadows 0\n            -cullingOverride \"none\" \n            -lowQualityLighting 0\n            -maximumNumHardwareLights 1\n            -occlusionCulling 0\n            -shadingModel 0\n            -useBaseRenderer 0\n            -useReducedRenderer 0\n            -smallObjectCulling 0\n            -smallObjectThreshold -1 \n            -interactiveDisableShadows 0\n            -interactiveBackFaceCull 0\n            -sortTransparent 1\n            -nurbsCurves 1\n            -nurbsSurfaces 1\n"
		+ "            -polymeshes 1\n            -subdivSurfaces 1\n            -planes 1\n            -lights 1\n            -cameras 1\n            -controlVertices 1\n            -hulls 1\n            -grid 1\n            -joints 1\n            -ikHandles 1\n            -deformers 1\n            -dynamics 1\n            -fluids 1\n            -hairSystems 1\n            -follicles 1\n            -nCloths 1\n            -nParticles 1\n            -nRigids 1\n            -dynamicConstraints 1\n            -locators 1\n            -manipulators 1\n            -dimensions 1\n            -handles 1\n            -pivots 1\n            -textures 1\n            -strokes 1\n            -shadows 0\n            $editorName;\nmodelEditor -e -viewSelected 0 $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"outlinerPanel\" (localizedPanelLabel(\"Outliner\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `outlinerPanel -unParent -l (localizedPanelLabel(\"Outliner\")) -mbv $menusOkayInPanels `;\n"
		+ "\t\t\t$editorName = $panelName;\n            outlinerEditor -e \n                -showShapes 0\n                -showAttributes 0\n                -showConnected 0\n                -showAnimCurvesOnly 0\n                -showMuteInfo 0\n                -organizeByLayer 1\n                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n                -autoExpand 0\n                -showDagOnly 1\n                -showAssets 1\n                -showContainedOnly 1\n                -showPublishedAsConnected 0\n                -showContainerContents 1\n                -ignoreDagHierarchy 0\n                -expandConnections 0\n                -showUnitlessCurves 1\n                -showCompounds 1\n                -showLeafs 1\n                -showNumericAttrsOnly 0\n                -highlightActive 1\n                -autoSelectNewObjects 0\n                -doNotSelectNewObjects 0\n                -dropIsParent 1\n                -transmitFilters 0\n                -setFilter \"defaultSetFilter\" \n                -showSetMembers 1\n"
		+ "                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -displayMode \"DAG\" \n                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n                -showAttrValues 0\n                -highlightSecondary 0\n                -showUVAttrsOnly 0\n                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\toutlinerPanel -edit -l (localizedPanelLabel(\"Outliner\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        outlinerEditor -e \n            -showShapes 0\n            -showAttributes 0\n            -showConnected 0\n            -showAnimCurvesOnly 0\n            -showMuteInfo 0\n"
		+ "            -organizeByLayer 1\n            -showAnimLayerWeight 1\n            -autoExpandLayers 1\n            -autoExpand 0\n            -showDagOnly 1\n            -showAssets 1\n            -showContainedOnly 1\n            -showPublishedAsConnected 0\n            -showContainerContents 1\n            -ignoreDagHierarchy 0\n            -expandConnections 0\n            -showUnitlessCurves 1\n            -showCompounds 1\n            -showLeafs 1\n            -showNumericAttrsOnly 0\n            -highlightActive 1\n            -autoSelectNewObjects 0\n            -doNotSelectNewObjects 0\n            -dropIsParent 1\n            -transmitFilters 0\n            -setFilter \"defaultSetFilter\" \n            -showSetMembers 1\n            -allowMultiSelection 1\n            -alwaysToggleSelect 0\n            -directSelect 0\n            -displayMode \"DAG\" \n            -expandObjects 0\n            -setsIgnoreFilters 1\n            -containersIgnoreFilters 0\n            -editAttrName 0\n            -showAttrValues 0\n            -highlightSecondary 0\n"
		+ "            -showUVAttrsOnly 0\n            -showTextureNodesOnly 0\n            -attrAlphaOrder \"default\" \n            -animLayerFilterOptions \"allAffecting\" \n            -sortOrder \"none\" \n            -longNames 0\n            -niceNames 1\n            -showNamespace 1\n            $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"graphEditor\" (localizedPanelLabel(\"Graph Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"graphEditor\" -l (localizedPanelLabel(\"Graph Editor\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = ($panelName+\"OutlineEd\");\n            outlinerEditor -e \n                -showShapes 1\n                -showAttributes 1\n                -showConnected 1\n                -showAnimCurvesOnly 1\n                -showMuteInfo 0\n                -organizeByLayer 1\n                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n                -autoExpand 1\n"
		+ "                -showDagOnly 0\n                -showAssets 1\n                -showContainedOnly 0\n                -showPublishedAsConnected 0\n                -showContainerContents 0\n                -ignoreDagHierarchy 0\n                -expandConnections 1\n                -showUnitlessCurves 1\n                -showCompounds 0\n                -showLeafs 1\n                -showNumericAttrsOnly 1\n                -highlightActive 0\n                -autoSelectNewObjects 1\n                -doNotSelectNewObjects 0\n                -dropIsParent 1\n                -transmitFilters 1\n                -setFilter \"0\" \n                -showSetMembers 0\n                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -displayMode \"DAG\" \n                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n                -showAttrValues 0\n                -highlightSecondary 0\n                -showUVAttrsOnly 0\n"
		+ "                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                $editorName;\n\n\t\t\t$editorName = ($panelName+\"GraphEd\");\n            animCurveEditor -e \n                -displayKeys 1\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 1\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"integer\" \n                -snapValue \"none\" \n                -showResults \"off\" \n                -showBufferCurves \"off\" \n                -smoothness \"fine\" \n                -resultSamples 1\n                -resultScreenSamples 0\n                -resultUpdate \"delayed\" \n                -clipTime \"on\" \n                -constrainDrag 0\n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n"
		+ "\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Graph Editor\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = ($panelName+\"OutlineEd\");\n            outlinerEditor -e \n                -showShapes 1\n                -showAttributes 1\n                -showConnected 1\n                -showAnimCurvesOnly 1\n                -showMuteInfo 0\n                -organizeByLayer 1\n                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n                -autoExpand 1\n                -showDagOnly 0\n                -showAssets 1\n                -showContainedOnly 0\n                -showPublishedAsConnected 0\n                -showContainerContents 0\n                -ignoreDagHierarchy 0\n                -expandConnections 1\n                -showUnitlessCurves 1\n                -showCompounds 0\n                -showLeafs 1\n                -showNumericAttrsOnly 1\n                -highlightActive 0\n                -autoSelectNewObjects 1\n                -doNotSelectNewObjects 0\n                -dropIsParent 1\n"
		+ "                -transmitFilters 1\n                -setFilter \"0\" \n                -showSetMembers 0\n                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -displayMode \"DAG\" \n                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n                -showAttrValues 0\n                -highlightSecondary 0\n                -showUVAttrsOnly 0\n                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                $editorName;\n\n\t\t\t$editorName = ($panelName+\"GraphEd\");\n            animCurveEditor -e \n                -displayKeys 1\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 1\n                -displayInfinities 0\n"
		+ "                -autoFit 0\n                -snapTime \"integer\" \n                -snapValue \"none\" \n                -showResults \"off\" \n                -showBufferCurves \"off\" \n                -smoothness \"fine\" \n                -resultSamples 1\n                -resultScreenSamples 0\n                -resultUpdate \"delayed\" \n                -clipTime \"on\" \n                -constrainDrag 0\n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"dopeSheetPanel\" (localizedPanelLabel(\"Dope Sheet\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"dopeSheetPanel\" -l (localizedPanelLabel(\"Dope Sheet\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = ($panelName+\"OutlineEd\");\n            outlinerEditor -e \n                -showShapes 1\n                -showAttributes 1\n                -showConnected 1\n                -showAnimCurvesOnly 1\n                -showMuteInfo 0\n                -organizeByLayer 1\n"
		+ "                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n                -autoExpand 0\n                -showDagOnly 0\n                -showAssets 1\n                -showContainedOnly 0\n                -showPublishedAsConnected 0\n                -showContainerContents 0\n                -ignoreDagHierarchy 0\n                -expandConnections 1\n                -showUnitlessCurves 0\n                -showCompounds 1\n                -showLeafs 1\n                -showNumericAttrsOnly 1\n                -highlightActive 0\n                -autoSelectNewObjects 0\n                -doNotSelectNewObjects 1\n                -dropIsParent 1\n                -transmitFilters 0\n                -setFilter \"0\" \n                -showSetMembers 0\n                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -displayMode \"DAG\" \n                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n"
		+ "                -showAttrValues 0\n                -highlightSecondary 0\n                -showUVAttrsOnly 0\n                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                $editorName;\n\n\t\t\t$editorName = ($panelName+\"DopeSheetEd\");\n            dopeSheetEditor -e \n                -displayKeys 1\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"integer\" \n                -snapValue \"none\" \n                -outliner \"dopeSheetPanel1OutlineEd\" \n                -showSummary 1\n                -showScene 0\n                -hierarchyBelow 0\n                -showTicks 1\n                -selectionWindow 0 0 0 0 \n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n"
		+ "\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Dope Sheet\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = ($panelName+\"OutlineEd\");\n            outlinerEditor -e \n                -showShapes 1\n                -showAttributes 1\n                -showConnected 1\n                -showAnimCurvesOnly 1\n                -showMuteInfo 0\n                -organizeByLayer 1\n                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n                -autoExpand 0\n                -showDagOnly 0\n                -showAssets 1\n                -showContainedOnly 0\n                -showPublishedAsConnected 0\n                -showContainerContents 0\n                -ignoreDagHierarchy 0\n                -expandConnections 1\n                -showUnitlessCurves 0\n                -showCompounds 1\n                -showLeafs 1\n                -showNumericAttrsOnly 1\n                -highlightActive 0\n                -autoSelectNewObjects 0\n                -doNotSelectNewObjects 1\n                -dropIsParent 1\n"
		+ "                -transmitFilters 0\n                -setFilter \"0\" \n                -showSetMembers 0\n                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -displayMode \"DAG\" \n                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n                -showAttrValues 0\n                -highlightSecondary 0\n                -showUVAttrsOnly 0\n                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                $editorName;\n\n\t\t\t$editorName = ($panelName+\"DopeSheetEd\");\n            dopeSheetEditor -e \n                -displayKeys 1\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n"
		+ "                -autoFit 0\n                -snapTime \"integer\" \n                -snapValue \"none\" \n                -outliner \"dopeSheetPanel1OutlineEd\" \n                -showSummary 1\n                -showScene 0\n                -hierarchyBelow 0\n                -showTicks 1\n                -selectionWindow 0 0 0 0 \n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"clipEditorPanel\" (localizedPanelLabel(\"Trax Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"clipEditorPanel\" -l (localizedPanelLabel(\"Trax Editor\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = clipEditorNameFromPanel($panelName);\n            clipEditor -e \n                -displayKeys 0\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"none\" \n"
		+ "                -snapValue \"none\" \n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Trax Editor\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = clipEditorNameFromPanel($panelName);\n            clipEditor -e \n                -displayKeys 0\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"none\" \n                -snapValue \"none\" \n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"hyperGraphPanel\" (localizedPanelLabel(\"Hypergraph Hierarchy\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"hyperGraphPanel\" -l (localizedPanelLabel(\"Hypergraph Hierarchy\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = ($panelName+\"HyperGraphEd\");\n"
		+ "            hyperGraph -e \n                -graphLayoutStyle \"hierarchicalLayout\" \n                -orientation \"horiz\" \n                -mergeConnections 1\n                -zoom 0.308664\n                -animateTransition 0\n                -showRelationships 1\n                -showShapes 0\n                -showDeformers 0\n                -showExpressions 0\n                -showConstraints 0\n                -showUnderworld 0\n                -showInvisible 0\n                -transitionFrames 5\n                -currentNode \"phys_tree1\" \n                -opaqueContainers 0\n                -dropTargetNode \"m_tree1\" \n                -dropNode \"m_tree_leaves1\" \n                -freeform 1\n                -imagePosition 0 0 \n                -imageScale 1\n                -imageEnabled 0\n                -graphType \"DAG\" \n                -heatMapDisplay 0\n                -updateSelection 1\n                -updateNodeAdded 1\n                -useDrawOverrideColor 0\n                -limitGraphTraversal -1\n                -range 0 0 \n"
		+ "                -iconSize \"largeIcons\" \n                -showCachedConnections 0\n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Hypergraph Hierarchy\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = ($panelName+\"HyperGraphEd\");\n            hyperGraph -e \n                -graphLayoutStyle \"hierarchicalLayout\" \n                -orientation \"horiz\" \n                -mergeConnections 1\n                -zoom 0.308664\n                -animateTransition 0\n                -showRelationships 1\n                -showShapes 0\n                -showDeformers 0\n                -showExpressions 0\n                -showConstraints 0\n                -showUnderworld 0\n                -showInvisible 0\n                -transitionFrames 5\n                -currentNode \"phys_tree1\" \n                -opaqueContainers 0\n                -dropTargetNode \"m_tree1\" \n                -dropNode \"m_tree_leaves1\" \n                -freeform 1\n                -imagePosition 0 0 \n"
		+ "                -imageScale 1\n                -imageEnabled 0\n                -graphType \"DAG\" \n                -heatMapDisplay 0\n                -updateSelection 1\n                -updateNodeAdded 1\n                -useDrawOverrideColor 0\n                -limitGraphTraversal -1\n                -range 0 0 \n                -iconSize \"largeIcons\" \n                -showCachedConnections 0\n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"hyperShadePanel\" (localizedPanelLabel(\"Hypershade\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"hyperShadePanel\" -l (localizedPanelLabel(\"Hypershade\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Hypershade\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"visorPanel\" (localizedPanelLabel(\"Visor\")) `;\n"
		+ "\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"visorPanel\" -l (localizedPanelLabel(\"Visor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Visor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"polyTexturePlacementPanel\" (localizedPanelLabel(\"UV Texture Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"polyTexturePlacementPanel\" -l (localizedPanelLabel(\"UV Texture Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"UV Texture Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"multiListerPanel\" (localizedPanelLabel(\"Multilister\")) `;\n"
		+ "\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"multiListerPanel\" -l (localizedPanelLabel(\"Multilister\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Multilister\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"renderWindowPanel\" (localizedPanelLabel(\"Render View\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"renderWindowPanel\" -l (localizedPanelLabel(\"Render View\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Render View\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"blendShapePanel\" (localizedPanelLabel(\"Blend Shape\")) `;\n"
		+ "\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\tblendShapePanel -unParent -l (localizedPanelLabel(\"Blend Shape\")) -mbv $menusOkayInPanels ;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tblendShapePanel -edit -l (localizedPanelLabel(\"Blend Shape\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"dynRelEdPanel\" (localizedPanelLabel(\"Dynamic Relationships\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"dynRelEdPanel\" -l (localizedPanelLabel(\"Dynamic Relationships\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Dynamic Relationships\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"devicePanel\" (localizedPanelLabel(\"Devices\")) `;\n\tif (\"\" == $panelName) {\n"
		+ "\t\tif ($useSceneConfig) {\n\t\t\tdevicePanel -unParent -l (localizedPanelLabel(\"Devices\")) -mbv $menusOkayInPanels ;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tdevicePanel -edit -l (localizedPanelLabel(\"Devices\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"relationshipPanel\" (localizedPanelLabel(\"Relationship Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"relationshipPanel\" -l (localizedPanelLabel(\"Relationship Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Relationship Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"referenceEditorPanel\" (localizedPanelLabel(\"Reference Editor\")) `;\n\tif (\"\" == $panelName) {\n"
		+ "\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"referenceEditorPanel\" -l (localizedPanelLabel(\"Reference Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Reference Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"componentEditorPanel\" (localizedPanelLabel(\"Component Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"componentEditorPanel\" -l (localizedPanelLabel(\"Component Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Component Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"dynPaintScriptedPanelType\" (localizedPanelLabel(\"Paint Effects\")) `;\n"
		+ "\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"dynPaintScriptedPanelType\" -l (localizedPanelLabel(\"Paint Effects\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Paint Effects\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"webBrowserPanel\" (localizedPanelLabel(\"Web Browser\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"webBrowserPanel\" -l (localizedPanelLabel(\"Web Browser\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Web Browser\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"scriptEditorPanel\" (localizedPanelLabel(\"Script Editor\")) `;\n"
		+ "\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"scriptEditorPanel\" -l (localizedPanelLabel(\"Script Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Script Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"Stereo\" (localizedPanelLabel(\"Stereo\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"Stereo\" -l (localizedPanelLabel(\"Stereo\")) -mbv $menusOkayInPanels `;\nstring $editorName = ($panelName+\"Editor\");\n            stereoCameraView -e \n                -camera \"persp\" \n                -useInteractiveMode 0\n                -displayLights \"default\" \n                -displayAppearance \"wireframe\" \n                -activeOnly 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n"
		+ "                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n                -activeComponentsXray 0\n                -displayTextures 0\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n                -colorResolution 4 4 \n                -bumpResolution 4 4 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n"
		+ "                -lowQualityLighting 0\n                -maximumNumHardwareLights 0\n                -occlusionCulling 0\n                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n"
		+ "                -locators 1\n                -manipulators 1\n                -dimensions 1\n                -handles 1\n                -pivots 1\n                -textures 1\n                -strokes 1\n                -shadows 0\n                -displayMode \"centerEye\" \n                -viewColor 0 0 0 1 \n                $editorName;\nstereoCameraView -e -viewSelected 0 $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Stereo\")) -mbv $menusOkayInPanels  $panelName;\nstring $editorName = ($panelName+\"Editor\");\n            stereoCameraView -e \n                -camera \"persp\" \n                -useInteractiveMode 0\n                -displayLights \"default\" \n                -displayAppearance \"wireframe\" \n                -activeOnly 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n"
		+ "                -xray 0\n                -jointXray 0\n                -activeComponentsXray 0\n                -displayTextures 0\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n                -colorResolution 4 4 \n                -bumpResolution 4 4 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n                -lowQualityLighting 0\n                -maximumNumHardwareLights 0\n                -occlusionCulling 0\n                -shadingModel 0\n"
		+ "                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -dimensions 1\n                -handles 1\n                -pivots 1\n"
		+ "                -textures 1\n                -strokes 1\n                -shadows 0\n                -displayMode \"centerEye\" \n                -viewColor 0 0 0 1 \n                $editorName;\nstereoCameraView -e -viewSelected 0 $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\tif ($useSceneConfig) {\n        string $configName = `getPanel -cwl (localizedPanelLabel(\"Current Layout\"))`;\n        if (\"\" != $configName) {\n\t\t\tpanelConfiguration -edit -label (localizedPanelLabel(\"Current Layout\")) \n\t\t\t\t-defaultImage \"\"\n\t\t\t\t-image \"\"\n\t\t\t\t-sc false\n\t\t\t\t-configString \"global string $gMainPane; paneLayout -e -cn \\\"horizontal2\\\" -ps 1 100 46 -ps 2 100 54 $gMainPane;\"\n\t\t\t\t-removeAllPanels\n\t\t\t\t-ap false\n\t\t\t\t\t(localizedPanelLabel(\"Persp View\")) \n\t\t\t\t\t\"modelPanel\"\n"
		+ "\t\t\t\t\t\"$panelName = `modelPanel -unParent -l (localizedPanelLabel(\\\"Persp View\\\")) -mbv $menusOkayInPanels `;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera persp` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"smoothShaded\\\" \\n    -activeOnly 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 1\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName\"\n"
		+ "\t\t\t\t\t\"modelPanel -edit -l (localizedPanelLabel(\\\"Persp View\\\")) -mbv $menusOkayInPanels  $panelName;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera persp` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"smoothShaded\\\" \\n    -activeOnly 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 1\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName\"\n"
		+ "\t\t\t\t-ap false\n\t\t\t\t\t(localizedPanelLabel(\"Hypergraph Hierarchy\")) \n\t\t\t\t\t\"scriptedPanel\"\n\t\t\t\t\t\"$panelName = `scriptedPanel -unParent  -type \\\"hyperGraphPanel\\\" -l (localizedPanelLabel(\\\"Hypergraph Hierarchy\\\")) -mbv $menusOkayInPanels `;\\n\\n\\t\\t\\t$editorName = ($panelName+\\\"HyperGraphEd\\\");\\n            hyperGraph -e \\n                -graphLayoutStyle \\\"hierarchicalLayout\\\" \\n                -orientation \\\"horiz\\\" \\n                -mergeConnections 1\\n                -zoom 0.308664\\n                -animateTransition 0\\n                -showRelationships 1\\n                -showShapes 0\\n                -showDeformers 0\\n                -showExpressions 0\\n                -showConstraints 0\\n                -showUnderworld 0\\n                -showInvisible 0\\n                -transitionFrames 5\\n                -currentNode \\\"phys_tree1\\\" \\n                -opaqueContainers 0\\n                -dropTargetNode \\\"m_tree1\\\" \\n                -dropNode \\\"m_tree_leaves1\\\" \\n                -freeform 1\\n                -imagePosition 0 0 \\n                -imageScale 1\\n                -imageEnabled 0\\n                -graphType \\\"DAG\\\" \\n                -heatMapDisplay 0\\n                -updateSelection 1\\n                -updateNodeAdded 1\\n                -useDrawOverrideColor 0\\n                -limitGraphTraversal -1\\n                -range 0 0 \\n                -iconSize \\\"largeIcons\\\" \\n                -showCachedConnections 0\\n                $editorName\"\n"
		+ "\t\t\t\t\t\"scriptedPanel -edit -l (localizedPanelLabel(\\\"Hypergraph Hierarchy\\\")) -mbv $menusOkayInPanels  $panelName;\\n\\n\\t\\t\\t$editorName = ($panelName+\\\"HyperGraphEd\\\");\\n            hyperGraph -e \\n                -graphLayoutStyle \\\"hierarchicalLayout\\\" \\n                -orientation \\\"horiz\\\" \\n                -mergeConnections 1\\n                -zoom 0.308664\\n                -animateTransition 0\\n                -showRelationships 1\\n                -showShapes 0\\n                -showDeformers 0\\n                -showExpressions 0\\n                -showConstraints 0\\n                -showUnderworld 0\\n                -showInvisible 0\\n                -transitionFrames 5\\n                -currentNode \\\"phys_tree1\\\" \\n                -opaqueContainers 0\\n                -dropTargetNode \\\"m_tree1\\\" \\n                -dropNode \\\"m_tree_leaves1\\\" \\n                -freeform 1\\n                -imagePosition 0 0 \\n                -imageScale 1\\n                -imageEnabled 0\\n                -graphType \\\"DAG\\\" \\n                -heatMapDisplay 0\\n                -updateSelection 1\\n                -updateNodeAdded 1\\n                -useDrawOverrideColor 0\\n                -limitGraphTraversal -1\\n                -range 0 0 \\n                -iconSize \\\"largeIcons\\\" \\n                -showCachedConnections 0\\n                $editorName\"\n"
		+ "\t\t\t\t$configName;\n\n            setNamedPanelLayout (localizedPanelLabel(\"Current Layout\"));\n        }\n\n        panelHistory -e -clear mainPanelHistory;\n        setFocus `paneLayout -q -p1 $gMainPane`;\n        sceneUIReplacement -deleteRemaining;\n        sceneUIReplacement -clear;\n\t}\n\n\ngrid -spacing 5 -size 12 -divisions 5 -displayAxes yes -displayGridLines yes -displayDivisionLines yes -displayPerspectiveLabels no -displayOrthographicLabels no -displayAxesBold yes -perspectiveLabelPosition axis -orthographicLabelPosition edge;\nviewManip -drawCompass 0 -compassAngle 0 -frontParameters \"\" -homeParameters \"\" -selectionLockParameters \"\";\n}\n");
	setAttr ".st" 3;
createNode script -n "sceneConfigurationScriptNode";
	setAttr ".b" -type "string" "playbackOptions -min 1 -max 24 -ast 1 -aet 48 ";
	setAttr ".st" 6;
createNode polyCylinder -n "polyCylinder2";
	setAttr ".r" 0.4;
	setAttr ".h" 10;
	setAttr ".sa" 8;
	setAttr ".sc" 1;
	setAttr ".cuv" 3;
createNode polyCut -n "polyCut18";
	setAttr ".uopa" yes;
	setAttr ".ics" -type "componentList" 1 "f[5:6]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 5.3899237070882098 5 0 1;
	setAttr ".pc" -type "double3" 4.8301330476581397 4.3587558998478828 1.1876283315984404 ;
	setAttr ".ro" -type "double3" -100.57846998454826 39.485605024231205 1.0540129114259977 ;
	setAttr ".ps" -type "double2" 1 10 ;
createNode polyCut -n "polyCut19";
	setAttr ".uopa" yes;
	setAttr ".ics" -type "componentList" 2 "f[5:6]" "f[24:25]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 5.3899237070882098 5 0 1;
	setAttr ".pc" -type "double3" 5.3665538906715122 4.8614135545047166 0.70368160839207405 ;
	setAttr ".ro" -type "double3" -101.00436196893465 31.896316339261844 0.81467546232699517 ;
	setAttr ".ps" -type "double2" 1 10 ;
createNode polyExtrudeFace -n "polyExtrudeFace16";
	setAttr ".ics" -type "componentList" 1 "f[24:25]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 5.3899237070882098 5 0 1;
	setAttr ".ws" yes;
	setAttr ".pvt" -type "float3" 5.5899239 4.5733976 0.19999999 ;
	setAttr ".rs" 61151;
	setAttr ".lt" -type "double3" -1.2212453270876722e-015 -8.8817841970012523e-016 
		1.5508275829818363 ;
	setAttr ".c[0]"  0 1 1;
createNode polyPlane -n "polyPlane2";
	setAttr ".uvs" -type "string" "map";
	setAttr ".w" 4;
	setAttr ".h" 4;
	setAttr ".sw" 1;
	setAttr ".sh" 1;
	setAttr ".cuv" 2;
createNode polyCut -n "polyCut20";
	setAttr ".uopa" yes;
	setAttr ".ics" -type "componentList" 1 "f[3]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 5.3899237070882098 5 0 1;
	setAttr ".pc" -type "double3" 4.8627677276618497 5.8301472121873728 -0.44532112528169981 ;
	setAttr ".ro" -type "double3" -91.460811708894823 -69.850376140857861 -2.4752305077745342 ;
	setAttr ".ps" -type "double2" 1 8.4138059616088867 ;
createNode polyTweak -n "polyTweak1";
	setAttr ".uopa" yes;
	setAttr -s 21 ".tk";
	setAttr ".tk[8:15]" -type "float3" -0.098653808 -1.5861943 0.098653845  
		0 -1.5861943 0.13951758  0.098653808 -1.5861943 0.098653845  0.13951762 -1.5861943 
		5.1974349e-009  0.098653808 -1.5861943 -0.098653831  0 -1.5861943 -0.13951758  -0.098653808 
		-1.5861943 -0.098653845  -0.13951762 -1.5861943 5.1974349e-009 ;
	setAttr ".tk[17:29]" -type "float3" 0 1.2931429 0  0 0.17629921 -0.043893673  
		0.0065077655 0.10823496 -0.018181361  0 0.17804651 0.043893673  0 -0.12980616 -0.043893673  
		0.020690368 -0.069213659 -0.018181361  0 -0.11780737 0.043893673  1.3369677 1.6981341 
		-0.038957346  1.9544895 1.7358832 -0.56551874  1.2843344 1.335428 -0.075837791  1.9244659 
		1.3129497 -0.58655602  0.7905128 1.7469807 0.59516865  0.76149505 1.3382111 0.57483596 ;
createNode polyCut -n "polyCut21";
	setAttr ".uopa" yes;
	setAttr ".ics" -type "componentList" 2 "f[3]" "f[34]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 5.3899237070882098 5 0 1;
	setAttr ".pc" -type "double3" 4.9542046961767632 5.5990121788339398 -0.14367065926929232 ;
	setAttr ".ro" -type "double3" -95.759572981236133 -69.800000000252751 0 ;
	setAttr ".ps" -type "double2" 1 8.4138059616088867 ;
createNode polyExtrudeFace -n "polyExtrudeFace17";
	setAttr ".ics" -type "componentList" 1 "f[35]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 5.3899237070882098 5 0 1;
	setAttr ".ws" yes;
	setAttr ".pvt" -type "float3" 5.1289549 5.6226053 0.10864401 ;
	setAttr ".rs" 46023;
	setAttr ".ls" -type "double3" 0.55598563776004717 0.64716570879725188 1 ;
	setAttr ".c[0]"  0 1 1;
createNode polyTweak -n "polyTweak2";
	setAttr ".uopa" yes;
	setAttr -s 4 ".tk[30:33]" -type "float3"  0 -0.086897813 0 0 -0.086897813 
		0 0 -0.077827856 0 0 -0.077827856 0;
createNode polyExtrudeFace -n "polyExtrudeFace18";
	setAttr ".ics" -type "componentList" 1 "f[35]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 5.3899237070882098 5 0 1;
	setAttr ".ws" yes;
	setAttr ".pvt" -type "float3" 5.1290102 5.6222062 0.10818241 ;
	setAttr ".rs" 64109;
	setAttr ".lt" -type "double3" -0.14213044193141144 0.80316566046536486 2.2461509882380968 ;
	setAttr ".c[0]"  0 1 1;
createNode polyCylinder -n "polyCylinder3";
	setAttr ".r" 0.4;
	setAttr ".h" 11;
	setAttr ".sc" 1;
	setAttr ".cuv" 3;
createNode phong -n "mat_phys";
	setAttr ".c" -type "float3" 0.98016644 0 1 ;
	setAttr ".it" -type "float3" 0.60329998 0.60329998 0.60329998 ;
createNode shadingEngine -n "phong3SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo3";
createNode polyUnite -n "polyUnite1";
	setAttr -s 8 ".ip";
	setAttr -s 8 ".im";
createNode groupId -n "groupId3";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts1";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0]";
createNode groupId -n "groupId4";
	setAttr ".ihi" 0;
createNode groupId -n "groupId5";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts2";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0]";
createNode groupId -n "groupId6";
	setAttr ".ihi" 0;
createNode groupId -n "groupId7";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts3";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0]";
createNode groupId -n "groupId8";
	setAttr ".ihi" 0;
createNode groupId -n "groupId9";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts4";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0]";
createNode groupId -n "groupId10";
	setAttr ".ihi" 0;
createNode groupId -n "groupId11";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts5";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0]";
createNode groupId -n "groupId12";
	setAttr ".ihi" 0;
createNode groupId -n "groupId13";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts6";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0]";
createNode groupId -n "groupId14";
	setAttr ".ihi" 0;
createNode groupId -n "groupId15";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts7";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0]";
createNode groupId -n "groupId16";
	setAttr ".ihi" 0;
createNode groupId -n "groupId17";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts8";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0]";
createNode groupId -n "groupId18";
	setAttr ".ihi" 0;
createNode groupId -n "groupId19";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts9";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:7]";
createNode groupId -n "groupId20";
	setAttr ".ihi" 0;
createNode script -n "rg_export";
	addAttr -ci true -sn "time" -ln "time" -dt "string";
	setAttr ".time" -type "string" "2011-04-01T21:56:14.593000";
select -ne :time1;
	setAttr ".o" 1;
select -ne :renderPartition;
	setAttr -s 5 ".st";
select -ne :renderGlobalsList1;
select -ne :defaultShaderList1;
	setAttr -s 5 ".s";
select -ne :postProcessList1;
	setAttr -s 2 ".p";
select -ne :defaultRenderUtilityList1;
select -ne :lightList1;
select -ne :defaultTextureList1;
select -ne :initialShadingGroup;
	setAttr ".ro" yes;
	setAttr -s 2 ".gn";
select -ne :initialParticleSE;
	setAttr ".ro" yes;
select -ne :hardwareRenderGlobals;
	setAttr ".ctrs" 256;
	setAttr ".btrs" 512;
select -ne :defaultHardwareRenderGlobals;
	setAttr ".fn" -type "string" "im";
	setAttr ".res" -type "string" "ntsc_4d 646 485 1.333";
select -ne :hyperGraphLayout;
	setAttr -s 3 ".hyp";
	setAttr ".hyp[226].x" 430.33941650390625;
	setAttr ".hyp[226].y" -176.35675048828125;
	setAttr ".hyp[226].isf" yes;
	setAttr ".hyp[227].x" 432.33941650390625;
	setAttr ".hyp[227].y" -231.35676574707031;
	setAttr ".hyp[227].isf" yes;
	setAttr ".hyp[228].x" 398.97686767578125;
	setAttr ".hyp[228].y" -108.95329284667969;
	setAttr ".hyp[228].isf" yes;
connectAttr "polyExtrudeFace18.out" "m_tree1Shape.i";
connectAttr "groupId19.id" "m_tree_leaves1Shape.iog.og[0].gid";
connectAttr "phong2SG.mwc" "m_tree_leaves1Shape.iog.og[0].gco";
connectAttr "groupParts9.og" "m_tree_leaves1Shape.i";
connectAttr "groupId20.id" "m_tree_leaves1Shape.ciog.cog[0].cgid";
connectAttr "polyCylinder3.out" "phys_tree1Shape.i";
connectAttr "groupId3.id" "|i_leaves1|transform8|i_leavesShape3.iog.og[0].gid";
connectAttr "phong2SG.mwc" "|i_leaves1|transform8|i_leavesShape3.iog.og[0].gco";
connectAttr "groupId5.id" "|i_leaves3|transform7|i_leavesShape3.iog.og[0].gid";
connectAttr "phong2SG.mwc" "|i_leaves3|transform7|i_leavesShape3.iog.og[0].gco";
connectAttr "groupId7.id" "|i_leaves4|transform6|i_leavesShape3.iog.og[0].gid";
connectAttr "phong2SG.mwc" "|i_leaves4|transform6|i_leavesShape3.iog.og[0].gco";
connectAttr "groupId9.id" "|i_leaves5|transform5|i_leavesShape3.iog.og[0].gid";
connectAttr "phong2SG.mwc" "|i_leaves5|transform5|i_leavesShape3.iog.og[0].gco";
connectAttr "groupId11.id" "|i_leaves6|transform4|i_leavesShape3.iog.og[0].gid";
connectAttr "phong2SG.mwc" "|i_leaves6|transform4|i_leavesShape3.iog.og[0].gco";
connectAttr "groupId13.id" "|i_leaves7|transform3|i_leavesShape3.iog.og[0].gid";
connectAttr "phong2SG.mwc" "|i_leaves7|transform3|i_leavesShape3.iog.og[0].gco";
connectAttr "groupId15.id" "|i_leaves8|transform2|i_leavesShape3.iog.og[0].gid";
connectAttr "phong2SG.mwc" "|i_leaves8|transform2|i_leavesShape3.iog.og[0].gco";
connectAttr "groupId17.id" "|i_leaves9|transform1|i_leavesShape3.iog.og[0].gid";
connectAttr "phong2SG.mwc" "|i_leaves9|transform1|i_leavesShape3.iog.og[0].gco";
connectAttr "groupParts8.og" "|i_leaves1|transform8|i_leavesShape3.i";
connectAttr "groupId4.id" "|i_leaves1|transform8|i_leavesShape3.ciog.cog[0].cgid"
		;
connectAttr "groupId6.id" "|i_leaves3|transform7|i_leavesShape3.ciog.cog[0].cgid"
		;
connectAttr "groupId8.id" "|i_leaves4|transform6|i_leavesShape3.ciog.cog[0].cgid"
		;
connectAttr "groupId10.id" "|i_leaves5|transform5|i_leavesShape3.ciog.cog[0].cgid"
		;
connectAttr "groupId12.id" "|i_leaves6|transform4|i_leavesShape3.ciog.cog[0].cgid"
		;
connectAttr "groupId14.id" "|i_leaves7|transform3|i_leavesShape3.ciog.cog[0].cgid"
		;
connectAttr "groupId16.id" "|i_leaves8|transform2|i_leavesShape3.ciog.cog[0].cgid"
		;
connectAttr "groupId18.id" "|i_leaves9|transform1|i_leavesShape3.ciog.cog[0].cgid"
		;
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[0].llnk";
connectAttr ":initialShadingGroup.msg" "lightLinker1.lnk[0].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[1].llnk";
connectAttr ":initialParticleSE.msg" "lightLinker1.lnk[1].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[2].llnk";
connectAttr "phong1SG.msg" "lightLinker1.lnk[2].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[3].llnk";
connectAttr "phong2SG.msg" "lightLinker1.lnk[3].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[4].llnk";
connectAttr "phong3SG.msg" "lightLinker1.lnk[4].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[0].sllk";
connectAttr ":initialShadingGroup.msg" "lightLinker1.slnk[0].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[1].sllk";
connectAttr ":initialParticleSE.msg" "lightLinker1.slnk[1].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[2].sllk";
connectAttr "phong1SG.msg" "lightLinker1.slnk[2].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[3].sllk";
connectAttr "phong2SG.msg" "lightLinker1.slnk[3].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[4].sllk";
connectAttr "phong3SG.msg" "lightLinker1.slnk[4].solk";
connectAttr "layerManager.dli[0]" "defaultLayer.id";
connectAttr "renderLayerManager.rlmi[0]" "defaultRenderLayer.rlid";
connectAttr "mat_tree.oc" "phong1SG.ss";
connectAttr "m_tree1Shape.iog" "phong1SG.dsm" -na;
connectAttr "phong1SG.msg" "materialInfo1.sg";
connectAttr "mat_tree.msg" "materialInfo1.m";
connectAttr "file1.oc" "mat_leaves.c";
connectAttr "file1.ot" "mat_leaves.it";
connectAttr "mat_leaves.oc" "phong2SG.ss";
connectAttr "|i_leaves1|transform8|i_leavesShape3.iog.og[0]" "phong2SG.dsm" -na;
connectAttr "|i_leaves1|transform8|i_leavesShape3.ciog.cog[0]" "phong2SG.dsm" -na
		;
connectAttr "|i_leaves3|transform7|i_leavesShape3.iog.og[0]" "phong2SG.dsm" -na;
connectAttr "|i_leaves3|transform7|i_leavesShape3.ciog.cog[0]" "phong2SG.dsm" -na
		;
connectAttr "|i_leaves4|transform6|i_leavesShape3.iog.og[0]" "phong2SG.dsm" -na;
connectAttr "|i_leaves4|transform6|i_leavesShape3.ciog.cog[0]" "phong2SG.dsm" -na
		;
connectAttr "|i_leaves5|transform5|i_leavesShape3.iog.og[0]" "phong2SG.dsm" -na;
connectAttr "|i_leaves5|transform5|i_leavesShape3.ciog.cog[0]" "phong2SG.dsm" -na
		;
connectAttr "|i_leaves6|transform4|i_leavesShape3.iog.og[0]" "phong2SG.dsm" -na;
connectAttr "|i_leaves6|transform4|i_leavesShape3.ciog.cog[0]" "phong2SG.dsm" -na
		;
connectAttr "|i_leaves7|transform3|i_leavesShape3.iog.og[0]" "phong2SG.dsm" -na;
connectAttr "|i_leaves7|transform3|i_leavesShape3.ciog.cog[0]" "phong2SG.dsm" -na
		;
connectAttr "|i_leaves8|transform2|i_leavesShape3.iog.og[0]" "phong2SG.dsm" -na;
connectAttr "|i_leaves8|transform2|i_leavesShape3.ciog.cog[0]" "phong2SG.dsm" -na
		;
connectAttr "|i_leaves9|transform1|i_leavesShape3.iog.og[0]" "phong2SG.dsm" -na;
connectAttr "|i_leaves9|transform1|i_leavesShape3.ciog.cog[0]" "phong2SG.dsm" -na
		;
connectAttr "m_tree_leaves1Shape.iog.og[0]" "phong2SG.dsm" -na;
connectAttr "groupId3.msg" "phong2SG.gn" -na;
connectAttr "groupId4.msg" "phong2SG.gn" -na;
connectAttr "groupId5.msg" "phong2SG.gn" -na;
connectAttr "groupId6.msg" "phong2SG.gn" -na;
connectAttr "groupId7.msg" "phong2SG.gn" -na;
connectAttr "groupId8.msg" "phong2SG.gn" -na;
connectAttr "groupId9.msg" "phong2SG.gn" -na;
connectAttr "groupId10.msg" "phong2SG.gn" -na;
connectAttr "groupId11.msg" "phong2SG.gn" -na;
connectAttr "groupId12.msg" "phong2SG.gn" -na;
connectAttr "groupId13.msg" "phong2SG.gn" -na;
connectAttr "groupId14.msg" "phong2SG.gn" -na;
connectAttr "groupId15.msg" "phong2SG.gn" -na;
connectAttr "groupId16.msg" "phong2SG.gn" -na;
connectAttr "groupId17.msg" "phong2SG.gn" -na;
connectAttr "groupId18.msg" "phong2SG.gn" -na;
connectAttr "phong2SG.msg" "materialInfo2.sg";
connectAttr "mat_leaves.msg" "materialInfo2.m";
connectAttr "file1.msg" "materialInfo2.t" -na;
connectAttr "place2dTexture1.c" "file1.c";
connectAttr "place2dTexture1.tf" "file1.tf";
connectAttr "place2dTexture1.rf" "file1.rf";
connectAttr "place2dTexture1.mu" "file1.mu";
connectAttr "place2dTexture1.mv" "file1.mv";
connectAttr "place2dTexture1.s" "file1.s";
connectAttr "place2dTexture1.wu" "file1.wu";
connectAttr "place2dTexture1.wv" "file1.wv";
connectAttr "place2dTexture1.re" "file1.re";
connectAttr "place2dTexture1.of" "file1.of";
connectAttr "place2dTexture1.r" "file1.ro";
connectAttr "place2dTexture1.n" "file1.n";
connectAttr "place2dTexture1.vt1" "file1.vt1";
connectAttr "place2dTexture1.vt2" "file1.vt2";
connectAttr "place2dTexture1.vt3" "file1.vt3";
connectAttr "place2dTexture1.vc1" "file1.vc1";
connectAttr "place2dTexture1.o" "file1.uv";
connectAttr "place2dTexture1.ofs" "file1.fs";
connectAttr "polyCylinder2.out" "polyCut18.ip";
connectAttr "m_tree1Shape.wm" "polyCut18.mp";
connectAttr "polyCut18.out" "polyCut19.ip";
connectAttr "m_tree1Shape.wm" "polyCut19.mp";
connectAttr "polyCut19.out" "polyExtrudeFace16.ip";
connectAttr "m_tree1Shape.wm" "polyExtrudeFace16.mp";
connectAttr "polyTweak1.out" "polyCut20.ip";
connectAttr "m_tree1Shape.wm" "polyCut20.mp";
connectAttr "polyExtrudeFace16.out" "polyTweak1.ip";
connectAttr "polyCut20.out" "polyCut21.ip";
connectAttr "m_tree1Shape.wm" "polyCut21.mp";
connectAttr "polyTweak2.out" "polyExtrudeFace17.ip";
connectAttr "m_tree1Shape.wm" "polyExtrudeFace17.mp";
connectAttr "polyCut21.out" "polyTweak2.ip";
connectAttr "polyExtrudeFace17.out" "polyExtrudeFace18.ip";
connectAttr "m_tree1Shape.wm" "polyExtrudeFace18.mp";
connectAttr "mat_phys.oc" "phong3SG.ss";
connectAttr "phys_tree1Shape.iog" "phong3SG.dsm" -na;
connectAttr "phong3SG.msg" "materialInfo3.sg";
connectAttr "mat_phys.msg" "materialInfo3.m";
connectAttr "|i_leaves1|transform8|i_leavesShape3.o" "polyUnite1.ip[0]";
connectAttr "|i_leaves1|transform8|i_leavesShape3.o" "polyUnite1.ip[1]";
connectAttr "|i_leaves1|transform8|i_leavesShape3.o" "polyUnite1.ip[2]";
connectAttr "|i_leaves1|transform8|i_leavesShape3.o" "polyUnite1.ip[3]";
connectAttr "|i_leaves1|transform8|i_leavesShape3.o" "polyUnite1.ip[4]";
connectAttr "|i_leaves1|transform8|i_leavesShape3.o" "polyUnite1.ip[5]";
connectAttr "|i_leaves1|transform8|i_leavesShape3.o" "polyUnite1.ip[6]";
connectAttr "|i_leaves1|transform8|i_leavesShape3.o" "polyUnite1.ip[7]";
connectAttr "|i_leaves1|transform8|i_leavesShape3.wm" "polyUnite1.im[0]";
connectAttr "|i_leaves3|transform7|i_leavesShape3.wm" "polyUnite1.im[1]";
connectAttr "|i_leaves4|transform6|i_leavesShape3.wm" "polyUnite1.im[2]";
connectAttr "|i_leaves5|transform5|i_leavesShape3.wm" "polyUnite1.im[3]";
connectAttr "|i_leaves6|transform4|i_leavesShape3.wm" "polyUnite1.im[4]";
connectAttr "|i_leaves7|transform3|i_leavesShape3.wm" "polyUnite1.im[5]";
connectAttr "|i_leaves8|transform2|i_leavesShape3.wm" "polyUnite1.im[6]";
connectAttr "|i_leaves9|transform1|i_leavesShape3.wm" "polyUnite1.im[7]";
connectAttr "polyPlane2.out" "groupParts1.ig";
connectAttr "groupId3.id" "groupParts1.gi";
connectAttr "groupParts1.og" "groupParts2.ig";
connectAttr "groupId5.id" "groupParts2.gi";
connectAttr "groupParts2.og" "groupParts3.ig";
connectAttr "groupId7.id" "groupParts3.gi";
connectAttr "groupParts3.og" "groupParts4.ig";
connectAttr "groupId9.id" "groupParts4.gi";
connectAttr "groupParts4.og" "groupParts5.ig";
connectAttr "groupId11.id" "groupParts5.gi";
connectAttr "groupParts5.og" "groupParts6.ig";
connectAttr "groupId13.id" "groupParts6.gi";
connectAttr "groupParts6.og" "groupParts7.ig";
connectAttr "groupId15.id" "groupParts7.gi";
connectAttr "groupParts7.og" "groupParts8.ig";
connectAttr "groupId17.id" "groupParts8.gi";
connectAttr "polyUnite1.out" "groupParts9.ig";
connectAttr "groupId19.id" "groupParts9.gi";
connectAttr "phong1SG.pa" ":renderPartition.st" -na;
connectAttr "phong2SG.pa" ":renderPartition.st" -na;
connectAttr "phong3SG.pa" ":renderPartition.st" -na;
connectAttr "mat_tree.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_leaves.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_phys.msg" ":defaultShaderList1.s" -na;
connectAttr "place2dTexture1.msg" ":defaultRenderUtilityList1.u" -na;
connectAttr "lightLinker1.msg" ":lightList1.ln" -na;
connectAttr "file1.msg" ":defaultTextureList1.tx" -na;
connectAttr "m_tree_leaves1Shape.ciog.cog[0]" ":initialShadingGroup.dsm" -na;
connectAttr "groupId19.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId20.msg" ":initialShadingGroup.gn" -na;
connectAttr "m_tree_leaves1.msg" ":hyperGraphLayout.hyp[226].dn";
connectAttr "phys_tree1.msg" ":hyperGraphLayout.hyp[227].dn";
connectAttr "m_tree1.msg" ":hyperGraphLayout.hyp[228].dn";
// End of tree_01.ma
