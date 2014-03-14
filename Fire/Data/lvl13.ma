//Maya ASCII 2014 scene
//Name: lvl13.ma
//Last modified: Fri, Mar 14, 2014 02:38:22 PM
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
	setAttr ".t" -type "double3" -3.6167082785984128 4.6516300415364329 -11.259774125451635 ;
	setAttr ".r" -type "double3" -8.1383527299073499 374.19999999989119 0 ;
	setAttr ".rp" -type "double3" 0 -3.6637359812630166e-015 7.1054273576010019e-015 ;
	setAttr ".rpt" -type "double3" 7.5981871650220083e-014 1.6471638655843653e-014 1.307148822229979e-013 ;
createNode camera -s -n "perspShape" -p "persp";
	setAttr -k off ".v" no;
	setAttr ".rnd" no;
	setAttr ".fl" 34.999999999999979;
	setAttr ".fcp" 3000;
	setAttr ".coi" 133.75531851563341;
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
	setAttr ".imn" -type "string" "C:/RnD/pd/Fire/Data/lvl13.jpg";
	setAttr ".cov" -type "short2" 1024 768 ;
	setAttr ".f" 0;
	setAttr ".dm" 2;
	setAttr ".dic" yes;
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
	setAttr ".pv" -type "double2" 0 1 ;
	setAttr ".uvst[0].uvsn" -type "string" "map_gnd";
	setAttr ".cuvs" -type "string" "map_gnd";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 14 ".pt";
	setAttr ".pt[2]" -type "float3" 0 0.57173091 0 ;
	setAttr ".pt[5]" -type "float3" 0 0.57173091 0 ;
	setAttr ".pt[6]" -type "float3" 0 0.57173091 0 ;
	setAttr ".pt[7]" -type "float3" 0 0.57173091 0 ;
	setAttr ".pt[8]" -type "float3" 0 1.8499894 0 ;
	setAttr ".pt[9]" -type "float3" 0 1.8499894 0 ;
	setAttr ".pt[10]" -type "float3" 0 3.3193109 0 ;
	setAttr ".pt[11]" -type "float3" 0 3.3193109 0 ;
	setAttr ".pt[12]" -type "float3" 0 3.3193109 0 ;
	setAttr ".pt[13]" -type "float3" 0 3.3193104 0 ;
	setAttr ".pt[14]" -type "float3" 0 -1.0730646 -24.577202 ;
	setAttr ".pt[15]" -type "float3" 0 -1.0730646 -24.577202 ;
	setAttr ".pt[16]" -type "float3" 0 2.3472998 0 ;
	setAttr ".pt[17]" -type "float3" 0 2.3472998 0 ;
	setAttr ".rgvtx" -type "string" (
		"[-28.25436019897461, -14.601387023925781, -39.71055603027344, 22.44190216064453, -14.601386070251465, -40.23103332519531, -98.40283203125, -40.0561637878418, -190.23101806640625, -10.862857818603516, -19.044231414794922, -115.23103332519531, 33.322998046875, -19.044231414794922, -115.23103332519531, 97.7762451171875, 5.989115238189697, -190.2310333251953, -2.0306529998779297, -24.12482452392578, -190.2310333251953, 39.606876373291016, -24.12482452392578, -190.2310333251953, -6.036128997802734, -27.130878448486328, -265.23101806640625, 37.033042907714844, -27.130878448486328, -265.23101806640625, -169.62310791015625, -38.18972396850586, -340.23101806640625, -15.166370391845703, -30.88197135925293, -340.23101806640625, 27.90279769897461, -30.88197135925293, -340.23101806640625, 97.7762451171875, 8.7366943359375, -340.23101806640625, -14.44040584564209, -13.560460090637207, -22.71518898010254, 9.180140495300293, -13.560460090637207, -18.93045997619629, -25.931838989257812, -32.8176155090332, -397.7475280761719, 17.1373291015625, -32.8176155090332, -397.7475280761719]");
	setAttr ".rgf" -type "string" "[[0,3,2],[0,1,4,3],[1,5,4],[2,3,6],[3,4,7,6],[4,5,7],[2,6,8],[6,7,9,8],[7,5,9],[2,8,11,10],[8,9,12,11],[9,5,13,12],[1,0,14,15],[11,12,17,16]]";
	setAttr ".rgn" -type "string" (
		"[-0.05171438306570053, 0.9961832165718079, -0.07031835615634918, -0.08184357732534409, 0.9936133027076721, -0.07768066972494125, -0.09713861346244812, 0.9952676892280579, -0.0025215381756424904, -0.05171438306570053, 0.9961832165718079, -0.07031835615634918, -0.05334187299013138, 0.9963803291320801, -0.06618805974721909, -0.20431464910507202, 0.9749496579170227, -0.08791395276784897, -0.08184357732534409, 0.9936133027076721, -0.07768066972494125, -0.05334187299013138, 0.9963803291320801, -0.06618805974721909, -0.4802956283092499, 0.8768814206123352, -0.019878435879945755, -0.20431464910507202, 0.9749496579170227, -0.08791395276784897, -0.09713861346244812, 0.9952676892280579, -0.0025215381756424904, -0.08184357732534409, 0.9936133027076721, -0.07768066972494125, -0.08290135115385056, 0.9949678182601929, -0.05626998841762543, -0.08184357732534409, 0.9936133027076721, -0.07768066972494125, -0.20431464910507202, 0.9749496579170227, -0.08791395276784897, -0.21299143135547638, 0.9753464460372925, -0.057741377502679825, -0.08290135115385056, 0.9949678182601929, -0.05626998841762543, -0.20431464910507202, 0.9749496579170227, -0.08791395276784897, -0.4802956283092499, 0.8768814206123352, -0.019878435879945755, -0.21299143135547638, 0.9753464460372925, -0.057741377502679825, -0.09713861346244812, 0.9952676892280579, -0.0025215381756424904, -0.08290135115385056, 0.9949678182601929, -0.05626998841762543, -0.06580079346895218, 0.9978324174880981, 0.0008885699207894504, -0.08290135115385056, 0.9949678182601929, -0.05626998841762543, -0.21299143135547638, 0.9753464460372925, -0.057741377502679825, -0.3522184193134308, 0.9359078407287598, -0.004318025894463062, -0.06580079346895218, 0.9978324174880981, 0.0008885699207894504, -0.21299143135547638, 0.9753464460372925, -0.057741377502679825, -0.4802956283092499, 0.8768814206123352, -0.019878435879945755, -0.3522184193134308, 0.9359078407287598, -0.004318025894463062, -0.09713861346244812, 0.9952676892280579, -0.0025215381756424904, -0.06580079346895218, 0.9978324174880981, 0.0008885699207894504, -0.05653620511293411, 0.9983955025672913, 0.003154281759634614, -0.07601127028465271, 0.9969298839569092, 0.0187941063195467, -0.06580079346895218, 0.9978324174880981, 0.0008885699207894504, -0.3522184193134308, 0.9359078407287598, -0.004318025894463062, -0.29176613688468933, 0.9564396142959595, -0.009785305708646774, -0.05653620511293411, 0.9983955025672913, 0.003154281759634614, -0.3522184193134308, 0.9359078407287598, -0.004318025894463062, -0.4802956283092499, 0.8768814206123352, -0.019878435879945755, -0.4935295283794403, 0.8695791959762573, 0.016148487105965614, -0.29176613688468933, 0.9564396142959595, -0.009785305708646774, -0.05334187299013138, 0.9963803291320801, -0.06618805974721909, -0.05171438306570053, 0.9961832165718079, -0.07031835615634918, 0.0023857434280216694, 0.9985209703445435, -0.054316215217113495, 0.002385743660852313, 0.9985209703445435, -0.054316211491823196, -0.05653620511293411, 0.9983955025672913, 0.003154281759634614, -0.29176613688468933, 0.9564396142959595, -0.009785305708646774, 0.0, 0.9994341731071472, -0.03363466262817383, 0.0, 0.999434232711792, -0.03363466635346413]");
	setAttr ".rguv0" -type "string" (
		"[0.138888880610466, 0.0, 0.277777761220932, 0.25, 0.0, 0.5625, 0.138888880610466, 0.0, 0.6944444179534912, 0.0, 0.555555522441864, 0.25, 0.277777761220932, 0.25, 0.6944444179534912, 0.0, 0.8333332538604736, 0.5625, 0.555555522441864, 0.25, 0.0, 0.5625, 0.277777761220932, 0.25, 0.277777761220932, 0.5, 0.277777761220932, 0.25, 0.555555522441864, 0.25, 0.555555522441864, 0.5, 0.277777761220932, 0.5, 0.555555522441864, 0.25, 0.8333332538604736, 0.5625, 0.555555522441864, 0.5, 0.0, 0.5625, 0.277777761220932, 0.5, 0.277777761220932, 0.75, 0.277777761220932, 0.5, 0.555555522441864, 0.5, 0.555555522441864, 0.75, 0.277777761220932, 0.75, 0.555555522441864, 0.5, 0.8333332538604736, 0.5625, 0.555555522441864, 0.75, 0.0, 0.5625, 0.277777761220932, 0.75, 0.277777761220932, 1.0, 0.0, 1.0, 0.277777761220932, 0.75, 0.555555522441864, 0.75, 0.555555522441864, 1.0, 0.277777761220932, 1.0, 0.555555522441864, 0.75, 0.8333332538604736, 0.5625, 0.8333332538604736, 1.0, 0.555555522441864, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0]");
