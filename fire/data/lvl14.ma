//Maya ASCII 2014 scene
//Name: lvl14.ma
//Last modified: Wed, Mar 19, 2014 02:38:15 PM
//Codeset: 1252
requires maya "2014";
currentUnit -l centimeter -a degree -t film;
fileInfo "application" "maya";
fileInfo "product" "Maya 2014";
fileInfo "version" "2014";
fileInfo "cutIdentifier" "201307170459-880822";
fileInfo "osv" "Microsoft Windows 7 Home Premium Edition, 64-bit Windows 7 Service Pack 1 (Build 7601)\n";
createNode transform -s -n "persp";
	setAttr ".v" no;
	setAttr ".t" -type "double3" -237.49068989444157 201.69927232365819 141.2880173233433 ;
	setAttr ".r" -type "double3" -46.538352729878625 328.9999999997018 -1.855270656538256e-015 ;
	setAttr ".rp" -type "double3" 0 -3.6637359812630166e-015 7.1054273576010019e-015 ;
	setAttr ".rpt" -type "double3" 7.5981871650220083e-014 1.6471638655843653e-014 1.307148822229979e-013 ;
createNode camera -s -n "perspShape" -p "persp";
	setAttr -k off ".v" no;
	setAttr ".rnd" no;
	setAttr ".fl" 34.999999999999979;
	setAttr ".fcp" 3000;
	setAttr ".coi" 348.69523873690781;
	setAttr ".imn" -type "string" "persp";
	setAttr ".den" -type "string" "persp_depth";
	setAttr ".man" -type "string" "persp_mask";
	setAttr ".tp" -type "double3" 2.6763682683859997 -43.434322581729532 -65.551290233755537 ;
	setAttr ".hc" -type "string" "viewSet -p %camera";
createNode transform -s -n "top";
	setAttr ".v" no;
	setAttr ".t" -type "double3" -139.08752424051127 328.10620860542798 -238.99641165536957 ;
	setAttr ".r" -type "double3" -89.999999999999986 0 0 ;
	setAttr ".rp" -type "double3" -1.4210854715202004e-014 2.8421709430404007e-014 5.6843418860808015e-014 ;
	setAttr ".rpt" -type "double3" 0 2.8421709430404014e-014 -8.526512829121206e-014 ;
createNode camera -s -n "topShape" -p "top";
	setAttr -k off ".v" no;
	setAttr ".rnd" no;
	setAttr ".fcp" 3000;
	setAttr ".coi" 342.76349171827724;
	setAttr ".ow" 1032.1116096618834;
	setAttr ".imn" -type "string" "top";
	setAttr ".den" -type "string" "top_depth";
	setAttr ".man" -type "string" "top_mask";
	setAttr ".tp" -type "double3" -83.826400115855606 -14.657283112849314 -188.57944025884137 ;
	setAttr ".hc" -type "string" "viewSet -t %camera";
	setAttr ".o" yes;
createNode transform -s -n "front";
	setAttr ".v" no;
	setAttr ".t" -type "double3" -67.323738093102108 -11.335937802345992 118.58013551712867 ;
createNode camera -s -n "frontShape" -p "front";
	setAttr -k off ".v" no;
	setAttr ".rnd" no;
	setAttr ".fcp" 3000;
	setAttr ".coi" 298.89084990082017;
	setAttr ".ow" 16.123471560141638;
	setAttr ".imn" -type "string" "front";
	setAttr ".den" -type "string" "front_depth";
	setAttr ".man" -type "string" "front_mask";
	setAttr ".tp" -type "double3" -77.477038100237976 -10.93133054164951 -187.25550099587122 ;
	setAttr ".hc" -type "string" "viewSet -f %camera";
	setAttr ".o" yes;
createNode transform -s -n "side";
	setAttr ".v" no;
	setAttr ".t" -type "double3" 158.75180992728727 4.7456978594743404 -120.59850653453674 ;
	setAttr ".r" -type "double3" 0 89.999999999999986 0 ;
createNode camera -s -n "sideShape" -p "side";
	setAttr -k off ".v" no;
	setAttr ".rnd" no;
	setAttr ".fcp" 3000;
	setAttr ".coi" 100.1;
	setAttr ".ow" 371.18298247036017;
	setAttr ".imn" -type "string" "side";
	setAttr ".den" -type "string" "side_depth";
	setAttr ".man" -type "string" "side_mask";
	setAttr ".hc" -type "string" "viewSet -s %camera";
	setAttr ".o" yes;
createNode transform -n "camera1";
createNode camera -n "cameraShape1" -p "camera1";
	setAttr -k off ".v";
	setAttr ".rnd" no;
	setAttr ".ff" 0;
	setAttr ".fl" 38.165442343712215;
	setAttr ".ncp" 0.01;
	setAttr ".coi" 25.257929144004212;
	setAttr ".ow" 30;
	setAttr ".imn" -type "string" "camera1";
	setAttr ".den" -type "string" "camera1_depth";
	setAttr ".man" -type "string" "camera1_mask";
createNode transform -n "transform1" -p "cameraShape1";
createNode imagePlane -n "imagePlane1" -p "transform1";
	setAttr -k off ".v";
	setAttr ".fc" 232857424;
	setAttr ".imn" -type "string" "C:/RnD/pd/Fire/Data/lvl14.jpg";
	setAttr ".cov" -type "short2" 1024 768 ;
	setAttr ".f" 0;
	setAttr ".dm" 2;
	setAttr ".d" 400;
	setAttr ".s" -type "double2" 1.41732 0.945 ;
	setAttr ".ic" -type "double3" 0 0 -5 ;
	setAttr ".w" 30;
	setAttr ".h" 30;
createNode transform -n "m_ground";
createNode mesh -n "m_phys_groundShape" -p "m_ground";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "string";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	addAttr -ci true -sn "rgn" -ln "rgn" -dt "string";
	addAttr -ci true -sn "rguv0" -ln "rguv0" -dt "string";
	setAttr -k off ".v";
	setAttr -s 2 ".iog";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".pv" -type "double2" 0.25 1 ;
	setAttr ".uvst[0].uvsn" -type "string" "map_env";
	setAttr ".cuvs" -type "string" "map_env";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 28 ".pt[0:27]" -type "float3"  25.274256 -9.3827772 -202.40866 
		25.274256 -9.3827772 -202.40866 11.634624 -12.22854 -211.75284 11.634624 -11.351673 
		-211.75284 16.235819 1.9310083 -190.61835 -15.012567 -22.597118 -197.95914 38.874237 
		-11.351673 -158.89397 15.181513 -7.3918629 -153.41737 0 -11.351673 -165.09401 0 -11.351673 
		-165.09401 9.5477943 1.9310083 -165.09401 -47.388367 -20.371006 -197.95914 38.874237 
		-11.351673 -158.89397 15.181513 -7.3918629 -153.41737 0 -11.351673 -165.09401 0 -11.351673 
		-165.09401 17.525345 4.1591139 -165.09401 -54.514053 -20.371006 -197.95914 38.874237 
		-11.351673 -158.89397 18.831808 -5.4730806 -151.96623 -4.7683716e-007 -9.5651712 
		-165.09401 0 -11.351673 -165.09401 21.304583 8.6017685 -165.09401 -41.071507 -22.597118 
		-163.6467 0 -11.351673 -165.09401 0 -10.27783 -193.95648 0 -10.27783 -193.95648 22.867979 
		13.672722 -165.09401;
	setAttr ".rgvtx" -type "string" (
		"[-51.50697326660156, -16.312175750732422, -56.992095947265625, -51.50697326660156, -16.312175750732422, -56.992095947265625, -51.76173782348633, -19.4285831451416, -56.803985595703125, -28.65194320678711, -18.822357177734375, -47.271697998046875, 8.783931732177734, -5.8103179931640625, -16.604904174804688, -92.03022003173828, -22.891155242919922, -163.62619018554688, -15.033626556396484, -11.916352272033691, -115.02875518798828, -15.61656379699707, -8.227184295654297, -100.01983642578125, -14.725536346435547, -12.457636833190918, -102.16419982910156, 16.5264892578125, -12.728279113769531, -92.63191223144531, 48.079071044921875, 0.283760666847229, -83.09961700439453, -86.34300231933594, -11.516146659851074, -255.6452178955078, 23.029388427734375, -2.7674551010131836, -207.0477752685547, 22.446449279785156, 0.9217128753662109, -192.0388946533203, 24.06684112548828, -3.3087406158447266, -194.18324279785156, 58.694061279296875, -3.57938289642334, -184.65093994140625, 94.11963653564453, 11.660762786865234, -175.11866760253906, -55.40568161010742, -2.36724853515625, -347.6642761230469, 61.09239196777344, 6.381443023681641, -299.06683349609375, 64.15974426269531, 11.989392280578613, -282.6068115234375, 66.45924377441406, 7.626660346984863, -286.2022705078125, 100.2490234375, 5.569515228271484, -276.66998291015625, 135.96188354492188, 25.252315521240234, -267.1376953125, 19.209678649902344, 4.284893035888672, -395.8385925292969, 83.39097595214844, 15.259696960449219, -387.75360107421875, 113.08958435058594, 16.062896728515625, -407.08380126953125, 147.01303100585938, 15.792253494262695, -397.551513671875, 175.58828735351562, 39.472164154052734, -359.1567687988281]");
	setAttr ".rgf" -type "string" "[[5,0,6],[0,1,7,6],[1,2,8,7],[2,3,9,8],[3,4,10,9],[5,6,12,11],[6,7,13,12],[7,8,14,13],[8,9,15,14],[9,10,16,15],[11,12,18,17],[12,13,19,18],[13,14,20,19],[14,15,21,20],[15,16,22,21],[17,18,23],[18,19,24,23],[19,20,25,24],[20,21,26,25],[21,22,27,26]]";
	setAttr ".rgn" -type "string" (
		"[-0.14317889511585236, 0.987362265586853, 0.06793871521949768, -0.13502918183803558, 0.990792989730835, -0.009814758785068989, -0.1959284245967865, 0.9802224040031433, 0.027858184650540352, -0.13502918183803558, 0.990792989730835, -0.009814758785068989, 0.7757675051689148, -0.0987667441368103, 0.6232415437698364, 0.25397002696990967, 0.9349231719970703, 0.2478262186050415, -0.1959284245967865, 0.9802224040031433, 0.027858184650540352, 0.7757675051689148, -0.0987667441368103, 0.6232415437698364, 0.06732220947742462, 0.9789469838142395, 0.1926928460597992, 0.0939793735742569, 0.9833594560623169, 0.1554732620716095, 0.25397002696990967, 0.9349231719970703, 0.2478262186050415, 0.06732220947742462, 0.9789469838142395, 0.1926928460597992, -0.18653929233551025, 0.9822129011154175, -0.02146788127720356, -0.20924881100654602, 0.9776548147201538, -0.020148329436779022, 0.0939793735742569, 0.9833594560623169, 0.1554732620716095, -0.18653929233551025, 0.9822129011154175, -0.02146788127720356, -0.29066717624664307, 0.9497637152671814, -0.1160237044095993, -0.3411155641078949, 0.9357627630233765, -0.08937717974185944, -0.20924881100654602, 0.9776548147201538, -0.020148329436779022, -0.14317889511585236, 0.987362265586853, 0.06793871521949768, -0.1959284245967865, 0.9802224040031433, 0.027858184650540352, -0.16185398399829865, 0.9856254458427429, 0.048433005809783936, -0.11819112300872803, 0.9907934665679932, 0.06602322310209274, -0.1959284245967865, 0.9802224040031433, 0.027858184650540352, 0.25397002696990967, 0.9349231719970703, 0.2478262186050415, -0.1390049010515213, 0.9888433218002319, 0.05354001373052597, -0.16185398399829865, 0.9856254458427429, 0.048433005809783936, 0.25397002696990967, 0.9349231719970703, 0.2478262186050415, 0.0939793735742569, 0.9833594560623169, 0.1554732620716095, 0.1093335822224617, 0.9824615120887756, 0.1510479599237442, -0.1390049010515213, 0.9888433218002319, 0.05354001373052597, 0.0939793735742569, 0.9833594560623169, 0.1554732620716095, -0.20924881100654602, 0.9776548147201538, -0.020148329436779022, -0.21585538983345032, 0.9763941764831543, 0.007790657691657543, 0.1093335822224617, 0.9824615120887756, 0.1510479599237442, -0.20924881100654602, 0.9776548147201538, -0.020148329436779022, -0.3411155641078949, 0.9357627630233765, -0.08937717974185944, -0.40051984786987305, 0.9130226969718933, -0.07728734612464905, -0.21585538983345032, 0.9763941764831543, 0.007790657691657543, -0.11819112300872803, 0.9907934665679932, 0.06602322310209274, -0.16185398399829865, 0.9856254458427429, 0.048433005809783936, -0.15400123596191406, 0.9871444702148438, 0.0427694246172905, -0.09841760247945786, 0.9938398003578186, 0.05095426365733147, -0.16185398399829865, 0.9856254458427429, 0.048433005809783936, -0.1390049010515213, 0.9888433218002319, 0.05354001373052597, -0.13116243481636047, 0.9902390837669373, 0.0471479557454586, -0.15400123596191406, 0.9871444702148438, 0.0427694246172905, -0.1390049010515213, 0.9888433218002319, 0.05354001373052597, 0.1093335822224617, 0.9824615120887756, 0.1510479599237442, 0.06706365942955017, 0.9919019937515259, 0.10785622894763947, -0.13116243481636047, 0.9902390837669373, 0.0471479557454586, 0.1093335822224617, 0.9824615120887756, 0.1510479599237442, -0.21585538983345032, 0.9763941764831543, 0.007790657691657543, -0.25282248854637146, 0.9674896001815796, -0.006690880283713341, 0.06706365942955017, 0.9919019937515259, 0.10785622894763947, -0.21585538983345032, 0.9763941764831543, 0.007790657691657543, -0.40051984786987305, 0.9130226969718933, -0.07728734612464905, -0.4718787372112274, 0.8764859437942505, -0.09540842473506927, -0.25282248854637146, 0.9674896001815796, -0.006690880283713341, -0.09841760247945786, 0.9938398003578186, 0.05095426365733147, -0.15400123596191406, 0.9871444702148438, 0.0427694246172905, -0.1404276341199875, 0.9898797869682312, 0.020445961505174637, -0.15400123596191406, 0.9871444702148438, 0.0427694246172905, -0.13116243481636047, 0.9902390837669373, 0.0471479557454586, -0.07848943024873734, 0.9948930144309998, 0.06346051394939423, -0.1404276341199875, 0.9898797869682312, 0.020445961505174637, -0.13116243481636047, 0.9902390837669373, 0.0471479557454586, 0.06706365942955017, 0.9919019937515259, 0.10785622894763947, 0.029965318739414215, 0.9960247874259949, 0.08388534933328629, -0.07848943024873734, 0.9948930144309998, 0.06346051394939423, 0.06706365942955017, 0.9919019937515259, 0.10785622894763947, -0.25282248854637146, 0.9674896001815796, -0.006690880283713341, -0.23503783345222473, 0.9719738364219666, -0.004915397148579359, 0.029965318739414215, 0.9960247874259949, 0.08388534933328629, -0.25282248854637146, 0.9674896001815796, -0.006690880283713341, -0.4718787372112274, 0.8764859437942505, -0.09540842473506927, -0.5012439489364624, 0.858938455581665, -0.10478132963180542, -0.23503783345222473, 0.9719738364219666, -0.004915397148579359]");
	setAttr ".rguv0" -type "string" "[0.0, 0.25, 0.0625, 0.0, 0.0625, 0.25, 0.0625, 0.0, 0.125, 0.0, 0.125, 0.25, 0.0625, 0.25, 0.125, 0.0, 0.1875, 0.0, 0.1875, 0.25, 0.125, 0.25, 0.1875, 0.0, 0.25, 0.0, 0.25, 0.25, 0.1875, 0.25, 0.25, 0.0, 0.3125, 0.0, 0.3125, 0.25, 0.25, 0.25, 0.0, 0.25, 0.0625, 0.25, 0.0625, 0.5, 0.0, 0.5, 0.0625, 0.25, 0.125, 0.25, 0.125, 0.5, 0.0625, 0.5, 0.125, 0.25, 0.1875, 0.25, 0.1875, 0.5, 0.125, 0.5, 0.1875, 0.25, 0.25, 0.25, 0.25, 0.5, 0.1875, 0.5, 0.25, 0.25, 0.3125, 0.25, 0.3125, 0.5, 0.25, 0.5, 0.0, 0.5, 0.0625, 0.5, 0.0625, 0.75, 0.0, 0.75, 0.0625, 0.5, 0.125, 0.5, 0.125, 0.75, 0.0625, 0.75, 0.125, 0.5, 0.1875, 0.5, 0.1875, 0.75, 0.125, 0.75, 0.1875, 0.5, 0.25, 0.5, 0.25, 0.75, 0.1875, 0.75, 0.25, 0.5, 0.3125, 0.5, 0.3125, 0.75, 0.25, 0.75, 0.0, 0.75, 0.0625, 0.75, 0.0625, 1.0, 0.0625, 0.75, 0.125, 0.75, 0.125, 1.0, 0.0625, 1.0, 0.125, 0.75, 0.1875, 0.75, 0.1875, 1.0, 0.125, 1.0, 0.1875, 0.75, 0.25, 0.75, 0.25, 1.0, 0.1875, 1.0, 0.25, 0.75, 0.3125, 0.75, 0.3125, 1.0, 0.25, 1.0]";
createNode transform -n "m_flowers" -p "m_ground";
	setAttr ".t" -type "double3" -4.7297783380110801 -11.215796560665225 -41.241210081263667 ;
	setAttr ".r" -type "double3" 43.90722161876387 30.796600181716535 9.7593421543998158 ;
	setAttr ".rp" -type "double3" -15.47802189890135 34.283644321933735 20.834223640158598 ;
	setAttr ".rpt" -type "double3" 20.207800236912433 -23.067847761268514 20.406986441105065 ;
	setAttr ".sp" -type "double3" -15.47802189890135 34.283644321933735 20.834223640158598 ;
createNode mesh -n "m_flowersShape" -p "m_flowers";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "string";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	addAttr -ci true -sn "rgn" -ln "rgn" -dt "string";
	addAttr -ci true -sn "rguv0" -ln "rguv0" -dt "string";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".pv" -type "double2" 0 0.18518517911434174 ;
	setAttr ".uvst[0].uvsn" -type "string" "map_flowers";
	setAttr ".cuvs" -type "string" "map_flowers";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".rgvtx" -type "string" "[-13.5, -5.551115123125783e-16, 2.5, 13.5, -5.551115123125783e-16, 2.5, -13.5, 5.551115123125783e-16, -2.5, 13.5, 5.551115123125783e-16, -2.5]";
	setAttr ".rgf" -type "string" "[[0,1,3,2]]";
	setAttr ".rgn" -type "string" "[0.0, 1.0, 2.220446049250313e-16, 0.0, 1.0, 2.220446049250313e-16, 0.0, 1.0, 2.220446049250313e-16, 0.0, 1.0, 2.220446049250313e-16]";
	setAttr ".rguv0" -type "string" "[0.0, 0.0, 1.0, 0.0, 1.0, 0.18518517911434174, 0.0, 0.18518517911434174]";
createNode transform -n "m_background" -p "m_ground";
	setAttr ".t" -type "double3" 0 41.761907002548654 -358.48075183067994 ;
	setAttr ".r" -type "double3" 90 0 0 ;
	setAttr ".rp" -type "double3" 0 358.48075183067994 41.761907002548732 ;
	setAttr ".rpt" -type "double3" 0 -400.24265883322863 316.71884482813124 ;
	setAttr ".sp" -type "double3" 0 358.48075183067994 41.761907002548732 ;