createNode transform -n "phys_ground" -p "m_ground";
createNode transform -n "m_background" -p "m_ground";
	setAttr ".t" -type "double3" 5.2479687278070202 31.453758211100109 -296.91042320795771 ;
	setAttr ".r" -type "double3" 0 -3.1805546814635168e-015 0 ;
	setAttr ".s" -type "double3" 1.2 1.2 1.2 ;
	setAttr ".rp" -type "double3" -5.2479687278070193 -31.453758211100094 296.91042320795765 ;
	setAttr ".sp" -type "double3" -5.2479687278070193 -31.453758211100116 296.91042320795771 ;
	setAttr ".spt" -type "double3" 2.7755575615628914e-016 6.3282712403633923e-015 -3.0198066269804258e-014 ;
createNode mesh -n "m_backgroundShape" -p "m_background";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "string";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	addAttr -ci true -sn "rgn" -ln "rgn" -dt "string";
	addAttr -ci true -sn "rguv0" -ln "rguv0" -dt "string";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".pv" -type "double2" 0 0.3333333432674408 ;
	setAttr ".uvst[0].uvsn" -type "string" "map_bk_env";
	setAttr ".cuvs" -type "string" "map_bk_env";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 4 ".pt";
	setAttr ".pt[0]" -type "float3" 0 3.1998088 -1.9078822e-013 ;
	setAttr ".pt[1]" -type "float3" 0 1.0342292 -4.7697055e-014 ;
	setAttr ".pt[4]" -type "float3" -1.0422964 5.2588911 1.1447293e-012 ;
	setAttr ".pt[5]" -type "float3" -5.1571593 1.1465448 1.9078821e-013 ;
	setAttr ".rgvtx" -type "string" "[-150.0, -65.54606628417969, -1.3394480294880162e-13, 140.6639862060547, -24.397045135498047, 2.365200320753741e-13, -150.0, 76.07714080810547, 5.684341886080802e-14, 140.6639862060547, 76.07714080810547, 3.410605131648481e-13, 22.368942260742188, -58.17298889160156, 1.3425504197944749e-12, 76.44232940673828, -26.97852325439453, 4.334194678504155e-13]";
	setAttr ".rgf" -type "string" "[[0,4,2],[3,2,1],[1,2,5],[4,5,2]]";
	setAttr ".rgn" -type "string" "[-8.508275701547049e-15, -1.3471535983234717e-15, 1.0, 3.343744880300912e-16, 1.000626871535933e-14, 1.0, -4.985817616105998e-15, 5.499116299889433e-16, 1.0, -9.778199165080478e-16, -1.040470920209949e-15, 1.0, -4.985817616105998e-15, 5.499116299889433e-16, 1.0, -7.246602974194587e-16, -3.08099975992516e-16, 1.0, -7.246602974194587e-16, -3.08099975992516e-16, 1.0, -4.985817616105998e-15, 5.499116299889433e-16, 1.0, 4.142429451623733e-15, 1.2756189130707675e-14, 1.0, 3.343744880300912e-16, 1.000626871535933e-14, 1.0, 4.142429451623733e-15, 1.2756189130707675e-14, 1.0, -4.985817616105998e-15, 5.499116299889433e-16, 1.0]";
	setAttr ".rguv0" -type "string" "[0.0, 0.0, 0.6200265288352966, 0.0, 0.0, 0.3333333432674408, 1.0, 0.3333333432674408, 0.0, 0.3333333432674408, 1.0, 0.0, 1.0, 0.0, 0.0, 0.3333333432674408, 0.8171036243438721, 0.0, 0.6200265288352966, 0.0, 0.8171036243438721, 0.0, 0.0, 0.3333333432674408]";
createNode transform -n "phys_pos_car_start" -p "m_ground";
	setAttr ".t" -type "double3" -13.529203811803738 -31.08845891047147 -371.60828911507309 ;
	setAttr ".r" -type "double3" 0 190.36874605574096 0 ;
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
	setAttr ".t" -type "double3" -5.7088233867333242 -29.146017750148168 -323.472893958643 ;
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
	setAttr ".t" -type "double3" 6.7663270835944189 -22.610502761106225 -185.10042993305794 ;
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
	setAttr ".t" -type "double3" 0.27388649685633304 -69.604312555395836 18.895134817857627 ;
	setAttr ".r" -type "double3" 0 -3.1805546814635168e-015 0 ;
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
	setAttr ".t" -type "double3" -14.63467427972604 -10.885190423000207 -15.394261886983184 ;
	setAttr ".r" -type "double3" 0 -3.1805546814635168e-015 0 ;
	setAttr ".s" -type "double3" 1 0.99999999999999978 0.99999999999999978 ;
	setAttr ".spt" -type "double3" 0 1.2621774483536189e-029 1.2621774483536189e-028 ;
createNode transform -n "phys_tree1" -p "m_ground";
	setAttr ".t" -type "double3" -46.929548510556693 -16.388007545135821 -114.1503126631855 ;
createNode mesh -n "phys_treeShape1" -p "phys_tree1";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "phys_tree2" -p "m_ground";
	setAttr ".t" -type "double3" -32.180833315715404 -18.626912583248732 -158.69838412961974 ;