createNode mesh -n "m_backgroundShape" -p "m_background";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "string";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	addAttr -ci true -sn "rgn" -ln "rgn" -dt "string";
	addAttr -ci true -sn "rguv0" -ln "rguv0" -dt "string";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".pv" -type "double2" 0 0.46378713846206665 ;
	setAttr ".uvst[0].uvsn" -type "string" "map_bkg";
	setAttr ".cuvs" -type "string" "map_bkg";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 7 ".pt[0:6]" -type "float3"  -7.3571315 -5.8050698e-014 
		5.4370871 9.7919207 -4.0833432e-014 -72.102661 -7.3571315 2.0270902e-015 -9.1292028 
		9.7919207 2.0270902e-015 -9.1292028 -2.6513267 -4.9931227e-014 -31.129667 11.260447 
		-1.0322898e-013 -47.098064 -0.3827666 -4.6170678e-014 -48.065647;
	setAttr ".rgvtx" -type "string" "[-172.3571319580078, -7.504263324024235e-14, 81.96196746826172, 174.7919158935547, -5.782536721590098e-14, 4.422218322753906, -172.3571319580078, 1.9019027075895673e-14, -85.65408325195312, 174.7919158935547, 1.9019027075895673e-14, -85.65408325195312, -57.42604064941406, -6.692316514324281e-14, 45.39521026611328, 79.58595275878906, -1.2022091763937148e-13, 29.426815032958984, 133.27122497558594, -6.316261668424042e-14, 28.459232330322266]";
	setAttr ".rgf" -type "string" "[[0,4,5,6,1,3,2]]";
	setAttr ".rgn" -type "string" "[6.045540165838939e-17, 1.0, 7.8835664565739e-16, 6.045540827583429e-17, 1.0, 7.883567515365084e-16, 6.045540165838939e-17, 1.0, 7.883567515365084e-16, 6.045540165838939e-17, 1.0, 7.883567515365084e-16, 6.045540165838939e-17, 1.0, 7.883566985969492e-16, 6.045540165838939e-17, 1.0, 7.883566985969492e-16, 6.045540165838939e-17, 1.0, 7.883566985969492e-16]";
	setAttr ".rguv0" -type "string" "[0.0, 0.0, 0.33401602506637573, 0.0, 0.7070469856262207, 0.0, 0.9050120711326599, 0.0, 1.0, 0.0, 1.0, 0.46378713846206665, 0.0, 0.46378713846206665]";
createNode transform -n "phys_ground" -p "m_ground";
createNode transform -n "phys_pos_car_start" -p "m_ground";
	setAttr ".t" -type "double3" 116.72415636184753 14.270644661879965 -370.79822116770896 ;
	setAttr ".r" -type "double3" -4.5133169238253954 155.25692866638474 -1.0299334361031054 ;
	setAttr ".s" -type "double3" 1 0.99999999999999867 0.99999999999999811 ;
	setAttr ".rpt" -type "double3" 1.1102230246251565e-015 1.4988010832439613e-015 -1.1102230246251565e-015 ;
	setAttr ".spt" -type "double3" 3.5340968553901329e-028 3.944304526105059e-029 -1.5382787651809733e-029 ;
createNode mesh -n "phys_pos_car_startShape" -p "phys_pos_car_start";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "phys_pos_path_1" -p "m_ground";
	setAttr ".t" -type "double3" 94.184422653475494 11.276459403354401 -323.47289395864277 ;
	setAttr ".r" -type "double3" 0 -3.1805546814635168e-015 0 ;
	setAttr ".s" -type "double3" 1 0.99999999999999978 0.99999999999999978 ;
	setAttr ".spt" -type "double3" 0 3.944304526105059e-030 9.4663308626521417e-030 ;
createNode mesh -n "phys_pos_path_Shape1" -p "phys_pos_path_1";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "phys_pos_path_2" -p "m_ground";
	setAttr ".t" -type "double3" -4.6300292271678058 -11.467160189398239 -96.23743231795811 ;
	setAttr ".r" -type "double3" 0 -3.1805546814635168e-015 0 ;
	setAttr ".s" -type "double3" 1 0.99999999999999978 0.99999999999999978 ;
	setAttr ".spt" -type "double3" 0 1.2621774483536189e-029 1.2621774483536189e-028 ;
createNode mesh -n "phys_pos_path_Shape2" -p "phys_pos_path_2";
	setAttr -k off ".v";
	setAttr -s 2 ".iog";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 196 ".pt";
	setAttr ".pt[0]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[1]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[2]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[3]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[4]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[5]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[6]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[7]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[8]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[9]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[10]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[11]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[12]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[13]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[14]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[15]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[16]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[17]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[18]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[19]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[20]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[28]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[29]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[30]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[31]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[32]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[33]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[34]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[35]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[36]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[37]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[38]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[39]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[48]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[49]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[50]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[51]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[52]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[53]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[54]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[55]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[56]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[57]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[58]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[59]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[68]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[69]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[70]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[71]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[72]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[73]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[74]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[75]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[76]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[77]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[78]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[79]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[89]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[90]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[91]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[92]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[93]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[94]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[95]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[96]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[97]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[98]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[99]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[109]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[110]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[111]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[112]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[113]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[114]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[115]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[116]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[117]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[118]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[119]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[129]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[130]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[131]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[132]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[133]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[134]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[135]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[136]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[137]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[138]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[139]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[149]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[150]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[151]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[152]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[153]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[154]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[155]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[156]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[157]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[158]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[169]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[170]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[171]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[172]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[173]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[174]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[175]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[176]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[177]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[178]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[189]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[190]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[191]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[192]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[193]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[194]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[195]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[196]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[197]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[198]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[209]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[210]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[211]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[212]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[213]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[214]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[215]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[216]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[217]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[218]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[229]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[230]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[231]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[232]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[233]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[234]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[235]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[236]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[237]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[238]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[249]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[250]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[251]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[252]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[253]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[254]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[255]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[256]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[257]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[258]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[270]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[271]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[272]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[273]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[274]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[275]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[276]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[277]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[278]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[290]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[291]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[292]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[293]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[294]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[295]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[296]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[297]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[298]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[310]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[311]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[312]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[313]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[314]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[315]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[316]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[317]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[318]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[330]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[331]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[332]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[333]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[334]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[335]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[336]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[337]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[351]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[352]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[353]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[354]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[355]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[356]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[357]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[373]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[374]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[375]" -type "float3" 0 4.7683716e-007 0 ;
	setAttr ".pt[380]" -type "float3" 0 4.7683716e-007 0 ;
createNode transform -n "phys_trig_car_eater" -p "m_ground";
	setAttr ".t" -type "double3" -70.549549630772418 -95.150348940204466 0.37565301331608048 ;
	setAttr ".r" -type "double3" 0 -24.560869046020684 0 ;
	setAttr ".s" -type "double3" 1 0.99999999999999978 0.99999999999999978 ;
	setAttr ".spt" -type "double3" 0 3.1554436208840472e-029 1.5146129380243427e-028 ;
createNode mesh -n "phys_trig_car_eaterShape" -p "phys_trig_car_eater";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "phys_pos_path_3" -p "m_ground";
	setAttr ".t" -type "double3" -56.734353930081262 -17.382814425613706 -42.12984374931505 ;
	setAttr ".r" -type "double3" 0 -3.1805546814635168e-015 0 ;
	setAttr ".s" -type "double3" 1 0.99999999999999978 0.99999999999999978 ;
	setAttr ".spt" -type "double3" 0 1.2621774483536189e-029 1.2621774483536189e-028 ;
parent -s -nc -r -add "|m_ground|m_phys_groundShape" "phys_ground" ;
parent -s -nc -r -add "|m_ground|phys_pos_path_2|phys_pos_path_Shape2" "phys_pos_path_3" ;
createNode lightLinker -s -n "lightLinker1";
	setAttr -s 4 ".lnk";
	setAttr -s 4 ".slnk";
createNode displayLayerManager -n "layerManager";
createNode displayLayer -n "defaultLayer";
createNode renderLayerManager -n "renderLayerManager";
createNode renderLayer -n "defaultRenderLayer";
	setAttr ".g" yes;