createNode mesh -n "phys_treeShape2" -p "phys_tree2";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
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
		+ "        modelEditor -e \n            -pluginObjects \"gpuCacheDisplayFilter\" 1 \n            $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"modelPanel\" (localizedPanelLabel(\"Persp View\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `modelPanel -unParent -l (localizedPanelLabel(\"Persp View\")) -mbv $menusOkayInPanels `;\n\t\t\t$editorName = $panelName;\n            modelEditor -e \n                -camera \"persp\" \n                -useInteractiveMode 0\n                -displayLights \"default\" \n                -displayAppearance \"smoothShaded\" \n                -activeOnly 0\n                -ignorePanZoom 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n                -activeComponentsXray 0\n"
		+ "                -displayTextures 1\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n                -rendererName \"base_OpenGL_Renderer\" \n                -objectFilterShowInHUD 1\n                -isFiltered 0\n                -colorResolution 256 256 \n                -bumpResolution 512 512 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n                -lowQualityLighting 0\n                -maximumNumHardwareLights 1\n                -occlusionCulling 0\n"
		+ "                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n                -imagePlane 1\n                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -pluginShapes 1\n"
		+ "                -dimensions 1\n                -handles 1\n                -pivots 1\n                -textures 1\n                -strokes 1\n                -motionTrails 1\n                -clipGhosts 1\n                -greasePencils 1\n                -shadows 0\n                $editorName;\n            modelEditor -e -viewSelected 0 $editorName;\n            modelEditor -e \n                -pluginObjects \"gpuCacheDisplayFilter\" 1 \n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tmodelPanel -edit -l (localizedPanelLabel(\"Persp View\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        modelEditor -e \n            -camera \"persp\" \n            -useInteractiveMode 0\n            -displayLights \"default\" \n            -displayAppearance \"smoothShaded\" \n            -activeOnly 0\n            -ignorePanZoom 0\n            -wireframeOnShaded 0\n            -headsUpDisplay 1\n            -selectionHiliteDisplay 1\n            -useDefaultMaterial 0\n            -bufferMode \"double\" \n"
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
		+ "\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"hyperGraphPanel\" (localizedPanelLabel(\"Hypergraph Hierarchy\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"hyperGraphPanel\" -l (localizedPanelLabel(\"Hypergraph Hierarchy\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = ($panelName+\"HyperGraphEd\");\n            hyperGraph -e \n                -graphLayoutStyle \"hierarchicalLayout\" \n                -orientation \"horiz\" \n                -mergeConnections 1\n                -zoom 0.984487\n                -animateTransition 0\n                -showRelationships 1\n                -showShapes 0\n                -showDeformers 0\n                -showExpressions 0\n                -showConstraints 0\n                -showConnectionFromSelected 0\n                -showConnectionToSelected 0\n                -showUnderworld 0\n                -showInvisible 0\n                -transitionFrames 1\n                -currentNode \"phys_tree\" \n"
		+ "                -opaqueContainers 0\n                -dropTargetNode \"m_ground\" \n                -dropNode \"phys_tree2\" \n                -feedbackNode \"|camera1\" \n                -feedbackGadget \"Outputs\" \n                -freeform 0\n                -imagePosition 0 0 \n                -imageScale 1\n                -imageEnabled 0\n                -graphType \"DAG\" \n                -heatMapDisplay 0\n                -updateSelection 1\n                -updateNodeAdded 1\n                -useDrawOverrideColor 0\n                -limitGraphTraversal -1\n                -range 0 0 \n                -iconSize \"largeIcons\" \n                -showCachedConnections 0\n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Hypergraph Hierarchy\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = ($panelName+\"HyperGraphEd\");\n            hyperGraph -e \n                -graphLayoutStyle \"hierarchicalLayout\" \n                -orientation \"horiz\" \n                -mergeConnections 1\n"
		+ "                -zoom 0.984487\n                -animateTransition 0\n                -showRelationships 1\n                -showShapes 0\n                -showDeformers 0\n                -showExpressions 0\n                -showConstraints 0\n                -showConnectionFromSelected 0\n                -showConnectionToSelected 0\n                -showUnderworld 0\n                -showInvisible 0\n                -transitionFrames 1\n                -currentNode \"phys_tree\" \n                -opaqueContainers 0\n                -dropTargetNode \"m_ground\" \n                -dropNode \"phys_tree2\" \n                -feedbackNode \"|camera1\" \n                -feedbackGadget \"Outputs\" \n                -freeform 0\n                -imagePosition 0 0 \n                -imageScale 1\n                -imageEnabled 0\n                -graphType \"DAG\" \n                -heatMapDisplay 0\n                -updateSelection 1\n                -updateNodeAdded 1\n                -useDrawOverrideColor 0\n                -limitGraphTraversal -1\n                -range 0 0 \n"
		+ "                -iconSize \"largeIcons\" \n                -showCachedConnections 0\n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"hyperShadePanel\" (localizedPanelLabel(\"Hypershade\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"hyperShadePanel\" -l (localizedPanelLabel(\"Hypershade\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Hypershade\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"visorPanel\" (localizedPanelLabel(\"Visor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"visorPanel\" -l (localizedPanelLabel(\"Visor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n"
		+ "\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Visor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"nodeEditorPanel\" (localizedPanelLabel(\"Node Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"nodeEditorPanel\" -l (localizedPanelLabel(\"Node Editor\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = ($panelName+\"NodeEditorEd\");\n            nodeEditor -e \n                -allAttributes 0\n                -allNodes 0\n                -autoSizeNodes 1\n                -createNodeCommand \"nodeEdCreateNodeCommand\" \n                -defaultPinnedState 0\n                -ignoreAssets 1\n                -additiveGraphingMode 0\n                -settingsChangedCallback \"nodeEdSyncControls\" \n                -traversalDepthLimit -1\n                -keyPressCommand \"nodeEdKeyPressCommand\" \n                -keyReleaseCommand \"nodeEdKeyReleaseCommand\" \n                -nodeTitleMode \"name\" \n"
		+ "                -gridSnap 0\n                -gridVisibility 1\n                -popupMenuScript \"nodeEdBuildPanelMenus\" \n                -island 0\n                -showNamespace 1\n                -showShapes 1\n                -showSGShapes 0\n                -showTransforms 1\n                -syncedSelection 1\n                -extendToShapes 1\n                $editorName;\n\t\t\tif (`objExists nodeEditorPanel1Info`) nodeEditor -e -restoreInfo nodeEditorPanel1Info $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Node Editor\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = ($panelName+\"NodeEditorEd\");\n            nodeEditor -e \n                -allAttributes 0\n                -allNodes 0\n                -autoSizeNodes 1\n                -createNodeCommand \"nodeEdCreateNodeCommand\" \n                -defaultPinnedState 0\n                -ignoreAssets 1\n                -additiveGraphingMode 0\n                -settingsChangedCallback \"nodeEdSyncControls\" \n"
		+ "                -traversalDepthLimit -1\n                -keyPressCommand \"nodeEdKeyPressCommand\" \n                -keyReleaseCommand \"nodeEdKeyReleaseCommand\" \n                -nodeTitleMode \"name\" \n                -gridSnap 0\n                -gridVisibility 1\n                -popupMenuScript \"nodeEdBuildPanelMenus\" \n                -island 0\n                -showNamespace 1\n                -showShapes 1\n                -showSGShapes 0\n                -showTransforms 1\n                -syncedSelection 1\n                -extendToShapes 1\n                $editorName;\n\t\t\tif (`objExists nodeEditorPanel1Info`) nodeEditor -e -restoreInfo nodeEditorPanel1Info $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"createNodePanel\" (localizedPanelLabel(\"Create Node\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"createNodePanel\" -l (localizedPanelLabel(\"Create Node\")) -mbv $menusOkayInPanels `;\n"
		+ "\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Create Node\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"polyTexturePlacementPanel\" (localizedPanelLabel(\"UV Texture Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"polyTexturePlacementPanel\" -l (localizedPanelLabel(\"UV Texture Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"UV Texture Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"renderWindowPanel\" (localizedPanelLabel(\"Render View\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"renderWindowPanel\" -l (localizedPanelLabel(\"Render View\")) -mbv $menusOkayInPanels `;\n"
		+ "\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Render View\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"blendShapePanel\" (localizedPanelLabel(\"Blend Shape\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\tblendShapePanel -unParent -l (localizedPanelLabel(\"Blend Shape\")) -mbv $menusOkayInPanels ;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tblendShapePanel -edit -l (localizedPanelLabel(\"Blend Shape\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"dynRelEdPanel\" (localizedPanelLabel(\"Dynamic Relationships\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"dynRelEdPanel\" -l (localizedPanelLabel(\"Dynamic Relationships\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n"
		+ "\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Dynamic Relationships\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"relationshipPanel\" (localizedPanelLabel(\"Relationship Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"relationshipPanel\" -l (localizedPanelLabel(\"Relationship Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Relationship Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"referenceEditorPanel\" (localizedPanelLabel(\"Reference Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"referenceEditorPanel\" -l (localizedPanelLabel(\"Reference Editor\")) -mbv $menusOkayInPanels `;\n"
		+ "\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Reference Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"componentEditorPanel\" (localizedPanelLabel(\"Component Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"componentEditorPanel\" -l (localizedPanelLabel(\"Component Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Component Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"dynPaintScriptedPanelType\" (localizedPanelLabel(\"Paint Effects\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"dynPaintScriptedPanelType\" -l (localizedPanelLabel(\"Paint Effects\")) -mbv $menusOkayInPanels `;\n"
		+ "\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Paint Effects\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"scriptEditorPanel\" (localizedPanelLabel(\"Script Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"scriptEditorPanel\" -l (localizedPanelLabel(\"Script Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Script Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"Stereo\" (localizedPanelLabel(\"Stereo\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"Stereo\" -l (localizedPanelLabel(\"Stereo\")) -mbv $menusOkayInPanels `;\nstring $editorName = ($panelName+\"Editor\");\n"
		+ "            stereoCameraView -e \n                -camera \"camera1\" \n                -useInteractiveMode 0\n                -displayLights \"default\" \n                -displayAppearance \"wireframe\" \n                -activeOnly 0\n                -ignorePanZoom 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n                -activeComponentsXray 0\n                -displayTextures 0\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n"
		+ "                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n                -objectFilterShowInHUD 1\n                -isFiltered 0\n                -colorResolution 4 4 \n                -bumpResolution 4 4 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n                -lowQualityLighting 0\n                -maximumNumHardwareLights 0\n                -occlusionCulling 0\n                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n"
		+ "                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n                -imagePlane 1\n                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -pluginShapes 1\n                -dimensions 1\n                -handles 1\n                -pivots 1\n                -textures 1\n                -strokes 1\n                -motionTrails 1\n                -clipGhosts 1\n                -greasePencils 1\n                -shadows 0\n                -displayMode \"centerEye\" \n                -viewColor 0 0 0 1 \n                -useCustomBackground 1\n                $editorName;\n            stereoCameraView -e -viewSelected 0 $editorName;\n"
		+ "            stereoCameraView -e \n                -pluginObjects \"gpuCacheDisplayFilter\" 1 \n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Stereo\")) -mbv $menusOkayInPanels  $panelName;\nstring $editorName = ($panelName+\"Editor\");\n            stereoCameraView -e \n                -camera \"camera1\" \n                -useInteractiveMode 0\n                -displayLights \"default\" \n                -displayAppearance \"wireframe\" \n                -activeOnly 0\n                -ignorePanZoom 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n                -activeComponentsXray 0\n                -displayTextures 0\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n"
		+ "                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n                -objectFilterShowInHUD 1\n                -isFiltered 0\n                -colorResolution 4 4 \n                -bumpResolution 4 4 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n                -lowQualityLighting 0\n                -maximumNumHardwareLights 0\n                -occlusionCulling 0\n                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n"
		+ "                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n                -imagePlane 1\n                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -pluginShapes 1\n                -dimensions 1\n                -handles 1\n                -pivots 1\n                -textures 1\n                -strokes 1\n                -motionTrails 1\n"
		+ "                -clipGhosts 1\n                -greasePencils 1\n                -shadows 0\n                -displayMode \"centerEye\" \n                -viewColor 0 0 0 1 \n                -useCustomBackground 1\n                $editorName;\n            stereoCameraView -e -viewSelected 0 $editorName;\n            stereoCameraView -e \n                -pluginObjects \"gpuCacheDisplayFilter\" 1 \n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\tif ($useSceneConfig) {\n        string $configName = `getPanel -cwl (localizedPanelLabel(\"Current Layout\"))`;\n        if (\"\" != $configName) {\n\t\t\tpanelConfiguration -edit -label (localizedPanelLabel(\"Current Layout\")) \n\t\t\t\t-defaultImage \"\"\n\t\t\t\t-image \"\"\n\t\t\t\t-sc false\n\t\t\t\t-configString \"global string $gMainPane; paneLayout -e -cn \\\"quad\\\" -ps 1 50 50 -ps 2 50 50 -ps 3 36 50 -ps 4 64 50 $gMainPane;\"\n\t\t\t\t-removeAllPanels\n\t\t\t\t-ap false\n\t\t\t\t\t(localizedPanelLabel(\"Hypergraph Hierarchy\")) \n\t\t\t\t\t\"scriptedPanel\"\n"
		+ "\t\t\t\t\t\"$panelName = `scriptedPanel -unParent  -type \\\"hyperGraphPanel\\\" -l (localizedPanelLabel(\\\"Hypergraph Hierarchy\\\")) -mbv $menusOkayInPanels `;\\n\\n\\t\\t\\t$editorName = ($panelName+\\\"HyperGraphEd\\\");\\n            hyperGraph -e \\n                -graphLayoutStyle \\\"hierarchicalLayout\\\" \\n                -orientation \\\"horiz\\\" \\n                -mergeConnections 1\\n                -zoom 0.984487\\n                -animateTransition 0\\n                -showRelationships 1\\n                -showShapes 0\\n                -showDeformers 0\\n                -showExpressions 0\\n                -showConstraints 0\\n                -showConnectionFromSelected 0\\n                -showConnectionToSelected 0\\n                -showUnderworld 0\\n                -showInvisible 0\\n                -transitionFrames 1\\n                -currentNode \\\"phys_tree\\\" \\n                -opaqueContainers 0\\n                -dropTargetNode \\\"m_ground\\\" \\n                -dropNode \\\"phys_tree2\\\" \\n                -feedbackNode \\\"|camera1\\\" \\n                -feedbackGadget \\\"Outputs\\\" \\n                -freeform 0\\n                -imagePosition 0 0 \\n                -imageScale 1\\n                -imageEnabled 0\\n                -graphType \\\"DAG\\\" \\n                -heatMapDisplay 0\\n                -updateSelection 1\\n                -updateNodeAdded 1\\n                -useDrawOverrideColor 0\\n                -limitGraphTraversal -1\\n                -range 0 0 \\n                -iconSize \\\"largeIcons\\\" \\n                -showCachedConnections 0\\n                $editorName\"\n"
		+ "\t\t\t\t\t\"scriptedPanel -edit -l (localizedPanelLabel(\\\"Hypergraph Hierarchy\\\")) -mbv $menusOkayInPanels  $panelName;\\n\\n\\t\\t\\t$editorName = ($panelName+\\\"HyperGraphEd\\\");\\n            hyperGraph -e \\n                -graphLayoutStyle \\\"hierarchicalLayout\\\" \\n                -orientation \\\"horiz\\\" \\n                -mergeConnections 1\\n                -zoom 0.984487\\n                -animateTransition 0\\n                -showRelationships 1\\n                -showShapes 0\\n                -showDeformers 0\\n                -showExpressions 0\\n                -showConstraints 0\\n                -showConnectionFromSelected 0\\n                -showConnectionToSelected 0\\n                -showUnderworld 0\\n                -showInvisible 0\\n                -transitionFrames 1\\n                -currentNode \\\"phys_tree\\\" \\n                -opaqueContainers 0\\n                -dropTargetNode \\\"m_ground\\\" \\n                -dropNode \\\"phys_tree2\\\" \\n                -feedbackNode \\\"|camera1\\\" \\n                -feedbackGadget \\\"Outputs\\\" \\n                -freeform 0\\n                -imagePosition 0 0 \\n                -imageScale 1\\n                -imageEnabled 0\\n                -graphType \\\"DAG\\\" \\n                -heatMapDisplay 0\\n                -updateSelection 1\\n                -updateNodeAdded 1\\n                -useDrawOverrideColor 0\\n                -limitGraphTraversal -1\\n                -range 0 0 \\n                -iconSize \\\"largeIcons\\\" \\n                -showCachedConnections 0\\n                $editorName\"\n"
		+ "\t\t\t\t-ap false\n\t\t\t\t\t(localizedPanelLabel(\"Persp View\")) \n\t\t\t\t\t\"modelPanel\"\n"
		+ "\t\t\t\t\t\"$panelName = `modelPanel -unParent -l (localizedPanelLabel(\\\"Persp View\\\")) -mbv $menusOkayInPanels `;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera persp` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"smoothShaded\\\" \\n    -activeOnly 0\\n    -ignorePanZoom 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 1\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -objectFilterShowInHUD 1\\n    -isFiltered 0\\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -imagePlane 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -pluginShapes 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -motionTrails 1\\n    -clipGhosts 1\\n    -greasePencils 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName;\\nmodelEditor -e \\n    -pluginObjects \\\"gpuCacheDisplayFilter\\\" 1 \\n    $editorName\"\n"
		+ "\t\t\t\t\t\"modelPanel -edit -l (localizedPanelLabel(\\\"Persp View\\\")) -mbv $menusOkayInPanels  $panelName;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera persp` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"smoothShaded\\\" \\n    -activeOnly 0\\n    -ignorePanZoom 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 1\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -objectFilterShowInHUD 1\\n    -isFiltered 0\\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -imagePlane 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -pluginShapes 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -motionTrails 1\\n    -clipGhosts 1\\n    -greasePencils 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName;\\nmodelEditor -e \\n    -pluginObjects \\\"gpuCacheDisplayFilter\\\" 1 \\n    $editorName\"\n"
		+ "\t\t\t\t-ap false\n\t\t\t\t\t(localizedPanelLabel(\"Side View\")) \n\t\t\t\t\t\"modelPanel\"\n"
		+ "\t\t\t\t\t\"$panelName = `modelPanel -unParent -l (localizedPanelLabel(\\\"Side View\\\")) -mbv $menusOkayInPanels `;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -camera \\\"camera1\\\" \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"wireframe\\\" \\n    -activeOnly 0\\n    -ignorePanZoom 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 1\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -objectFilterShowInHUD 1\\n    -isFiltered 0\\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -imagePlane 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -pluginShapes 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -motionTrails 1\\n    -clipGhosts 1\\n    -greasePencils 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName;\\nmodelEditor -e \\n    -pluginObjects \\\"gpuCacheDisplayFilter\\\" 1 \\n    $editorName\"\n"
		+ "\t\t\t\t\t\"modelPanel -edit -l (localizedPanelLabel(\\\"Side View\\\")) -mbv $menusOkayInPanels  $panelName;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -camera \\\"camera1\\\" \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"wireframe\\\" \\n    -activeOnly 0\\n    -ignorePanZoom 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 1\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -objectFilterShowInHUD 1\\n    -isFiltered 0\\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -imagePlane 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -pluginShapes 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -motionTrails 1\\n    -clipGhosts 1\\n    -greasePencils 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName;\\nmodelEditor -e \\n    -pluginObjects \\\"gpuCacheDisplayFilter\\\" 1 \\n    $editorName\"\n"
		+ "\t\t\t\t-ap false\n\t\t\t\t\t(localizedPanelLabel(\"Front View\")) \n\t\t\t\t\t\"modelPanel\"\n"
		+ "\t\t\t\t\t\"$panelName = `modelPanel -unParent -l (localizedPanelLabel(\\\"Front View\\\")) -mbv $menusOkayInPanels `;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera front` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"wireframe\\\" \\n    -activeOnly 0\\n    -ignorePanZoom 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 0\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -objectFilterShowInHUD 1\\n    -isFiltered 0\\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -imagePlane 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -pluginShapes 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -motionTrails 1\\n    -clipGhosts 1\\n    -greasePencils 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName;\\nmodelEditor -e \\n    -pluginObjects \\\"gpuCacheDisplayFilter\\\" 1 \\n    $editorName\"\n"
		+ "\t\t\t\t\t\"modelPanel -edit -l (localizedPanelLabel(\\\"Front View\\\")) -mbv $menusOkayInPanels  $panelName;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera front` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"wireframe\\\" \\n    -activeOnly 0\\n    -ignorePanZoom 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 0\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -objectFilterShowInHUD 1\\n    -isFiltered 0\\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -imagePlane 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -pluginShapes 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -motionTrails 1\\n    -clipGhosts 1\\n    -greasePencils 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName;\\nmodelEditor -e \\n    -pluginObjects \\\"gpuCacheDisplayFilter\\\" 1 \\n    $editorName\"\n"
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
	setAttr -s 3 ".dsm";
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo2";
createNode file -n "file1";
	setAttr ".ftn" -type "string" "C:/RnD/pd/Fire/Data/lvl13.jpg";
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
	setAttr ".w" 50;
	setAttr ".h" 50;
	setAttr ".d" 50;
	setAttr ".cuv" 4;
createNode polyPlane -n "polyPlane2";
	setAttr ".ax" -type "double3" 0 0 1 ;
	setAttr ".uvs" -type "string" "map_bk_env";
	setAttr ".w" 300;
	setAttr ".h" 100;
	setAttr ".sw" 1;
	setAttr ".sh" 1;
	setAttr ".cuv" 2;
createNode polySplit -n "polySplit1";
	setAttr ".e[0]"  0.90780032;
	setAttr ".d[0]"  -2147483648;
createNode polyTweak -n "polyTweak7";
	setAttr ".uopa" yes;
	setAttr -s 4 ".tk[0:3]" -type "float3"  0 1.029507518 -2.8421709e-014
		 0 -4.9341116 0 2.2737368e-013 21.626091 5.727483e-014 2.2737368e-013 21.626091 5.727483e-014;
createNode polyExtrudeEdge -n "polyExtrudeEdge2";
	setAttr ".uopa" yes;
	setAttr ".ics" -type "componentList" 1 "e[4]";
	setAttr ".ix" -type "matrix" 1 -6.9388939039072284e-018 -5.5511151231257827e-017 0
		 -8.6736173798840355e-018 1 -1.9949319973733282e-017 0 1.6653345369377348e-016 1.3877787807814457e-017 0.99999999999999944 0
		 5.2479687278070202 31.453758211100116 -296.91042320795759 1;
	setAttr ".ws" yes;
	setAttr ".pvt" -type "float3" 141.41801 -23.205433 -296.91043 ;
	setAttr ".rs" 46280;
	setAttr ".lt" -type "double3" -6.6002758813965556e-014 -15.836765631264482 0 ;
	setAttr ".c[0]"  0 1 1;
	setAttr ".cbn" -type "double3" 127.58805661843202 -23.480354337728009 -296.91042320795759 ;
	setAttr ".cbx" -type "double3" 155.24796872780701 -22.930511503255353 -296.91042320795759 ;