createNode script -n "uiConfigurationScriptNode";
	setAttr ".b" -type "string" (
		"// Maya Mel UI Configuration File.\n//\n//  This script is machine generated.  Edit at your own risk.\n//\n//\n\nglobal string $gMainPane;\nif (`paneLayout -exists $gMainPane`) {\n\n\tglobal int $gUseScenePanelConfig;\n\tint    $useSceneConfig = $gUseScenePanelConfig;\n\tint    $menusOkayInPanels = `optionVar -q allowMenusInPanels`;\tint    $nVisPanes = `paneLayout -q -nvp $gMainPane`;\n\tint    $nPanes = 0;\n\tstring $editorName;\n\tstring $panelName;\n\tstring $itemFilterName;\n\tstring $panelConfig;\n\n\t//\n\t//  get current state of the UI\n\t//\n\tsceneUIReplacement -update $gMainPane;\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"modelPanel\" (localizedPanelLabel(\"Top View\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `modelPanel -unParent -l (localizedPanelLabel(\"Top View\")) -mbv $menusOkayInPanels `;\n\t\t\t$editorName = $panelName;\n            modelEditor -e \n                -camera \"top\" \n                -useInteractiveMode 0\n                -displayLights \"default\" \n                -displayAppearance \"wireframe\" \n"
		+ "                -activeOnly 0\n                -ignorePanZoom 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n                -activeComponentsXray 0\n                -displayTextures 0\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n                -rendererName \"base_OpenGL_Renderer\" \n"
		+ "                -objectFilterShowInHUD 1\n                -isFiltered 0\n                -colorResolution 256 256 \n                -bumpResolution 512 512 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n                -lowQualityLighting 0\n                -maximumNumHardwareLights 1\n                -occlusionCulling 0\n                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n"
		+ "                -imagePlane 1\n                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -pluginShapes 1\n                -dimensions 1\n                -handles 1\n                -pivots 1\n                -textures 1\n                -strokes 1\n                -motionTrails 1\n                -clipGhosts 1\n                -greasePencils 1\n                -shadows 0\n                $editorName;\n            modelEditor -e -viewSelected 0 $editorName;\n            modelEditor -e \n                -pluginObjects \"gpuCacheDisplayFilter\" 1 \n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tmodelPanel -edit -l (localizedPanelLabel(\"Top View\")) -mbv $menusOkayInPanels  $panelName;\n"
		+ "\t\t$editorName = $panelName;\n        modelEditor -e \n            -camera \"top\" \n            -useInteractiveMode 0\n            -displayLights \"default\" \n            -displayAppearance \"wireframe\" \n            -activeOnly 0\n            -ignorePanZoom 0\n            -wireframeOnShaded 0\n            -headsUpDisplay 1\n            -selectionHiliteDisplay 1\n            -useDefaultMaterial 0\n            -bufferMode \"double\" \n            -twoSidedLighting 1\n            -backfaceCulling 0\n            -xray 0\n            -jointXray 0\n            -activeComponentsXray 0\n            -displayTextures 0\n            -smoothWireframe 0\n            -lineWidth 1\n            -textureAnisotropic 0\n            -textureHilight 1\n            -textureSampling 2\n            -textureDisplay \"modulate\" \n            -textureMaxSize 8192\n            -fogging 0\n            -fogSource \"fragment\" \n            -fogMode \"linear\" \n            -fogStart 0\n            -fogEnd 100\n            -fogDensity 0.1\n            -fogColor 0.5 0.5 0.5 1 \n            -maxConstantTransparency 1\n"
		+ "            -rendererName \"base_OpenGL_Renderer\" \n            -objectFilterShowInHUD 1\n            -isFiltered 0\n            -colorResolution 256 256 \n            -bumpResolution 512 512 \n            -textureCompression 0\n            -transparencyAlgorithm \"frontAndBackCull\" \n            -transpInShadows 0\n            -cullingOverride \"none\" \n            -lowQualityLighting 0\n            -maximumNumHardwareLights 1\n            -occlusionCulling 0\n            -shadingModel 0\n            -useBaseRenderer 0\n            -useReducedRenderer 0\n            -smallObjectCulling 0\n            -smallObjectThreshold -1 \n            -interactiveDisableShadows 0\n            -interactiveBackFaceCull 0\n            -sortTransparent 1\n            -nurbsCurves 1\n            -nurbsSurfaces 1\n            -polymeshes 1\n            -subdivSurfaces 1\n            -planes 1\n            -lights 1\n            -cameras 1\n            -controlVertices 1\n            -hulls 1\n            -grid 1\n            -imagePlane 1\n            -joints 1\n"
		+ "            -ikHandles 1\n            -deformers 1\n            -dynamics 1\n            -fluids 1\n            -hairSystems 1\n            -follicles 1\n            -nCloths 1\n            -nParticles 1\n            -nRigids 1\n            -dynamicConstraints 1\n            -locators 1\n            -manipulators 1\n            -pluginShapes 1\n            -dimensions 1\n            -handles 1\n            -pivots 1\n            -textures 1\n            -strokes 1\n            -motionTrails 1\n            -clipGhosts 1\n            -greasePencils 1\n            -shadows 0\n            $editorName;\n        modelEditor -e -viewSelected 0 $editorName;\n        modelEditor -e \n            -pluginObjects \"gpuCacheDisplayFilter\" 1 \n            $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"modelPanel\" (localizedPanelLabel(\"Side View\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `modelPanel -unParent -l (localizedPanelLabel(\"Side View\")) -mbv $menusOkayInPanels `;\n"
		+ "\t\t\t$editorName = $panelName;\n            modelEditor -e \n                -camera \"camera1\" \n                -useInteractiveMode 0\n                -displayLights \"default\" \n                -displayAppearance \"wireframe\" \n                -activeOnly 0\n                -ignorePanZoom 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n                -activeComponentsXray 0\n                -displayTextures 1\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n"
		+ "                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n                -rendererName \"base_OpenGL_Renderer\" \n                -objectFilterShowInHUD 1\n                -isFiltered 0\n                -colorResolution 256 256 \n                -bumpResolution 512 512 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n                -lowQualityLighting 0\n                -maximumNumHardwareLights 1\n                -occlusionCulling 0\n                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n"
		+ "                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n                -imagePlane 1\n                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -pluginShapes 1\n                -dimensions 1\n                -handles 1\n                -pivots 1\n                -textures 1\n                -strokes 1\n                -motionTrails 1\n                -clipGhosts 1\n                -greasePencils 1\n                -shadows 0\n                $editorName;\n            modelEditor -e -viewSelected 0 $editorName;\n            modelEditor -e \n                -pluginObjects \"gpuCacheDisplayFilter\" 1 \n"
		+ "                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tmodelPanel -edit -l (localizedPanelLabel(\"Side View\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        modelEditor -e \n            -camera \"camera1\" \n            -useInteractiveMode 0\n            -displayLights \"default\" \n            -displayAppearance \"wireframe\" \n            -activeOnly 0\n            -ignorePanZoom 0\n            -wireframeOnShaded 0\n            -headsUpDisplay 1\n            -selectionHiliteDisplay 1\n            -useDefaultMaterial 0\n            -bufferMode \"double\" \n            -twoSidedLighting 1\n            -backfaceCulling 0\n            -xray 0\n            -jointXray 0\n            -activeComponentsXray 0\n            -displayTextures 1\n            -smoothWireframe 0\n            -lineWidth 1\n            -textureAnisotropic 0\n            -textureHilight 1\n            -textureSampling 2\n            -textureDisplay \"modulate\" \n            -textureMaxSize 8192\n            -fogging 0\n            -fogSource \"fragment\" \n"
		+ "            -fogMode \"linear\" \n            -fogStart 0\n            -fogEnd 100\n            -fogDensity 0.1\n            -fogColor 0.5 0.5 0.5 1 \n            -maxConstantTransparency 1\n            -rendererName \"base_OpenGL_Renderer\" \n            -objectFilterShowInHUD 1\n            -isFiltered 0\n            -colorResolution 256 256 \n            -bumpResolution 512 512 \n            -textureCompression 0\n            -transparencyAlgorithm \"frontAndBackCull\" \n            -transpInShadows 0\n            -cullingOverride \"none\" \n            -lowQualityLighting 0\n            -maximumNumHardwareLights 1\n            -occlusionCulling 0\n            -shadingModel 0\n            -useBaseRenderer 0\n            -useReducedRenderer 0\n            -smallObjectCulling 0\n            -smallObjectThreshold -1 \n            -interactiveDisableShadows 0\n            -interactiveBackFaceCull 0\n            -sortTransparent 1\n            -nurbsCurves 1\n            -nurbsSurfaces 1\n            -polymeshes 1\n            -subdivSurfaces 1\n            -planes 1\n"
		+ "            -lights 1\n            -cameras 1\n            -controlVertices 1\n            -hulls 1\n            -grid 1\n            -imagePlane 1\n            -joints 1\n            -ikHandles 1\n            -deformers 1\n            -dynamics 1\n            -fluids 1\n            -hairSystems 1\n            -follicles 1\n            -nCloths 1\n            -nParticles 1\n            -nRigids 1\n            -dynamicConstraints 1\n            -locators 1\n            -manipulators 1\n            -pluginShapes 1\n            -dimensions 1\n            -handles 1\n            -pivots 1\n            -textures 1\n            -strokes 1\n            -motionTrails 1\n            -clipGhosts 1\n            -greasePencils 1\n            -shadows 0\n            $editorName;\n        modelEditor -e -viewSelected 0 $editorName;\n        modelEditor -e \n            -pluginObjects \"gpuCacheDisplayFilter\" 1 \n            $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"modelPanel\" (localizedPanelLabel(\"Front View\")) `;\n"
		+ "\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `modelPanel -unParent -l (localizedPanelLabel(\"Front View\")) -mbv $menusOkayInPanels `;\n\t\t\t$editorName = $panelName;\n            modelEditor -e \n                -camera \"front\" \n                -useInteractiveMode 0\n                -displayLights \"default\" \n                -displayAppearance \"wireframe\" \n                -activeOnly 0\n                -ignorePanZoom 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n                -activeComponentsXray 0\n                -displayTextures 0\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n"
		+ "                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n                -rendererName \"base_OpenGL_Renderer\" \n                -objectFilterShowInHUD 1\n                -isFiltered 0\n                -colorResolution 256 256 \n                -bumpResolution 512 512 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n                -lowQualityLighting 0\n                -maximumNumHardwareLights 1\n                -occlusionCulling 0\n                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n                -interactiveDisableShadows 0\n"
		+ "                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n                -imagePlane 1\n                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -pluginShapes 1\n                -dimensions 1\n                -handles 1\n                -pivots 1\n                -textures 1\n                -strokes 1\n                -motionTrails 1\n                -clipGhosts 1\n                -greasePencils 1\n"
		+ "                -shadows 0\n                $editorName;\n            modelEditor -e -viewSelected 0 $editorName;\n            modelEditor -e \n                -pluginObjects \"gpuCacheDisplayFilter\" 1 \n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tmodelPanel -edit -l (localizedPanelLabel(\"Front View\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        modelEditor -e \n            -camera \"front\" \n            -useInteractiveMode 0\n            -displayLights \"default\" \n            -displayAppearance \"wireframe\" \n            -activeOnly 0\n            -ignorePanZoom 0\n            -wireframeOnShaded 0\n            -headsUpDisplay 1\n            -selectionHiliteDisplay 1\n            -useDefaultMaterial 0\n            -bufferMode \"double\" \n            -twoSidedLighting 1\n            -backfaceCulling 0\n            -xray 0\n            -jointXray 0\n            -activeComponentsXray 0\n            -displayTextures 0\n            -smoothWireframe 0\n            -lineWidth 1\n"
		+ "            -textureAnisotropic 0\n            -textureHilight 1\n            -textureSampling 2\n            -textureDisplay \"modulate\" \n            -textureMaxSize 8192\n            -fogging 0\n            -fogSource \"fragment\" \n            -fogMode \"linear\" \n            -fogStart 0\n            -fogEnd 100\n            -fogDensity 0.1\n            -fogColor 0.5 0.5 0.5 1 \n            -maxConstantTransparency 1\n            -rendererName \"base_OpenGL_Renderer\" \n            -objectFilterShowInHUD 1\n            -isFiltered 0\n            -colorResolution 256 256 \n            -bumpResolution 512 512 \n            -textureCompression 0\n            -transparencyAlgorithm \"frontAndBackCull\" \n            -transpInShadows 0\n            -cullingOverride \"none\" \n            -lowQualityLighting 0\n            -maximumNumHardwareLights 1\n            -occlusionCulling 0\n            -shadingModel 0\n            -useBaseRenderer 0\n            -useReducedRenderer 0\n            -smallObjectCulling 0\n            -smallObjectThreshold -1 \n"
		+ "            -interactiveDisableShadows 0\n            -interactiveBackFaceCull 0\n            -sortTransparent 1\n            -nurbsCurves 1\n            -nurbsSurfaces 1\n            -polymeshes 1\n            -subdivSurfaces 1\n            -planes 1\n            -lights 1\n            -cameras 1\n            -controlVertices 1\n            -hulls 1\n            -grid 1\n            -imagePlane 1\n            -joints 1\n            -ikHandles 1\n            -deformers 1\n            -dynamics 1\n            -fluids 1\n            -hairSystems 1\n            -follicles 1\n            -nCloths 1\n            -nParticles 1\n            -nRigids 1\n            -dynamicConstraints 1\n            -locators 1\n            -manipulators 1\n            -pluginShapes 1\n            -dimensions 1\n            -handles 1\n            -pivots 1\n            -textures 1\n            -strokes 1\n            -motionTrails 1\n            -clipGhosts 1\n            -greasePencils 1\n            -shadows 0\n            $editorName;\n        modelEditor -e -viewSelected 0 $editorName;\n"
		+ "        modelEditor -e \n            -pluginObjects \"gpuCacheDisplayFilter\" 1 \n            $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"modelPanel\" (localizedPanelLabel(\"Persp View\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `modelPanel -unParent -l (localizedPanelLabel(\"Persp View\")) -mbv $menusOkayInPanels `;\n\t\t\t$editorName = $panelName;\n            modelEditor -e \n                -camera \"persp\" \n                -useInteractiveMode 0\n                -displayLights \"default\" \n                -displayAppearance \"wireframe\" \n                -activeOnly 0\n                -ignorePanZoom 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n                -activeComponentsXray 0\n"
		+ "                -displayTextures 1\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n                -rendererName \"base_OpenGL_Renderer\" \n                -objectFilterShowInHUD 1\n                -isFiltered 0\n                -colorResolution 256 256 \n                -bumpResolution 512 512 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n                -lowQualityLighting 0\n                -maximumNumHardwareLights 1\n                -occlusionCulling 0\n"
		+ "                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n                -imagePlane 1\n                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -pluginShapes 1\n"
		+ "                -dimensions 1\n                -handles 1\n                -pivots 1\n                -textures 1\n                -strokes 1\n                -motionTrails 1\n                -clipGhosts 1\n                -greasePencils 1\n                -shadows 0\n                $editorName;\n            modelEditor -e -viewSelected 0 $editorName;\n            modelEditor -e \n                -pluginObjects \"gpuCacheDisplayFilter\" 1 \n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tmodelPanel -edit -l (localizedPanelLabel(\"Persp View\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        modelEditor -e \n            -camera \"persp\" \n            -useInteractiveMode 0\n            -displayLights \"default\" \n            -displayAppearance \"wireframe\" \n            -activeOnly 0\n            -ignorePanZoom 0\n            -wireframeOnShaded 0\n            -headsUpDisplay 1\n            -selectionHiliteDisplay 1\n            -useDefaultMaterial 0\n            -bufferMode \"double\" \n"
		+ "            -twoSidedLighting 1\n            -backfaceCulling 0\n            -xray 0\n            -jointXray 0\n            -activeComponentsXray 0\n            -displayTextures 1\n            -smoothWireframe 0\n            -lineWidth 1\n            -textureAnisotropic 0\n            -textureHilight 1\n            -textureSampling 2\n            -textureDisplay \"modulate\" \n            -textureMaxSize 8192\n            -fogging 0\n            -fogSource \"fragment\" \n            -fogMode \"linear\" \n            -fogStart 0\n            -fogEnd 100\n            -fogDensity 0.1\n            -fogColor 0.5 0.5 0.5 1 \n            -maxConstantTransparency 1\n            -rendererName \"base_OpenGL_Renderer\" \n            -objectFilterShowInHUD 1\n            -isFiltered 0\n            -colorResolution 256 256 \n            -bumpResolution 512 512 \n            -textureCompression 0\n            -transparencyAlgorithm \"frontAndBackCull\" \n            -transpInShadows 0\n            -cullingOverride \"none\" \n            -lowQualityLighting 0\n            -maximumNumHardwareLights 1\n"
		+ "            -occlusionCulling 0\n            -shadingModel 0\n            -useBaseRenderer 0\n            -useReducedRenderer 0\n            -smallObjectCulling 0\n            -smallObjectThreshold -1 \n            -interactiveDisableShadows 0\n            -interactiveBackFaceCull 0\n            -sortTransparent 1\n            -nurbsCurves 1\n            -nurbsSurfaces 1\n            -polymeshes 1\n            -subdivSurfaces 1\n            -planes 1\n            -lights 1\n            -cameras 1\n            -controlVertices 1\n            -hulls 1\n            -grid 1\n            -imagePlane 1\n            -joints 1\n            -ikHandles 1\n            -deformers 1\n            -dynamics 1\n            -fluids 1\n            -hairSystems 1\n            -follicles 1\n            -nCloths 1\n            -nParticles 1\n            -nRigids 1\n            -dynamicConstraints 1\n            -locators 1\n            -manipulators 1\n            -pluginShapes 1\n            -dimensions 1\n            -handles 1\n            -pivots 1\n            -textures 1\n"
		+ "            -strokes 1\n            -motionTrails 1\n            -clipGhosts 1\n            -greasePencils 1\n            -shadows 0\n            $editorName;\n        modelEditor -e -viewSelected 0 $editorName;\n        modelEditor -e \n            -pluginObjects \"gpuCacheDisplayFilter\" 1 \n            $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"outlinerPanel\" (localizedPanelLabel(\"Outliner\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `outlinerPanel -unParent -l (localizedPanelLabel(\"Outliner\")) -mbv $menusOkayInPanels `;\n\t\t\t$editorName = $panelName;\n            outlinerEditor -e \n                -showShapes 0\n                -showReferenceNodes 1\n                -showReferenceMembers 1\n                -showAttributes 0\n                -showConnected 0\n                -showAnimCurvesOnly 0\n                -showMuteInfo 0\n                -organizeByLayer 1\n                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n"
		+ "                -autoExpand 0\n                -showDagOnly 1\n                -showAssets 1\n                -showContainedOnly 1\n                -showPublishedAsConnected 0\n                -showContainerContents 1\n                -ignoreDagHierarchy 0\n                -expandConnections 0\n                -showUpstreamCurves 1\n                -showUnitlessCurves 1\n                -showCompounds 1\n                -showLeafs 1\n                -showNumericAttrsOnly 0\n                -highlightActive 1\n                -autoSelectNewObjects 0\n                -doNotSelectNewObjects 0\n                -dropIsParent 1\n                -transmitFilters 0\n                -setFilter \"defaultSetFilter\" \n                -showSetMembers 1\n                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -displayMode \"DAG\" \n                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n                -showAttrValues 0\n"
		+ "                -highlightSecondary 0\n                -showUVAttrsOnly 0\n                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                -showPinIcons 0\n                -mapMotionTrails 0\n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\toutlinerPanel -edit -l (localizedPanelLabel(\"Outliner\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        outlinerEditor -e \n            -showShapes 0\n            -showReferenceNodes 1\n            -showReferenceMembers 1\n            -showAttributes 0\n            -showConnected 0\n            -showAnimCurvesOnly 0\n            -showMuteInfo 0\n            -organizeByLayer 1\n            -showAnimLayerWeight 1\n            -autoExpandLayers 1\n            -autoExpand 0\n            -showDagOnly 1\n            -showAssets 1\n"
		+ "            -showContainedOnly 1\n            -showPublishedAsConnected 0\n            -showContainerContents 1\n            -ignoreDagHierarchy 0\n            -expandConnections 0\n            -showUpstreamCurves 1\n            -showUnitlessCurves 1\n            -showCompounds 1\n            -showLeafs 1\n            -showNumericAttrsOnly 0\n            -highlightActive 1\n            -autoSelectNewObjects 0\n            -doNotSelectNewObjects 0\n            -dropIsParent 1\n            -transmitFilters 0\n            -setFilter \"defaultSetFilter\" \n            -showSetMembers 1\n            -allowMultiSelection 1\n            -alwaysToggleSelect 0\n            -directSelect 0\n            -displayMode \"DAG\" \n            -expandObjects 0\n            -setsIgnoreFilters 1\n            -containersIgnoreFilters 0\n            -editAttrName 0\n            -showAttrValues 0\n            -highlightSecondary 0\n            -showUVAttrsOnly 0\n            -showTextureNodesOnly 0\n            -attrAlphaOrder \"default\" \n            -animLayerFilterOptions \"allAffecting\" \n"
		+ "            -sortOrder \"none\" \n            -longNames 0\n            -niceNames 1\n            -showNamespace 1\n            -showPinIcons 0\n            -mapMotionTrails 0\n            $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"graphEditor\" (localizedPanelLabel(\"Graph Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"graphEditor\" -l (localizedPanelLabel(\"Graph Editor\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = ($panelName+\"OutlineEd\");\n            outlinerEditor -e \n                -showShapes 1\n                -showReferenceNodes 0\n                -showReferenceMembers 0\n                -showAttributes 1\n                -showConnected 1\n                -showAnimCurvesOnly 1\n                -showMuteInfo 0\n                -organizeByLayer 1\n                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n                -autoExpand 1\n                -showDagOnly 0\n"
		+ "                -showAssets 1\n                -showContainedOnly 0\n                -showPublishedAsConnected 0\n                -showContainerContents 0\n                -ignoreDagHierarchy 0\n                -expandConnections 1\n                -showUpstreamCurves 1\n                -showUnitlessCurves 1\n                -showCompounds 0\n                -showLeafs 1\n                -showNumericAttrsOnly 1\n                -highlightActive 0\n                -autoSelectNewObjects 1\n                -doNotSelectNewObjects 0\n                -dropIsParent 1\n                -transmitFilters 1\n                -setFilter \"0\" \n                -showSetMembers 0\n                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -displayMode \"DAG\" \n                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n                -showAttrValues 0\n                -highlightSecondary 0\n                -showUVAttrsOnly 0\n"
		+ "                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                -showPinIcons 1\n                -mapMotionTrails 1\n                $editorName;\n\n\t\t\t$editorName = ($panelName+\"GraphEd\");\n            animCurveEditor -e \n                -displayKeys 1\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 1\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"integer\" \n                -snapValue \"none\" \n                -showResults \"off\" \n                -showBufferCurves \"off\" \n                -smoothness \"fine\" \n                -resultSamples 1\n                -resultScreenSamples 0\n                -resultUpdate \"delayed\" \n                -showUpstreamCurves 1\n                -stackedCurves 0\n                -stackedCurvesMin -1\n"
		+ "                -stackedCurvesMax 1\n                -stackedCurvesSpace 0.2\n                -displayNormalized 0\n                -preSelectionHighlight 0\n                -constrainDrag 0\n                -classicMode 1\n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Graph Editor\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = ($panelName+\"OutlineEd\");\n            outlinerEditor -e \n                -showShapes 1\n                -showReferenceNodes 0\n                -showReferenceMembers 0\n                -showAttributes 1\n                -showConnected 1\n                -showAnimCurvesOnly 1\n                -showMuteInfo 0\n                -organizeByLayer 1\n                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n                -autoExpand 1\n                -showDagOnly 0\n                -showAssets 1\n                -showContainedOnly 0\n                -showPublishedAsConnected 0\n                -showContainerContents 0\n"
		+ "                -ignoreDagHierarchy 0\n                -expandConnections 1\n                -showUpstreamCurves 1\n                -showUnitlessCurves 1\n                -showCompounds 0\n                -showLeafs 1\n                -showNumericAttrsOnly 1\n                -highlightActive 0\n                -autoSelectNewObjects 1\n                -doNotSelectNewObjects 0\n                -dropIsParent 1\n                -transmitFilters 1\n                -setFilter \"0\" \n                -showSetMembers 0\n                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -displayMode \"DAG\" \n                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n                -showAttrValues 0\n                -highlightSecondary 0\n                -showUVAttrsOnly 0\n                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n"
		+ "                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                -showPinIcons 1\n                -mapMotionTrails 1\n                $editorName;\n\n\t\t\t$editorName = ($panelName+\"GraphEd\");\n            animCurveEditor -e \n                -displayKeys 1\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 1\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"integer\" \n                -snapValue \"none\" \n                -showResults \"off\" \n                -showBufferCurves \"off\" \n                -smoothness \"fine\" \n                -resultSamples 1\n                -resultScreenSamples 0\n                -resultUpdate \"delayed\" \n                -showUpstreamCurves 1\n                -stackedCurves 0\n                -stackedCurvesMin -1\n                -stackedCurvesMax 1\n                -stackedCurvesSpace 0.2\n                -displayNormalized 0\n"
		+ "                -preSelectionHighlight 0\n                -constrainDrag 0\n                -classicMode 1\n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"dopeSheetPanel\" (localizedPanelLabel(\"Dope Sheet\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"dopeSheetPanel\" -l (localizedPanelLabel(\"Dope Sheet\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = ($panelName+\"OutlineEd\");\n            outlinerEditor -e \n                -showShapes 1\n                -showReferenceNodes 0\n                -showReferenceMembers 0\n                -showAttributes 1\n                -showConnected 1\n                -showAnimCurvesOnly 1\n                -showMuteInfo 0\n                -organizeByLayer 1\n                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n                -autoExpand 0\n                -showDagOnly 0\n                -showAssets 1\n                -showContainedOnly 0\n"
		+ "                -showPublishedAsConnected 0\n                -showContainerContents 0\n                -ignoreDagHierarchy 0\n                -expandConnections 1\n                -showUpstreamCurves 1\n                -showUnitlessCurves 0\n                -showCompounds 1\n                -showLeafs 1\n                -showNumericAttrsOnly 1\n                -highlightActive 0\n                -autoSelectNewObjects 0\n                -doNotSelectNewObjects 1\n                -dropIsParent 1\n                -transmitFilters 0\n                -setFilter \"0\" \n                -showSetMembers 0\n                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -displayMode \"DAG\" \n                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n                -showAttrValues 0\n                -highlightSecondary 0\n                -showUVAttrsOnly 0\n                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n"
		+ "                -animLayerFilterOptions \"allAffecting\" \n                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                -showPinIcons 0\n                -mapMotionTrails 1\n                $editorName;\n\n\t\t\t$editorName = ($panelName+\"DopeSheetEd\");\n            dopeSheetEditor -e \n                -displayKeys 1\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"integer\" \n                -snapValue \"none\" \n                -outliner \"dopeSheetPanel1OutlineEd\" \n                -showSummary 1\n                -showScene 0\n                -hierarchyBelow 0\n                -showTicks 1\n                -selectionWindow 0 0 0 0 \n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Dope Sheet\")) -mbv $menusOkayInPanels  $panelName;\n"
		+ "\t\t\t$editorName = ($panelName+\"OutlineEd\");\n            outlinerEditor -e \n                -showShapes 1\n                -showReferenceNodes 0\n                -showReferenceMembers 0\n                -showAttributes 1\n                -showConnected 1\n                -showAnimCurvesOnly 1\n                -showMuteInfo 0\n                -organizeByLayer 1\n                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n                -autoExpand 0\n                -showDagOnly 0\n                -showAssets 1\n                -showContainedOnly 0\n                -showPublishedAsConnected 0\n                -showContainerContents 0\n                -ignoreDagHierarchy 0\n                -expandConnections 1\n                -showUpstreamCurves 1\n                -showUnitlessCurves 0\n                -showCompounds 1\n                -showLeafs 1\n                -showNumericAttrsOnly 1\n                -highlightActive 0\n                -autoSelectNewObjects 0\n                -doNotSelectNewObjects 1\n                -dropIsParent 1\n"
		+ "                -transmitFilters 0\n                -setFilter \"0\" \n                -showSetMembers 0\n                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -displayMode \"DAG\" \n                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n                -showAttrValues 0\n                -highlightSecondary 0\n                -showUVAttrsOnly 0\n                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                -showPinIcons 0\n                -mapMotionTrails 1\n                $editorName;\n\n\t\t\t$editorName = ($panelName+\"DopeSheetEd\");\n            dopeSheetEditor -e \n                -displayKeys 1\n                -displayTangents 0\n                -displayActiveKeys 0\n"
		+ "                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"integer\" \n                -snapValue \"none\" \n                -outliner \"dopeSheetPanel1OutlineEd\" \n                -showSummary 1\n                -showScene 0\n                -hierarchyBelow 0\n                -showTicks 1\n                -selectionWindow 0 0 0 0 \n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"clipEditorPanel\" (localizedPanelLabel(\"Trax Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"clipEditorPanel\" -l (localizedPanelLabel(\"Trax Editor\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = clipEditorNameFromPanel($panelName);\n            clipEditor -e \n                -displayKeys 0\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n"
		+ "                -autoFit 0\n                -snapTime \"none\" \n                -snapValue \"none\" \n                -manageSequencer 0 \n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Trax Editor\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = clipEditorNameFromPanel($panelName);\n            clipEditor -e \n                -displayKeys 0\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"none\" \n                -snapValue \"none\" \n                -manageSequencer 0 \n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"sequenceEditorPanel\" (localizedPanelLabel(\"Camera Sequencer\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"sequenceEditorPanel\" -l (localizedPanelLabel(\"Camera Sequencer\")) -mbv $menusOkayInPanels `;\n"
		+ "\t\t\t$editorName = sequenceEditorNameFromPanel($panelName);\n            clipEditor -e \n                -displayKeys 0\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"none\" \n                -snapValue \"none\" \n                -manageSequencer 1 \n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Camera Sequencer\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = sequenceEditorNameFromPanel($panelName);\n            clipEditor -e \n                -displayKeys 0\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"none\" \n                -snapValue \"none\" \n                -manageSequencer 1 \n                $editorName;\n\t\tif (!$useSceneConfig) {\n"
		+ "\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"hyperGraphPanel\" (localizedPanelLabel(\"Hypergraph Hierarchy\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"hyperGraphPanel\" -l (localizedPanelLabel(\"Hypergraph Hierarchy\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = ($panelName+\"HyperGraphEd\");\n            hyperGraph -e \n                -graphLayoutStyle \"hierarchicalLayout\" \n                -orientation \"horiz\" \n                -mergeConnections 1\n                -zoom 0.875796\n                -animateTransition 0\n                -showRelationships 1\n                -showShapes 0\n                -showDeformers 0\n                -showExpressions 0\n                -showConstraints 0\n                -showConnectionFromSelected 0\n                -showConnectionToSelected 0\n                -showUnderworld 0\n                -showInvisible 0\n                -transitionFrames 1\n                -opaqueContainers 0\n"
		+ "                -freeform 0\n                -imagePosition 0 0 \n                -imageScale 1\n                -imageEnabled 0\n                -graphType \"DAG\" \n                -heatMapDisplay 0\n                -updateSelection 1\n                -updateNodeAdded 1\n                -useDrawOverrideColor 0\n                -limitGraphTraversal -1\n                -range 0 0 \n                -iconSize \"largeIcons\" \n                -showCachedConnections 0\n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Hypergraph Hierarchy\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = ($panelName+\"HyperGraphEd\");\n            hyperGraph -e \n                -graphLayoutStyle \"hierarchicalLayout\" \n                -orientation \"horiz\" \n                -mergeConnections 1\n                -zoom 0.875796\n                -animateTransition 0\n                -showRelationships 1\n                -showShapes 0\n                -showDeformers 0\n                -showExpressions 0\n"
		+ "                -showConstraints 0\n                -showConnectionFromSelected 0\n                -showConnectionToSelected 0\n                -showUnderworld 0\n                -showInvisible 0\n                -transitionFrames 1\n                -opaqueContainers 0\n                -freeform 0\n                -imagePosition 0 0 \n                -imageScale 1\n                -imageEnabled 0\n                -graphType \"DAG\" \n                -heatMapDisplay 0\n                -updateSelection 1\n                -updateNodeAdded 1\n                -useDrawOverrideColor 0\n                -limitGraphTraversal -1\n                -range 0 0 \n                -iconSize \"largeIcons\" \n                -showCachedConnections 0\n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"hyperShadePanel\" (localizedPanelLabel(\"Hypershade\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"hyperShadePanel\" -l (localizedPanelLabel(\"Hypershade\")) -mbv $menusOkayInPanels `;\n"
		+ "\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Hypershade\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"visorPanel\" (localizedPanelLabel(\"Visor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"visorPanel\" -l (localizedPanelLabel(\"Visor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Visor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"nodeEditorPanel\" (localizedPanelLabel(\"Node Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"nodeEditorPanel\" -l (localizedPanelLabel(\"Node Editor\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = ($panelName+\"NodeEditorEd\");\n"
		+ "            nodeEditor -e \n                -allAttributes 0\n                -allNodes 0\n                -autoSizeNodes 1\n                -createNodeCommand \"nodeEdCreateNodeCommand\" \n                -defaultPinnedState 0\n                -ignoreAssets 1\n                -additiveGraphingMode 0\n                -settingsChangedCallback \"nodeEdSyncControls\" \n                -traversalDepthLimit -1\n                -keyPressCommand \"nodeEdKeyPressCommand\" \n                -keyReleaseCommand \"nodeEdKeyReleaseCommand\" \n                -nodeTitleMode \"name\" \n                -gridSnap 0\n                -gridVisibility 1\n                -popupMenuScript \"nodeEdBuildPanelMenus\" \n                -island 0\n                -showNamespace 1\n                -showShapes 1\n                -showSGShapes 0\n                -showTransforms 1\n                -syncedSelection 1\n                -extendToShapes 1\n                $editorName;\n\t\t\tif (`objExists nodeEditorPanel1Info`) nodeEditor -e -restoreInfo nodeEditorPanel1Info $editorName;\n"
		+ "\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Node Editor\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = ($panelName+\"NodeEditorEd\");\n            nodeEditor -e \n                -allAttributes 0\n                -allNodes 0\n                -autoSizeNodes 1\n                -createNodeCommand \"nodeEdCreateNodeCommand\" \n                -defaultPinnedState 0\n                -ignoreAssets 1\n                -additiveGraphingMode 0\n                -settingsChangedCallback \"nodeEdSyncControls\" \n                -traversalDepthLimit -1\n                -keyPressCommand \"nodeEdKeyPressCommand\" \n                -keyReleaseCommand \"nodeEdKeyReleaseCommand\" \n                -nodeTitleMode \"name\" \n                -gridSnap 0\n                -gridVisibility 1\n                -popupMenuScript \"nodeEdBuildPanelMenus\" \n                -island 0\n                -showNamespace 1\n                -showShapes 1\n                -showSGShapes 0\n                -showTransforms 1\n"
		+ "                -syncedSelection 1\n                -extendToShapes 1\n                $editorName;\n\t\t\tif (`objExists nodeEditorPanel1Info`) nodeEditor -e -restoreInfo nodeEditorPanel1Info $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"createNodePanel\" (localizedPanelLabel(\"Create Node\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"createNodePanel\" -l (localizedPanelLabel(\"Create Node\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Create Node\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"polyTexturePlacementPanel\" (localizedPanelLabel(\"UV Texture Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"polyTexturePlacementPanel\" -l (localizedPanelLabel(\"UV Texture Editor\")) -mbv $menusOkayInPanels `;\n"
		+ "\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"UV Texture Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"renderWindowPanel\" (localizedPanelLabel(\"Render View\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"renderWindowPanel\" -l (localizedPanelLabel(\"Render View\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Render View\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"blendShapePanel\" (localizedPanelLabel(\"Blend Shape\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\tblendShapePanel -unParent -l (localizedPanelLabel(\"Blend Shape\")) -mbv $menusOkayInPanels ;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n"
		+ "\t\tblendShapePanel -edit -l (localizedPanelLabel(\"Blend Shape\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"dynRelEdPanel\" (localizedPanelLabel(\"Dynamic Relationships\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"dynRelEdPanel\" -l (localizedPanelLabel(\"Dynamic Relationships\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Dynamic Relationships\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"relationshipPanel\" (localizedPanelLabel(\"Relationship Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"relationshipPanel\" -l (localizedPanelLabel(\"Relationship Editor\")) -mbv $menusOkayInPanels `;\n"
		+ "\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Relationship Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"referenceEditorPanel\" (localizedPanelLabel(\"Reference Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"referenceEditorPanel\" -l (localizedPanelLabel(\"Reference Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Reference Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"componentEditorPanel\" (localizedPanelLabel(\"Component Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"componentEditorPanel\" -l (localizedPanelLabel(\"Component Editor\")) -mbv $menusOkayInPanels `;\n"
		+ "\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Component Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"dynPaintScriptedPanelType\" (localizedPanelLabel(\"Paint Effects\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"dynPaintScriptedPanelType\" -l (localizedPanelLabel(\"Paint Effects\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Paint Effects\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"scriptEditorPanel\" (localizedPanelLabel(\"Script Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"scriptEditorPanel\" -l (localizedPanelLabel(\"Script Editor\")) -mbv $menusOkayInPanels `;\n"
		+ "\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Script Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\tif ($useSceneConfig) {\n        string $configName = `getPanel -cwl (localizedPanelLabel(\"Current Layout\"))`;\n        if (\"\" != $configName) {\n\t\t\tpanelConfiguration -edit -label (localizedPanelLabel(\"Current Layout\")) \n\t\t\t\t-defaultImage \"\"\n\t\t\t\t-image \"\"\n\t\t\t\t-sc false\n\t\t\t\t-configString \"global string $gMainPane; paneLayout -e -cn \\\"single\\\" -ps 1 100 100 $gMainPane;\"\n\t\t\t\t-removeAllPanels\n\t\t\t\t-ap false\n\t\t\t\t\t(localizedPanelLabel(\"Persp View\")) \n\t\t\t\t\t\"modelPanel\"\n"
		+ "\t\t\t\t\t\"$panelName = `modelPanel -unParent -l (localizedPanelLabel(\\\"Persp View\\\")) -mbv $menusOkayInPanels `;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera persp` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"wireframe\\\" \\n    -activeOnly 0\\n    -ignorePanZoom 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 1\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -objectFilterShowInHUD 1\\n    -isFiltered 0\\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -imagePlane 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -pluginShapes 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -motionTrails 1\\n    -clipGhosts 1\\n    -greasePencils 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName;\\nmodelEditor -e \\n    -pluginObjects \\\"gpuCacheDisplayFilter\\\" 1 \\n    $editorName\"\n"
		+ "\t\t\t\t\t\"modelPanel -edit -l (localizedPanelLabel(\\\"Persp View\\\")) -mbv $menusOkayInPanels  $panelName;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera persp` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"wireframe\\\" \\n    -activeOnly 0\\n    -ignorePanZoom 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 1\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -objectFilterShowInHUD 1\\n    -isFiltered 0\\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -imagePlane 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -pluginShapes 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -motionTrails 1\\n    -clipGhosts 1\\n    -greasePencils 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName;\\nmodelEditor -e \\n    -pluginObjects \\\"gpuCacheDisplayFilter\\\" 1 \\n    $editorName\"\n"
		+ "\t\t\t\t$configName;\n\n            setNamedPanelLayout (localizedPanelLabel(\"Current Layout\"));\n        }\n\n        panelHistory -e -clear mainPanelHistory;\n        setFocus `paneLayout -q -p1 $gMainPane`;\n        sceneUIReplacement -deleteRemaining;\n        sceneUIReplacement -clear;\n\t}\n\n\ngrid -spacing 5 -size 12 -divisions 5 -displayAxes yes -displayGridLines yes -displayDivisionLines yes -displayPerspectiveLabels no -displayOrthographicLabels no -displayAxesBold yes -perspectiveLabelPosition axis -orthographicLabelPosition edge;\nviewManip -drawCompass 0 -compassAngle 0 -frontParameters \"\" -homeParameters \"\" -selectionLockParameters \"\";\n}\n");
	setAttr ".st" 3;