createNode deleteComponent -n "deleteComponent1";
	setAttr ".dc" -type "componentList" 1 "f[1]";
createNode deleteComponent -n "deleteComponent2";
	setAttr ".dc" -type "componentList" 1 "vtx[4]";
createNode hyperGraphInfo -n "nodeEditorPanel1Info";
createNode hyperView -n "hyperView1";
	setAttr ".dag" no;
createNode hyperLayout -n "hyperLayout1";
	setAttr ".ihi" 0;
	setAttr ".hyp[0].nvs" 1920;
	setAttr ".anf" yes;
createNode polyPlane -n "polyPlane3";
	setAttr ".uvs" -type "string" "map_gnd";
	setAttr ".w" 250;
	setAttr ".h" 300;
	setAttr ".sw" 3;
	setAttr ".sh" 4;
	setAttr ".cuv" 2;
createNode polyMergeVert -n "polyMergeVert1";
	setAttr ".ics" -type "componentList" 1 "vtx[0:1]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
createNode polyTweak -n "polyTweak8";
	setAttr ".uopa" yes;
	setAttr -s 20 ".tk[0:19]" -type "float3"  99.33066559 -14.60138702 -190.23103333
		 15.99732876 -14.60138702 -190.23103333 -24.26683426 -14.60138607 -190.23103333 0
		 -14.60138607 -190.23103333 0 -40.62789536 -265.23101807 29.37216377 -19.044231415
		 -190.23103333 -10.89200211 -19.044231415 -190.23103333 0 -14.60138607 -190.23103333
		 0 -40.62789536 -190.23103333 38.20436859 -24.69655609 -190.23103333 -2.059796572
		 -24.69655609 -190.23103333 0 -14.60138607 -190.23103333 -15.4310503 -39.16674805
		 -190.23103333 35.63053513 -28.98086739 -190.23103333 -4.63362932 -28.98086739 -190.23103333
		 0 -14.60138607 -190.23103333 0 -40.62789536 -190.23103333 26.50029373 -32.79138947
		 -190.23103333 -13.7638731 -32.79138947 -190.23103333 0 -14.60138607 -190.23103333;