createNode script -n "sceneConfigurationScriptNode";
	setAttr ".b" -type "string" "playbackOptions -min 1 -max 24 -ast 1 -aet 48 ";
	setAttr ".st" 6;
createNode phong -n "mat_phys";
	setAttr ".c" -type "float3" 0.96024317 0.45647505 0.96100003 ;
	setAttr ".it" -type "float3" 0.63635999 0.63635999 0.63635999 ;
createNode shadingEngine -n "phong1SG";
	setAttr ".ihi" 0;
	setAttr -s 5 ".dsm";
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo1";
createNode phong -n "mat_env";
	setAttr ".dc" 1;
	setAttr ".sc" -type "float3" 0 0 0 ;
	setAttr ".rfl" 0;
	setAttr ".cp" 2;
createNode shadingEngine -n "phong2SG";
	setAttr ".ihi" 0;
	setAttr -s 4 ".dsm";
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo2";
createNode file -n "file1";
	setAttr ".ftn" -type "string" "C:/RnD/pd/Fire/Data/lvl14.jpg";
createNode place2dTexture -n "place2dTexture1";
createNode polySphere -n "polySphere1";
	setAttr ".r" 1.9828444150788869;
createNode polySphere -n "polySphere2";
	setAttr ".r" 1.9828444150788869;