createNode polyMergeVert -n "polyMergeVert2";
	setAttr ".ics" -type "componentList" 2 "vtx[3]" "vtx[7]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
createNode polyTweak -n "polyTweak9";
	setAttr ".uopa" yes;
	setAttr ".tk[7]" -type "float3"  0 0 1.5258789e-005;
createNode polyMergeVert -n "polyMergeVert3";
	setAttr ".ics" -type "componentList" 2 "vtx[3]" "vtx[10]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
createNode polyTweak -n "polyTweak10";
	setAttr ".uopa" yes;
	setAttr ".tk[10]" -type "float3"  15.43104553 -1.46114731 75;
createNode polyMergeVert -n "polyMergeVert4";
	setAttr ".ics" -type "componentList" 1 "vtx[1:2]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
createNode polyTweak -n "polyTweak11";
	setAttr ".uopa" yes;
	setAttr -s 7 ".tk";
	setAttr ".tk[1]" -type "float3" 1.9073486e-006 0 0 ;
	setAttr ".tk[2]" -type "float3" -107.60016 0 0 ;
	setAttr ".tk[6]" -type "float3" -27.223755 20.01877 -75 ;
	setAttr ".tk[9]" -type "float3" -27.223755 20.01877 0 ;
	setAttr ".tk[12]" -type "float3" -27.223755 20.01877 0 ;
	setAttr ".tk[13]" -type "float3" -44.623116 -0.88113844 0 ;
	setAttr ".tk[16]" -type "float3" -27.223755 20.01877 0 ;
createNode polyMergeVert -n "polyMergeVert5";
	setAttr ".ics" -type "componentList" 2 "vtx[5]" "vtx[8]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
createNode polyMergeVert -n "polyMergeVert6";
	setAttr ".ics" -type "componentList" 2 "vtx[5]" "vtx[10]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
createNode polyTweak -n "polyTweak12";
	setAttr ".uopa" yes;
	setAttr -s 2 ".tk";
	setAttr ".tk[1]" -type "float3" 5.0420618 0 0 ;
	setAttr ".tk[10]" -type "float3" 0 0 74.999985 ;
createNode polySplit -n "polySplit2";
	setAttr ".e[0]"  0.62002653;
	setAttr ".d[0]"  -2147483645;
createNode polyTweak -n "polyTweak13";
	setAttr ".uopa" yes;
	setAttr -s 4 ".tk[0:3]" -type "float3"  0 -24.032194138 5.6843419e-014
		 -9.3360157 -13.80332088 2.8421709e-013 5.6843419e-014 4.45104694 5.6843419e-014 -9.3360157
		 4.45104694 3.4106051e-013;
createNode polySplit -n "polySplit3";
	setAttr ".e[0]"  0.51866013;
	setAttr ".d[0]"  -2147483644;
createNode polyTriangulate -n "polyTriangulate1";
	setAttr ".ics" -type "componentList" 1 "f[*]";
createNode polyTweak -n "polyTweak14";
	setAttr ".uopa" yes;
	setAttr -s 4 ".tk";
	setAttr ".tk[0]" -type "float3" 0 4.2568159 0 ;
	setAttr ".tk[1]" -type "float3" 0 43.30616 0 ;
	setAttr ".tk[4]" -type "float3" -6.8081398 6.9262347 0 ;
	setAttr ".tk[5]" -type "float3" -5.9031072 41.392464 0 ;
createNode polyExtrudeEdge -n "polyExtrudeEdge3";
	setAttr ".uopa" yes;
	setAttr ".ics" -type "componentList" 1 "e[1]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
	setAttr ".ws" yes;
	setAttr ".pvt" -type "float3" -2.906229 -14.601387 -39.970795 ;
	setAttr ".rs" 56335;
	setAttr ".lt" -type "double3" 4.9960036108132044e-016 -26.941158181493854 3.7747582837255322e-015 ;
	setAttr ".c[0]"  0 1 1;
	setAttr ".cbn" -type "double3" -28.254360198974609 -14.601387023925781 -40.231033325195313 ;
	setAttr ".cbx" -type "double3" 22.441902160644531 -14.601386070251465 -39.710556030273438 ;
createNode polyTweak -n "polyTweak15";
	setAttr ".uopa" yes;
	setAttr -s 7 ".tk";
	setAttr ".tk[0]" -type "float3" -2.5850265 0 0.52047598 ;
	setAttr ".tk[2]" -type "float3" 26.59717 0 0 ;
	setAttr ".tk[3]" -type "float3" 1.4316427 0 0 ;
	setAttr ".tk[4]" -type "float3" 2.5483303 0 0 ;
	setAttr ".tk[6]" -type "float3" 1.4316427 0 0 ;
	setAttr ".tk[11]" -type "float3" 0 -1.4098916 0 ;
	setAttr ".tk[12]" -type "float3" 0 -1.4098916 0 ;
createNode polyExtrudeEdge -n "polyExtrudeEdge4";
	setAttr ".uopa" yes;
	setAttr ".ics" -type "componentList" 1 "e[23]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
	setAttr ".ws" yes;
	setAttr ".pvt" -type "float3" 6.3682137 -34.201283 -340.23102 ;
	setAttr ".rs" 50347;
	setAttr ".lt" -type "double3" -10.765469233790547 57.444605551208262 3.0324933566521364 ;
	setAttr ".c[0]"  0 1 1;
	setAttr ".cbn" -type "double3" -15.166370391845703 -34.201282501220703 -340.23101806640625 ;
	setAttr ".cbx" -type "double3" 27.902797698974609 -34.201282501220703 -340.23101806640625 ;
createNode polyTweak -n "polyTweak16";
	setAttr ".uopa" yes;
	setAttr -s 2 ".tk[14:15]" -type "float3"  13.53785801 0.5246982 14.67974854
		 -13.53785801 0.52469748 18.98495483;
createNode polyCylinder -n "polyCylinder1";
	setAttr ".r" 0.5;
	setAttr ".h" 15;
	setAttr ".sc" 1;
	setAttr ".cuv" 3;
createNode polyCylinder -n "polyCylinder2";
	setAttr ".r" 0.5;
	setAttr ".h" 15;
	setAttr ".sc" 1;
	setAttr ".cuv" 3;
createNode script -n "rg_export";
	addAttr -ci true -sn "time" -ln "time" -dt "string";
	setAttr ".time" -type "string" "2014-03-14T14:38:22.038000";
select -ne :time1;
	setAttr ".o" 1;
	setAttr ".unw" 1;
select -ne :renderPartition;
	setAttr -s 4 ".st";
select -ne :initialShadingGroup;
	setAttr -s 2 ".dsm";
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
connectAttr "polyExtrudeEdge4.out" "|m_ground|m_phys_groundShape.i";
connectAttr "polyTriangulate1.out" "m_backgroundShape.i";
connectAttr "polyCube1.out" "phys_pos_car_startShape.i";
connectAttr "polySphere1.out" "phys_pos_path_Shape1.i";
connectAttr "polySphere2.out" "|m_ground|phys_pos_path_2|phys_pos_path_Shape2.i"
		;
connectAttr "polyCube2.out" "phys_trig_car_eaterShape.i";
connectAttr "polyCylinder1.out" "phys_treeShape1.i";
connectAttr "polyCylinder2.out" "phys_treeShape2.i";
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
connectAttr "polyTweak7.out" "polySplit1.ip";
connectAttr "polyPlane2.out" "polyTweak7.ip";
connectAttr "polySplit1.out" "polyExtrudeEdge2.ip";
connectAttr "m_backgroundShape.wm" "polyExtrudeEdge2.mp";
connectAttr "polyExtrudeEdge2.out" "deleteComponent1.ig";
connectAttr "deleteComponent1.og" "deleteComponent2.ig";
connectAttr "hyperView1.msg" "nodeEditorPanel1Info.b[0]";
connectAttr "hyperLayout1.msg" "hyperView1.hl";
connectAttr "polyTweak8.out" "polyMergeVert1.ip";
connectAttr "|m_ground|m_phys_groundShape.wm" "polyMergeVert1.mp";
connectAttr "polyPlane3.out" "polyTweak8.ip";
connectAttr "polyTweak9.out" "polyMergeVert2.ip";
connectAttr "|m_ground|m_phys_groundShape.wm" "polyMergeVert2.mp";
connectAttr "polyMergeVert1.out" "polyTweak9.ip";
connectAttr "polyTweak10.out" "polyMergeVert3.ip";
connectAttr "|m_ground|m_phys_groundShape.wm" "polyMergeVert3.mp";
connectAttr "polyMergeVert2.out" "polyTweak10.ip";
connectAttr "polyTweak11.out" "polyMergeVert4.ip";
connectAttr "|m_ground|m_phys_groundShape.wm" "polyMergeVert4.mp";
connectAttr "polyMergeVert3.out" "polyTweak11.ip";
connectAttr "polyMergeVert4.out" "polyMergeVert5.ip";
connectAttr "|m_ground|m_phys_groundShape.wm" "polyMergeVert5.mp";
connectAttr "polyTweak12.out" "polyMergeVert6.ip";
connectAttr "|m_ground|m_phys_groundShape.wm" "polyMergeVert6.mp";
connectAttr "polyMergeVert5.out" "polyTweak12.ip";
connectAttr "polyTweak13.out" "polySplit2.ip";
connectAttr "deleteComponent2.og" "polyTweak13.ip";
connectAttr "polySplit2.out" "polySplit3.ip";
connectAttr "polyTweak14.out" "polyTriangulate1.ip";
connectAttr "polySplit3.out" "polyTweak14.ip";
connectAttr "polyTweak15.out" "polyExtrudeEdge3.ip";
connectAttr "|m_ground|m_phys_groundShape.wm" "polyExtrudeEdge3.mp";
connectAttr "polyMergeVert6.out" "polyTweak15.ip";
connectAttr "polyTweak16.out" "polyExtrudeEdge4.ip";
connectAttr "|m_ground|m_phys_groundShape.wm" "polyExtrudeEdge4.mp";
connectAttr "polyExtrudeEdge3.out" "polyTweak16.ip";
connectAttr "phong1SG.pa" ":renderPartition.st" -na;
connectAttr "phong2SG.pa" ":renderPartition.st" -na;
connectAttr "phys_treeShape1.iog" ":initialShadingGroup.dsm" -na;
connectAttr "phys_treeShape2.iog" ":initialShadingGroup.dsm" -na;
connectAttr "mat_phys.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_env.msg" ":defaultShaderList1.s" -na;
connectAttr "file1.msg" ":defaultTextureList1.tx" -na;
connectAttr "imagePlane1.msg" ":defaultRenderUtilityList1.u" -na;
connectAttr "place2dTexture1.msg" ":defaultRenderUtilityList1.u" -na;
connectAttr "defaultRenderLayer.msg" ":defaultRenderingList1.r" -na;
// End of lvl13.ma