createNode polyCube -n "polyCube1";
	setAttr ".w" 3;
	setAttr ".h" 2;
	setAttr ".d" 7;
	setAttr ".cuv" 4;
createNode polyCube -n "polyCube2";
	setAttr ".w" 150;
	setAttr ".h" 150;
	setAttr ".d" 150;
	setAttr ".cuv" 4;
createNode hyperGraphInfo -n "nodeEditorPanel1Info";
createNode hyperView -n "hyperView1";
	setAttr ".dag" no;
createNode hyperLayout -n "hyperLayout1";
	setAttr ".ihi" 0;
	setAttr ".hyp[0].nvs" 1920;
	setAttr ".anf" yes;
createNode polyPlane -n "polyPlane1";
	setAttr ".uvs" -type "string" "map_env";
	setAttr ".w" 125;
	setAttr ".h" 400;
	setAttr ".sw" 5;
	setAttr ".sh" 4;
	setAttr ".cuv" 2;
createNode polyTweak -n "polyTweak1";
	setAttr ".uopa" yes;
	setAttr -s 30 ".tk[0:29]" -type "float3"  -63.44259644 -9.74977207 -73.21242523
		 -65.33280182 -9.74977207 -63.67631149 -67.2230072 -9.74977207 -54.14017868 -69.11325073
		 -9.74977207 -44.6040535 -71.0034561157 -9.74977207 -35.0679245 -72.89366913 -9.74977207
		 -25.53178215 -25.37959099 -3.21612787 -65.45407104 -27.26980209 -3.21612787 -55.91796494
		 -29.16002083 -3.21612787 -46.38180161 -31.050228119 -3.21612787 -36.84568024 -32.94044876
		 -3.21612787 -27.30955505 -34.83066559 -3.21612787 -17.77342606 12.68341827 3.31751728
		 -57.69568634 10.79320908 3.31751728 -48.15956879 8.90299416 3.31751728 -38.62343979
		 7.012779236 3.31751728 -29.087308884 5.12256622 3.31751728 -19.55117607 3.23235345
		 3.31751728 -10.015053749 50.74642944 9.85116386 -49.93732834 48.85621262 9.85116386
		 -40.40119553 46.96599579 9.85116386 -30.86507225 45.075786591 9.85116386 -21.32894325
		 43.18557358 9.85116386 -11.79280949 41.29536057 9.85116386 -2.25668716 88.80944824
		 16.38480759 -42.17897034 86.91924286 16.38480759 -32.6428299 85.029037476 16.38480759
		 -23.10670853 83.13879395 16.38480759 -13.57057858 81.24858856 16.38480759 -4.03444767
		 79.35837555 16.38480759 5.50167942;
createNode deleteComponent -n "deleteComponent1";
	setAttr ".dc" -type "componentList" 1 "vtx[0]";
createNode deleteComponent -n "deleteComponent2";
	setAttr ".dc" -type "componentList" 0;
createNode polyTweak -n "polyTweak2";
	setAttr ".uopa" yes;
	setAttr -s 29 ".tk[0:28]" -type "float3"  26.051574707 2.82037234 9.092882156
		 2.94178224 2.82037234 -0.44325078 -6.78311348 2.54972959 -0.44709212 -6.78311348
		 2.27908802 -0.4509334 2.94178224 2.0084457397 -0.45477468 10.86194134 2.92209005
		 -0.21298255 10.86194134 2.65144873 -0.21682382 10.86194134 2.38080621 -0.2206651
		 3.82469153 2.11016393 -0.22450638 11.96693897 1.83952212 -0.22834767 10.86194134
		 1.5688802 -0.23218894 10.86194134 5.53734207 0.0096032135 10.86194134 5.26670074
		 0.0057619051 10.86194134 4.99605846 0.0019206435 4.55406094 4.72541523 -0.0019206726
		 16.071493149 4.45477295 -0.0057619358 10.86194134 4.18413115 -0.0096032405 10.86194134
		 8.15259457 0.23218894 10.86194134 7.88195133 0.22834767 10.86194134 7.61130953 0.22450638
		 8.88345528 7.34066725 0.2206651 19.56344986 7.070025444 0.21682385 10.86194134 6.79938364
		 0.21298255 10.86194134 10.76784611 0.45477465 10.86194134 10.49720287 0.45093337
		 10.86194134 10.2265625 0.44709209 17.45079231 9.95591927 0.44325078 28.26444817 9.68527699
		 0.43940955 10.86194134 9.41463566 0.43556824;
createNode deleteComponent -n "deleteComponent3";
	setAttr ".dc" -type "componentList" 1 "vtx[23]";
createNode polyPlane -n "polyPlane2";
	setAttr ".uvs" -type "string" "map_flowers";
	setAttr ".w" 27;
	setAttr ".h" 5;
	setAttr ".sw" 1;
	setAttr ".sh" 1;
	setAttr ".cuv" 2;
createNode polyPlane -n "polyPlane3";
	setAttr ".uvs" -type "string" "map_bkg";
	setAttr ".w" 330;
	setAttr ".h" 153.04976096080131;
	setAttr ".sw" 1;
	setAttr ".sh" 1;
	setAttr ".cuv" 2;
createNode polySplit -n "polySplit1";
	setAttr ".e[0]"  0.33401603;
	setAttr ".d[0]"  -2147483648;
createNode polySplit -n "polySplit2";
	setAttr ".e[0]"  0.56012005;
	setAttr ".d[0]"  -2147483644;
createNode polySplit -n "polySplit3";
	setAttr ".e[0]"  0.67575717;
	setAttr ".d[0]"  -2147483643;
createNode script -n "rg_export";
	addAttr -ci true -sn "time" -ln "time" -dt "string";
	setAttr ".time" -type "string" "2014-03-19T14:38:14.997000";
select -ne :time1;
	setAttr ".o" 1;
	setAttr ".unw" 1;
select -ne :renderPartition;
	setAttr -s 4 ".st";
select -ne :initialShadingGroup;
	setAttr ".ro" yes;
select -ne :initialParticleSE;
	setAttr ".ro" yes;
select -ne :defaultShaderList1;
	setAttr -s 4 ".s";
select -ne :defaultTextureList1;
select -ne :postProcessList1;
	setAttr -s 2 ".p";
select -ne :defaultRenderUtilityList1;
	setAttr -s 2 ".u";
select -ne :defaultRenderingList1;
select -ne :renderGlobalsList1;
select -ne :defaultRenderGlobals;
	setAttr ".ep" 1;
select -ne :defaultResolution;
	setAttr ".w" 640;
	setAttr ".h" 480;
	setAttr ".dar" 1.3333332538604736;
select -ne :hardwareRenderGlobals;
	setAttr ".ctrs" 256;
	setAttr ".btrs" 512;
select -ne :hardwareRenderingGlobals;
	setAttr ".vac" 2;
select -ne :defaultHardwareRenderGlobals;
	setAttr ".fn" -type "string" "im";
	setAttr ".res" -type "string" "ntsc_4d 646 485 1.333";
connectAttr "imagePlane1.msg" "cameraShape1.ip" -na;
connectAttr "cameraShape1.msg" "imagePlane1.ltc";
connectAttr "deleteComponent3.og" "|m_ground|m_phys_groundShape.i";
connectAttr "polyPlane2.out" "m_flowersShape.i";
connectAttr "polySplit3.out" "m_backgroundShape.i";
connectAttr "polyCube1.out" "phys_pos_car_startShape.i";
connectAttr "polySphere1.out" "phys_pos_path_Shape1.i";
connectAttr "polySphere2.out" "|m_ground|phys_pos_path_2|phys_pos_path_Shape2.i"
		;
connectAttr "polyCube2.out" "phys_trig_car_eaterShape.i";
relationship "link" ":lightLinker1" ":initialShadingGroup.message" ":defaultLightSet.message";
relationship "link" ":lightLinker1" ":initialParticleSE.message" ":defaultLightSet.message";
relationship "link" ":lightLinker1" "phong1SG.message" ":defaultLightSet.message";
relationship "link" ":lightLinker1" "phong2SG.message" ":defaultLightSet.message";
relationship "shadowLink" ":lightLinker1" ":initialShadingGroup.message" ":defaultLightSet.message";
relationship "shadowLink" ":lightLinker1" ":initialParticleSE.message" ":defaultLightSet.message";
relationship "shadowLink" ":lightLinker1" "phong1SG.message" ":defaultLightSet.message";
relationship "shadowLink" ":lightLinker1" "phong2SG.message" ":defaultLightSet.message";
connectAttr "layerManager.dli[0]" "defaultLayer.id";
connectAttr "renderLayerManager.rlmi[0]" "defaultRenderLayer.rlid";
connectAttr "mat_phys.oc" "phong1SG.ss";
connectAttr "phys_pos_car_startShape.iog" "phong1SG.dsm" -na;
connectAttr "phys_pos_path_Shape1.iog" "phong1SG.dsm" -na;
connectAttr "|m_ground|phys_pos_path_2|phys_pos_path_Shape2.iog" "phong1SG.dsm" 
		-na;
connectAttr "phys_trig_car_eaterShape.iog" "phong1SG.dsm" -na;
connectAttr "|m_ground|phys_pos_path_3|phys_pos_path_Shape2.iog" "phong1SG.dsm" 
		-na;
connectAttr "phong1SG.msg" "materialInfo1.sg";
connectAttr "mat_phys.msg" "materialInfo1.m";
connectAttr "file1.oc" "mat_env.c";
connectAttr "mat_env.oc" "phong2SG.ss";
connectAttr "m_backgroundShape.iog" "phong2SG.dsm" -na;
connectAttr "|m_ground|m_phys_groundShape.iog" "phong2SG.dsm" -na;
connectAttr "m_flowersShape.iog" "phong2SG.dsm" -na;
connectAttr "|m_ground|phys_ground|m_phys_groundShape.iog" "phong2SG.dsm" -na;
connectAttr "phong2SG.msg" "materialInfo2.sg";
connectAttr "mat_env.msg" "materialInfo2.m";
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
connectAttr "hyperView1.msg" "nodeEditorPanel1Info.b[0]";
connectAttr "hyperLayout1.msg" "hyperView1.hl";
connectAttr "polyPlane1.out" "polyTweak1.ip";
connectAttr "polyTweak1.out" "deleteComponent1.ig";
connectAttr "deleteComponent1.og" "deleteComponent2.ig";
connectAttr "deleteComponent2.og" "polyTweak2.ip";
connectAttr "polyTweak2.out" "deleteComponent3.ig";
connectAttr "polyPlane3.out" "polySplit1.ip";
connectAttr "polySplit1.out" "polySplit2.ip";
connectAttr "polySplit2.out" "polySplit3.ip";
connectAttr "phong1SG.pa" ":renderPartition.st" -na;
connectAttr "phong2SG.pa" ":renderPartition.st" -na;
connectAttr "mat_phys.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_env.msg" ":defaultShaderList1.s" -na;
connectAttr "file1.msg" ":defaultTextureList1.tx" -na;
connectAttr "imagePlane1.msg" ":defaultRenderUtilityList1.u" -na;
connectAttr "place2dTexture1.msg" ":defaultRenderUtilityList1.u" -na;
connectAttr "defaultRenderLayer.msg" ":defaultRenderingList1.r" -na;
// End of lvl14.ma
