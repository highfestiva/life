//Maya ASCII 2009 scene
//Name: level_07.ma
//Last modified: Tue, Jul 30, 2013 02:50:20 PM
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
	setAttr ".t" -type "double3" -50.610815630592938 43.078439441836402 336.94079404474166 ;
	setAttr ".r" -type "double3" -3.3383527297714899 -15.800000000000118 1.5635768264400581e-012 ;
	setAttr ".rp" -type "double3" -7.1054273576010019e-015 0 0 ;
	setAttr ".rpt" -type "double3" -4.682534143319496e-015 -1.1090946422296633e-015 
		-1.484517683741257e-014 ;
createNode camera -s -n "perspShape" -p "persp";
	setAttr -k off ".v";
	setAttr ".fl" 34.999999999999979;
	setAttr ".fcp" 3000;
	setAttr ".coi" 329.96966168091177;
	setAttr ".imn" -type "string" "persp";
	setAttr ".den" -type "string" "persp_depth";
	setAttr ".man" -type "string" "persp_mask";
	setAttr ".tp" -type "double3" -24.176367568481247 39.573611270738944 1.1244457513378165 ;
	setAttr ".hc" -type "string" "viewSet -p %camera";
	setAttr -s 2 ".b";
createNode transform -s -n "top";
	setAttr ".t" -type "double3" 0.35295377438606579 100.24904921614335 5.4317589722171933 ;
	setAttr ".r" -type "double3" -89.999999999999986 0 0 ;
createNode camera -s -n "topShape" -p "top";
	setAttr -k off ".v";
	setAttr ".rnd" no;
	setAttr ".fcp" 3000;
	setAttr ".coi" 100.1;
	setAttr ".ow" 99.951714147754714;
	setAttr ".imn" -type "string" "top";
	setAttr ".den" -type "string" "top_depth";
	setAttr ".man" -type "string" "top_mask";
	setAttr ".hc" -type "string" "viewSet -t %camera";
	setAttr ".o" yes;
createNode transform -s -n "front";
	setAttr ".t" -type "double3" 0 0 100.1 ;
createNode camera -s -n "frontShape" -p "front";
	setAttr -k off ".v";
	setAttr ".rnd" no;
	setAttr ".fcp" 3000;
	setAttr ".coi" 100.1;
	setAttr ".ow" 30;
	setAttr ".imn" -type "string" "front";
	setAttr ".den" -type "string" "front_depth";
	setAttr ".man" -type "string" "front_mask";
	setAttr ".hc" -type "string" "viewSet -f %camera";
	setAttr ".o" yes;
createNode transform -s -n "side";
	setAttr ".t" -type "double3" 100.1 0 0 ;
	setAttr ".r" -type "double3" 0 89.999999999999986 0 ;
createNode camera -s -n "sideShape" -p "side";
	setAttr -k off ".v";
	setAttr ".rnd" no;
	setAttr ".fcp" 3000;
	setAttr ".coi" 100.1;
	setAttr ".ow" 30;
	setAttr ".imn" -type "string" "side";
	setAttr ".den" -type "string" "side_depth";
	setAttr ".man" -type "string" "side_mask";
	setAttr ".hc" -type "string" "viewSet -s %camera";
	setAttr ".o" yes;
createNode transform -n "i_m_hangar";
	setAttr ".t" -type "double3" 0.35295377438606579 0.78258325556203723 5.4317589722171711 ;
createNode transform -n "transform1" -p "i_m_hangar";
createNode mesh -n "i_m_hangarShape" -p "|i_m_hangar|transform1";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map_hangar";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "m_hangar";
	setAttr ".s" -type "double3" 4 2 4 ;
createNode mesh -n "m_hangarShape" -p "m_hangar";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	addAttr -ci true -sn "rgn" -ln "rgn" -dt "vectorArray";
	addAttr -ci true -sn "rguv0" -ln "rguv0" -dt "vectorArray";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".pv" -type "double2" 0.91679644584655762 2.1871035099029541 ;
	setAttr ".uvst[0].uvsn" -type "string" "map_hangar";
	setAttr ".cuvs" -type "string" "map_hangar";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 6 ".pt";
	setAttr ".pt[0]" -type "float3" 0 0 17.127069 ;
	setAttr ".pt[2]" -type "float3" 0 0 17.127069 ;
	setAttr ".pt[4:7]" -type "float3" 0 20.53385 0  0 20.53385 17.127069  
		0 20.53385 17.127069  0 20.53385 0 ;
	setAttr ".rgvtx" -type "vectorArray" 8 -23.553211212158203 0.25 -6.4689846038818359 -23.55320930480957
		 0.25 25 23.679733276367188 0.25 -6.5304737091064453 23.679733276367188 0.25 25 23.679733276367188
		 38.975875854492188 25 23.679733276367188 38.975879669189453 -6.5304737091064453 -23.553211212158203
		 38.975879669189453 -6.4689846038818359 -23.55320930480957 38.975875854492188 25 ;
	setAttr ".rgf" -type "string" "[[2,3,4,5],[0,2,5,6],[1,0,6,7],[6,5,4,7]]";
	setAttr ".rgn" -type "vectorArray" 16 -1 0 0 -1 0 0 -1 0 0 -1 0 0 0.0013018286554142833
		 0 0.99999910593032837 0.0013018286554142833 0 0.99999910593032837 0.0013018286554142833
		 0 0.99999910593032837 0.0013018286554142833 0 0.99999910593032837 1 0 -6.0610425123286404e-008 1
		 0 -6.0610425123286404e-008 1 0 -6.0610425123286404e-008 1 0 -6.0610425123286404e-008 -7.8827257221636415e-011
		 -0.99999994039535522 -1.9617250757164584e-007 -7.8827257221636415e-011 -0.99999994039535522
		 -1.9617250757164584e-007 -7.8827257221636415e-011 -0.99999994039535522 -1.9617250757164584e-007 -7.8827257221636415e-011
		 -0.99999994039535522 -1.9617250757164584e-007 ;
	setAttr ".rguv0" -type "vectorArray" 16 2.6693053245544434 -1.2822554111480713
		 0 0.85596847534179688 -1.2822554111480713 0 0.85596847534179688 1.5590784549713135
		 0 2.6693053245544434 1.5590784549713135 0 -1.300450325012207 -0.63886499404907227
		 0 1.8323161602020264 -0.63886499404907227 0 1.8323161602020264 1.2798933982849121
		 0 -1.300450325012207 1.2798933982849121 0 0.85596847534179688 -1.2822554111480713
		 0 2.6632306575775146 -1.2822554111480713 0 2.6632306575775146 1.5590784549713135
		 0 0.85596847534179688 1.5590784549713135 0 2.8468086719512939 2.1904428005218506
		 0 2.8576314449310303 0.18351545929908752 0 0.9193274974822998 0.18017193675041199
		 0 0.91679644584655762 2.1871035099029541 0 ;
createNode transform -n "m_rack" -p "m_hangar";
	setAttr ".t" -type "double3" 15.571443257476606 0 14.364245880444315 ;
	setAttr ".s" -type "double3" 0.5 1 0.5 ;
	setAttr ".rp" -type "double3" -15.571443257476606 0 -14.364245880444315 ;
	setAttr ".sp" -type "double3" -31.142886514953211 0 -28.72849176088863 ;
	setAttr ".spt" -type "double3" 15.571443257476606 0 14.364245880444315 ;
createNode mesh -n "m_rackShape" -p "m_rack";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	addAttr -ci true -sn "rgn" -ln "rgn" -dt "vectorArray";
	setAttr -k off ".v";
	setAttr -s 4 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".rgvtx" -type "vectorArray" 56 -20.624794006347656 19.75 -20.812261581420898 4.3752059936523437
		 19.75 -20.812261581420898 -20.624794006347656 20.25 -20.812261581420898 4.3752059936523437
		 20.25 -20.812261581420898 -20.624794006347656 20.25 -32.812263488769531 4.3752059936523437
		 20.25 -32.812263488769531 -20.624794006347656 19.75 -32.812263488769531 4.3752059936523437
		 19.75 -32.812263488769531 4.0283923149108887 0.021775245666503906 -20.697963714599609 4.5283923149108887
		 0.021775245666503906 -20.697963714599609 4.0283923149108887 25.021774291992188 -20.697963714599609 4.5283923149108887
		 25.021774291992188 -20.697963714599609 4.0283923149108887 25.021774291992188 -21.197963714599609 4.5283923149108887
		 25.021774291992188 -21.197963714599609 4.0283923149108887 0.021775245666503906 -21.197963714599609 4.5283923149108887
		 0.021775245666503906 -21.197963714599609 4.0283923149108887 0.021775245666503906
		 -32.412857055664063 4.5283923149108887 0.021775245666503906 -32.412857055664063 4.0283923149108887
		 25.021774291992188 -32.412857055664063 4.5283923149108887 25.021774291992188 -32.412857055664063 4.0283923149108887
		 25.021774291992188 -32.912857055664063 4.5283923149108887 25.021774291992188 -32.912857055664063 4.0283923149108887
		 0.021775245666503906 -32.912857055664063 4.5283923149108887 0.021775245666503906
		 -32.912857055664063 -8.0660305023193359 0.021775245666503906 -20.697963714599609 -7.5660305023193359
		 0.021775245666503906 -20.697963714599609 -8.0660305023193359 25.021774291992188 -20.697963714599609 -7.5660305023193359
		 25.021774291992188 -20.697963714599609 -8.0660305023193359 25.021774291992188 -21.197963714599609 -7.5660305023193359
		 25.021774291992188 -21.197963714599609 -8.0660305023193359 0.021775245666503906 -21.197963714599609 -7.5660305023193359
		 0.021775245666503906 -21.197963714599609 -8.0660305023193359 0.021775245666503906
		 -32.412857055664063 -7.5660305023193359 0.021775245666503906 -32.412857055664063 -8.0660305023193359
		 25.021774291992188 -32.412857055664063 -7.5660305023193359 25.021774291992188 -32.412857055664063 -8.0660305023193359
		 25.021774291992188 -32.912857055664063 -7.5660305023193359 25.021774291992188 -32.912857055664063 -8.0660305023193359
		 0.021775245666503906 -32.912857055664063 -7.5660305023193359 0.021775245666503906
		 -32.912857055664063 -20.758083343505859 0.021775245666503906 -20.697963714599609 -20.258083343505859
		 0.021775245666503906 -20.697963714599609 -20.758083343505859 25.021774291992188 -20.697963714599609 -20.258083343505859
		 25.021774291992188 -20.697963714599609 -20.758083343505859 25.021774291992188 -21.197963714599609 -20.258083343505859
		 25.021774291992188 -21.197963714599609 -20.758083343505859 0.021775245666503906 -21.197963714599609 -20.258083343505859
		 0.021775245666503906 -21.197963714599609 -20.758083343505859 0.021775245666503906
		 -32.412857055664063 -20.258083343505859 0.021775245666503906 -32.412857055664063 -20.758083343505859
		 25.021774291992188 -32.412857055664063 -20.258083343505859 25.021774291992188 -32.412857055664063 -20.758083343505859
		 25.021774291992188 -32.912857055664063 -20.258083343505859 25.021774291992188 -32.912857055664063 -20.758083343505859
		 0.021775245666503906 -32.912857055664063 -20.258083343505859 0.021775245666503906
		 -32.912857055664063 ;
	setAttr ".rgf" -type "string" "[[0,1,3,2],[2,3,5,4],[4,5,7,6],[6,7,1,0],[1,7,5,3],[6,0,2,4],[8,9,11,10],[10,11,13,12],[12,13,15,14],[14,15,9,8],[9,15,13,11],[14,8,10,12],[16,17,19,18],[18,19,21,20],[20,21,23,22],[22,23,17,16],[17,23,21,19],[22,16,18,20],[24,25,27,26],[26,27,29,28],[28,29,31,30],[30,31,25,24],[25,31,29,27],[30,24,26,28],[32,33,35,34],[34,35,37,36],[36,37,39,38],[38,39,33,32],[33,39,37,35],[38,32,34,36],[40,41,43,42],[42,43,45,44],[44,45,47,46],[46,47,41,40],[41,47,45,43],[46,40,42,44],[48,49,51,50],[50,51,53,52],[52,53,55,54],[54,55,49,48],[49,55,53,51],[54,48,50,52]]";
	setAttr ".rgn" -type "vectorArray" 168 0 0 1 0 0 1 0 0 1 0 0 1 0 0.99999994039535522
		 0 0 0.99999994039535522 0 0 0.99999994039535522 0 0 0.99999994039535522 0 0 0 -1 0
		 0 -1 0 0 -1 0 0 -1 0 -0.99999994039535522 0 0 -0.99999994039535522 0 0 -0.99999994039535522
		 0 0 -0.99999994039535522 0 1 0 0 1 0 0 1 0 0 1 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 0 0
		 0 1 0 0 1 0 0 1 0 0 1 0 1 0 0 1 0 0 1 0 0 1 0 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 -1 0 0
		 -1 0 0 -1 0 0 -1 0 1 0 0 1 0 0 1 0 0 1 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 0 0 0 1 0 0
		 1 0 0 1 0 0 1 0 1 0 0 1 0 0 1 0 0 1 0 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 -1 0 0 -1 0 0
		 -1 0 0 -1 0 1 0 0 1 0 0 1 0 0 1 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 0 0 0 1 0 0 1 0 0 1 0
		 0 1 0 1 0 0 1 0 0 1 0 0 1 0 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 -1 0 0 -1 0 0 -1 0 0 -1
		 0 1 0 0 1 0 0 1 0 0 1 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 0 0 0 1 0 0 1 0 0 1 0 0 1 0 1
		 0 0 1 0 0 1 0 0 1 0 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 -1 0 0 -1 0 0 -1 0 0 -1 0 1 0 0 1
		 0 0 1 0 0 1 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 0 0 0 1 0 0 1 0 0 1 0 0 1 0 1 0 0 1 0 0
		 1 0 0 1 0 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 -1 0 0 -1 0 0 -1 0 0 -1 0 1 0 0 1 0 0 1 0
		 0 1 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 0 0 0 1 0 0 1 0 0 1 0 0 1 0 1 0 0 1 0 0 1 0 0 1
		 0 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 -1 0 0 -1 0 0 -1 0 0 -1 0 1 0 0 1 0 0 1 0 0 1 0 0 -1
		 0 0 -1 0 0 -1 0 0 -1 0 0 ;
createNode transform -n "phys_rack1" -p "m_rack";
	setAttr ".t" -type "double3" -39.267680643511234 10 -55.540753859022885 ;
createNode mesh -n "phys_rackShape1" -p "phys_rack1";
	setAttr -s 2 ".wm";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 4 ".iog";
	setAttr -s 2 ".iog[2].og";
	setAttr -s 2 ".iog[3].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr -s 2 ".ciog";
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "phys_rack2" -p "m_rack";
	setAttr ".t" -type "double3" -39.267680643511234 20 -55.540753859022885 ;
createNode transform -n "phys_rackPillar1" -p "m_rack";
	setAttr ".t" -type "double3" -26.864494116642746 12.52177498900233 -49.676455586786901 ;
createNode mesh -n "phys_rackPillar1Shape" -p "phys_rackPillar1";
	setAttr -s 6 ".wm";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 12 ".iog";
	setAttr -s 2 ".iog[6].og";
	setAttr -s 2 ".iog[7].og";
	setAttr -s 2 ".iog[8].og";
	setAttr -s 2 ".iog[9].og";
	setAttr -s 2 ".iog[10].og";
	setAttr -s 2 ".iog[11].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr -s 6 ".ciog";
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "phys_rackPillar2" -p "m_rack";
	setAttr ".t" -type "double3" -26.864494116642746 12.52177498900233 -61.391348366470581 ;
createNode transform -n "phys_rackPillar3" -p "m_rack";
	setAttr ".t" -type "double3" -38.958916899425958 12.52177498900233 -49.676455586786901 ;
createNode transform -n "phys_rackPillar4" -p "m_rack";
	setAttr ".t" -type "double3" -38.958916899425958 12.52177498900233 -61.391348366470581 ;
createNode transform -n "phys_rackPillar5" -p "m_rack";
	setAttr ".t" -type "double3" -51.650969701164314 12.52177498900233 -49.676455586786901 ;
createNode transform -n "phys_rackPillar6" -p "m_rack";
	setAttr ".t" -type "double3" -51.650969701164314 12.52177498900233 -61.391348366470581 ;
createNode transform -n "phys_rack" -p "m_rack";
	setAttr ".t" -type "double3" -8.1331177064615829 19.939213092950919 -26.744204085396582 ;
	setAttr ".s" -type "double3" 0.5 0.5 0.5 ;
	setAttr ".rp" -type "double3" -23.009768808491629 -19.939213092950919 -1.9842876754920482 ;
	setAttr ".sp" -type "double3" -46.019537616983257 -39.878426185901837 -3.9685753509840964 ;
	setAttr ".spt" -type "double3" 23.009768808491629 19.939213092950919 1.9842876754920482 ;
createNode mesh -n "phys_rackShape" -p "phys_rack";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "m_floor" -p "m_hangar";
	setAttr ".t" -type "double3" 0 0 17.152343963824936 ;
	setAttr ".rp" -type "double3" 0 0 -17.152343963824936 ;
	setAttr ".sp" -type "double3" 0 0 -17.152343963824936 ;
createNode mesh -n "m_floorShape" -p "m_floor";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	addAttr -ci true -sn "rgn" -ln "rgn" -dt "vectorArray";
	addAttr -ci true -sn "rguv0" -ln "rguv0" -dt "vectorArray";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".pv" -type "double2" 0.39287859201431274 0.48841962218284607 ;
	setAttr -s 2 ".uvst";
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".uvst[1].uvsn" -type "string" "map_hangar";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".rgvtx" -type "vectorArray" 4 -23.55320930480957 0.25000011920928955
		 25 23.679733276367188 0.25000011920928955 25 23.679733276367188 0.24999988079071045
		 -23.657543182373047 -23.553211212158203 0.24999988079071045 -23.596054077148437 ;
	setAttr ".rgf" -type "string" "[[0,1,2,3]]";
	setAttr ".rgn" -type "vectorArray" 4 -3.1914464560661715e-012 0.99999994039535522
		 -4.7750421394709974e-009 -3.1914464560661715e-012 0.99999994039535522 -4.7750421394709974e-009 -3.1914464560661715e-012
		 0.99999994039535522 -4.7750421394709974e-009 -3.1914464560661715e-012 0.99999994039535522
		 -4.7750421394709974e-009 ;
	setAttr ".rguv0" -type "vectorArray" 4 0.39810776710510254 0.3396645188331604
		 0 0.59877347946166992 0.3396645188331604 0 0.59478926658630371 0.48861679434776306
		 0 0.39287859201431274 0.48841962218284607 0 ;
createNode transform -n "phys_floor" -p "m_floor";
	setAttr ".t" -type "double3" -0.35295377438606579 -1.158091272501105 0 ;
	setAttr ".s" -type "double3" 1 1 0.5 ;
	setAttr ".rp" -type "double3" 0.35295377438606579 1.158091272501105 -17.152343963824936 ;
	setAttr ".sp" -type "double3" 0.35295377438606579 1.158091272501105 -34.304687927649873 ;
	setAttr ".spt" -type "double3" 0 0 17.152343963824936 ;
createNode mesh -n "phys_floorShape" -p "phys_floor";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "phys_wall1" -p "m_hangar";
	setAttr ".t" -type "double3" -24.31524210919838 20 17.152343963824936 ;
	setAttr ".s" -type "double3" 0.5 1 0.5 ;
	setAttr ".rp" -type "double3" 24.31524210919838 -20 -17.152343963824936 ;
	setAttr ".sp" -type "double3" 48.630484218396759 -20 -34.304687927649873 ;
	setAttr ".spt" -type "double3" -24.31524210919838 0 17.152343963824936 ;
createNode mesh -n "phys_wallShape1" -p "phys_wall1";
	setAttr -k off ".v";
	setAttr -s 2 ".iog";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "phys_wall2" -p "m_hangar";
	setAttr ".t" -type "double3" 24.430354048629376 20 17.152343963824936 ;
	setAttr ".s" -type "double3" 0.5 1 0.5 ;
	setAttr ".rp" -type "double3" -24.430354048629376 -20 -17.152343963824936 ;
	setAttr ".sp" -type "double3" -48.860708097258751 -20 -34.304687927649873 ;
	setAttr ".spt" -type "double3" 24.430354048629376 0 17.152343963824936 ;
createNode transform -n "phys_ceiling" -p "m_hangar";
	setAttr ".t" -type "double3" 0 40 17.152343963824936 ;
	setAttr ".s" -type "double3" 1 1 0.5 ;
	setAttr ".rp" -type "double3" 0 -40 -17.152343963824936 ;
	setAttr ".sp" -type "double3" 0 -40 -34.304687927649873 ;
	setAttr ".spt" -type "double3" 0 0 17.152343963824936 ;
createNode mesh -n "phys_ceilingShape" -p "phys_ceiling";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "phys_hangar" -p "m_hangar";
	setAttr ".t" -type "double3" -0.35295377438606579 20 -7.3208420714251794 ;
	setAttr ".s" -type "double3" 1 1 0.5 ;
	setAttr ".rp" -type "double3" 0.35295377438606579 -20 7.3208420714251794 ;
	setAttr ".sp" -type "double3" 0.35295377438606579 -20 14.641684142850359 ;
	setAttr ".spt" -type "double3" 0 0 -7.3208420714251794 ;
createNode mesh -n "phys_hangarShape" -p "phys_hangar";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "m_startingpad" -p "m_hangar";
	setAttr ".t" -type "double3" -16.624535820656448 0.091000828542435019 -4.9960036108132044e-016 ;
	setAttr ".s" -type "double3" 0.5 1 0.5 ;
	setAttr ".rp" -type "double3" 16.624535820656448 -0.091000828542435019 4.9960036108132044e-016 ;
	setAttr ".sp" -type "double3" 33.249071641312895 -0.091000828542435019 9.9920072216264089e-016 ;
	setAttr ".spt" -type "double3" -16.624535820656448 0 -4.9960036108132044e-016 ;
createNode transform -n "phys_startingpad" -p "m_startingpad";
	setAttr ".t" -type "double3" 0 0 6.6613381477509392e-016 ;
	setAttr ".rp" -type "double3" 33.249071641312895 -0.091000828542435019 3.3306690738754696e-016 ;
	setAttr ".sp" -type "double3" 33.249071641312895 -0.091000828542435019 3.3306690738754696e-016 ;
createNode mesh -n "m_landingpadShape" -p "phys_startingpad";
	addAttr -ci true -sn "mso" -ln "miShadingSamplesOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "msh" -ln "miShadingSamples" -min 0 -smx 8 -at "float";
	addAttr -ci true -sn "mdo" -ln "miMaxDisplaceOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "mmd" -ln "miMaxDisplace" -min 0 -smx 1 -at "float";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	addAttr -ci true -sn "rgn" -ln "rgn" -dt "vectorArray";
	addAttr -ci true -sn "rguv0" -ln "rguv0" -dt "vectorArray";
	setAttr -k off ".v";
	setAttr -s 4 ".iog";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".pv" -type "double2" 0 1 ;
	setAttr ".uvst[0].uvsn" -type "string" "helipad_map";
	setAttr ".cuvs" -type "string" "helipad_map";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".rgvtx" -type "vectorArray" 8 -3 -2.0000002384185791 3 3 -2.0000002384185791
		 3 -3 1.9999997615814209 3 3 1.9999997615814209 3 -3 1.9999997615814209 -3.0000002384185791 3
		 1.9999997615814209 -3.0000002384185791 -3 -2.0000002384185791 -3.0000002384185791 3
		 -2.0000002384185791 -3.0000002384185791 ;
	setAttr ".rgf" -type "string" "[[0,1,3,2],[2,3,5,4],[4,5,7,6],[6,7,1,0],[1,7,5,3],[6,0,2,4]]";
	setAttr ".rgn" -type "vectorArray" 24 0 0 1 0 0 1 0 0 1 0 0 1 0 1 0 0 1 0 0 1
		 0 0 1 0 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 -1 0 0 -1 0 0 -1 0 0 -1 0 1 0 0 1 0 0 1 0 0 1
		 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 0 ;
	setAttr ".rguv0" -type "vectorArray" 24 0 0 0 1 0 0 1 0 0 0 0 0 0 0 0 1 0 0 1
		 1 0 0 1 0 0 1 0 1 1 0 1 1 0 0 1 0 0 1 0 1 1 0 1 0 0 0 0 0 1 0 0 1 1 0 1 1 0 1 0 0 0
		 1 0 0 0 0 0 0 0 0 1 0 ;
createNode transform -n "phys_pos_start" -p "m_startingpad";
	addAttr -ci true -sn "nts" -ln "notes" -dt "string";
	setAttr ".t" -type "double3" 0 3.7125818424270509 1.6069320361872788e-014 ;
	setAttr ".r" -type "double3" 0 -89.999999999999986 0 ;
	setAttr ".s" -type "double3" 2.5 2.5 2.5 ;
	setAttr ".rp" -type "double3" -9.896131370482513e-015 -3.8035826709694862 -33.249071641312895 ;
	setAttr ".rpt" -type "double3" 33.249071641312902 0 33.249071641312881 ;
	setAttr ".sp" -type "double3" -3.958452548193005e-015 -1.5214330683877948 -13.299628656525158 ;
	setAttr ".spt" -type "double3" -5.9376788222895056e-015 -2.2821496025816912 -19.949442984787737 ;
	setAttr ".nts" -type "string" "\t";
createNode mesh -n "phys_pos_startShape" -p "phys_pos_start";
	addAttr -ci true -sn "mso" -ln "miShadingSamplesOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "msh" -ln "miShadingSamples" -min 0 -smx 8 -at "float";
	addAttr -ci true -sn "mdo" -ln "miMaxDisplaceOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "mmd" -ln "miMaxDisplace" -min 0 -smx 1 -at "float";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "m_landingpad" -p "m_hangar";
	setAttr ".t" -type "double3" 14.630467575282308 0.031698116491645167 -4.9960036108132054e-016 ;
	setAttr ".s" -type "double3" 0.5 1 0.5 ;
	setAttr ".rp" -type "double3" -14.630467575282308 -0.031698116491645167 4.9960036108132054e-016 ;
	setAttr ".sp" -type "double3" -29.260935150564617 -0.031698116491645167 9.9920072216264108e-016 ;
	setAttr ".spt" -type "double3" 14.630467575282308 0 -4.9960036108132054e-016 ;
createNode transform -n "phys_landingpad" -p "m_landingpad";
	setAttr ".t" -type "double3" 0 1.7763568394002505e-015 6.6613381477509392e-016 ;
	setAttr ".rp" -type "double3" -29.260935150564617 -0.031698116491646944 3.3306690738754716e-016 ;
	setAttr ".sp" -type "double3" -29.260935150564617 -0.031698116491646944 3.3306690738754716e-016 ;
createNode transform -n "phys_trig_landing" -p "m_landingpad";
	setAttr ".t" -type "double3" -0.10202169147094509 2.1139647063773754 9.9920072216264089e-016 ;
	setAttr ".s" -type "double3" 0.5 0.5 0.5 ;
	setAttr ".rp" -type "double3" -29.158913459093672 -2.1456628228690207 1.9721522630525295e-031 ;
	setAttr ".sp" -type "double3" -58.317826918187343 -4.2913256457380413 3.944304526105059e-031 ;
	setAttr ".spt" -type "double3" 29.158913459093672 2.1456628228690207 -1.9721522630525295e-031 ;
createNode mesh -n "phys_trig_landingShape" -p "phys_trig_landing";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "phys_pos_sim" -p "m_hangar";
	setAttr ".t" -type "double3" -4.984480076104993 2.4956291177296821 0 ;
	setAttr ".s" -type "double3" 0.5 1 0.5 ;
createNode mesh -n "phys_pos_simShape" -p "phys_pos_sim";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "phys_pos_path_start" -p "m_hangar";
	setAttr ".t" -type "double3" -16.431560189923687 5.9008765316442187 0 ;
	setAttr ".s" -type "double3" 0.5 1 0.5 ;
createNode mesh -n "phys_pos_path_Shape1" -p "phys_pos_path_start";
	addAttr -ci true -sn "mso" -ln "miShadingSamplesOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "msh" -ln "miShadingSamples" -min 0 -smx 8 -at "float";
	addAttr -ci true -sn "mdo" -ln "miMaxDisplaceOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "mmd" -ln "miMaxDisplace" -min 0 -smx 1 -at "float";
	setAttr -k off ".v";
	setAttr -s 6 ".iog";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "phys_pos_path_1" -p "m_hangar";
	setAttr ".t" -type "double3" -8.6790862631503138 20.191211151214748 0 ;
	setAttr ".s" -type "double3" 0.5 1 0.5 ;
createNode transform -n "phys_pos_path_land" -p "m_hangar";
	setAttr ".t" -type "double3" 14.675494954668036 2.1913978358993083 0 ;
	setAttr ".s" -type "double3" 0.5 1 0.5 ;
createNode transform -n "phys_pos_path_2" -p "m_hangar";
	setAttr ".t" -type "double3" -2.1287663760125786 18.996169459761344 0 ;
	setAttr ".s" -type "double3" 0.5 1 0.5 ;
createNode transform -n "phys_pos_path_3" -p "m_hangar";
	setAttr ".t" -type "double3" 4.1888776688469935 11.470273896201622 0 ;
	setAttr ".s" -type "double3" 0.5 1 0.5 ;
createNode transform -n "phys_pos_path_4" -p "m_hangar";
	setAttr ".t" -type "double3" 10.096276879523904 11.470273896201622 0 ;
	setAttr ".s" -type "double3" 0.5 1 0.5 ;
createNode transform -n "i_m_rack3";
	setAttr ".t" -type "double3" -8.124794128558019 10 -26.812262098134255 ;
createNode transform -n "transform9" -p "i_m_rack3";
createNode transform -n "i_m_rack4";
	setAttr ".t" -type "double3" -8.124794128558019 20 -26.812262098134255 ;
createNode transform -n "transform8" -p "i_m_rack4";
createNode transform -n "i_m_rackPillar7";
	setAttr ".t" -type "double3" 4.2783923983104666 12.52177498900233 -20.947963825898274 ;
createNode transform -n "transform7" -p "i_m_rackPillar7";
createNode transform -n "i_m_rackPillar8";
	setAttr ".t" -type "double3" 4.2783923983104666 12.52177498900233 -32.66285660558195 ;
createNode transform -n "transform6" -p "i_m_rackPillar8";
createNode transform -n "i_m_rackPillar9";
	setAttr ".t" -type "double3" -7.8160303844727448 12.52177498900233 -20.947963825898274 ;
createNode transform -n "transform5" -p "i_m_rackPillar9";
createNode transform -n "i_m_rackPillar10";
	setAttr ".t" -type "double3" -7.8160303844727448 12.52177498900233 -32.66285660558195 ;
createNode transform -n "transform4" -p "i_m_rackPillar10";
createNode transform -n "i_m_rackPillar11";
	setAttr ".t" -type "double3" -20.508083186211103 12.52177498900233 -20.947963825898274 ;
createNode transform -n "transform3" -p "i_m_rackPillar11";
createNode transform -n "i_m_rackPillar12";
	setAttr ".t" -type "double3" -20.508083186211103 12.52177498900233 -32.66285660558195 ;
createNode transform -n "transform2" -p "i_m_rackPillar12";
createNode transform -n "i_m_cabin";
	setAttr ".r" -type "double3" 12.157291444336778 0 0 ;
	setAttr ".s" -type "double3" 0.41496719552532679 0.50718522082715978 1.0321282858061136 ;
	setAttr ".rp" -type "double3" 0 2.0032436025600404 -1.147612769536082 ;
	setAttr ".rpt" -type "double3" 0 0.19675639743996037 0.44761276953608164 ;
	setAttr ".sp" -type "double3" 0 3.949727871196612 -1.1118896607312456 ;
	setAttr ".spt" -type "double3" 0 -1.9464842686365709 -0.035723108804836139 ;
createNode transform -n "transform6" -p "i_m_cabin";
createNode mesh -n "i_m_cabinShape" -p "|i_m_cabin|transform6";
	addAttr -ci true -sn "mso" -ln "miShadingSamplesOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "msh" -ln "miShadingSamples" -min 0 -smx 8 -at "float";
	addAttr -ci true -sn "mdo" -ln "miMaxDisplaceOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "mmd" -ln "miMaxDisplace" -min 0 -smx 1 -at "float";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 4 ".iog[0].og";
	setAttr -av ".iog[0].og[0].gco";
	setAttr -av ".iog[0].og[0].gid";
	setAttr -av ".iog[0].og[1].gco";
	setAttr -av ".iog[0].og[2].gco";
	setAttr -av ".iog[0].og[2].gid";
	setAttr -av ".iog[0].og[4].gco";
	setAttr -av ".iog[0].og[4].gid";
	setAttr -av ".iog[0].og[11].gco";
	setAttr -av ".iog[0].og[11].gid";
	setAttr -av ".iog[0].og[12].gco";
	setAttr -av ".iog[0].og[12].gid";
	setAttr -av ".iog[0].og[13].gco";
	setAttr -av ".iog[0].og[13].gid";
	setAttr -av ".iog[0].og[14].gco";
	setAttr -av ".iog[0].og[14].gid";
	setAttr -av ".iog[0].og[15].gco";
	setAttr -av ".iog[0].og[15].gid";
	setAttr -av ".iog[0].og[16].gco";
	setAttr -av ".iog[0].og[16].gid";
	setAttr -av ".iog[0].og[21].gco";
	setAttr -av ".iog[0].og[21].gid";
	setAttr -av ".iog[0].og[28].gco";
	setAttr -av ".iog[0].og[28].gid";
	setAttr -av ".iog[0].og[30].gco";
	setAttr -av ".iog[0].og[30].gid";
	setAttr -av ".iog[0].og[34].gco";
	setAttr -av ".iog[0].og[34].gid";
	setAttr -av ".iog[0].og[36].gco";
	setAttr -av ".iog[0].og[36].gid";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 25 ".pt";
	setAttr ".pt[101]" -type "float3" 0.56094182 0.009343667 0.077039868 ;
	setAttr ".pt[103]" -type "float3" -0.57184654 0.0093436595 0.074236751 ;
	setAttr ".pt[118]" -type "float3" 0.57580149 0.0093436632 0.081323892 ;
	setAttr ".pt[120]" -type "float3" -0.57879615 0.0093436595 0.07815332 ;
	setAttr ".pt[135]" -type "float3" 0.58318251 0.0093436632 0.080764532 ;
	setAttr ".pt[137]" -type "float3" -0.58300179 0.0093436595 0.077326417 ;
	setAttr ".pt[152]" -type "float3" 0.58263248 0.0093436614 0.075173765 ;
	setAttr ".pt[154]" -type "float3" -0.58318251 0.0093436576 0.07142169 ;
	setAttr ".pt[169]" -type "float3" 0.57279515 0.0093436604 0.064526476 ;
	setAttr ".pt[171]" -type "float3" -0.57711625 0.0093436595 0.060440894 ;
	setAttr ".pt[189]" -type "float3" 0.55133873 0.0093436595 0.04922625 ;
	setAttr ".pt[191]" -type "float3" -0.5615083 0.0093436576 0.044921324 ;
	setAttr ".pt[209]" -type "float3" 0.51492989 0.0093436576 0.030089408 ;
	setAttr ".pt[211]" -type "float3" -0.53103793 0.0093436595 0.025809258 ;
	setAttr ".pt[229]" -type "float3" 0.46015272 0.0093436558 0.0080032162 ;
	setAttr ".pt[231]" -type "float3" -0.48092136 0.0093436558 0.0040958463 ;
	setAttr ".pt[249:251]" -type "float3" 0.38657257 0.0093436595 -0.016496405  
		0 0 0  -0.41021973 0.0093436595 -0.019699734 ;
	setAttr ".pt[269]" -type "float3" 0.29699728 0.0093436595 -0.043591805 ;
	setAttr ".pt[271]" -type "float3" -0.32143703 0.0093436595 -0.045916919 ;
	setAttr ".pt[289]" -type "float3" 0.19719188 0 0 ;
	setAttr ".pt[291]" -type "float3" -0.19719188 0 0 ;
	setAttr ".pt[309]" -type "float3" 0.075114422 0 0 ;
	setAttr ".pt[311]" -type "float3" -0.07171613 0 0 ;
createNode transform -n "i_m_engine1";
	setAttr ".t" -type "double3" 0 -0.17158197337961856 -1.0983263099933867 ;
	setAttr ".r" -type "double3" 4.7708320221952736e-015 0 0 ;
	setAttr ".s" -type "double3" 1 1.0000000000000002 1.0000000000000002 ;
	setAttr ".rp" -type "double3" 0 2.3715819733796173 0.39832630999338653 ;
	setAttr ".sp" -type "double3" 0 2.3715819733796168 0.39832630999338636 ;
	setAttr ".spt" -type "double3" 0 4.4408920985006271e-016 1.1102230246251568e-016 ;
createNode transform -n "transform2" -p "i_m_engine1";
createNode mesh -n "i_m_engineShape1" -p "|i_m_engine1|transform2";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "i_m_engine0";
	setAttr ".t" -type "double3" 0 0.40770745978299128 -1.5187211081952101 ;
	setAttr ".r" -type "double3" 11.104859886164098 0 0 ;
	setAttr ".s" -type "double3" 1 1.0000000000000002 1.0000000000000002 ;
	setAttr ".rp" -type "double3" 0 1.9164241899906895 0.45818678149904962 ;
	setAttr ".rpt" -type "double3" 0 -0.12413164977368171 0.36053432669616087 ;
	setAttr ".sp" -type "double3" 0 1.9164241899906895 0.45818678149904946 ;
	setAttr ".spt" -type "double3" 0 4.4408920985006271e-016 1.1102230246251568e-016 ;
createNode transform -n "transform1" -p "i_m_engine0";
createNode mesh -n "i_m_engineShape0" -p "|i_m_engine0|transform1";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 4 ".pt";
	setAttr ".pt[0:1]" -type "float3" -0.41480842 0.39555883 0.73998702  0.41160119 
		0.39555895 0.7399869 ;
	setAttr ".pt[6:7]" -type "float3" 0.06041038 -0.04321342 -0.14408514  
		-0.060410261 -0.043213412 -0.14408511 ;
createNode transform -n "i_m_tail";
	setAttr ".t" -type "double3" 0 0.8761984935213204 -4.1890854842828267 ;
	setAttr ".r" -type "double3" 98.000000000000043 0 0 ;
	setAttr ".s" -type "double3" 0.63338941286823081 0.81117515919650685 0.63338941286823092 ;
	setAttr ".rp" -type "double3" 0 3.2708923834827561 -1.7965052075802168 ;
	setAttr ".rpt" -type "double3" 0 -1.947090877004078 5.2855906918630433 ;
	setAttr ".sp" -type "double3" 0 4.0322886449366528 -2.8363360218557334 ;
	setAttr ".spt" -type "double3" 0 -0.76139626145389683 1.0398308142755168 ;
createNode transform -n "transform5" -p "i_m_tail";
createNode mesh -n "i_m_tailShape" -p "|i_m_tail|transform5";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 21 ".pt";
	setAttr ".pt[0:19]" -type "float3" -0.42879879 8.8817842e-016 0.072851822  
		-0.37822521 8.8817842e-016 0.13857239  -0.29945508 8.8817842e-016 0.19072872  -0.20019883 
		8.8817842e-016 0.22421497  -0.090172403 8.8817842e-016 0.23575366  -0.013157542 8.8817842e-016 
		0.22421497  0.052562974 8.8817842e-016 0.19072866  0.10471918 8.8817842e-016 0.13857237  
		0.13820554 8.8817842e-016 0.072851762  0.14974417 8.8817842e-016 -4.9556618e-008  
		0.13820554 8.8817842e-016 -0.072851926  0.10471924 8.8817842e-016 -0.13857251  0.052563034 
		8.8817842e-016 -0.19072866  -0.013157587 8.8817842e-016 -0.22421503  -0.090172395 
		8.8817842e-016 -0.23575366  -0.20019877 8.8817842e-016 -0.22421497  -0.29945496 8.8817842e-016 
		-0.19072866  -0.37822503 8.8817842e-016 -0.13857248  -0.42879859 8.8817842e-016 -0.072851926  
		-0.44622511 8.8817842e-016 -4.9556618e-008 ;
	setAttr ".pt[40]" -type "float3" -0.090172403 8.8817842e-016 -4.9556618e-008 ;
createNode transform -n "i_m_tower";
	setAttr ".t" -type "double3" 0 1.612022657971623 -0.74907711775550834 ;
	setAttr ".r" -type "double3" 4.7708320221952736e-015 0 0 ;
	setAttr ".s" -type "double3" 0.67616177494628127 1.0000000000000002 1.0000000000000002 ;
	setAttr ".rp" -type "double3" 0 0.58797734202837637 0.049077117755508957 ;
	setAttr ".sp" -type "double3" 0 0.58797734202837582 0.049077117755508382 ;
	setAttr ".spt" -type "double3" 0 1.1102230246251568e-016 1.387778780781446e-017 ;
createNode transform -n "transform4" -p "i_m_tower";
createNode mesh -n "i_m_towerShape" -p "|i_m_tower|transform4";
	addAttr -ci true -sn "mso" -ln "miShadingSamplesOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "msh" -ln "miShadingSamples" -min 0 -smx 8 -at "float";
	addAttr -ci true -sn "mdo" -ln "miMaxDisplaceOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "mmd" -ln "miMaxDisplace" -min 0 -smx 1 -at "float";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 42 ".pt[0:41]" -type "float3"  0.02974396 -0.05469846 -0.0078727966 
		0.025301725 -0.069043852 -0.014974954 0.018382777 -0.080428421 -0.020611253 0.0096643986 
		-0.087737761 -0.024229979 3.4952115e-009 -0.090256386 -0.02547691 -0.0096643921 -0.087737761 
		-0.024229979 -0.018382769 -0.080428421 -0.020611249 -0.02530171 -0.069043845 -0.014974948 
		-0.029743934 -0.054698452 -0.0078727938 -0.031274628 -0.038796473 4.6168389e-009 
		-0.029743934 -0.022894492 0.0078728003 -0.025301706 -0.0085491156 0.014974955 -0.018382763 
		0.0028354514 0.020611249 -0.0096643893 0.010144792 0.024229979 2.5631552e-009 0.012663419 
		0.02547691 0.009664393 0.010144788 0.024229983 0.018382765 0.0028354514 0.020611249 
		0.02530171 -0.0085491156 0.014974954 0.029743934 -0.022894494 0.0078727975 0.031274628 
		-0.038796473 4.6168389e-009 -0.039542247 0 0.10028844 -0.033636648 0 0.11777801 -0.024438454 
		0 0.13165781 -0.012848056 0 0.14056918 -4.6466084e-009 0 0.14363982 0.012848047 0 
		0.14056918 0.024438441 0 0.13165779 0.033636626 0 0.117778 0.039542224 0 0.10028843 
		0.041577153 0 0.080901109 0.039542224 0 0.06151377 0.033636622 0 0.04402421 0.024438435 
		0 0.030144412 0.012848043 0 0.021233041 -3.4075127e-009 0 0.018162388 -0.012848049 
		0 0.021233041 -0.024438437 0 0.03014442 -0.033636626 0 0.04402421 -0.039542224 0 
		0.061513778 -0.041577153 0 0.080901109 3.4952115e-009 -0.038796473 4.6168389e-009 
		-4.6466084e-009 0 0.080901109;
createNode transform -n "i_m_tail_fin";
	setAttr ".t" -type "double3" -0.057228007750387826 1.1937849221450325 -6.5069023593432789 ;
	setAttr ".r" -type "double3" 4.7708320221952736e-015 0 0 ;
	setAttr ".s" -type "double3" 1 1.0000000000000002 1.0000000000000002 ;
	setAttr ".rp" -type "double3" 0.057228007750387826 1.0062150778549668 5.8069023593432787 ;
	setAttr ".sp" -type "double3" 0.057228007750387826 1.0062150778549659 5.8069023593432778 ;
	setAttr ".spt" -type "double3" 0 2.2204460492503136e-016 8.8817841970012543e-016 ;
createNode transform -n "transform3" -p "i_m_tail_fin";
createNode mesh -n "i_m_tail_finShape" -p "|i_m_tail_fin|transform3";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 8 ".pt";
	setAttr ".pt[0:1]" -type "float3" 0 0 -0.011276692  0 0 -0.011276692 ;
	setAttr ".pt[4:7]" -type "float3" 0 -0.017866889 -0.079549588  0 -0.017866889 
		-0.079549588  0 0.017866891 -0.0097649619  0 0.017866891 -0.0097649619 ;
	setAttr ".pt[10:11]" -type "float3" 0 0 0.050234932  0 0 0.050234932 ;
createNode transform -n "i_m_landingbar0";
	setAttr ".t" -type "double3" 1 -1.4247692357198674 -0.06434460132996872 ;
	setAttr ".r" -type "double3" 90 0 0 ;
	setAttr ".s" -type "double3" 1 0.71467302346768746 1.0000000000000002 ;
	setAttr ".rp" -type "double3" -0.99999999999999956 -0.6356553986700304 -3.6247692357198664 ;
	setAttr ".rpt" -type "double3" 0 4.2604246343898966 2.9891138370498354 ;
	setAttr ".sp" -type "double3" -1 -0.88943527710860959 -3.6247692357198655 ;
	setAttr ".spt" -type "double3" 0 0.25377987843857919 -8.8817841970012543e-016 ;
createNode transform -n "transform12" -p "i_m_landingbar0";
createNode mesh -n "i_m_landingbarShape0" -p "transform12";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog";
	setAttr -s 2 ".iog[0].og";
	setAttr -s 2 ".iog[1].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr -s 2 ".ciog";
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 17 ".pt";
	setAttr ".pt[8:23]" -type "float3" 0 1.8571051 4.3715032e-016  0 1.8571051 
		4.3715032e-016  0 1.8571051 4.3715032e-016  0 1.8571051 4.6639979e-016  0 1.8571051 
		4.3715032e-016  0 1.8571051 4.3715032e-016  0 1.8571051 4.3715032e-016  0 1.8571051 
		4.6639979e-016  0 -0.18786046 -0.097372927  0 -0.18786046 -0.097372927  0 -0.18786046 
		-0.097372927  0 -0.18786046 -0.097372927  0 -0.18786046 -0.097372927  0 -0.18786046 
		-0.097372927  0 -0.18786046 -0.097372927  0 -0.18786046 -0.097372927 ;
	setAttr ".pt[25]" -type "float3" 0 -0.18786046 -0.097372927 ;
createNode transform -n "i_m_landingbar1";
	setAttr ".t" -type "double3" -1 -1.4247692357198674 -0.06434460132996872 ;
	setAttr ".r" -type "double3" 90 0 0 ;
	setAttr ".s" -type "double3" 1 0.71467302346768746 1.0000000000000002 ;
	setAttr ".rp" -type "double3" 0.99999999999999956 -0.6356553986700304 -3.6247692357198664 ;
	setAttr ".rpt" -type "double3" 0 4.2604246343898966 2.9891138370498354 ;
	setAttr ".sp" -type "double3" 1 -0.88943527710860959 -3.6247692357198655 ;
	setAttr ".spt" -type "double3" 0 0.25377987843857919 -8.8817841970012543e-016 ;
createNode transform -n "transform11" -p "i_m_landingbar1";
createNode transform -n "i_m_foot0";
	setAttr ".t" -type "double3" 0.58151191973272998 -0.99642112888701084 0.95092288224449173 ;
	setAttr ".r" -type "double3" 1.590277340731758e-015 1.5902773407317584e-015 45.000000000000014 ;
	setAttr ".s" -type "double3" 1 1.0000000000000002 1.0000000000000002 ;
	setAttr ".rp" -type "double3" 1.8490200339801435 2.6714020775477856 -1.6509228822444917 ;
	setAttr ".rpt" -type "double3" -2.4305319537128751 0.52501905133922477 0 ;
	setAttr ".sp" -type "double3" 1.8490200339801435 2.6714020775477838 -1.6509228822444912 ;
	setAttr ".spt" -type "double3" 0 4.4408920985006271e-016 -4.4408920985006271e-016 ;
createNode transform -n "transform10" -p "i_m_foot0";
createNode mesh -n "i_m_footShape0" -p "transform10";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 4 ".iog";
	setAttr -s 2 ".iog[0].og";
	setAttr -s 2 ".iog[1].og";
	setAttr -s 2 ".iog[2].og";
	setAttr -s 2 ".iog[3].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr -s 4 ".ciog";
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "i_m_foot2";
	setAttr ".t" -type "double3" -0.57739903559337036 -1.0015317717297909 0.95092288224449162 ;
	setAttr ".r" -type "double3" 1.590277340731758e-015 -1.5902773407317584e-015 -45.000000000000014 ;
	setAttr ".s" -type "double3" 1 1.0000000000000002 1.0000000000000002 ;
	setAttr ".rp" -type "double3" -1.8555420524556723 2.6721075994929615 -1.6509228822444912 ;
	setAttr ".rpt" -type "double3" 2.4329410880490427 0.52942417223682858 0 ;
	setAttr ".sp" -type "double3" -1.8555420524556716 2.6721075994929606 -1.6509228822444908 ;
	setAttr ".spt" -type "double3" 0 4.4408920985006271e-016 -4.4408920985006271e-016 ;
createNode transform -n "transform9" -p "i_m_foot2";
createNode transform -n "i_m_foot1";
	setAttr ".t" -type "double3" 0.69602077334815182 -0.91146294717234611 -1.24907711775551 ;
	setAttr ".r" -type "double3" -1.590277340731758e-015 -7.9513867036587899e-016 30.000000000000011 ;
	setAttr ".s" -type "double3" 1.0000000000000002 1.0000000000000002 1.0000000000000004 ;
	setAttr ".rp" -type "double3" 0.95295980230498167 3.0426163418593273 0.54907711775550949 ;
	setAttr ".rpt" -type "double3" -1.6489805756531348 0.068846605313018672 0 ;
	setAttr ".sp" -type "double3" 0.9529598023049819 3.0426163418593264 0.54907711775550927 ;
	setAttr ".spt" -type "double3" 2.2204460492503136e-016 8.8817841970012543e-016 2.2204460492503141e-016 ;
createNode transform -n "transform8" -p "i_m_foot1";
createNode transform -n "i_m_foot3";
	setAttr ".t" -type "double3" -0.6956017231963848 -0.91287975602752813 -1.2490771177555096 ;
	setAttr ".r" -type "double3" -1.590277340731758e-015 7.9513867036587899e-016 -30.000000000000011 ;
	setAttr ".s" -type "double3" 1.0000000000000002 1.0000000000000002 1.0000000000000004 ;
	setAttr ".rp" -type "double3" -0.95403111480946368 3.0436338092443376 0.54907711775550971 ;
	setAttr ".rpt" -type "double3" 1.6496328380058491 0.069245946783190848 0 ;
	setAttr ".sp" -type "double3" -0.95403111480946345 3.0436338092443362 0.54907711775550949 ;
	setAttr ".spt" -type "double3" -2.2204460492503136e-016 8.8817841970012543e-016 
		2.2204460492503141e-016 ;
createNode transform -n "transform7" -p "i_m_foot3";
createNode transform -n "i_m_body_whatever";
	setAttr ".rp" -type "double3" 0 2.2 -0.7 ;
	setAttr ".sp" -type "double3" 0 2.2 -0.7 ;
createNode transform -n "i_transform21" -p "i_m_body_whatever";
createNode mesh -n "m_bodyShape" -p "i_transform21";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	addAttr -ci true -sn "rgn" -ln "rgn" -dt "vectorArray";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog[0].og";
	setAttr -av ".iog[0].og[0].gco";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".rgvtx" -type "vectorArray" 414 0.13451318442821503 -1.0440655946731567
		 -0.33612218499183655 -0.13451312482357025 -1.0440657138824463 -0.33612215518951416 -0.14143547415733337
		 -1.0669591426849365 -0.22985236346721649 -0.13451312482357025 -1.0898525714874268
		 -0.12358256429433823 -0.11442369967699051 -1.1105049848556519 -0.027715200558304787 -0.083133675158023834
		 -1.1268949508666992 0.048365559428930283 -0.043705962598323822 -1.1374179124832153
		 0.097212426364421844 -4.2151047097149785e-009 -1.1410437822341919 0.11404388397932053 0.043705951422452927
		 -1.1374177932739258 0.097212396562099457 0.083133667707443237 -1.1268949508666992
		 0.048365559428930283 0.11442367732524872 -1.1105049848556519 -0.027715215459465981 0.13451310992240906
		 -1.0898525714874268 -0.12358258664608002 0.14143544435501099 -1.0669591426849365
		 -0.22985236346721649 0.26571416854858398 -0.98216402530670166 -0.43125042319297791 -0.26571407914161682
		 -0.98216402530670166 -0.43125033378601074 -0.27938830852508545 -1.0273871421813965
		 -0.22132748365402222 -0.26571407914161682 -1.0726103782653809 -0.011404622346162796 -0.22602987289428711
		 -1.113406777381897 0.17796950042247772 -0.16422031819820404 -1.1457829475402832 0.32825762033462524 -0.086335733532905579
		 -1.166569709777832 0.42474856972694397 -8.3264177774822201e-009 -1.1737323999404907
		 0.45799705386161804 0.086335711181163788 -1.166569709777832 0.42474856972694397 0.16422027349472046
		 -1.1457829475402832 0.32825762033462524 0.22602982819080353 -1.113406777381897 0.17796947062015533 0.26571401953697205
		 -1.0726103782653809 -0.011404667049646378 0.27938821911811829 -1.0273871421813965
		 -0.22132748365402222 0.39037242531776428 -0.89607828855514526 -0.51575982570648193 -0.39037227630615234
		 -0.89607828855514526 -0.51575970649719238 -0.41046169400215149 -0.96251767873764038
		 -0.20735280215740204 -0.39037227630615234 -1.0289570093154907 0.10105413943529129 -0.33207046985626221
		 -1.0888928174972534 0.37927201390266418 -0.24126331508159637 -1.136458158493042 0.60006701946258545 -0.12683963775634766
		 -1.166996955871582 0.74182611703872681 -1.2232709067916403e-008 -1.1775199174880981
		 0.7906728982925415 0.12683959305286407 -1.166996955871582 0.74182605743408203 0.2412632554769516
		 -1.1364580392837524 0.60006695985794067 0.33207041025161743 -1.0888928174972534 0.37927195429801941 0.39037218689918518
		 -1.0289570093154907 0.10105407983064651 0.41046160459518433 -0.96251767873764038
		 -0.20735280215740204 0.50541841983795166 -0.78792816400527954 -0.58756953477859497 -0.50541818141937256
		 -0.78792816400527954 -0.58756941556930542 -0.53142815828323364 -0.87394773960113525
		 -0.18827240169048309 -0.50541818141937256 -0.95996731519699097 0.21102461218833923 -0.42993438243865967
		 -1.0375666618347168 0.57123565673828125 -0.31236562132835388 -1.0991499423980713
		 0.8571007251739502 -0.16422031819820404 -1.1386888027191162 1.0406373739242554 -1.5837789746342423e-008
		 -1.1523128747940063 1.1038796901702881 0.16422027349472046 -1.1386886835098267 1.0406372547149658 0.31236553192138672
		 -1.0991498231887817 0.85710060596466064 0.4299342930316925 -1.0375666618347168 0.5712355375289917 0.50541806221008301
		 -0.95996731519699097 0.21102452278137207 0.53142803907394409 -0.87394773960113525
		 -0.18827240169048309 0.60801929235458374 -0.66037660837173462 -0.64491134881973267 -0.60801905393600464
		 -0.66037666797637939 -0.64491117000579834 -0.63930904865264893 -0.76385831832885742
		 -0.1645561158657074 -0.60801905393600464 -0.86734002828598022 0.31579893827438354 -0.51721185445785522
		 -0.96069222688674927 0.7491334080696106 -0.37577641010284424 -1.0347769260406494
		 1.0930297374725342 -0.19755735993385315 -1.082342267036438 1.3138246536254883 -1.9052889044246513e-008
		 -1.0987321138381958 1.3899053335189819 0.19755730032920837 -1.082342267036438 1.3138245344161987 0.37577632069587708
		 -1.0347769260406494 1.0930296182632446 0.51721173524856567 -0.96069222688674927 0.74913334846496582 0.60801887512207031
		 -0.86734002828598022 0.31579884886741638 0.6393088698387146 -0.76385831832885742
		 -0.1645561158657074 0.69564872980117798 -0.51656442880630493 -0.68637323379516602 -0.69564849138259888
		 -0.51656442880630493 -0.68637305498123169 -0.73144811391830444 -0.63496023416519165
		 -0.13678790628910065 -0.69564849138259888 -0.75335597991943359 0.41279727220535278 -0.59175390005111694
		 -0.86016237735748291 0.90858519077301025 -0.42993438243865967 -0.94492435455322266
		 1.3020446300506592 -0.22602987289428711 -0.99934494495391846 1.5546611547470093 -2.1798845395437638e-008
		 -1.018096923828125 1.6417069435119629 0.22602979838848114 -0.99934488534927368 1.5546610355377197 0.4299342930316925
		 -0.94492435455322266 1.3020445108413696 0.59175378084182739 -0.86016231775283813
		 0.9085850715637207 0.69564831256866455 -0.75335597991943359 0.41279715299606323 0.73144787549972534
		 -0.63496023416519165 -0.13678790628910065 0.76614898443222046 -0.36003267765045166
		 -0.71093440055847168 -0.76614874601364136 -0.36003270745277405 -0.71093422174453735 -0.8055763840675354
		 -0.49042725563049316 -0.10565153509378433 -0.76614874601364136 -0.62082183361053467
		 0.49963116645812988 -0.65172499418258667 -0.73845243453979492 1.0456644296646118 -0.47350588440895081
		 -0.83180463314056396 1.4789990186691284 -0.24893680214881897 -0.89174038171768188
		 1.7572168111801147 -2.400804177682403e-008 -0.91239285469055176 1.8530842065811157 0.248936727643013
		 -0.89174038171768188 1.7572166919708252 0.47350579500198364 -0.83180457353591919
		 1.4789988994598389 0.65172487497329712 -0.73845237493515015 1.0456643104553223 0.76614850759506226
		 -0.62082177400588989 0.49963104724884033 0.80557620525360107 -0.49042725563049316
		 -0.10565153509378433 0.81778413057327271 -0.19463567435741425 -0.71798998117446899 0.69564872980117798
		 -0.069077208638191223 -1.3008239269256592 0.50541836023330688 0.030566547065973282
		 -1.7633634805679321 -0.50541824102401733 0.030566470697522163 -1.7633631229400635 -0.69564849138259888
		 -0.069077290594577789 -1.3008235692977905 -0.81778383255004883 -0.19463573396205902
		 -0.71798974275588989 -0.85986876487731934 -0.33381831645965576 -0.071913652122020721 -0.81778383255004883
		 -0.47300088405609131 0.57416248321533203 -0.6956484317779541 -0.59855931997299194
		 1.1569961309432983 -0.50541818141937256 -0.69820302724838257 1.6195354461669922 -0.03294159471988678
		 -0.77429109811782837 1.9952337741851807 -2.5626080812912733e-008 -0.78422254323959351
		 2.0188324451446533 0.028416415676474571 -0.77368175983428955 1.9924052953720093 0.50541806221008301
		 -0.69820296764373779 1.6195353269577026 0.69564831256866455 -0.59855926036834717
		 1.1569960117340088 0.81778359413146973 -0.47300085425376892 0.57416236400604248 0.85986858606338501
		 -0.33381831645965576 -0.071913652122020721 0.84928274154663086 -0.024446079507470131
		 -0.70736628770828247 0.72244304418563843 0.10594853013753891 -1.3126492500305176 0.52488559484481812
		 0.2094302624464035 -1.7930043935775757 -0.52488547563552856 0.20943018794059753 -1.793004035949707 -0.72244280576705933
		 0.10594844818115234 -1.3126488924026489 -0.84928244352340698 -0.024446131661534309
		 -0.70736604928970337 -0.89298832416534424 -0.16898961365222931 -0.036405015736818314 -0.84928244352340698
		 -0.31353309750556946 0.63455605506896973 -0.72244274616241455 -0.44392764568328857
		 1.239838719367981 -0.52488535642623901 -0.54740935564041138 1.7201937437057495 -0.037009838968515396
		 -0.62689274549484253 2.1116528511047363 -2.6613120596152839e-008 -0.63674205541610718
		 2.1348702907562256 0.035767082124948502 -0.62620353698730469 2.1084535121917725 0.52488523721694946
		 -0.5474092960357666 1.72019362449646 0.722442626953125 -0.4439275860786438 1.2398384809494019 0.84928220510482788
		 -0.31353306770324707 0.63455593585968018 0.89298814535140991 -0.16898961365222931
		 -0.036405015736818314 0.85986918210983276 0.14634528756141663 -0.6793249249458313 0.73144841194152832
		 0.27836525440216064 -1.2921526432037354 0.53142833709716797 0.38313692808151245 -1.7784956693649292 -0.53142821788787842
		 0.38313683867454529 -1.7784953117370605 -0.73144811391830444 0.27836519479751587
		 -1.2921522855758667 -0.85986888408660889 0.14634522795677185 -0.67932462692260742 -0.90411949157714844
		 -6.6613381477509392e-016 2.2204460492503131e-016 -0.85986888408660889 -0.14634522795677185
		 0.67932462692260742 -0.73144805431365967 -0.27836516499519348 1.2921521663665771 -0.53142809867858887
		 -0.38313674926757813 1.7784948348999023 -0.037386685609817505 -0.46332672238349915
		 2.1732337474822998 -2.6944855235910836e-008 -0.47358304262161255 2.1983404159545898 0.037461582571268082
		 -0.46257936954498291 2.1697647571563721 0.53142803907394409 -0.38313671946525574
		 1.7784947156906128 0.73144787549972534 -0.27836513519287109 1.2921520471572876 0.85986858606338501
		 -0.14634519815444946 0.6793244481086731 0.90411931276321411 -6.6613381477509392e-016
		 2.2204460492503131e-016 0.84928274154663086 0.31353315711021423 -0.63455629348754883 0.72244304418563843
		 0.44392776489257813 -1.2398391962051392 0.52488559484481812 0.54740947484970093 -1.7201944589614868 -0.52488547563552856
		 0.54740941524505615 -1.7201941013336182 -0.72244280576705933 0.44392767548561096
		 -1.2398388385772705 -0.84928244352340698 0.31353309750556946 -0.63455605506896973 -0.89298832416534424
		 0.16898961365222931 0.036405015736818314 -0.84928244352340698 0.024446131661534309
		 0.70736604928970337 -0.72244274616241455 -0.10594842582941055 1.3126487731933594 -0.52488535642623901
		 -0.20943009853363037 1.7930036783218384 -0.034175202250480652 -0.28757670521736145
		 2.1782577037811279 -2.6613120596152839e-008 -0.29876285791397095 2.2076802253723145 0.03394688293337822
		 -0.286761075258255 2.174471378326416 0.52488523721694946 -0.20943008363246918 1.7930035591125488 0.722442626953125
		 -0.10594837367534637 1.3126485347747803 0.84928220510482788 0.024446157738566399
		 0.70736593008041382 0.89298814535140991 0.16898961365222931 0.036405015736818314 0.81778413057327271
		 0.47300094366073608 -0.57416272163391113 0.69564872980117798 0.59855937957763672
		 -1.1569966077804565 0.50541836023330688 0.69820314645767212 -1.6195361614227295 -0.50541824102401733
		 0.69820308685302734 -1.6195358037948608 -0.69564849138259888 0.59855931997299194
		 -1.1569962501525879 -0.81778383255004883 0.47300088405609131 -0.57416248321533203 -0.85986876487731934
		 0.33381831645965576 0.071913652122020721 -0.81778383255004883 0.19463573396205902
		 0.71798974275588989 -0.6956484317779541 0.06907731294631958 1.300823450088501 -0.50541818141937256
		 -0.030566392466425896 1.7633627653121948 -0.028022849932312965 -0.10393452644348145
		 2.1264352798461914 -2.5626080812912733e-008 -0.11658593267202377 2.1626596450805664 0.026229660958051682
		 -0.10304643213748932 2.1223127841949463 0.50541806221008301 -0.030566366389393806
		 1.7633626461029053 0.69564831256866455 0.069077335298061371 1.3008233308792114 0.81778359413146973
		 0.19463574886322021 0.71798962354660034 0.85986858606338501 0.33381831645965576 0.071913652122020721 0.76614898443222046
		 0.62082183361053467 -0.49963134527206421 0.65172529220581055 0.73845255374908447
		 -1.04566490650177 0.47350609302520752 0.83180475234985352 -1.4789997339248657 0.24893684685230255
		 0.89174056053161621 -1.7572176456451416 0 0.91239303350448608 -1.853084921836853 -0.24893684685230255
		 0.89174050092697144 -1.7572174072265625 -0.47350600361824036 0.83180469274520874
		 -1.4789993762969971 -0.65172505378723145 0.73845243453979492 -1.0456645488739014 -0.76614874601364136
		 0.62082183361053467 -0.49963116645812988 -0.8055763840675354 0.49042725563049316
		 0.10565153509378433 -0.76614874601364136 0.36003270745277405 0.71093422174453735 -0.65172499418258667
		 0.24240210652351379 1.2569675445556641 -0.47350588440895081 0.14904990792274475 1.6903020143508911 -0.020149309188127518
		 0.083046890795230865 2.01918625831604 -2.400804177682403e-008 0.068461678922176361
		 2.064387321472168 0.015929197892546654 0.083982661366462708 2.0148425102233887 0.47350579500198364
		 0.14904993772506714 1.6903018951416016 0.65172487497329712 0.24240212142467499 1.2569674253463745 0.76614850759506226
		 0.36003273725509644 0.7109341025352478 0.80557620525360107 0.49042725563049316 0.10565153509378433 0.69564872980117798
		 0.75335603952407837 -0.41279745101928711 0.59175419807434082 0.86016243696212769
		 -0.90858554840087891 0.42993453145027161 0.94492447376251221 -1.3020452260971069 0.22602994740009308
		 0.99934506416320801 -1.554661750793457 0 1.0180970430374146 -1.6417075395584106 -0.22602994740009308
		 0.99934500455856323 -1.5546616315841675 -0.42993444204330444 0.94492447376251221
		 -1.3020449876785278 -0.59175395965576172 0.86016237735748291 -0.9085853099822998 -0.69564849138259888
		 0.75335597991943359 -0.41279727220535278 -0.73144811391830444 0.63496023416519165
		 0.13678790628910065 -0.69564849138259888 0.51656442880630493 0.68637305498123169 -0.59175390005111694
		 0.40975809097290039 1.1821609735488892 -0.42993438243865967 0.32499608397483826 1.5756205320358276 -0.012350860051810741
		 0.26866790652275085 1.8595945835113525 -2.1798845395437638e-008 0.25182348489761353
		 1.9152827262878418 0.0056664790026843548 0.26959824562072754 1.8552759885787964 0.4299342930316925
		 0.32499611377716064 1.5756204128265381 0.59175378084182739 0.40975809097290039 1.1821608543395996 0.69564831256866455
		 0.51656448841094971 0.68637293577194214 0.73144787549972534 0.63496023416519165 0.13678790628910065 0.60801929235458374
		 0.867340087890625 -0.31579911708831787 0.51721209287643433 0.96069228649139404 -0.7491338849067688 0.37577658891677856
		 1.034777045249939 -1.0930302143096924 0.19755741953849792 1.0823423862457275 -1.313825249671936 0
		 1.0987322330474854 -1.3899058103561401 -0.19755741953849792 1.0823423862457275 -1.3138251304626465 -0.37577646970748901
		 1.034777045249939 -1.0930299758911133 -0.5172119140625 0.96069222688674927 -0.74913352727890015 -0.60801905393600464
		 0.86734002828598022 -0.31579893827438354 -0.63930904865264893 0.76385831832885742
		 0.1645561158657074 -0.60801905393600464 0.66037666797637939 0.64491117000579834 -0.51721185445785522
		 0.56702446937561035 1.0782456398010254 -0.37577641010284424 0.49293974041938782 1.4221419095993042 -0.0066090659238398075
		 0.44826748967170715 1.6520099639892578 -1.9052889044246513e-008 0.42898455262184143
		 1.7190176248550415 -0.0020092891063541174 0.44911685585975647 1.6480673551559448 0.37577632069587708
		 0.49293977022171021 1.4221417903900146 0.51721173524856567 0.56702446937561035 1.0782456398010254 0.60801887512207031
		 0.66037666797637939 0.64491105079650879 0.6393088698387146 0.76385831832885742 0.1645561158657074 0.50541841983795166
		 0.95996737480163574 -0.21102476119995117 0.42993459105491638 1.0375667810440063 -0.57123595476150513 0.31236574053764343
		 1.0991499423980713 -0.85710108280181885 0.16422037780284882 1.1386888027191162 -1.0406378507614136 0
		 1.1523129940032959 -1.1038801670074463 -0.16422037780284882 1.1386888027191162 -1.040637731552124 -0.31236568093299866
		 1.0991499423980713 -0.8571009635925293 -0.42993444204330444 1.0375666618347168 -0.57123571634292603 -0.50541818141937256
		 0.95996731519699097 -0.21102461218833923 -0.53142815828323364 0.87394773960113525
		 0.18827240169048309 -0.50541818141937256 0.78792816400527954 0.58756941556930542 -0.42993438243865967
		 0.71032881736755371 0.94778043031692505 -0.31236562132835388 0.64874559640884399
		 1.2336455583572388 -0.0038053835742175579 0.61742514371871948 1.4015356302261353 -1.5837789746342423e-008
		 0.59558260440826416 1.4804245233535767 -0.0060074618086218834 0.6181214451789856
		 1.3983033895492554 0.31236553192138672 0.64874565601348877 1.2336454391479492 0.4299342930316925
		 0.71032881736755371 0.9477803111076355 0.50541806221008301 0.78792822360992432 0.58756929636001587 0.53142803907394409
		 0.87394773960113525 0.18827240169048309 0.39037242531776428 1.0289570093154907 -0.10105425864458084 0.33207061886787415
		 1.0888928174972534 -0.37927225232124329 0.24126340448856354 1.136458158493042 -0.6000673770904541 0.12683966755867004
		 1.166996955871582 -0.74182647466659546 0 1.1775199174880981 -0.79067325592041016 -0.12683966755867004
		 1.166996955871582 -0.74182641506195068 -0.24126335978507996 1.136458158493042 -0.60006719827651978 -0.33207049965858459
		 1.0888928174972534 -0.37927207350730896 -0.39037227630615234 1.0289570093154907 -0.10105413943529129 -0.41046169400215149
		 0.96251767873764038 0.20735280215740204 -0.39037227630615234 0.89607828855514526
		 0.51575970649719238 -0.33207046985626221 0.83614253997802734 0.79397761821746826 -0.24126331508159637
		 0.78857719898223877 1.0147725343704224 -0.0035955035127699375 0.77214628458023071
		 1.1135463714599609 -1.2232709067916403e-008 0.74751543998718262 1.205378532409668 -0.0065462291240692139
		 0.77265167236328125 1.1112003326416016 0.2412632554769516 0.78857719898223877 1.0147725343704224 0.33207041025161743
		 0.83614253997802734 0.79397755861282349 0.39037218689918518 0.89607834815979004 0.51575964689254761 0.41046160459518433
		 0.96251767873764038 0.20735280215740204 0.26571416854858398 1.0726103782653809 0.011404546909034252 0.22602997720241547
		 1.113406777381897 -0.17796967923641205 0.16422039270401001 1.1457830667495728 -0.32825785875320435 0.086335755884647369
		 1.1665698289871216 -0.42474880814552307 0 1.1737323999404907 -0.45799729228019714 -0.086335755884647369
		 1.1665698289871216 -0.42474880814552307 -0.16422034800052643 1.1457829475402832 -0.32825773954391479 -0.2260299026966095
		 1.113406777381897 -0.1779695600271225 -0.26571407914161682 1.0726103782653809 0.011404622346162796 -0.27938830852508545
		 1.0273871421813965 0.22132748365402222 -0.26571407914161682 0.98216402530670166 0.43125033378601074 -0.22602987289428711
		 0.94136762619018555 0.62062448263168335 -0.16422031819820404 0.90899145603179932
		 0.77091258764266968 -0.0045075686648488045 0.88820463418960571 0.86740350723266602 -8.3264177774822201e-009
		 0.8810420036315918 0.90065199136734009 0.0045075439848005772 0.88820463418960571
		 0.86740350723266602 0.16422027349472046 0.90899145603179932 0.77091258764266968 0.22602982819080353
		 0.94136762619018555 0.62062442302703857 0.26571401953697205 0.98216402530670166 0.43125030398368835 0.27938821911811829
		 1.0273871421813965 0.22132748365402222 0.13451318442821503 1.0898525714874268 0.12358251214027405 0.11442375183105469
		 1.1105049848556519 0.027715111151337624 0.083133712410926819 1.1268949508666992 -0.048365678638219833 0.043705973774194717
		 1.1374179124832153 -0.097212545573711395 0 1.1410437822341919 -0.11404400318861008 -0.043705973774194717
		 1.1374179124832153 -0.097212515771389008 -0.083133697509765625 1.1268949508666992
		 -0.048365619033575058 -0.1144237145781517 1.1105049848556519 0.027715170755982399 -0.13451312482357025
		 1.0898525714874268 0.12358256429433823 -0.14143547415733337 1.0669591426849365 0.22985236346721649 -0.13451312482357025
		 1.0440657138824463 0.33612215518951416 -0.11442369967699051 1.0234131813049316 0.43198952078819275 -0.083133675158023834
		 1.0070233345031738 0.50807029008865356 -0.012535940855741501 0.99650031328201294
		 0.55691713094711304 -4.2151047097149785e-009 0.9928743839263916 0.57374858856201172 0.013946108520030975
		 0.99650037288665771 0.55691713094711304 0.083133667707443237 1.0070233345031738 0.50807029008865356 0.11442367732524872
		 1.0234131813049316 0.43198949098587036 0.13451310992240906 1.0440657138824463 0.33612212538719177 0.14143544435501099
		 1.0669591426849365 0.22985236346721649 0 -1.080258846282959 -0.23271748423576355 0
		 1.080258846282959 0.23271748423576355 -0.058083802461624146 1.2378846406936646 -6.5956950187683105 -0.057939000427722931
		 1.258637547492981 -6.592778205871582 -0.05771351233124733 1.2751071453094482 -6.5904636383056641 -0.057429313659667969
		 1.2856813669204712 -6.5889773368835449 -0.057114243507385254 1.2893248796463013 -6.5884652137756348 -0.077708356082439423
		 1.2856813669204712 -6.5889773368835449 -0.098665319383144379 1.2751071453094482 -6.5904636383056641 -0.11529676616191864
		 1.258637547492981 -6.592778205871582 -0.12597480416297913 1.2378846406936646 -6.5956950187683105 -0.12965419888496399
		 1.2148797512054443 -6.5989279747009277 -0.12597480416297913 1.1918749809265137 -6.6021609306335449 -0.11529670655727386
		 1.1711220741271973 -6.6050777435302734 -0.098665244877338409 1.1546523571014404 -6.6073923110961914 -0.07770836353302002
		 1.144078254699707 -6.6088786125183105 -0.057114243507385254 1.140434741973877 -6.6093907356262207 -0.057429313659667969
		 1.144078254699707 -6.6088786125183105 -0.05771351233124733 1.1546523571014404 -6.6073923110961914 -0.057939000427722931
		 1.1711220741271973 -6.6050777435302734 -0.058083802461624146 1.1918749809265137 -6.6021609306335449 -0.058133751153945923
		 1.2148797512054443 -6.5989279747009277 0.21351280808448792 0.60621654987335205 -1.7695878744125366 0.18162484467029572
		 0.66819113492965698 -1.7608779668807983 0.13195815682411194 0.71737450361251831 -1.7539657354354858 0.069374509155750275
		 0.74895209074020386 -1.7495278120040894 0 0.75983297824859619 -1.7479985952377319 -0.069374509155750275
		 0.74895209074020386 -1.7495278120040894 -0.13195814192295074 0.71737444400787354
		 -1.7539657354354858 -0.18162478506565094 0.66819113492965698 -1.7608779668807983 -0.21351273357868195
		 0.60621654987335205 -1.7695878744125366 -0.22450056672096252 0.53751718997955322
		 -1.779242992401123 -0.21351273357868195 0.46881785988807678 -1.7888981103897095 -0.18162477016448975
		 0.40684327483177185 -1.7976080179214478 -0.13195811212062836 0.35765999555587769
		 -1.8045202493667603 -0.069374486804008484 0.32608237862586975 -1.8089581727981567 -6.6906364892815873e-009
		 0.31520146131515503 -1.8104873895645142 0.06937447190284729 0.32608237862586975 -1.8089581727981567 0.13195808231830597
		 0.35765999555587769 -1.8045202493667603 0.18162472546100616 0.40684330463409424 -1.7976080179214478 0.21351268887519836
		 0.46881785988807678 -1.7888981103897095 0.22450050711631775 0.53751718997955322 -1.779242992401123 -0.057114243507385254
		 1.2148797512054443 -6.5989279747009277 0 0.53751718997955322 -1.779242992401123 0.14872542023658752
		 1.1073241233825684 -0.81875336170196533 0.12651340663433075 1.0929787158966064 -0.88160920143127441 0.091917358338832855
		 1.0815942287445068 -0.9314919114112854 0.048323813825845718 1.0742849111557007 -0.96351850032806396 2.3633284218504969e-009
		 1.0717662572860718 -0.97455418109893799 -0.04832381010055542 1.0742849111557007 -0.96351850032806396 -0.091917343437671661
		 1.0815942287445068 -0.93149185180664063 -0.12651336193084717 1.092978835105896 -0.88160920143127441 -0.14872536063194275
		 1.1073241233825684 -0.81875336170196533 -0.1563790887594223 1.1232261657714844 -0.74907714128494263 -0.14872536063194275
		 1.1391282081604004 -0.67940092086791992 -0.12651334702968597 1.1534734964370728 -0.61654508113861084 -0.091917321085929871
		 1.1648581027984619 -0.56666243076324463 -0.048323795199394226 1.1721674203872681
		 -0.53463584184646606 -2.2971307078734071e-009 1.174686074256897 -0.52360022068023682 0.048323784023523331
		 1.1721674203872681 -0.53463584184646606 0.09191729873418808 1.1648581027984619 -0.5666624903678894 0.12651333212852478
		 1.1534734964370728 -0.61654514074325562 0.14872533082962036 1.1391282081604004 -0.67940092086791992 0.15637905895709991
		 1.1232261657714844 -0.74907714128494263 0.10187674313783646 2.0620226860046387 -0.71059215068817139 0.086661539971828461
		 2.0620226860046387 -0.74885624647140503 0.062963284552097321 2.0620226860046387 -0.77922284603118896 0.033101752400398254
		 2.0620226860046387 -0.79871934652328491 -3.1418589951925924e-009 2.0620226860046387
		 -0.80543744564056396 -0.033101759850978851 2.0620226860046387 -0.79871934652328491 -0.062963277101516724
		 2.0620226860046387 -0.77922284603118896 -0.086661510169506073 2.0620226860046387
		 -0.74885624647140503 -0.10187670588493347 2.0620226860046387 -0.71059215068817139 -0.10711950063705444
		 2.0620226860046387 -0.66817605495452881 -0.10187670588493347 2.0620226860046387 -0.62575995922088623 -0.086661510169506073
		 2.0620226860046387 -0.58749586343765259 -0.06296326220035553 2.0620226860046387 -0.55712932348251343 -0.033101748675107956
		 2.0620226860046387 -0.53763276338577271 -6.3342682210532075e-009 2.0620226860046387
		 -0.53091472387313843 0.033101733773946762 2.0620226860046387 -0.53763276338577271 0.062963239848613739
		 2.0620226860046387 -0.55712932348251343 0.086661472916603088 2.0620226860046387 -0.58749586343765259 0.10187667608261108
		 2.0620226860046387 -0.62575995922088623 0.10711947828531265 2.0620226860046387 -0.66817605495452881 2.3633284218504969e-009
		 1.1232261657714844 -0.74907714128494263 -3.1418589951925924e-009 2.0620226860046387
		 -0.66817605495452881 -0.067228004336357117 0.69378495216369629 -6.3681788444519043 -0.047228008508682251
		 0.69378495216369629 -6.3681788444519043 -0.067228004336357117 1.1937849521636963
		 -6.3569021224975586 -0.047228008508682251 1.1937849521636963 -6.3569021224975586 -0.067228004336357117
		 1.6759181022644043 -6.4364519119262695 -0.047228008508682251 1.6759181022644043 -6.4364519119262695 -0.067228004336357117
		 1.7116518020629883 -6.6666669845581055 -0.047228008508682251 1.7116518020629883 -6.6666669845581055 -0.067228004336357117
		 1.1937849521636963 -6.6569023132324219 -0.047228008508682251 1.1937849521636963 -6.6569023132324219 -0.067228004336357117
		 0.69378495216369629 -6.6066675186157227 -0.047228008508682251 0.69378495216369629
		 -6.6066675186157227 ;
	setAttr ".rgf" -type "string" (
		"[[1,2,15,14],[2,3,16,15],[3,4,17,16],[4,5,18,17],[5,6,19,18],[6,7,20,19],[7,8,21,20],[8,9,22,21],[9,10,23,22],[10,11,24,23],[11,12,25,24],[12,0,13,25],[14,15,28,27],[15,16,29,28],[16,17,30,29],[17,18,31,30],[18,19,32,31],[19,20,33,32],[20,21,34,33],[21,22,35,34],[22,23,36,35],[23,24,37,36],[24,25,38,37],[25,13,26,38],[27,28,41,40],[28,29,42,41],[29,30,43,42],[30,31,44,43],[31,32,45,44],[32,33,46,45],[33,34,47,46],[34,35,48,47],[35,36,49,48],[36,37,50,49],[37,38,51,50],[38,26,39,51],[40,41,54,53],[41,42,55,54],[42,43,56,55],[43,44,57,56],[44,45,58,57],[45,46,59,58],[46,47,60,59],[47,48,61,60],[48,49,62,61],[49,50,63,62],[50,51,64,63],[51,39,52,64],[53,54,67,66],[54,55,68,67],[55,56,69,68],[56,57,70,69],[57,58,71,70],[58,59,72,71],[59,60,73,72],[60,61,74,73],[61,62,75,74],[62,63,76,75],[63,64,77,76],[64,52,65,77],[66,67,80,79],[67,68,81,80],[68,69,82,81],[69,70,83,82],[70,71,84,83],[71,72,85,84],[72,73,86,85],[73,74,87,86],[74,75,88,87],[75,76,89,88],[76,77,90,89],[77,65,78,90],[79,80,97,96],[80,81,98,97],[81,82,99,98],[82,83,100,99],[83,84,101,100],[84,85,102,101],[85,86,103,102],[86,87,104,103],[87,88,105,104],[88,89,106,105],[89,90,107,106],[90,78,91,107],[91,92,109,108],[92,93,110,109],[94,95,112,111],[95,96,113,112],[96,97,114,113],[97,98,115,114],[98,99,116,115],[99,100,117,116],[100,101,118,117],[101,102,119,118],[102,103,120,119],[103,104,121,120],[104,105,122,121],[105,106,123,122],[106,107,124,123],[107,91,108,124],[108,109,126,125],[109,110,127,126],[111,112,129,128],[112,113,130,129],[113,114,131,130],[114,115,132,131],[115,116,133,132],[116,117,134,133],[117,118,135,134],[118,119,136,135],[119,120,137,136],[120,121,138,137],[121,122,139,138],[122,123,140,139],[123,124,141,140],[124,108,125,141],[125,126,143,142],[126,127,144,143],[128,129,146,145],[129,130,147,146],[130,131,148,147],[131,132,149,148],[132,133,150,149],[133,134,151,150],[134,135,152,151],[135,136,153,152],[136,137,154,153],[137,138,155,154],[138,139,156,155],[139,140,157,156],[140,141,158,157],[141,125,142,158],[142,143,160,159],[143,144,161,160],[145,146,163,162],[146,147,164,163],[147,148,165,164],[148,149,166,165],[149,150,167,166],[150,151,168,167],[151,152,169,168],[152,153,170,169],[153,154,171,170],[154,155,172,171],[155,156,173,172],[156,157,174,173],[157,158,175,174],[158,142,159,175],[159,160,177,176],[160,161,178,177],[162,163,183,182],[163,164,184,183],[164,165,185,184],[165,166,186,185],[166,167,187,186],[167,168,188,187],[168,169,189,188],[169,170,190,189],[170,171,191,190],[171,172,192,191],[172,173,193,192],[173,174,194,193],[174,175,195,194],[175,159,176,195],[176,177,197,196],[177,178,198,197],[178,179,199,198],[179,180,200,199],[180,181,201,200],[181,182,202,201],[182,183,203,202],[183,184,204,203],[184,185,205,204],[185,186,206,205],[186,187,207,206],[187,188,208,207],[188,189,209,208],[189,190,210,209],[190,191,211,210],[191,192,212,211],[192,193,213,212],[193,194,214,213],[194,195,215,214],[195,176,196,215],[196,197,217,216],[197,198,218,217],[198,199,219,218],[199,200,220,219],[200,201,221,220],[201,202,222,221],[202,203,223,222],[203,204,224,223],[204,205,225,224],[205,206,226,225],[206,207,227,226],[207,208,228,227],[208,209,229,228],[209,210,230,229],[210,211,231,230],[211,212,232,231],[212,213,233,232],[213,214,234,233],[214,215,235,234],[215,196,216,235],[216,217,237,236],[217,218,238,237],[218,219,239,238],[219,220,240,239],[220,221,241,240],[221,222,242,241],[222,223,243,242],[223,224,244,243],[224,225,245,244],[225,226,246,245],[226,227,247,246],[227,228,248,247],[228,229,249,248],[229,230,250,249],[230,231,251,250],[231,232,252,251],[232,233,253,252],[233,234,254,253],[234,235,255,254],[235,216,236,255],[236,237,257,256],[237,238,258,257],[238,239,259,258],[239,240,260,259],[240,241,261,260],[241,242,262,261],[242,243,263,262],[243,244,264,263],[244,245,265,264],[245,246,266,265],[246,247,267,266],[247,248,268,267],[248,249,269,268],[249,250,270,269],[250,251,271,270],[251,252,272,271],[252,253,273,272],[253,254,274,273],[254,255,275,274],[255,236,256,275],[256,257,277,276],[257,258,278,277],[258,259,279,278],[259,260,280,279],[260,261,281,280],[261,262,282,281],[262,263,283,282],[263,264,284,283],[264,265,285,284],[265,266,286,285],[266,267,287,286],[267,268,288,287],[268,269,289,288],[269,270,290,289],[270,271,291,290],[271,272,292,291],[272,273,293,292],[273,274,294,293],[274,275,295,294],[275,256,276,295],[276,277,297,296],[277,278,298,297],[278,279,299,298],[279,280,300,299],[280,281,301,300],[281,282,302,301],[282,283,303,302],[283,284,304,303],[284,285,305,304],[285,286,306,305],[286,287,307,306],[287,288,308,307],[288,289,309,308],[289,290,310,309],[290,291,311,310],[291,292,312,311],[292,293,313,312],[293,294,314,313],[294,295,315,314],[295,276,296,315],[2,1,316],[3,2,316],[4,3,316],[5,4,316],[6,5,316],[7,6,316],[8,7,316],[9,8,316],[10,9,316],[11,10,316],[12,11,316],[0,12,316],[296,297,317],[297,298,317],[298,299,317],[299,300,317],[300,301,317],[301,302,317],[302,303,317],[303,304,317],[304,305,317],[305,306,317],[306,307,317],[307,308,317],[308,309,317],[309,310,317],[310,311,317],[311,312,317],[312,313,317],[313,314,317],[314,315,317],[315,296,317],[318,319,339,338],[319,320,340,339],[320,321,341,340],[321,322,342,341],[322,323,343,342],[323,324,344,343],[324,325,345,344],[325,326,346,345],[326,327,347,346],[327,328,348,347],[328,329,349,348],[329,330,350,349],[330,331,351,350],[331,332,352,351],[332,333,353,352],[333,334,354,353],[334,335,355,354],[335,336,356,355],[336,337,357,356],[337,318,338,357],[319,318,358],[320,319,358],[321,320,358],[322,321,358],[323,322,358],[324,323,358],[325,324,358],[326,325,358],[327,326,358],[328,327,358],[329,328,358],[330,329,358],[331,330,358],[332,331,358],[333,332,358],[334,333,358],[335,334,358],[336,335,358],[337,336,358],[318,337,358],[338,339,359],[339,340,359],[340,341,359],[341,342,359],[342,343,359],[343,344,359],[344,345,359],[345,346,359],[346,347,359],[347,348,359],[348,349,359],[349,350,359],[350,351,359],[351,352,359],[352,353,359],[353,354,359],[354,355,359],[355,356,359],[356,357,359],[357,338,359],[360,361,381,380],[361,362,382,381],[362,363,383,382],[363,364,384,383],[364,365,385,384],[365,366,386,385],[366,367,387,386],[367,368,388,387],[368,369,389,388],[369,370,390,389],[370,371,391,390],[371,372,392,391],[372,373,393,392],[373,374,394,393],[374,375,395,394],[375,376,396,395],[376,377,397,396],[377,378,398,397],[378,379,399,398],[379,360,380,399],[361,360,400],[362,361,400],[363,362,400],[364,363,400],[365,364,400],[366,365,400],[367,366,400],[368,367,400],[369,368,400],[370,369,400],[371,370,400],[372,371,400],[373,372,400],[374,373,400],[375,374,400],[376,375,400],[377,376,400],[378,377,400],[379,378,400],[360,379,400],[380,381,401],[381,382,401],[382,383,401],[383,384,401],[384,385,401],[385,386,401],[386,387,401],[387,388,401],[388,389,401],[389,390,401],[390,391,401],[391,392,401],[392,393,401],[393,394,401],[394,395,401],[395,396,401],[396,397,401],[397,398,401],[398,399,401],[399,380,401],[402,403,405,404],[404,405,407,406],[406,407,409,408],[408,409,411,410],[410,411,413,412],[412,413,403,402],[403,413,411,405],[405,411,409,407],[412,402,404,410],[410,404,406,408]]");
	setAttr ".rgn" -type "vectorArray" 1672 -0.18948487937450409 -0.95724946260452271
		 -0.21856078505516052 -0.18949861824512482 -0.95986044406890869 -0.2067808210849762 -0.36815297603607178
		 -0.90891349315643311 -0.19580478966236115 -0.36805197596549988 -0.903728187084198
		 -0.21866224706172943 -0.18949861824512482 -0.95986044406890869 -0.2067808210849762 -0.18058434128761292
		 -0.96619850397109985 -0.18398293852806091 -0.35215535759925842 -0.92355859279632568
		 -0.15174427628517151 -0.36815297603607178 -0.90891349315643311 -0.19580478966236115 -0.18058434128761292
		 -0.96619850397109985 -0.18398293852806091 -0.1544276624917984 -0.97434884309768677
		 -0.16369587182998657 -0.30421051383018494 -0.94593101739883423 -0.11256325244903564 -0.35215535759925842
		 -0.92355859279632568 -0.15174427628517151 -0.1544276624917984 -0.97434884309768677
		 -0.16369587182998657 -0.11294329166412354 -0.98256951570510864 -0.14765195548534393 -0.22542650997638702
		 -0.97088062763214111 -0.081079088151454926 -0.30421051383018494 -0.94593101739883423
		 -0.11256325244903564 -0.11294329166412354 -0.98256951570510864 -0.14765195548534393 -0.059700168669223785
		 -0.98872506618499756 -0.13732673227787018 -0.12049256265163422 -0.99087744951248169
		 -0.060360103845596313 -0.22542650997638702 -0.97088062763214111 -0.081079088151454926 -0.059700168669223785
		 -0.98872506618499756 -0.13732673227787018 8.5309159203461604e-007 -0.99101394414901733
		 -0.13375833630561829 2.0923032195696578e-007 -0.99858987331390381 -0.053085923194885254 -0.12049256265163422
		 -0.99087744951248169 -0.060360103845596313 8.5309159203461604e-007 -0.99101394414901733
		 -0.13375833630561829 0.059700269252061844 -0.98872506618499756 -0.13732674717903137 0.12049268931150436
		 -0.99087738990783691 -0.060360267758369446 2.0923032195696578e-007 -0.99858987331390381
		 -0.053085923194885254 0.059700269252061844 -0.98872506618499756 -0.13732674717903137 0.11294253915548325
		 -0.98256951570510864 -0.147652268409729 0.22542642056941986 -0.97088062763214111
		 -0.081078968942165375 0.12049268931150436 -0.99087738990783691 -0.060360267758369446 0.11294253915548325
		 -0.98256951570510864 -0.147652268409729 0.15442764759063721 -0.97434878349304199
		 -0.16369619965553284 0.30421045422554016 -0.94593101739883423 -0.11256314814090729 0.22542642056941986
		 -0.97088062763214111 -0.081078968942165375 0.15442764759063721 -0.97434878349304199
		 -0.16369619965553284 0.18058440089225769 -0.9661986231803894 -0.18398270010948181 0.35215538740158081
		 -0.92355841398239136 -0.15174438059329987 0.30421045422554016 -0.94593101739883423
		 -0.11256314814090729 0.18058440089225769 -0.9661986231803894 -0.18398270010948181 0.18949857354164124
		 -0.95986044406890869 -0.20678053796291351 0.36815285682678223 -0.90891355276107788
		 -0.19580492377281189 0.35215538740158081 -0.92355841398239136 -0.15174438059329987 0.18949857354164124
		 -0.95986044406890869 -0.20678053796291351 0.18948470056056976 -0.95724940299987793
		 -0.21856106817722321 0.36805170774459839 -0.90372830629348755 -0.21866242587566376 0.36815285682678223
		 -0.90891355276107788 -0.19580492377281189 -0.36805197596549988 -0.903728187084198
		 -0.21866224706172943 -0.36815297603607178 -0.90891349315643311 -0.19580478966236115 -0.52756726741790771
		 -0.83046138286590576 -0.17890425026416779 -0.52726978063583374 -0.82292211055755615
		 -0.21162629127502441 -0.36815297603607178 -0.90891349315643311 -0.19580478966236115 -0.35215535759925842
		 -0.92355859279632568 -0.15174427628517151 -0.50742506980895996 -0.85389089584350586
		 -0.11571607738733292 -0.52756726741790771 -0.83046138286590576 -0.17890425026416779 -0.35215535759925842
		 -0.92355859279632568 -0.15174427628517151 -0.30421051383018494 -0.94593101739883423
		 -0.11256325244903564 -0.44509264826774597 -0.89356511831283569 -0.058599032461643219 -0.50742506980895996
		 -0.85389089584350586 -0.11571607738733292 -0.30421051383018494 -0.94593101739883423
		 -0.11256325244903564 -0.22542650997638702 -0.97088062763214111 -0.081079088151454926 -0.33678299188613892
		 -0.94152086973190308 -0.010759084485471249 -0.44509264826774597 -0.89356511831283569
		 -0.058599032461643219 -0.22542650997638702 -0.97088062763214111 -0.081079088151454926 -0.12049256265163422
		 -0.99087744951248169 -0.060360103845596313 -0.18341659009456635 -0.98278129100799561
		 0.022344658151268959 -0.33678299188613892 -0.94152086973190308 -0.010759084485471249 -0.12049256265163422
		 -0.99087744951248169 -0.060360103845596313 2.0923032195696578e-007 -0.99858987331390381
		 -0.053085923194885254 1.5549782972357207e-007 -0.99940896034240723 0.03437795490026474 -0.18341659009456635
		 -0.98278129100799561 0.022344658151268959 2.0923032195696578e-007 -0.99858987331390381
		 -0.053085923194885254 0.12049268931150436 -0.99087738990783691 -0.060360267758369446 0.18341706693172455
		 -0.98278117179870605 0.022344749420881271 1.5549782972357207e-007 -0.99940896034240723
		 0.03437795490026474 0.12049268931150436 -0.99087738990783691 -0.060360267758369446 0.22542642056941986
		 -0.97088062763214111 -0.081078968942165375 0.33678317070007324 -0.9415208101272583
		 -0.010758840478956699 0.18341706693172455 -0.98278117179870605 0.022344749420881271 0.22542642056941986
		 -0.97088062763214111 -0.081078968942165375 0.30421045422554016 -0.94593101739883423
		 -0.11256314814090729 0.44509261846542358 -0.89356505870819092 -0.058598771691322327 0.33678317070007324
		 -0.9415208101272583 -0.010758840478956699 0.30421045422554016 -0.94593101739883423
		 -0.11256314814090729 0.35215538740158081 -0.92355841398239136 -0.15174438059329987 0.50742512941360474
		 -0.85389083623886108 -0.11571596562862396 0.44509261846542358 -0.89356505870819092
		 -0.058598771691322327 0.35215538740158081 -0.92355841398239136 -0.15174438059329987 0.36815285682678223
		 -0.90891355276107788 -0.19580492377281189 0.52756732702255249 -0.83046144247055054
		 -0.1789039671421051 0.50742512941360474 -0.85389083623886108 -0.11571596562862396 0.36815285682678223
		 -0.90891355276107788 -0.19580492377281189 0.36805170774459839 -0.90372830629348755
		 -0.21866242587566376 0.52726995944976807 -0.82292217016220093 -0.21162585914134979 0.52756732702255249
		 -0.83046144247055054 -0.1789039671421051 -0.52726978063583374 -0.82292211055755615
		 -0.21162629127502441 -0.52756726741790771 -0.83046138286590576 -0.17890425026416779 -0.66301876306533813
		 -0.73181390762329102 -0.15765294432640076 -0.66242861747741699 -0.7222791314125061
		 -0.1987491101026535 -0.52756726741790771 -0.83046138286590576 -0.17890425026416779 -0.50742506980895996
		 -0.85389089584350586 -0.11571607738733292 -0.64178597927093506 -0.7629317045211792
		 -0.077755875885486603 -0.66301876306533813 -0.73181390762329102 -0.15765294432640076 -0.50742506980895996
		 -0.85389089584350586 -0.11571607738733292 -0.44509264826774597 -0.89356511831283569
		 -0.058599032461643219 -0.5734635591506958 -0.81922447681427002 -0.0032599873375147581 -0.64178597927093506
		 -0.7629317045211792 -0.077755875885486603 -0.44509264826774597 -0.89356511831283569
		 -0.058599032461643219 -0.33678299188613892 -0.94152086973190308 -0.010759084485471249 -0.44589656591415405
		 -0.89284485578536987 0.063278742134571075 -0.5734635591506958 -0.81922447681427002
		 -0.0032599873375147581 -0.33678299188613892 -0.94152086973190308 -0.010759084485471249 -0.18341659009456635
		 -0.98278129100799561 0.022344658151268959 -0.24935130774974823 -0.9617881178855896
		 0.11308140307664871 -0.44589656591415405 -0.89284485578536987 0.063278742134571075 -0.18341659009456635
		 -0.98278129100799561 0.022344658151268959 1.5549782972357207e-007 -0.99940896034240723
		 0.03437795490026474 2.6347569814788585e-007 -0.99121993780136108 0.13222314417362213 -0.24935130774974823
		 -0.9617881178855896 0.11308140307664871 1.5549782972357207e-007 -0.99940896034240723
		 0.03437795490026474 0.18341706693172455 -0.98278117179870605 0.022344749420881271 0.24935169517993927
		 -0.96178799867630005 0.11308132857084274 2.6347569814788585e-007 -0.99121993780136108
		 0.13222314417362213 0.18341706693172455 -0.98278117179870605 0.022344749420881271 0.33678317070007324
		 -0.9415208101272583 -0.010758840478956699 0.44589650630950928 -0.89284497499465942
		 0.063278764486312866 0.24935169517993927 -0.96178799867630005 0.11308132857084274 0.33678317070007324
		 -0.9415208101272583 -0.010758840478956699 0.44509261846542358 -0.89356505870819092
		 -0.058598771691322327 0.57346361875534058 -0.81922453641891479 -0.0032599759288132191 0.44589650630950928
		 -0.89284497499465942 0.063278764486312866 0.44509261846542358 -0.89356505870819092
		 -0.058598771691322327 0.50742512941360474 -0.85389083623886108 -0.11571596562862396 0.64178621768951416
		 -0.7629314661026001 -0.077755838632583618 0.57346361875534058 -0.81922453641891479
		 -0.0032599759288132191 0.50742512941360474 -0.85389083623886108 -0.11571596562862396 0.52756732702255249
		 -0.83046144247055054 -0.1789039671421051 0.66301894187927246 -0.73181390762329102
		 -0.15765255689620972 0.64178621768951416 -0.7629314661026001 -0.077755838632583618 0.52756732702255249
		 -0.83046144247055054 -0.1789039671421051 0.52726995944976807 -0.82292217016220093
		 -0.21162585914134979 0.66242885589599609 -0.7222791314125061 -0.19874846935272217 0.66301894187927246
		 -0.73181390762329102 -0.15765255689620972 -0.66242861747741699 -0.7222791314125061
		 -0.1987491101026535 -0.66301876306533813 -0.73181390762329102 -0.15765294432640076 -0.77319401502609253
		 -0.61994707584381104 -0.13355365395545959 -0.77225857973098755 -0.6088407039642334
		 -0.18146541714668274 -0.66301876306533813 -0.73181390762329102 -0.15765294432640076 -0.64178597927093506
		 -0.7629317045211792 -0.077755875885486603 -0.75327032804489136 -0.65652757883071899
		 -0.039437290281057358 -0.77319401502609253 -0.61994707584381104 -0.13355365395545959 -0.64178597927093506
		 -0.7629317045211792 -0.077755875885486603 -0.5734635591506958 -0.81922447681427002
		 -0.0032599873375147581 -0.68642950057983398 -0.7253340482711792 0.052011363208293915 -0.75327032804489136
		 -0.65652757883071899 -0.039437290281057358 -0.5734635591506958 -0.81922447681427002
		 -0.0032599873375147581 -0.44589656591415405 -0.89284485578536987 0.063278742134571075 -0.55090194940567017
		 -0.82263880968093872 0.1406143456697464 -0.68642950057983398 -0.7253340482711792
		 0.052011363208293915 -0.44589656591415405 -0.89284485578536987 0.063278742134571075 -0.24935130774974823
		 -0.9617881178855896 0.11308140307664871 -0.31874388456344604 -0.92331373691558838
		 0.21422916650772095 -0.55090194940567017 -0.82263880968093872 0.1406143456697464 -0.24935130774974823
		 -0.9617881178855896 0.11308140307664871 2.6347569814788585e-007 -0.99121993780136108
		 0.13222314417362213 2.42542824935299e-007 -0.96957296133041382 0.24480247497558594 -0.31874388456344604
		 -0.92331373691558838 0.21422916650772095 2.6347569814788585e-007 -0.99121993780136108
		 0.13222314417362213 0.24935169517993927 -0.96178799867630005 0.11308132857084274 0.31874403357505798
		 -0.92331361770629883 0.2142290472984314 2.42542824935299e-007 -0.96957296133041382
		 0.24480247497558594 0.24935169517993927 -0.96178799867630005 0.11308132857084274 0.44589650630950928
		 -0.89284497499465942 0.063278764486312866 0.55090177059173584 -0.82263880968093872
		 0.14061428606510162 0.31874403357505798 -0.92331361770629883 0.2142290472984314 0.44589650630950928
		 -0.89284497499465942 0.063278764486312866 0.57346361875534058 -0.81922453641891479
		 -0.0032599759288132191 0.68642950057983398 -0.72533410787582397 0.052011314779520035 0.55090177059173584
		 -0.82263880968093872 0.14061428606510162 0.57346361875534058 -0.81922453641891479
		 -0.0032599759288132191 0.64178621768951416 -0.7629314661026001 -0.077755838632583618 0.75327056646347046
		 -0.65652745962142944 -0.039437264204025269 0.68642950057983398 -0.72533410787582397
		 0.052011314779520035 0.64178621768951416 -0.7629314661026001 -0.077755838632583618 0.66301894187927246
		 -0.73181390762329102 -0.15765255689620972 0.77319425344467163 -0.61994695663452148
		 -0.13355331122875214 0.75327056646347046 -0.65652745962142944 -0.039437264204025269 0.66301894187927246
		 -0.73181390762329102 -0.15765255689620972 0.66242885589599609 -0.7222791314125061
		 -0.19874846935272217 0.7722586989402771 -0.60884082317352295 -0.18146473169326782 0.77319425344467163
		 -0.61994695663452148 -0.13355331122875214 -0.77225857973098755 -0.6088407039642334
		 -0.18146541714668274 -0.77319401502609253 -0.61994707584381104 -0.13355365395545959 -0.85907262563705444
		 -0.50037455558776855 -0.10779441148042679 -0.85778999328613281 -0.48812410235404968
		 -0.16103115677833557 -0.77319401502609253 -0.61994707584381104 -0.13355365395545959 -0.75327032804489136
		 -0.65652757883071899 -0.039437290281057358 -0.84186047315597534 -0.53969180583953857
		 -0.0019122714875265956 -0.85907262563705444 -0.50037455558776855 -0.10779441148042679 -0.75327032804489136
		 -0.65652757883071899 -0.039437290281057358 -0.68642950057983398 -0.7253340482711792
		 0.052011363208293915 -0.78174102306365967 -0.61455786228179932 0.10582748800516129 -0.84186047315597534
		 -0.53969180583953857 -0.0019122714875265956 -0.68642950057983398 -0.7253340482711792
		 0.052011363208293915 -0.55090194940567017 -0.82263880968093872 0.1406143456697464 -0.64882862567901611
		 -0.72841328382492065 0.22008040547370911 -0.78174102306365967 -0.61455786228179932
		 0.10582748800516129 -0.55090194940567017 -0.82263880968093872 0.1406143456697464 -0.31874388456344604
		 -0.92331373691558838 0.21422916650772095 -0.39102858304977417 -0.86009573936462402
		 0.3276154100894928 -0.64882862567901611 -0.72841328382492065 0.22008040547370911 -0.31874388456344604
		 -0.92331373691558838 0.21422916650772095 2.42542824935299e-007 -0.96957296133041382
		 0.24480247497558594 2.2584207215459173e-007 -0.9262702465057373 0.37685999274253845 -0.39102858304977417
		 -0.86009573936462402 0.3276154100894928 2.42542824935299e-007 -0.96957296133041382
		 0.24480247497558594 0.31874403357505798 -0.92331361770629883 0.2142290472984314 0.39102882146835327
		 -0.8600957989692688 0.32761538028717041 2.2584207215459173e-007 -0.9262702465057373
		 0.37685999274253845 0.31874403357505798 -0.92331361770629883 0.2142290472984314 0.55090177059173584
		 -0.82263880968093872 0.14061428606510162 0.64882874488830566 -0.7284131646156311
		 0.22008046507835388 0.39102882146835327 -0.8600957989692688 0.32761538028717041 0.55090177059173584
		 -0.82263880968093872 0.14061428606510162 0.68642950057983398 -0.72533410787582397
		 0.052011314779520035 0.78174120187759399 -0.61455786228179932 0.10582751035690308 0.64882874488830566
		 -0.7284131646156311 0.22008046507835388 0.68642950057983398 -0.72533410787582397
		 0.052011314779520035 0.75327056646347046 -0.65652745962142944 -0.039437264204025269 0.84186059236526489
		 -0.53969162702560425 -0.0019122817320749164 0.78174120187759399 -0.61455786228179932
		 0.10582751035690308 0.75327056646347046 -0.65652745962142944 -0.039437264204025269 0.77319425344467163
		 -0.61994695663452148 -0.13355331122875214 0.85907280445098877 -0.50037431716918945
		 -0.10779406875371933 0.84186059236526489 -0.53969162702560425 -0.0019122817320749164 0.77319425344467163
		 -0.61994695663452148 -0.13355331122875214 0.7722586989402771 -0.60884082317352295
		 -0.18146473169326782 0.85779023170471191 -0.48812392354011536 -0.16103045642375946 0.85907280445098877
		 -0.50037431716918945 -0.10779406875371933 -0.85778999328613281 -0.48812410235404968
		 -0.16103115677833557 -0.85907262563705444 -0.50037455558776855 -0.10779441148042679 -0.92270457744598389
		 -0.37686210870742798 -0.081186473369598389 -0.9211159348487854 -0.36386066675186157
		 -0.13838641345500946 -0.85907262563705444 -0.50037455558776855 -0.10779441148042679 -0.84186047315597534
		 -0.53969180583953857 -0.0019122714875265956 -0.9086228609085083 -0.41622519493103027
		 0.03407638892531395 -0.92270457744598389 -0.37686210870742798 -0.081186473369598389 -0.84186047315597534
		 -0.53969180583953857 -0.0019122714875265956 -0.78174102306365967 -0.61455786228179932
		 0.10582748800516129 -0.85766357183456421 -0.4896986186504364 0.15687069296836853 -0.9086228609085083
		 -0.41622519493103027 0.03407638892531395 -0.78174102306365967 -0.61455786228179932
		 0.10582748800516129 -0.64882862567901611 -0.72841328382492065 0.22008040547370911 -0.73933255672454834
		 -0.57370263338088989 0.35249501466751099 -0.85766357183456421 -0.4896986186504364
		 0.15687069296836853 -0.64882862567901611 -0.72841328382492065 0.22008040547370911 -0.39102858304977417
		 -0.86009573936462402 0.3276154100894928 -0.47503048181533813 -0.71419507265090942
		 0.51407319307327271 -0.73933255672454834 -0.57370263338088989 0.35249501466751099 -0.39102858304977417
		 -0.86009573936462402 0.3276154100894928 2.2584207215459173e-007 -0.9262702465057373
		 0.37685999274253845 0.0040670502930879593 -0.83114945888519287 0.55603426694869995 -0.47503048181533813
		 -0.71419507265090942 0.51407319307327271 2.2584207215459173e-007 -0.9262702465057373
		 0.37685999274253845 0.39102882146835327 -0.8600957989692688 0.32761538028717041 0.47873282432556152
		 -0.70860785245895386 0.51835274696350098 0.0040670502930879593 -0.83114945888519287
		 0.55603426694869995 0.39102882146835327 -0.8600957989692688 0.32761538028717041 0.64882874488830566
		 -0.7284131646156311 0.22008046507835388 0.73945713043212891 -0.5722164511680603 0.35464265942573547 0.47873282432556152
		 -0.70860785245895386 0.51835274696350098 0.64882874488830566 -0.7284131646156311
		 0.22008046507835388 0.78174120187759399 -0.61455786228179932 0.10582751035690308 0.85766363143920898
		 -0.48969849944114685 0.15687057375907898 0.73945713043212891 -0.5722164511680603
		 0.35464265942573547 0.78174120187759399 -0.61455786228179932 0.10582751035690308 0.84186059236526489
		 -0.53969162702560425 -0.0019122817320749164 0.90862292051315308 -0.4162251353263855
		 0.034076347947120667 0.85766363143920898 -0.48969849944114685 0.15687057375907898 0.84186059236526489
		 -0.53969162702560425 -0.0019122817320749164 0.85907280445098877 -0.50037431716918945
		 -0.10779406875371933 0.92270451784133911 -0.37686219811439514 -0.081186093389987946 0.90862292051315308
		 -0.4162251353263855 0.034076347947120667 0.85907280445098877 -0.50037431716918945
		 -0.10779406875371933 0.85779023170471191 -0.48812392354011536 -0.16103045642375946 0.92111587524414063
		 -0.36386093497276306 -0.13838574290275574 0.92270451784133911 -0.37686219811439514
		 -0.081186093389987946 -0.9211159348487854 -0.36386066675186157 -0.13838641345500946 -0.92270457744598389
		 -0.37686210870742798 -0.081186473369598389 -0.96629762649536133 -0.25165426731109619
		 -0.054213292896747589 -0.96455377340316772 -0.21348366141319275 -0.1551155298948288 -0.92270457744598389
		 -0.37686210870742798 -0.081186473369598389 -0.9086228609085083 -0.41622519493103027
		 0.03407638892531395 -0.95497924089431763 -0.28875052928924561 0.068101771175861359 -0.96629762649536133
		 -0.25165426731109619 -0.054213292896747589 -0.9086228609085083 -0.41622519493103027
		 0.03407638892531395 -0.85766357183456421 -0.4896986186504364 0.15687069296836853 -0.91286957263946533
		 -0.35368436574935913 0.20390301942825317 -0.95497924089431763 -0.28875052928924561
		 0.068101771175861359 -0.85766357183456421 -0.4896986186504364 0.15687069296836853 -0.73933255672454834
		 -0.57370263338088989 0.35249501466751099 -0.76888567209243774 -0.44347909092903137
		 0.46058768033981323 -0.91286957263946533 -0.35368436574935913 0.20390301942825317 -0.73933255672454834
		 -0.57370263338088989 0.35249501466751099 -0.47503048181533813 -0.71419507265090942
		 0.51407319307327271 -0.5460701584815979 -0.54948461055755615 0.63235598802566528 -0.76888567209243774
		 -0.44347909092903137 0.46058768033981323 -0.47503048181533813 -0.71419507265090942
		 0.51407319307327271 0.0040670502930879593 -0.83114945888519287 0.55603426694869995 0.025579584762454033
		 -0.69753849506378174 0.71609067916870117 -0.5460701584815979 -0.54948461055755615
		 0.63235598802566528 0.0040670502930879593 -0.83114945888519287 0.55603426694869995 0.47873282432556152
		 -0.70860785245895386 0.51835274696350098 0.56639498472213745 -0.53762608766555786
		 0.62462395429611206 0.025579584762454033 -0.69753849506378174 0.71609067916870117 0.47873282432556152
		 -0.70860785245895386 0.51835274696350098 0.73945713043212891 -0.5722164511680603
		 0.35464265942573547 0.76741969585418701 -0.44338080286979675 0.46312025189399719 0.56639498472213745
		 -0.53762608766555786 0.62462395429611206 0.73945713043212891 -0.5722164511680603
		 0.35464265942573547 0.85766363143920898 -0.48969849944114685 0.15687057375907898 0.91286957263946533
		 -0.35368430614471436 0.20390287041664124 0.76741969585418701 -0.44338080286979675
		 0.46312025189399719 0.85766363143920898 -0.48969849944114685 0.15687057375907898 0.90862292051315308
		 -0.4162251353263855 0.034076347947120667 0.95497918128967285 -0.28875067830085754
		 0.068101756274700165 0.91286957263946533 -0.35368430614471436 0.20390287041664124 0.90862292051315308
		 -0.4162251353263855 0.034076347947120667 0.92270451784133911 -0.37686219811439514
		 -0.081186093389987946 0.96629750728607178 -0.25165471434593201 -0.054212894290685654 0.95497918128967285
		 -0.28875067830085754 0.068101756274700165 0.92270451784133911 -0.37686219811439514
		 -0.081186093389987946 0.92111587524414063 -0.36386093497276306 -0.13838574290275574 0.96455371379852295
		 -0.21348413825035095 -0.15511506795883179 0.96629750728607178 -0.25165471434593201
		 -0.054212894290685654 0.96455371379852295 -0.21348413825035095 -0.15511506795883179 0.93238502740859985
		 -0.16448387503623962 -0.32187449932098389 0.94639319181442261 -0.089797817170619965
		 -0.31028434634208679 0.98221516609191895 -0.10370443016290665 -0.1565210223197937 0.93238502740859985
		 -0.16448387503623962 -0.32187449932098389 0.89914274215698242 -0.16503438353538513
		 -0.40534675121307373 0.91502457857131958 -0.084312498569488525 -0.39448872208595276 0.94639319181442261
		 -0.089797817170619965 -0.31028434634208679 -0.8991428017616272 -0.16503456234931946
		 -0.40534654259681702 -0.93238502740859985 -0.16448391973972321 -0.32187432050704956 -0.94639313220977783
		 -0.08979768306016922 -0.31028413772583008 -0.91502475738525391 -0.084312453866004944
		 -0.39448857307434082 -0.93238502740859985 -0.16448391973972321 -0.32187432050704956 -0.96455377340316772
		 -0.21348366141319275 -0.1551155298948288 -0.98221504688262939 -0.10370432585477829
		 -0.15652130544185638 -0.94639313220977783 -0.08979768306016922 -0.31028413772583008 -0.96455377340316772
		 -0.21348366141319275 -0.1551155298948288 -0.96629762649536133 -0.25165426731109619
		 -0.054213292896747589 -0.99167400598526001 -0.12588596343994141 -0.027119310572743416 -0.98221504688262939
		 -0.10370432585477829 -0.15652130544185638 -0.96629762649536133 -0.25165426731109619
		 -0.054213292896747589 -0.95497924089431763 -0.28875052928924561 0.068101771175861359 -0.98221510648727417
		 -0.15895265340805054 0.099937833845615387 -0.99167400598526001 -0.12588596343994141
		 -0.027119310572743416 -0.95497924089431763 -0.28875052928924561 0.068101771175861359 -0.91286957263946533
		 -0.35368436574935913 0.20390301942825317 -0.94639313220977783 -0.20959088206291199
		 0.24578778445720673 -0.98221510648727417 -0.15895265340805054 0.099937833845615387 -0.91286957263946533
		 -0.35368436574935913 0.20390301942825317 -0.76888567209243774 -0.44347909092903137
		 0.46058768033981323 -0.78842413425445557 -0.32758781313896179 0.52064734697341919 -0.94639313220977783
		 -0.20959088206291199 0.24578778445720673 -0.76888567209243774 -0.44347909092903137
		 0.46058768033981323 -0.5460701584815979 -0.54948461055755615 0.63235598802566528 -0.59544873237609863
		 -0.40089663863182068 0.69622045755386353 -0.78842413425445557 -0.32758781313896179
		 0.52064734697341919 -0.5460701584815979 -0.54948461055755615 0.63235598802566528 0.025579584762454033
		 -0.69753849506378174 0.71609067916870117 0.039873447269201279 -0.49873119592666626
		 0.86583900451660156 -0.59544873237609863 -0.40089663863182068 0.69622045755386353 0.025579584762454033
		 -0.69753849506378174 0.71609067916870117 0.56639498472213745 -0.53762608766555786
		 0.62462395429611206 0.62439614534378052 -0.39220371842384338 0.67550390958786011 0.039873447269201279
		 -0.49873119592666626 0.86583900451660156 0.56639498472213745 -0.53762608766555786
		 0.62462395429611206 0.76741969585418701 -0.44338080286979675 0.46312025189399719 0.78649157285690308
		 -0.3300652801990509 0.52200376987457275 0.62439614534378052 -0.39220371842384338
		 0.67550390958786011 0.76741969585418701 -0.44338080286979675 0.46312025189399719 0.91286957263946533
		 -0.35368430614471436 0.20390287041664124 0.94639325141906738 -0.20959070324897766
		 0.24578775465488434 0.78649157285690308 -0.3300652801990509 0.52200376987457275 0.91286957263946533
		 -0.35368430614471436 0.20390287041664124 0.95497918128967285 -0.28875067830085754
		 0.068101756274700165 0.98221510648727417 -0.15895259380340576 0.099937863647937775 0.94639325141906738
		 -0.20959070324897766 0.24578775465488434 0.95497918128967285 -0.28875067830085754
		 0.068101756274700165 0.96629750728607178 -0.25165471434593201 -0.054212894290685654 0.99167400598526001
		 -0.12588603794574738 -0.02711888775229454 0.98221510648727417 -0.15895259380340576
		 0.099937863647937775 0.96629750728607178 -0.25165471434593201 -0.054212894290685654 0.96455371379852295
		 -0.21348413825035095 -0.15511506795883179 0.98221516609191895 -0.10370443016290665
		 -0.1565210223197937 0.99167400598526001 -0.12588603794574738 -0.02711888775229454 0.98221516609191895
		 -0.10370443016290665 -0.1565210223197937 0.94639319181442261 -0.089797817170619965
		 -0.31028434634208679 0.95763570070266724 0.060648053884506226 -0.28152376413345337 0.99119335412979126
		 0.027887651696801186 -0.12945243716239929 0.94639319181442261 -0.089797817170619965
		 -0.31028434634208679 0.91502457857131958 -0.084312498569488525 -0.39448872208595276 0.92781943082809448
		 0.078558750450611115 -0.36466392874717712 0.95763570070266724 0.060648053884506226
		 -0.28152376413345337 -0.91502475738525391 -0.084312453866004944 -0.39448857307434082 -0.94639313220977783
		 -0.08979768306016922 -0.31028413772583008 -0.95763576030731201 0.060647863894701004
		 -0.28152364492416382 -0.92781943082809448 0.078558385372161865 -0.36466372013092041 -0.94639313220977783
		 -0.08979768306016922 -0.31028413772583008 -0.98221504688262939 -0.10370432585477829
		 -0.15652130544185638 -0.99119335412979126 0.02788764052093029 -0.12945277988910675 -0.95763576030731201
		 0.060647863894701004 -0.28152364492416382 -0.98221504688262939 -0.10370432585477829
		 -0.15652130544185638 -0.99167400598526001 -0.12588596343994141 -0.027119310572743416 -0.99999988079071045
		 -3.647021173946996e-008 0 -0.99119335412979126 0.02788764052093029 -0.12945277988910675 -0.99167400598526001
		 -0.12588596343994141 -0.027119310572743416 -0.98221510648727417 -0.15895265340805054
		 0.099937833845615387 -0.99119341373443604 -0.027887500822544098 0.12945282459259033 -0.99999988079071045
		 -3.647021173946996e-008 0 -0.98221510648727417 -0.15895265340805054 0.099937833845615387 -0.94639313220977783
		 -0.20959088206291199 0.24578778445720673 -0.95763576030731201 -0.060647744685411453
		 0.28152388334274292 -0.99119341373443604 -0.027887500822544098 0.12945282459259033 -0.94639313220977783
		 -0.20959088206291199 0.24578778445720673 -0.78842413425445557 -0.32758781313896179
		 0.52064734697341919 -0.80525565147399902 -0.11999386548995972 0.58065879344940186 -0.95763576030731201
		 -0.060647744685411453 0.28152388334274292 -0.78842413425445557 -0.32758781313896179
		 0.52064734697341919 -0.59544873237609863 -0.40089663863182068 0.69622045755386353 -0.61474406719207764
		 -0.15770253539085388 0.77279990911483765 -0.80525565147399902 -0.11999386548995972
		 0.58065879344940186 -0.59544873237609863 -0.40089663863182068 0.69622045755386353 0.039873447269201279
		 -0.49873119592666626 0.86583900451660156 0.032291334122419357 -0.20134195685386658
		 0.97898852825164795 -0.61474406719207764 -0.15770253539085388 0.77279990911483765 0.039873447269201279
		 -0.49873119592666626 0.86583900451660156 0.62439614534378052 -0.39220371842384338
		 0.67550390958786011 0.63787394762039185 -0.15409253537654877 0.75456756353378296 0.032291334122419357
		 -0.20134195685386658 0.97898852825164795 0.62439614534378052 -0.39220371842384338
		 0.67550390958786011 0.78649157285690308 -0.3300652801990509 0.52200376987457275 0.80388075113296509
		 -0.12050553411245346 0.5824553370475769 0.63787394762039185 -0.15409253537654877
		 0.75456756353378296 0.78649157285690308 -0.3300652801990509 0.52200376987457275 0.94639325141906738
		 -0.20959070324897766 0.24578775465488434 0.95763576030731201 -0.060648061335086823
		 0.28152376413345337 0.80388075113296509 -0.12050553411245346 0.5824553370475769 0.94639325141906738
		 -0.20959070324897766 0.24578775465488434 0.98221510648727417 -0.15895259380340576
		 0.099937863647937775 0.99119329452514648 -0.027887705713510513 0.12945282459259033 0.95763576030731201
		 -0.060648061335086823 0.28152376413345337 0.98221510648727417 -0.15895259380340576
		 0.099937863647937775 0.99167400598526001 -0.12588603794574738 -0.02711888775229454 0.99999988079071045
		 -3.7405346375862791e-008 4.1519933802192099e-007 0.99119329452514648 -0.027887705713510513
		 0.12945282459259033 0.99167400598526001 -0.12588603794574738 -0.02711888775229454 0.98221516609191895
		 -0.10370443016290665 -0.1565210223197937 0.99119335412979126 0.027887651696801186
		 -0.12945243716239929 0.99999988079071045 -3.7405346375862791e-008 4.1519933802192099e-007 0.99119335412979126
		 0.027887651696801186 -0.12945243716239929 0.95763570070266724 0.060648053884506226
		 -0.28152376413345337 0.94639313220977783 0.20959112048149109 -0.24578769505023956 0.98221516609191895
		 0.15895275771617889 -0.09993743896484375 0.95763570070266724 0.060648053884506226
		 -0.28152376413345337 0.92781943082809448 0.078558750450611115 -0.36466392874717712 0.91502463817596436
		 0.23926328122615814 -0.32478156685829163 0.94639313220977783 0.20959112048149109
		 -0.24578769505023956 -0.92781943082809448 0.078558385372161865 -0.36466372013092041 -0.95763576030731201
		 0.060647863894701004 -0.28152364492416382 -0.94639319181442261 0.20959076285362244
		 -0.24578762054443359 -0.91502475738525391 0.2392628937959671 -0.32478150725364685 -0.95763576030731201
		 0.060647863894701004 -0.28152364492416382 -0.99119335412979126 0.02788764052093029
		 -0.12945277988910675 -0.98221510648727417 0.15895268321037292 -0.099937766790390015 -0.94639319181442261
		 0.20959076285362244 -0.24578762054443359 -0.99119335412979126 0.02788764052093029
		 -0.12945277988910675 -0.99999988079071045 -3.647021173946996e-008 0 -0.99167400598526001
		 0.1258859783411026 0.027119304984807968 -0.98221510648727417 0.15895268321037292
		 -0.099937766790390015 -0.99999988079071045 -3.647021173946996e-008 0 -0.99119341373443604
		 -0.027887500822544098 0.12945282459259033 -0.98221510648727417 0.10370434075593948
		 0.15652139484882355 -0.99167400598526001 0.1258859783411026 0.027119304984807968 -0.99119341373443604
		 -0.027887500822544098 0.12945282459259033 -0.95763576030731201 -0.060647744685411453
		 0.28152388334274292 -0.94639313220977783 0.089797675609588623 0.31028428673744202 -0.98221510648727417
		 0.10370434075593948 0.15652139484882355 -0.95763576030731201 -0.060647744685411453
		 0.28152388334274292 -0.80525565147399902 -0.11999386548995972 0.58065879344940186 -0.78520733118057251
		 0.093420565128326416 0.61214542388916016 -0.94639313220977783 0.089797675609588623
		 0.31028428673744202 -0.80525565147399902 -0.11999386548995972 0.58065879344940186 -0.61474406719207764
		 -0.15770253539085388 0.77279990911483765 -0.62600654363632202 0.091828562319278717
		 0.77439206838607788 -0.78520733118057251 0.093420565128326416 0.61214542388916016 -0.61474406719207764
		 -0.15770253539085388 0.77279990911483765 0.032291334122419357 -0.20134195685386658
		 0.97898852825164795 0.033952329307794571 0.11577266454696655 0.99269527196884155 -0.62600654363632202
		 0.091828562319278717 0.77439206838607788 0.032291334122419357 -0.20134195685386658
		 0.97898852825164795 0.63787394762039185 -0.15409253537654877 0.75456756353378296 0.64986497163772583
		 0.091680675745010376 0.75450003147125244 0.033952329307794571 0.11577266454696655
		 0.99269527196884155 0.63787394762039185 -0.15409253537654877 0.75456756353378296 0.80388075113296509
		 -0.12050553411245346 0.5824553370475769 0.78345441818237305 0.094646640121936798
		 0.61419969797134399 0.64986497163772583 0.091680675745010376 0.75450003147125244 0.80388075113296509
		 -0.12050553411245346 0.5824553370475769 0.95763576030731201 -0.060648061335086823
		 0.28152376413345337 0.94639325141906738 0.089797534048557281 0.31028425693511963 0.78345441818237305
		 0.094646640121936798 0.61419969797134399 0.95763576030731201 -0.060648061335086823
		 0.28152376413345337 0.99119329452514648 -0.027887705713510513 0.12945282459259033 0.98221510648727417
		 0.1037043109536171 0.15652135014533997 0.94639325141906738 0.089797534048557281 0.31028425693511963 0.99119329452514648
		 -0.027887705713510513 0.12945282459259033 0.99999988079071045 -3.7405346375862791e-008
		 4.1519933802192099e-007 0.99167400598526001 0.12588600814342499 0.027119699865579605 0.98221510648727417
		 0.1037043109536171 0.15652135014533997 0.99999988079071045 -3.7405346375862791e-008
		 4.1519933802192099e-007 0.99119335412979126 0.027887651696801186 -0.12945243716239929 0.98221516609191895
		 0.15895275771617889 -0.09993743896484375 0.99167400598526001 0.12588600814342499
		 0.027119699865579605 0.98221516609191895 0.15895275771617889 -0.09993743896484375 0.94639313220977783
		 0.20959112048149109 -0.24578769505023956 0.91286957263946533 0.35368436574935913
		 -0.2039029449224472 0.95497918128967285 0.28875067830085754 -0.068101361393928528 0.94639313220977783
		 0.20959112048149109 -0.24578769505023956 0.91502463817596436 0.23926328122615814
		 -0.32478156685829163 0.87728720903396606 0.39249539375305176 -0.27625089883804321 0.91286957263946533
		 0.35368436574935913 -0.2039029449224472 -0.91502475738525391 0.2392628937959671 -0.32478150725364685 -0.94639319181442261
		 0.20959076285362244 -0.24578762054443359 -0.91286963224411011 0.35368430614471436
		 -0.20390287041664124 -0.87728720903396606 0.39249551296234131 -0.27625072002410889 -0.94639319181442261
		 0.20959076285362244 -0.24578762054443359 -0.98221510648727417 0.15895268321037292
		 -0.099937766790390015 -0.95497918128967285 0.28875043988227844 -0.06810171902179718 -0.91286963224411011
		 0.35368430614471436 -0.20390287041664124 -0.98221510648727417 0.15895268321037292
		 -0.099937766790390015 -0.99167400598526001 0.1258859783411026 0.027119304984807968 -0.96629756689071655
		 0.25165435671806335 0.054213296622037888 -0.95497918128967285 0.28875043988227844
		 -0.06810171902179718 -0.99167400598526001 0.1258859783411026 0.027119304984807968 -0.98221510648727417
		 0.10370434075593948 0.15652139484882355 -0.95497918128967285 0.23509734869003296
		 0.18095307052135468 -0.96629756689071655 0.25165435671806335 0.054213296622037888 -0.98221510648727417
		 0.10370434075593948 0.15652139484882355 -0.94639313220977783 0.089797675609588623
		 0.31028428673744202 -0.91286957263946533 0.23835565149784088 0.33144471049308777 -0.95497918128967285
		 0.23509734869003296 0.18095307052135468 -0.94639313220977783 0.089797675609588623
		 0.31028428673744202 -0.78520733118057251 0.093420565128326416 0.61214542388916016 -0.73238158226013184
		 0.29319536685943604 0.61453521251678467 -0.91286957263946533 0.23835565149784088
		 0.33144471049308777 -0.78520733118057251 0.093420565128326416 0.61214542388916016 -0.62600654363632202
		 0.091828562319278717 0.77439206838607788 -0.63565671443939209 0.29595872759819031
		 0.7129860520362854 -0.73238158226013184 0.29319536685943604 0.61453521251678467 -0.62600654363632202
		 0.091828562319278717 0.77439206838607788 0.033952329307794571 0.11577266454696655
		 0.99269527196884155 0.044873286038637161 0.38406038284301758 0.92221689224243164 -0.63565671443939209
		 0.29595872759819031 0.7129860520362854 0.033952329307794571 0.11577266454696655 0.99269527196884155 0.64986497163772583
		 0.091680675745010376 0.75450003147125244 0.66699486970901489 0.28861558437347412
		 0.68689066171646118 0.044873286038637161 0.38406038284301758 0.92221689224243164 0.64986497163772583
		 0.091680675745010376 0.75450003147125244 0.78345441818237305 0.094646640121936798
		 0.61419969797134399 0.72967934608459473 0.29546645283699036 0.61665827035903931 0.66699486970901489
		 0.28861558437347412 0.68689066171646118 0.78345441818237305 0.094646640121936798
		 0.61419969797134399 0.94639325141906738 0.089797534048557281 0.31028425693511963 0.91286957263946533
		 0.23835577070713043 0.33144468069076538 0.72967934608459473 0.29546645283699036 0.61665827035903931 0.94639325141906738
		 0.089797534048557281 0.31028425693511963 0.98221510648727417 0.1037043109536171 0.15652135014533997 0.95497912168502808
		 0.23509739339351654 0.18095298111438751 0.91286957263946533 0.23835577070713043 0.33144468069076538 0.98221510648727417
		 0.1037043109536171 0.15652135014533997 0.99167400598526001 0.12588600814342499 0.027119699865579605 0.96629756689071655
		 0.25165447592735291 0.054213684052228928 0.95497912168502808 0.23509739339351654
		 0.18095298111438751 0.99167400598526001 0.12588600814342499 0.027119699865579605 0.98221516609191895
		 0.15895275771617889 -0.09993743896484375 0.95497918128967285 0.28875067830085754
		 -0.068101361393928528 0.96629756689071655 0.25165447592735291 0.054213684052228928 0.95497918128967285
		 0.28875067830085754 -0.068101361393928528 0.91286957263946533 0.35368436574935913
		 -0.2039029449224472 0.85766351222991943 0.48969879746437073 -0.15687066316604614 0.9086228609085083
		 0.41622528433799744 -0.03407600149512291 0.91286957263946533 0.35368436574935913
		 -0.2039029449224472 0.87728720903396606 0.39249539375305176 -0.27625089883804321 0.75125175714492798
		 0.60757148265838623 -0.25783276557922363 0.85766351222991943 0.48969879746437073
		 -0.15687066316604614 -0.87728720903396606 0.39249551296234131 -0.27625072002410889 -0.91286963224411011
		 0.35368430614471436 -0.20390287041664124 -0.85766369104385376 0.4896983802318573
		 -0.15687055885791779 -0.75125205516815186 0.60757118463516235 -0.25783255696296692 -0.91286963224411011
		 0.35368430614471436 -0.20390287041664124 -0.95497918128967285 0.28875043988227844
		 -0.06810171902179718 -0.9086228609085083 0.41622498631477356 -0.03407634049654007 -0.85766369104385376
		 0.4896983802318573 -0.15687055885791779 -0.95497918128967285 0.28875043988227844
		 -0.06810171902179718 -0.96629756689071655 0.25165435671806335 0.054213296622037888 -0.92270451784133911
		 0.37686195969581604 0.081186458468437195 -0.9086228609085083 0.41622498631477356
		 -0.03407634049654007 -0.96629756689071655 0.25165435671806335 0.054213296622037888 -0.95497918128967285
		 0.23509734869003296 0.18095307052135468 -0.90862274169921875 0.36527463793754578
		 0.20243293046951294 -0.92270451784133911 0.37686195969581604 0.081186458468437195 -0.95497918128967285
		 0.23509734869003296 0.18095307052135468 -0.91286957263946533 0.23835565149784088
		 0.33144471049308777 -0.85766357183456421 0.38167077302932739 0.34458762407302856 -0.90862274169921875
		 0.36527463793754578 0.20243293046951294 -0.91286957263946533 0.23835565149784088
		 0.33144471049308777 -0.73238158226013184 0.29319536685943604 0.61453521251678467 -0.65599334239959717
		 0.4662783145904541 0.59351253509521484 -0.85766357183456421 0.38167077302932739 0.34458762407302856 -0.73238158226013184
		 0.29319536685943604 0.61453521251678467 -0.63565671443939209 0.29595872759819031
		 0.7129860520362854 -0.6541130542755127 0.43371015787124634 0.61970287561416626 -0.65599334239959717
		 0.4662783145904541 0.59351253509521484 -0.63565671443939209 0.29595872759819031 0.7129860520362854 0.044873286038637161
		 0.38406038284301758 0.92221689224243164 0.068058758974075317 0.57772868871688843
		 0.81338649988174438 -0.6541130542755127 0.43371015787124634 0.61970287561416626 0.044873286038637161
		 0.38406038284301758 0.92221689224243164 0.66699486970901489 0.28861558437347412 0.68689066171646118 0.70198345184326172
		 0.41068854928016663 0.58185398578643799 0.068058758974075317 0.57772868871688843
		 0.81338649988174438 0.66699486970901489 0.28861558437347412 0.68689066171646118 0.72967934608459473
		 0.29546645283699036 0.61665827035903931 0.65232241153717041 0.46876484155654907 0.59559637308120728 0.70198345184326172
		 0.41068854928016663 0.58185398578643799 0.72967934608459473 0.29546645283699036 0.61665827035903931 0.91286957263946533
		 0.23835577070713043 0.33144468069076538 0.85766369104385376 0.38167065382003784 0.34458759427070618 0.65232241153717041
		 0.46876484155654907 0.59559637308120728 0.91286957263946533 0.23835577070713043 0.33144468069076538 0.95497912168502808
		 0.23509739339351654 0.18095298111438751 0.90862292051315308 0.3652743399143219 0.20243285596370697 0.85766369104385376
		 0.38167065382003784 0.34458759427070618 0.95497912168502808 0.23509739339351654 0.18095298111438751 0.96629756689071655
		 0.25165447592735291 0.054213684052228928 0.92270463705062866 0.37686195969581604
		 0.081186823546886444 0.90862292051315308 0.3652743399143219 0.20243285596370697 0.96629756689071655
		 0.25165447592735291 0.054213684052228928 0.95497918128967285 0.28875067830085754
		 -0.068101361393928528 0.9086228609085083 0.41622528433799744 -0.03407600149512291 0.92270463705062866
		 0.37686195969581604 0.081186823546886444 0.9086228609085083 0.41622528433799744 -0.03407600149512291 0.85766351222991943
		 0.48969879746437073 -0.15687066316604614 0.78174084424972534 0.61455816030502319
		 -0.10582739859819412 0.84186053276062012 0.53969168663024902 0.0019126307452097535 0.85766351222991943
		 0.48969879746437073 -0.15687066316604614 0.75125175714492798 0.60757148265838623
		 -0.25783276557922363 0.64882838726043701 0.72841358184814453 -0.22008031606674194 0.78174084424972534
		 0.61455816030502319 -0.10582739859819412 0.75125175714492798 0.60757148265838623
		 -0.25783276557922363 0.42627400159835815 0.81795418262481689 -0.38631787896156311 0.39102843403816223
		 0.8600960373878479 -0.3276151716709137 0.64882838726043701 0.72841358184814453 -0.22008031606674194 0.42627400159835815
		 0.81795418262481689 -0.38631787896156311 -2.8391968953656033e-007 0.8944011926651001
		 -0.44726547598838806 -1.6459675578062161e-007 0.92627036571502686 -0.37685966491699219 0.39102843403816223
		 0.8600960373878479 -0.3276151716709137 -2.8391968953656033e-007 0.8944011926651001
		 -0.44726547598838806 -0.42627435922622681 0.81795412302017212 -0.38631764054298401 -0.39102870225906372
		 0.86009585857391357 -0.32761520147323608 -1.6459675578062161e-007 0.92627036571502686
		 -0.37685966491699219 -0.42627435922622681 0.81795412302017212 -0.38631764054298401 -0.75125205516815186
		 0.60757118463516235 -0.25783255696296692 -0.64882868528366089 0.72841328382492065
		 -0.22008024156093597 -0.39102870225906372 0.86009585857391357 -0.32761520147323608 -0.75125205516815186
		 0.60757118463516235 -0.25783255696296692 -0.85766369104385376 0.4896983802318573
		 -0.15687055885791779 -0.78174120187759399 0.61455786228179932 -0.10582734644412994 -0.64882868528366089
		 0.72841328382492065 -0.22008024156093597 -0.85766369104385376 0.4896983802318573
		 -0.15687055885791779 -0.9086228609085083 0.41622498631477356 -0.03407634049654007 -0.84186059236526489
		 0.53969168663024902 0.0019123238744214177 -0.78174120187759399 0.61455786228179932
		 -0.10582734644412994 -0.9086228609085083 0.41622498631477356 -0.03407634049654007 -0.92270451784133911
		 0.37686195969581604 0.081186458468437195 -0.85907256603240967 0.500374436378479 0.10779441893100739 -0.84186059236526489
		 0.53969168663024902 0.0019123238744214177 -0.92270451784133911 0.37686195969581604
		 0.081186458468437195 -0.90862274169921875 0.36527463793754578 0.20243293046951294 -0.84186047315597534
		 0.49260756373405457 0.22047342360019684 -0.85907256603240967 0.500374436378479 0.10779441893100739 -0.90862274169921875
		 0.36527463793754578 0.20243293046951294 -0.85766357183456421 0.38167077302932739
		 0.34458762407302856 -0.78174108266830444 0.5164717435836792 0.34948241710662842 -0.84186047315597534
		 0.49260756373405457 0.22047342360019684 -0.85766357183456421 0.38167077302932739
		 0.34458762407302856 -0.65599334239959717 0.4662783145904541 0.59351253509521484 -0.56558305025100708
		 0.60855329036712646 0.55657762289047241 -0.78174108266830444 0.5164717435836792 0.34948241710662842 -0.65599334239959717
		 0.4662783145904541 0.59351253509521484 -0.6541130542755127 0.43371015787124634 0.61970287561416626 -0.68140822649002075
		 0.51339292526245117 0.52164202928543091 -0.56558305025100708 0.60855329036712646
		 0.55657762289047241 -0.6541130542755127 0.43371015787124634 0.61970287561416626 0.068058758974075317
		 0.57772868871688843 0.81338649988174438 0.10249388962984085 0.7080497145652771 0.6986849308013916 -0.68140822649002075
		 0.51339292526245117 0.52164202928543091 0.068058758974075317 0.57772868871688843
		 0.81338649988174438 0.70198345184326172 0.41068854928016663 0.58185398578643799 0.75304937362670898
		 0.46258911490440369 0.46789729595184326 0.10249388962984085 0.7080497145652771 0.6986849308013916 0.70198345184326172
		 0.41068854928016663 0.58185398578643799 0.65232241153717041 0.46876484155654907 0.59559637308120728 0.56133723258972168
		 0.61067670583724976 0.55854690074920654 0.75304937362670898 0.46258911490440369 0.46789729595184326 0.65232241153717041
		 0.46876484155654907 0.59559637308120728 0.85766369104385376 0.38167065382003784 0.34458759427070618 0.78174120187759399
		 0.5164715051651001 0.34948223829269409 0.56133723258972168 0.61067670583724976 0.55854690074920654 0.85766369104385376
		 0.38167065382003784 0.34458759427070618 0.90862292051315308 0.3652743399143219 0.20243285596370697 0.84186065196990967
		 0.49260744452476501 0.22047334909439087 0.78174120187759399 0.5164715051651001 0.34948223829269409 0.90862292051315308
		 0.3652743399143219 0.20243285596370697 0.92270463705062866 0.37686195969581604 0.081186823546886444 0.85907274484634399
		 0.50037407875061035 0.10779473185539246 0.84186065196990967 0.49260744452476501 0.22047334909439087 0.92270463705062866
		 0.37686195969581604 0.081186823546886444 0.9086228609085083 0.41622528433799744 -0.03407600149512291 0.84186053276062012
		 0.53969168663024902 0.0019126307452097535 0.85907274484634399 0.50037407875061035
		 0.10779473185539246 0.84186053276062012 0.53969168663024902 0.0019126307452097535 0.78174084424972534
		 0.61455816030502319 -0.10582739859819412 0.68642938137054443 0.72533416748046875
		 -0.052011322230100632 0.75327056646347046 0.65652734041213989 0.039437614381313324 0.78174084424972534
		 0.61455816030502319 -0.10582739859819412 0.64882838726043701 0.72841358184814453
		 -0.22008031606674194 0.55090165138244629 0.82263904809951782 -0.14061425626277924 0.68642938137054443
		 0.72533416748046875 -0.052011322230100632 0.64882838726043701 0.72841358184814453
		 -0.22008031606674194 0.39102843403816223 0.8600960373878479 -0.3276151716709137 0.31874379515647888
		 0.92331379652023315 -0.21422877907752991 0.55090165138244629 0.82263904809951782
		 -0.14061425626277924 0.39102843403816223 0.8600960373878479 -0.3276151716709137 -1.6459675578062161e-007
		 0.92627036571502686 -0.37685966491699219 -5.6845973261943072e-008 0.96957302093505859
		 -0.24480223655700684 0.31874379515647888 0.92331379652023315 -0.21422877907752991 -1.6459675578062161e-007
		 0.92627036571502686 -0.37685966491699219 -0.39102870225906372 0.86009585857391357
		 -0.32761520147323608 -0.31874397397041321 0.92331361770629883 -0.21422897279262543 -5.6845973261943072e-008
		 0.96957302093505859 -0.24480223655700684 -0.39102870225906372 0.86009585857391357
		 -0.32761520147323608 -0.64882868528366089 0.72841328382492065 -0.22008024156093597 -0.55090188980102539
		 0.82263892889022827 -0.14061416685581207 -0.31874397397041321 0.92331361770629883
		 -0.21422897279262543 -0.64882868528366089 0.72841328382492065 -0.22008024156093597 -0.78174120187759399
		 0.61455786228179932 -0.10582734644412994 -0.68642950057983398 0.72533398866653442
		 -0.05201118066906929 -0.55090188980102539 0.82263892889022827 -0.14061416685581207 -0.78174120187759399
		 0.61455786228179932 -0.10582734644412994 -0.84186059236526489 0.53969168663024902
		 0.0019123238744214177 -0.75327038764953613 0.65652745962142944 0.039437361061573029 -0.68642950057983398
		 0.72533398866653442 -0.05201118066906929 -0.84186059236526489 0.53969168663024902
		 0.0019123238744214177 -0.85907256603240967 0.500374436378479 0.10779441893100739 -0.77319413423538208
		 0.61994695663452148 0.13355369865894318 -0.75327038764953613 0.65652745962142944
		 0.039437361061573029 -0.85907256603240967 0.500374436378479 0.10779441893100739 -0.84186047315597534
		 0.49260756373405457 0.22047342360019684 -0.75327038764953613 0.61453074216842651
		 0.23438368737697601 -0.77319413423538208 0.61994695663452148 0.13355369865894318 -0.84186047315597534
		 0.49260756373405457 0.22047342360019684 -0.78174108266830444 0.5164717435836792 0.34948241710662842 -0.68642944097518921
		 0.6395803689956665 0.346051424741745 -0.75327038764953613 0.61453074216842651 0.23438368737697601 -0.78174108266830444
		 0.5164717435836792 0.34948241710662842 -0.56558305025100708 0.60855329036712646 0.55657762289047241 -0.46802395582199097
		 0.72192788124084473 0.50967997312545776 -0.68642944097518921 0.6395803689956665 0.346051424741745 -0.56558305025100708
		 0.60855329036712646 0.55657762289047241 -0.68140822649002075 0.51339292526245117
		 0.52164202928543091 -0.7044336199760437 0.55959129333496094 0.4366128146648407 -0.46802395582199097
		 0.72192788124084473 0.50967997312545776 -0.68140822649002075 0.51339292526245117
		 0.52164202928543091 0.10249388962984085 0.7080497145652771 0.6986849308013916 0.13951115310192108
		 0.79635858535766602 0.58851474523544312 -0.7044336199760437 0.55959129333496094 0.4366128146648407 0.10249388962984085
		 0.7080497145652771 0.6986849308013916 0.75304937362670898 0.46258911490440369 0.46789729595184326 0.80202662944793701
		 0.47062098979949951 0.36778959631919861 0.13951115310192108 0.79635858535766602 0.58851474523544312 0.75304937362670898
		 0.46258911490440369 0.46789729595184326 0.56133723258972168 0.61067670583724976 0.55854690074920654 0.4636867344379425
		 0.7235027551651001 0.51140826940536499 0.80202662944793701 0.47062098979949951 0.36778959631919861 0.56133723258972168
		 0.61067670583724976 0.55854690074920654 0.78174120187759399 0.5164715051651001 0.34948223829269409 0.68642961978912354
		 0.63958030939102173 0.34605133533477783 0.4636867344379425 0.7235027551651001 0.51140826940536499 0.78174120187759399
		 0.5164715051651001 0.34948223829269409 0.84186065196990967 0.49260744452476501 0.22047334909439087 0.75327050685882568
		 0.61453068256378174 0.23438362777233124 0.68642961978912354 0.63958030939102173 0.34605133533477783 0.84186065196990967
		 0.49260744452476501 0.22047334909439087 0.85907274484634399 0.50037407875061035 0.10779473185539246 0.77319419384002686
		 0.61994677782058716 0.13355395197868347 0.75327050685882568 0.61453068256378174 0.23438362777233124 0.85907274484634399
		 0.50037407875061035 0.10779473185539246 0.84186053276062012 0.53969168663024902 0.0019126307452097535 0.75327056646347046
		 0.65652734041213989 0.039437614381313324 0.77319419384002686 0.61994677782058716
		 0.13355395197868347 0.75327056646347046 0.65652734041213989 0.039437614381313324 0.68642938137054443
		 0.72533416748046875 -0.052011322230100632 0.57346326112747192 0.8192247748374939
		 0.0032599589321762323 0.64178591966629028 0.76293164491653442 0.077756114304065704 0.68642938137054443
		 0.72533416748046875 -0.052011322230100632 0.55090165138244629 0.82263904809951782
		 -0.14061425626277924 0.44589611887931824 0.89284509420394897 -0.063278727233409882 0.57346326112747192
		 0.8192247748374939 0.0032599589321762323 0.55090165138244629 0.82263904809951782
		 -0.14061425626277924 0.31874379515647888 0.92331379652023315 -0.21422877907752991 0.24935133755207062
		 0.96178817749023438 -0.11308136582374573 0.44589611887931824 0.89284509420394897
		 -0.063278727233409882 0.31874379515647888 0.92331379652023315 -0.21422877907752991 -5.6845973261943072e-008
		 0.96957302093505859 -0.24480223655700684 -5.6459082742321698e-008 0.99121999740600586
		 -0.13222308456897736 0.24935133755207062 0.96178817749023438 -0.11308136582374573 -5.6845973261943072e-008
		 0.96957302093505859 -0.24480223655700684 -0.31874397397041321 0.92331361770629883
		 -0.21422897279262543 -0.24935144186019897 0.96178805828094482 -0.11308136582374573 -5.6459082742321698e-008
		 0.99121999740600586 -0.13222308456897736 -0.31874397397041321 0.92331361770629883
		 -0.21422897279262543 -0.55090188980102539 0.82263892889022827 -0.14061416685581207 -0.44589647650718689
		 0.89284497499465942 -0.063278667628765106 -0.24935144186019897 0.96178805828094482
		 -0.11308136582374573 -0.55090188980102539 0.82263892889022827 -0.14061416685581207 -0.68642950057983398
		 0.72533398866653442 -0.05201118066906929 -0.5734635591506958 0.81922447681427002
		 0.0032601493876427412 -0.44589647650718689 0.89284497499465942 -0.063278667628765106 -0.68642950057983398
		 0.72533398866653442 -0.05201118066906929 -0.75327038764953613 0.65652745962142944
		 0.039437361061573029 -0.64178603887557983 0.76293158531188965 0.077755965292453766 -0.5734635591506958
		 0.81922447681427002 0.0032601493876427412 -0.75327038764953613 0.65652745962142944
		 0.039437361061573029 -0.77319413423538208 0.61994695663452148 0.13355369865894318 -0.66301876306533813
		 0.73181390762329102 0.15765294432640076 -0.64178603887557983 0.76293158531188965
		 0.077755965292453766 -0.77319413423538208 0.61994695663452148 0.13355369865894318 -0.75327038764953613
		 0.61453074216842651 0.23438368737697601 -0.64178597927093506 0.72727417945861816
		 0.24327555298805237 -0.66301876306533813 0.73181390762329102 0.15765294432640076 -0.75327038764953613
		 0.61453074216842651 0.23438368737697601 -0.68642944097518921 0.6395803689956665 0.346051424741745 -0.57346349954605103
		 0.74790036678314209 0.33434182405471802 -0.64178597927093506 0.72727417945861816
		 0.24327555298805237 -0.68642944097518921 0.6395803689956665 0.346051424741745 -0.46802395582199097
		 0.72192788124084473 0.50967997312545776 -0.36721503734588623 0.81035482883453369
		 0.45659413933753967 -0.57346349954605103 0.74790036678314209 0.33434182405471802 -0.46802395582199097
		 0.72192788124084473 0.50967997312545776 -0.7044336199760437 0.55959129333496094 0.4366128146648407 -0.70642024278640747
		 0.60169363021850586 0.37274003028869629 -0.36721503734588623 0.81035482883453369
		 0.45659413933753967 -0.7044336199760437 0.55959129333496094 0.4366128146648407 0.13951115310192108
		 0.79635858535766602 0.58851474523544312 -0.52799183130264282 -0.60197478532791138
		 -0.59904175996780396 -0.70642024278640747 0.60169363021850586 0.37274003028869629 0.13951115310192108
		 0.79635858535766602 0.58851474523544312 0.80202662944793701 0.47062098979949951 0.36778959631919861 0.83310800790786743
		 0.46854707598686218 0.29392936825752258 -0.52799183130264282 -0.60197478532791138
		 -0.59904175996780396 0.80202662944793701 0.47062098979949951 0.36778959631919861 0.4636867344379425
		 0.7235027551651001 0.51140826940536499 0.36322182416915894 0.81138324737548828 0.45795947313308716 0.83310800790786743
		 0.46854707598686218 0.29392936825752258 0.4636867344379425 0.7235027551651001 0.51140826940536499 0.68642961978912354
		 0.63958030939102173 0.34605133533477783 0.57346367835998535 0.74790030717849731 0.33434179425239563 0.36322182416915894
		 0.81138324737548828 0.45795947313308716 0.68642961978912354 0.63958030939102173 0.34605133533477783 0.75327050685882568
		 0.61453068256378174 0.23438362777233124 0.64178615808486938 0.72727406024932861 0.24327544867992401 0.57346367835998535
		 0.74790030717849731 0.33434179425239563 0.75327050685882568 0.61453068256378174 0.23438362777233124 0.77319419384002686
		 0.61994677782058716 0.13355395197868347 0.66301876306533813 0.73181390762329102 0.15765313804149628 0.64178615808486938
		 0.72727406024932861 0.24327544867992401 0.77319419384002686 0.61994677782058716 0.13355395197868347 0.75327056646347046
		 0.65652734041213989 0.039437614381313324 0.64178591966629028 0.76293164491653442
		 0.077756114304065704 0.66301876306533813 0.73181390762329102 0.15765313804149628 0.64178591966629028
		 0.76293164491653442 0.077756114304065704 0.57346326112747192 0.8192247748374939 0.0032599589321762323 0.44509238004684448
		 0.89356523752212524 0.058598939329385757 0.50742495059967041 0.85389095544815063
		 0.11571621149778366 0.57346326112747192 0.8192247748374939 0.0032599589321762323 0.44589611887931824
		 0.89284509420394897 -0.063278727233409882 0.33678287267684937 0.9415208101272583
		 0.010758914984762669 0.44509238004684448 0.89356523752212524 0.058598939329385757 0.44589611887931824
		 0.89284509420394897 -0.063278727233409882 0.24935133755207062 0.96178817749023438
		 -0.11308136582374573 0.18341656029224396 0.98278129100799561 -0.022344931960105896 0.33678287267684937
		 0.9415208101272583 0.010758914984762669 0.24935133755207062 0.96178817749023438 -0.11308136582374573 -5.6459082742321698e-008
		 0.99121999740600586 -0.13222308456897736 -1.873467780910687e-008 0.99940896034240723
		 -0.034378122538328171 0.18341656029224396 0.98278129100799561 -0.022344931960105896 -5.6459082742321698e-008
		 0.99121999740600586 -0.13222308456897736 -0.24935144186019897 0.96178805828094482
		 -0.11308136582374573 -0.18341666460037231 0.98278129100799561 -0.022344736382365227 -1.873467780910687e-008
		 0.99940896034240723 -0.034378122538328171 -0.24935144186019897 0.96178805828094482
		 -0.11308136582374573 -0.44589647650718689 0.89284497499465942 -0.063278667628765106 -0.3367830216884613
		 0.94152075052261353 0.010759124532341957 -0.18341666460037231 0.98278129100799561
		 -0.022344736382365227 -0.44589647650718689 0.89284497499465942 -0.063278667628765106 -0.5734635591506958
		 0.81922447681427002 0.0032601493876427412 -0.44509264826774597 0.89356517791748047
		 0.058599047362804413 -0.3367830216884613 0.94152075052261353 0.010759124532341957 -0.5734635591506958
		 0.81922447681427002 0.0032601493876427412 -0.64178603887557983 0.76293158531188965
		 0.077755965292453766 -0.50742506980895996 0.85389089584350586 0.11571605503559113 -0.44509264826774597
		 0.89356517791748047 0.058599047362804413 -0.64178603887557983 0.76293158531188965
		 0.077755965292453766 -0.66301876306533813 0.73181390762329102 0.15765294432640076 -0.52756720781326294
		 0.83046132326126099 0.1789042055606842 -0.50742506980895996 0.85389089584350586 0.11571605503559113 -0.66301876306533813
		 0.73181390762329102 0.15765294432640076 -0.64178597927093506 0.72727417945861816
		 0.24327555298805237 -0.50742506980895996 0.82579517364501953 0.24613456428050995 -0.52756720781326294
		 0.83046132326126099 0.1789042055606842 -0.64178597927093506 0.72727417945861816 0.24327555298805237 -0.57346349954605103
		 0.74790036678314209 0.33434182405471802 -0.44509255886077881 0.8384324312210083 0.31452104449272156 -0.50742506980895996
		 0.82579517364501953 0.24613456428050995 -0.57346349954605103 0.74790036678314209
		 0.33434182405471802 -0.36721503734588623 0.81035482883453369 0.45659413933753967 -0.27339988946914673
		 0.87381631135940552 0.40211641788482666 -0.44509255886077881 0.8384324312210083 0.31452104449272156 -0.36721503734588623
		 0.81035482883453369 0.45659413933753967 -0.70642024278640747 0.60169363021850586
		 0.37274003028869629 -0.66356927156448364 0.66822284460067749 0.33638370037078857 -0.27339988946914673
		 0.87381631135940552 0.40211641788482666 -0.70642024278640747 0.60169363021850586
		 0.37274003028869629 -0.52799183130264282 -0.60197478532791138 -0.59904175996780396 0.3086848258972168
		 0.74708670377731323 0.58870625495910645 -0.66356927156448364 0.66822284460067749
		 0.33638370037078857 -0.52799183130264282 -0.60197478532791138 -0.59904175996780396 0.83310800790786743
		 0.46854707598686218 0.29392936825752258 0.82482635974884033 0.50040245056152344 0.26317065954208374 0.3086848258972168
		 0.74708670377731323 0.58870625495910645 0.83310800790786743 0.46854707598686218 0.29392936825752258 0.36322182416915894
		 0.81138324737548828 0.45795947313308716 0.2708955705165863 0.8739359974861145 0.40354859828948975 0.82482635974884033
		 0.50040245056152344 0.26317065954208374 0.36322182416915894 0.81138324737548828 0.45795947313308716 0.57346367835998535
		 0.74790030717849731 0.33434179425239563 0.44509267807006836 0.8384324312210083 0.31452107429504395 0.2708955705165863
		 0.8739359974861145 0.40354859828948975 0.57346367835998535 0.74790030717849731 0.33434179425239563 0.64178615808486938
		 0.72727406024932861 0.24327544867992401 0.50742512941360474 0.82579505443572998 0.24613447487354279 0.44509267807006836
		 0.8384324312210083 0.31452107429504395 0.64178615808486938 0.72727406024932861 0.24327544867992401 0.66301876306533813
		 0.73181390762329102 0.15765313804149628 0.52756726741790771 0.83046132326126099 0.17890432476997375 0.50742512941360474
		 0.82579505443572998 0.24613447487354279 0.66301876306533813 0.73181390762329102 0.15765313804149628 0.64178591966629028
		 0.76293164491653442 0.077756114304065704 0.50742495059967041 0.85389095544815063
		 0.11571621149778366 0.52756726741790771 0.83046132326126099 0.17890432476997375 0.50742495059967041
		 0.85389095544815063 0.11571621149778366 0.44509238004684448 0.89356523752212524 0.058598939329385757 0.30421063303947449
		 0.94593101739883423 0.11256326735019684 0.35215532779693604 0.92355853319168091 0.15174432098865509 0.44509238004684448
		 0.89356523752212524 0.058598939329385757 0.33678287267684937 0.9415208101272583 0.010758914984762669 0.22542662918567657
		 0.97088056802749634 0.081079103052616119 0.30421063303947449 0.94593101739883423
		 0.11256326735019684 0.33678287267684937 0.9415208101272583 0.010758914984762669 0.18341656029224396
		 0.98278129100799561 -0.022344931960105896 0.12049218267202377 0.99087750911712646
		 0.060360055416822433 0.22542662918567657 0.97088056802749634 0.081079103052616119 0.18341656029224396
		 0.98278129100799561 -0.022344931960105896 -1.873467780910687e-008 0.99940896034240723
		 -0.034378122538328171 4.6703196865394148e-009 0.99858999252319336 0.053085841238498688 0.12049218267202377
		 0.99087750911712646 0.060360055416822433 -1.873467780910687e-008 0.99940896034240723
		 -0.034378122538328171 -0.18341666460037231 0.98278129100799561 -0.022344736382365227 -0.12049245834350586
		 0.99087750911712646 0.060360215604305267 4.6703196865394148e-009 0.99858999252319336
		 0.053085841238498688 -0.18341666460037231 0.98278129100799561 -0.022344736382365227 -0.3367830216884613
		 0.94152075052261353 0.010759124532341957 -0.22542671859264374 0.97088050842285156
		 0.081079147756099701 -0.12049245834350586 0.99087750911712646 0.060360215604305267 -0.3367830216884613
		 0.94152075052261353 0.010759124532341957 -0.44509264826774597 0.89356517791748047
		 0.058599047362804413 -0.30421051383018494 0.945931077003479 0.1125631108880043 -0.22542671859264374
		 0.97088050842285156 0.081079147756099701 -0.44509264826774597 0.89356517791748047
		 0.058599047362804413 -0.50742506980895996 0.85389089584350586 0.11571605503559113 -0.35215532779693604
		 0.92355853319168091 0.15174414217472076 -0.30421051383018494 0.945931077003479 0.1125631108880043 -0.50742506980895996
		 0.85389089584350586 0.11571605503559113 -0.52756720781326294 0.83046132326126099
		 0.1789042055606842 -0.36815300583839417 0.90891361236572266 0.19580483436584473 -0.35215532779693604
		 0.92355853319168091 0.15174414217472076 -0.52756720781326294 0.83046132326126099
		 0.1789042055606842 -0.50742506980895996 0.82579517364501953 0.24613456428050995 -0.3521554172039032
		 0.90411758422851563 0.24198733270168304 -0.36815300583839417 0.90891361236572266
		 0.19580483436584473 -0.50742506980895996 0.82579517364501953 0.24613456428050995 -0.44509255886077881
		 0.8384324312210083 0.31452104449272156 -0.3042106032371521 0.90837299823760986 0.28690493106842041 -0.3521554172039032
		 0.90411758422851563 0.24198733270168304 -0.44509255886077881 0.8384324312210083 0.31452104449272156 -0.27339988946914673
		 0.87381631135940552 0.40211641788482666 -0.18668906390666962 0.92094248533248901
		 0.34207040071487427 -0.3042106032371521 0.90837299823760986 0.28690493106842041 -0.27339988946914673
		 0.87381631135940552 0.40211641788482666 -0.66356927156448364 0.66822284460067749
		 0.33638370037078857 -0.41745847463607788 0.84547597169876099 0.33301472663879395 -0.18668906390666962
		 0.92094248533248901 0.34207040071487427 -0.66356927156448364 0.66822284460067749
		 0.33638370037078857 0.3086848258972168 0.74708670377731323 0.58870625495910645 0.0075810388661921024
		 0.93833571672439575 0.34564250707626343 -0.41745847463607788 0.84547597169876099
		 0.33301472663879395 0.3086848258972168 0.74708670377731323 0.58870625495910645 0.82482635974884033
		 0.50040245056152344 0.26317065954208374 0.48140791058540344 0.81618613004684448 0.31950992345809937 0.0075810388661921024
		 0.93833571672439575 0.34564250707626343 0.82482635974884033 0.50040245056152344 0.26317065954208374 0.2708955705165863
		 0.8739359974861145 0.40354859828948975 0.18619886040687561 0.9207082986831665 0.34296643733978271 0.48140791058540344
		 0.81618613004684448 0.31950992345809937 0.2708955705165863 0.8739359974861145 0.40354859828948975 0.44509267807006836
		 0.8384324312210083 0.31452107429504395 0.30421063303947449 0.90837299823760986 0.2869049608707428 0.18619886040687561
		 0.9207082986831665 0.34296643733978271 0.44509267807006836 0.8384324312210083 0.31452107429504395 0.50742512941360474
		 0.82579505443572998 0.24613447487354279 0.35215544700622559 0.90411752462387085 0.24198748171329498 0.30421063303947449
		 0.90837299823760986 0.2869049608707428 0.50742512941360474 0.82579505443572998 0.24613447487354279 0.52756726741790771
		 0.83046132326126099 0.17890432476997375 0.36815297603607178 0.90891349315643311 0.19580487906932831 0.35215544700622559
		 0.90411752462387085 0.24198748171329498 0.52756726741790771 0.83046132326126099 0.17890432476997375 0.50742495059967041
		 0.85389095544815063 0.11571621149778366 0.35215532779693604 0.92355853319168091 0.15174432098865509 0.36815297603607178
		 0.90891349315643311 0.19580487906932831 0.35215532779693604 0.92355853319168091 0.15174432098865509 0.30421063303947449
		 0.94593101739883423 0.11256326735019684 0.15442770719528198 0.97434878349304199 0.16369663178920746 0.1805843859910965
		 0.96619856357574463 0.18398289382457733 0.30421063303947449 0.94593101739883423 0.11256326735019684 0.22542662918567657
		 0.97088056802749634 0.081079103052616119 0.11294324696063995 0.98256939649581909
		 0.14765229821205139 0.15442770719528198 0.97434878349304199 0.16369663178920746 0.22542662918567657
		 0.97088056802749634 0.081079103052616119 0.12049218267202377 0.99087750911712646
		 0.060360055416822433 0.059699855744838715 0.98872512578964233 0.13732671737670898 0.11294324696063995
		 0.98256939649581909 0.14765229821205139 0.12049218267202377 0.99087750911712646 0.060360055416822433 4.6703196865394148e-009
		 0.99858999252319336 0.053085841238498688 3.1241942366477815e-008 0.99101400375366211
		 0.13375838100910187 0.059699855744838715 0.98872512578964233 0.13732671737670898 4.6703196865394148e-009
		 0.99858999252319336 0.053085841238498688 -0.12049245834350586 0.99087750911712646
		 0.060360215604305267 -0.059700008481740952 0.98872506618499756 0.13732659816741943 3.1241942366477815e-008
		 0.99101400375366211 0.13375838100910187 -0.12049245834350586 0.99087750911712646
		 0.060360215604305267 -0.22542671859264374 0.97088050842285156 0.081079147756099701 -0.1129433885216713
		 0.98256957530975342 0.14765186607837677 -0.059700008481740952 0.98872506618499756
		 0.13732659816741943 -0.22542671859264374 0.97088050842285156 0.081079147756099701 -0.30421051383018494
		 0.945931077003479 0.1125631108880043 -0.15442764759063721 0.97434884309768677 0.16369576752185822 -0.1129433885216713
		 0.98256957530975342 0.14765186607837677 -0.30421051383018494 0.945931077003479 0.1125631108880043 -0.35215532779693604
		 0.92355853319168091 0.15174414217472076 -0.1805843710899353 0.96619856357574463 0.18398287892341614 -0.15442764759063721
		 0.97434884309768677 0.16369576752185822 -0.35215532779693604 0.92355853319168091
		 0.15174414217472076 -0.36815300583839417 0.90891361236572266 0.19580483436584473 -0.18949863314628601
		 0.95986044406890869 0.20678083598613739 -0.1805843710899353 0.96619856357574463 0.18398287892341614 -0.36815300583839417
		 0.90891361236572266 0.19580483436584473 -0.3521554172039032 0.90411758422851563 0.24198733270168304 -0.1805843859910965
		 0.95624947547912598 0.23016586899757385 -0.18949863314628601 0.95986044406890869
		 0.20678083598613739 -0.3521554172039032 0.90411758422851563 0.24198733270168304 -0.3042106032371521
		 0.90837299823760986 0.28690493106842041 -0.15442754328250885 0.95532399415969849
		 0.25200849771499634 -0.1805843859910965 0.95624947547912598 0.23016586899757385 -0.3042106032371521
		 0.90837299823760986 0.28690493106842041 -0.18668906390666962 0.92094248533248901
		 0.34207040071487427 -0.095524318516254425 0.95641452074050903 0.27594661712646484 -0.15442754328250885
		 0.95532399415969849 0.25200849771499634 -0.18668906390666962 0.92094248533248901
		 0.34207040071487427 -0.41745847463607788 0.84547597169876099 0.33301472663879395 -0.12896370887756348
		 0.94933944940567017 0.28657060861587524 -0.095524318516254425 0.95641452074050903
		 0.27594661712646484 -0.41745847463607788 0.84547597169876099 0.33301472663879395 0.0075810388661921024
		 0.93833571672439575 0.34564250707626343 -0.0072888177819550037 0.95743674039840698
		 0.28855130076408386 -0.12896370887756348 0.94933944940567017 0.28657060861587524 0.0075810388661921024
		 0.93833571672439575 0.34564250707626343 0.48140791058540344 0.81618613004684448 0.31950992345809937 0.12194138765335083
		 0.9501383900642395 0.28699710965156555 -0.0072888177819550037 0.95743674039840698
		 0.28855130076408386 0.48140791058540344 0.81618613004684448 0.31950992345809937 0.18619886040687561
		 0.9207082986831665 0.34296643733978271 0.095782771706581116 0.95639204978942871 0.27593439817428589 0.12194138765335083
		 0.9501383900642395 0.28699710965156555 0.18619886040687561 0.9207082986831665 0.34296643733978271 0.30421063303947449
		 0.90837299823760986 0.2869049608707428 0.15442764759063721 0.95532393455505371 0.25200846791267395 0.095782771706581116
		 0.95639204978942871 0.27593439817428589 0.30421063303947449 0.90837299823760986 0.2869049608707428 0.35215544700622559
		 0.90411752462387085 0.24198748171329498 0.18058446049690247 0.95624953508377075 0.23016579449176788 0.15442764759063721
		 0.95532393455505371 0.25200846791267395 0.35215544700622559 0.90411752462387085 0.24198748171329498 0.36815297603607178
		 0.90891349315643311 0.19580487906932831 0.18949869275093079 0.95986044406890869 0.20678038895130157 0.18058446049690247
		 0.95624953508377075 0.23016579449176788 0.36815297603607178 0.90891349315643311 0.19580487906932831 0.35215532779693604
		 0.92355853319168091 0.15174432098865509 0.1805843859910965 0.96619856357574463 0.18398289382457733 0.18949869275093079
		 0.95986044406890869 0.20678038895130157 -0.18949861824512482 -0.95986044406890869
		 -0.2067808210849762 -0.18948487937450409 -0.95724946260452271 -0.21856078505516052 4.9190138184940224e-008
		 -0.98156195878982544 -0.19114479422569275 -0.18058434128761292 -0.96619850397109985
		 -0.18398293852806091 -0.18949861824512482 -0.95986044406890869 -0.2067808210849762 4.9190138184940224e-008
		 -0.98156195878982544 -0.19114479422569275 -0.1544276624917984 -0.97434884309768677
		 -0.16369587182998657 -0.18058434128761292 -0.96619850397109985 -0.18398293852806091 4.9190138184940224e-008
		 -0.98156195878982544 -0.19114479422569275 -0.11294329166412354 -0.98256951570510864
		 -0.14765195548534393 -0.1544276624917984 -0.97434884309768677 -0.16369587182998657 4.9190138184940224e-008
		 -0.98156195878982544 -0.19114479422569275 -0.059700168669223785 -0.98872506618499756
		 -0.13732673227787018 -0.11294329166412354 -0.98256951570510864 -0.14765195548534393 4.9190138184940224e-008
		 -0.98156195878982544 -0.19114479422569275 8.5309159203461604e-007 -0.99101394414901733
		 -0.13375833630561829 -0.059700168669223785 -0.98872506618499756 -0.13732673227787018 4.9190138184940224e-008
		 -0.98156195878982544 -0.19114479422569275 0.059700269252061844 -0.98872506618499756
		 -0.13732674717903137 8.5309159203461604e-007 -0.99101394414901733 -0.13375833630561829 4.9190138184940224e-008
		 -0.98156195878982544 -0.19114479422569275 0.11294253915548325 -0.98256951570510864
		 -0.147652268409729 0.059700269252061844 -0.98872506618499756 -0.13732674717903137 4.9190138184940224e-008
		 -0.98156195878982544 -0.19114479422569275 0.15442764759063721 -0.97434878349304199
		 -0.16369619965553284 0.11294253915548325 -0.98256951570510864 -0.147652268409729 4.9190138184940224e-008
		 -0.98156195878982544 -0.19114479422569275 0.18058440089225769 -0.9661986231803894
		 -0.18398270010948181 0.15442764759063721 -0.97434878349304199 -0.16369619965553284 4.9190138184940224e-008
		 -0.98156195878982544 -0.19114479422569275 0.18949857354164124 -0.95986044406890869
		 -0.20678053796291351 0.18058440089225769 -0.9661986231803894 -0.18398270010948181 4.9190138184940224e-008
		 -0.98156195878982544 -0.19114479422569275 0.18948470056056976 -0.95724940299987793
		 -0.21856106817722321 0.18949857354164124 -0.95986044406890869 -0.20678053796291351 4.9190138184940224e-008
		 -0.98156195878982544 -0.19114479422569275 0.1805843859910965 0.96619856357574463
		 0.18398289382457733 0.15442770719528198 0.97434878349304199 0.16369663178920746 -0.00024464196758344769
		 0.97747534513473511 0.21104918420314789 0.15442770719528198 0.97434878349304199 0.16369663178920746 0.11294324696063995
		 0.98256939649581909 0.14765229821205139 -0.00024464196758344769 0.97747534513473511
		 0.21104918420314789 0.11294324696063995 0.98256939649581909 0.14765229821205139 0.059699855744838715
		 0.98872512578964233 0.13732671737670898 -0.00024464196758344769 0.97747534513473511
		 0.21104918420314789 0.059699855744838715 0.98872512578964233 0.13732671737670898 3.1241942366477815e-008
		 0.99101400375366211 0.13375838100910187 -0.00024464196758344769 0.97747534513473511
		 0.21104918420314789 3.1241942366477815e-008 0.99101400375366211 0.13375838100910187 -0.059700008481740952
		 0.98872506618499756 0.13732659816741943 -0.00024464196758344769 0.97747534513473511
		 0.21104918420314789 -0.059700008481740952 0.98872506618499756 0.13732659816741943 -0.1129433885216713
		 0.98256957530975342 0.14765186607837677 -0.00024464196758344769 0.97747534513473511
		 0.21104918420314789 -0.1129433885216713 0.98256957530975342 0.14765186607837677 -0.15442764759063721
		 0.97434884309768677 0.16369576752185822 -0.00024464196758344769 0.97747534513473511
		 0.21104918420314789 -0.15442764759063721 0.97434884309768677 0.16369576752185822 -0.1805843710899353
		 0.96619856357574463 0.18398287892341614 -0.00024464196758344769 0.97747534513473511
		 0.21104918420314789 -0.1805843710899353 0.96619856357574463 0.18398287892341614 -0.18949863314628601
		 0.95986044406890869 0.20678083598613739 -0.00024464196758344769 0.97747534513473511
		 0.21104918420314789 -0.18949863314628601 0.95986044406890869 0.20678083598613739 -0.1805843859910965
		 0.95624947547912598 0.23016586899757385 -0.00024464196758344769 0.97747534513473511
		 0.21104918420314789 -0.1805843859910965 0.95624947547912598 0.23016586899757385 -0.15442754328250885
		 0.95532399415969849 0.25200849771499634 -0.00024464196758344769 0.97747534513473511
		 0.21104918420314789 -0.15442754328250885 0.95532399415969849 0.25200849771499634 -0.095524318516254425
		 0.95641452074050903 0.27594661712646484 -0.00024464196758344769 0.97747534513473511
		 0.21104918420314789 -0.095524318516254425 0.95641452074050903 0.27594661712646484 -0.12896370887756348
		 0.94933944940567017 0.28657060861587524 -0.00024464196758344769 0.97747534513473511
		 0.21104918420314789 -0.12896370887756348 0.94933944940567017 0.28657060861587524 -0.0072888177819550037
		 0.95743674039840698 0.28855130076408386 -0.00024464196758344769 0.97747534513473511
		 0.21104918420314789 -0.0072888177819550037 0.95743674039840698 0.28855130076408386 0.12194138765335083
		 0.9501383900642395 0.28699710965156555 -0.00024464196758344769 0.97747534513473511
		 0.21104918420314789 0.12194138765335083 0.9501383900642395 0.28699710965156555 0.095782771706581116
		 0.95639204978942871 0.27593439817428589 -0.00024464196758344769 0.97747534513473511
		 0.21104918420314789 0.095782771706581116 0.95639204978942871 0.27593439817428589 0.15442764759063721
		 0.95532393455505371 0.25200846791267395 -0.00024464196758344769 0.97747534513473511
		 0.21104918420314789 0.15442764759063721 0.95532393455505371 0.25200846791267395 0.18058446049690247
		 0.95624953508377075 0.23016579449176788 -0.00024464196758344769 0.97747534513473511
		 0.21104918420314789 0.18058446049690247 0.95624953508377075 0.23016579449176788 0.18949869275093079
		 0.95986044406890869 0.20678038895130157 -0.00024464196758344769 0.97747534513473511
		 0.21104918420314789 0.18949869275093079 0.95986044406890869 0.20678038895130157 0.1805843859910965
		 0.96619856357574463 0.18398289382457733 -0.00024464196758344769 0.97747534513473511
		 0.21104918420314789 0.96973633766174316 0.24313893914222717 -0.022249052301049232 0.87547481060028076
		 0.48300278186798096 0.015881821513175964 0.87547481060028076 0.48300278186798096
		 0.015881821513175964 0.96973633766174316 0.24313893914222717 -0.022249052301049232 0.87547481060028076
		 0.48300278186798096 0.015881821513175964 0.69160807132720947 0.72011446952819824
		 0.055799491703510284 0.69160807132720947 0.72011446952819824 0.055799491703510284 0.87547481060028076
		 0.48300278186798096 0.015881821513175964 0.69160807132720947 0.72011446952819824
		 0.055799491703510284 0.39310365915298462 0.91501748561859131 0.090622879564762115 0.39310365915298462
		 0.91501748561859131 0.090622879564762115 0.69160807132720947 0.72011446952819824
		 0.055799491703510284 0.39310365915298462 0.91501748561859131 0.090622879564762115 0.023807870224118233
		 0.99386376142501831 0.10801813751459122 0.023807870224118233 0.99386376142501831
		 0.10801813751459122 0.39310365915298462 0.91501748561859131 0.090622879564762115 0.023807870224118233
		 0.99386376142501831 0.10801813751459122 -0.31105902791023254 0.94453638792037964
		 0.10532467812299728 -0.31105902791023254 0.94453638792037964 0.10532467812299728 0.023807870224118233
		 0.99386376142501831 0.10801813751459122 -0.31105902791023254 0.94453638792037964
		 0.10532467812299728 -0.5876191258430481 0.80426114797592163 0.088700398802757263 -0.5876191258430481
		 0.80426114797592163 0.088700398802757263 -0.31105902791023254 0.94453638792037964
		 0.10532467812299728 -0.5876191258430481 0.80426114797592163 0.088700398802757263 -0.80883008241653442
		 0.5849347710609436 0.060374684631824493 -0.80883008241653442 0.5849347710609436 0.060374684631824493 -0.5876191258430481
		 0.80426114797592163 0.088700398802757263 -0.80883008241653442 0.5849347710609436
		 0.060374684631824493 -0.9508664608001709 0.30873396992683411 0.023161634802818298 -0.9508664608001709
		 0.30873396992683411 0.023161634802818298 -0.80883008241653442 0.5849347710609436
		 0.060374684631824493 -0.9508664608001709 0.30873396992683411 0.023161634802818298 -0.99981021881103516
		 0.0027114739641547203 -0.01929415762424469 -0.99981021881103516 0.0027114739641547203
		 -0.01929415762424469 -0.9508664608001709 0.30873396992683411 0.023161634802818298 -0.99981021881103516
		 0.0027114739641547203 -0.01929415762424469 -0.95086610317230225 -0.30315902829170227
		 -0.062834367156028748 -0.95086610317230225 -0.30315902829170227 -0.062834367156028748 -0.99981021881103516
		 0.0027114739641547203 -0.01929415762424469 -0.95086610317230225 -0.30315902829170227
		 -0.062834367156028748 -0.80883002281188965 -0.57891726493835449 -0.10319405794143677 -0.80883002281188965
		 -0.57891726493835449 -0.10319405794143677 -0.95086610317230225 -0.30315902829170227
		 -0.062834367156028748 -0.80883002281188965 -0.57891726493835449 -0.10319405794143677 -0.5876191258430481
		 -0.797554612159729 -0.13642013072967529 -0.5876191258430481 -0.797554612159729 -0.13642013072967529 -0.80883002281188965
		 -0.57891726493835449 -0.10319405794143677 -0.5876191258430481 -0.797554612159729
		 -0.13642013072967529 -0.31105870008468628 -0.93697810173034668 -0.15910494327545166 -0.31105870008468628
		 -0.93697810173034668 -0.15910494327545166 -0.5876191258430481 -0.797554612159729
		 -0.13642013072967529 -0.31105870008468628 -0.93697810173034668 -0.15910494327545166 0.023807890713214874
		 -0.98513692617416382 -0.1701122522354126 0.023807890713214874 -0.98513692617416382
		 -0.1701122522354126 -0.31105870008468628 -0.93697810173034668 -0.15910494327545166 0.023807890713214874
		 -0.98513692617416382 -0.1701122522354126 0.39310380816459656 -0.90455031394958496
		 -0.16510072350502014 0.39310380816459656 -0.90455031394958496 -0.16510072350502014 0.023807890713214874
		 -0.98513692617416382 -0.1701122522354126 0.39310380816459656 -0.90455031394958496
		 -0.16510072350502014 0.6916084885597229 -0.70759838819503784 -0.14485248923301697 0.6916084885597229
		 -0.70759838819503784 -0.14485248923301697 0.39310380816459656 -0.90455031394958496
		 -0.16510072350502014 0.6916084885597229 -0.70759838819503784 -0.14485248923301697 0.87547504901885986
		 -0.46866926550865173 -0.11786699295043945 0.87547504901885986 -0.46866926550865173
		 -0.11786699295043945 0.6916084885597229 -0.70759838819503784 -0.14485248923301697 0.87547504901885986
		 -0.46866926550865173 -0.11786699295043945 0.96973615884780884 -0.22758813202381134
		 -0.088405504822731018 0.96973615884780884 -0.22758813202381134 -0.088405504822731018 0.87547504901885986
		 -0.46866926550865173 -0.11786699295043945 0.96973615884780884 -0.22758813202381134
		 -0.088405504822731018 0.99835109710693359 0.0079881623387336731 -0.056843582540750504 0.99835109710693359
		 0.0079881623387336731 -0.056843582540750504 0.96973615884780884 -0.22758813202381134
		 -0.088405504822731018 0.99835109710693359 0.0079881623387336731 -0.056843582540750504 0.96973633766174316
		 0.24313893914222717 -0.022249052301049232 0.96973633766174316 0.24313893914222717
		 -0.022249052301049232 0.99835109710693359 0.0079881623387336731 -0.056843582540750504 0.0012585038784891367
		 -0.13917540013790131 0.99026691913604736 0 -0.13917131721973419 0.99026834964752197 0.99997991323471069
		 0.00087192800128832459 -0.0062778815627098083 1.6868634133970772e-007 -0.13917368650436401
		 0.99026799201965332 0.0012585038784891367 -0.13917540013790131 0.99026691913604736 0.99997991323471069
		 0.00087192800128832459 -0.0062778815627098083 -0.0012583351926878095 -0.13917535543441772
		 0.99026691913604736 1.6868634133970772e-007 -0.13917368650436401 0.99026799201965332 0.99997991323471069
		 0.00087192800128832459 -0.0062778815627098083 0 -0.9979252815246582 -0.064382277429103851 -0.0012583351926878095
		 -0.13917535543441772 0.99026691913604736 0.99997991323471069 0.00087192800128832459
		 -0.0062778815627098083 1.7344284060527571e-005 0.13917620480060577 -0.99026763439178467 0
		 -0.9979252815246582 -0.064382277429103851 0.99997991323471069 0.00087192800128832459
		 -0.0062778815627098083 -1.6012927517294884e-007 0.1391737163066864 -0.99026799201965332 1.7344284060527571e-005
		 0.13917620480060577 -0.99026763439178467 0.99997991323471069 0.00087192800128832459
		 -0.0062778815627098083 -1.7504413335700519e-005 0.13917538523674011 -0.99026775360107422 -1.6012927517294884e-007
		 0.1391737163066864 -0.99026799201965332 0.99997991323471069 0.00087192800128832459
		 -0.0062778815627098083 0 0.13917198777198792 -0.99026823043823242 -1.7504413335700519e-005
		 0.13917538523674011 -0.99026775360107422 0.99997991323471069 0.00087192800128832459
		 -0.0062778815627098083 0 0.13916531205177307 -0.99026918411254883 0 0.13917198777198792
		 -0.99026823043823242 0.99997991323471069 0.00087192800128832459 -0.0062778815627098083 0
		 0.13917328417301178 -0.9902680516242981 0 0.13916531205177307 -0.99026918411254883 0.99997991323471069
		 0.00087192800128832459 -0.0062778815627098083 1.750433693814557e-005 0.13917519152164459
		 -0.99026775360107422 0 0.13917328417301178 -0.9902680516242981 0.99997991323471069
		 0.00087192800128832459 -0.0062778815627098083 1.5994010027498007e-007 0.13917364180088043
		 -0.99026799201965332 1.750433693814557e-005 0.13917519152164459 -0.99026775360107422 0.99997991323471069
		 0.00087192800128832459 -0.0062778815627098083 -3.6594061384676024e-005 0.13917621970176697
		 -0.99026763439178467 1.5994010027498007e-007 0.13917364180088043 -0.99026799201965332 0.99997991323471069
		 0.00087192800128832459 -0.0062778815627098083 0.99999910593032837 -0.00033074244856834412
		 -0.0013229697942733765 -3.6594061384676024e-005 0.13917621970176697 -0.99026763439178467 0.99997991323471069
		 0.00087192800128832459 -0.0062778815627098083 0.0025165649130940437 -0.13917537033557892
		 0.99026453495025635 0.99999910593032837 -0.00033074244856834412 -0.0013229697942733765 0.99997991323471069
		 0.00087192800128832459 -0.0062778815627098083 -1.504090789694601e-007 -0.13917332887649536
		 0.9902680516242981 0.0025165649130940437 -0.13917537033557892 0.99026453495025635 0.99997991323471069
		 0.00087192800128832459 -0.0062778815627098083 -0.0012584917712956667 -0.13917534053325653
		 0.99026697874069214 -1.504090789694601e-007 -0.13917332887649536 0.9902680516242981 0.99997991323471069
		 0.00087192800128832459 -0.0062778815627098083 0 -0.13917326927185059 0.99026799201965332 -0.0012584917712956667
		 -0.13917534053325653 0.99026697874069214 0.99997991323471069 0.00087192800128832459
		 -0.0062778815627098083 0 -0.13916611671447754 0.99026906490325928 0 -0.13917326927185059
		 0.99026799201965332 0.99997991323471069 0.00087192800128832459 -0.0062778815627098083 0
		 -0.13917131721973419 0.99026834964752197 0 -0.13916611671447754 0.99026906490325928 0.99997991323471069
		 0.00087192800128832459 -0.0062778815627098083 0 -0.13917317986488342 0.99026811122894287 0
		 -0.13917261362075806 0.99026811122894287 -4.7837932726224608e-008 -0.13917295634746552
		 0.99026817083358765 0 -0.13917261362075806 0.99026811122894287 0 -0.13917270302772522
		 0.99026811122894287 -4.7837932726224608e-008 -0.13917295634746552 0.99026817083358765 0
		 -0.13917270302772522 0.99026811122894287 0 -0.13917288184165955 0.99026811122894287 -4.7837932726224608e-008
		 -0.13917295634746552 0.99026817083358765 0 -0.13917288184165955 0.99026811122894287 0
		 -0.13917292654514313 0.99026811122894287 -4.7837932726224608e-008 -0.13917295634746552
		 0.99026817083358765 0 -0.13917292654514313 0.99026811122894287 0 -0.13917283713817596
		 0.99026811122894287 -4.7837932726224608e-008 -0.13917295634746552 0.99026817083358765 0
		 -0.13917283713817596 0.99026811122894287 0 -0.1391727477312088 0.99026811122894287 -4.7837932726224608e-008
		 -0.13917295634746552 0.99026817083358765 0 -0.1391727477312088 0.99026811122894287 0
		 -0.13917270302772522 0.99026811122894287 -4.7837932726224608e-008 -0.13917295634746552
		 0.99026817083358765 0 -0.13917270302772522 0.99026811122894287 0 -0.13917317986488342
		 0.99026811122894287 -4.7837932726224608e-008 -0.13917295634746552 0.99026817083358765 0
		 -0.13917317986488342 0.99026811122894287 0 -0.13917380571365356 0.9902680516242981 -4.7837932726224608e-008
		 -0.13917295634746552 0.99026817083358765 0 -0.13917380571365356 0.9902680516242981 -4.7838022965152049e-007
		 -0.13917325437068939 0.99026811122894287 -4.7837932726224608e-008 -0.13917295634746552
		 0.99026817083358765 -4.7838022965152049e-007 -0.13917325437068939 0.99026811122894287 -1.4351397794598597e-006
		 -0.13917270302772522 0.99026811122894287 -4.7837932726224608e-008 -0.13917295634746552
		 0.99026817083358765 -1.4351397794598597e-006 -0.13917270302772522 0.99026811122894287 -9.5675954980833922e-007
		 -0.13917280733585358 0.99026811122894287 -4.7837932726224608e-008 -0.13917295634746552
		 0.99026817083358765 -9.5675954980833922e-007 -0.13917280733585358 0.99026811122894287 0
		 -0.13917291164398193 0.99026811122894287 -4.7837932726224608e-008 -0.13917295634746552
		 0.99026817083358765 0 -0.13917291164398193 0.99026811122894287 0 -0.13917297124862671
		 0.99026811122894287 -4.7837932726224608e-008 -0.13917295634746552 0.99026817083358765 0
		 -0.13917297124862671 0.99026811122894287 0 -0.13917291164398193 0.99026811122894287 -4.7837932726224608e-008
		 -0.13917295634746552 0.99026817083358765 0 -0.13917291164398193 0.99026811122894287 0
		 -0.13917273283004761 0.99026811122894287 -4.7837932726224608e-008 -0.13917295634746552
		 0.99026817083358765 0 -0.13917273283004761 0.99026811122894287 9.5676045930304099e-007
		 -0.13917261362075806 0.99026811122894287 -4.7837932726224608e-008 -0.13917295634746552
		 0.99026817083358765 9.5676045930304099e-007 -0.13917261362075806 0.99026811122894287 9.5676045930304099e-007
		 -0.13917320966720581 0.99026811122894287 -4.7837932726224608e-008 -0.13917295634746552
		 0.99026817083358765 9.5676045930304099e-007 -0.13917320966720581 0.99026811122894287 0
		 -0.13917376101016998 0.9902680516242981 -4.7837932726224608e-008 -0.13917295634746552
		 0.99026817083358765 0 -0.13917376101016998 0.9902680516242981 0 -0.13917317986488342
		 0.99026811122894287 -4.7837932726224608e-008 -0.13917295634746552 0.99026817083358765 0.9674917459487915
		 0.07511342316865921 -0.24149037897586823 0.8713105320930481 0.10175502300262451 -0.48006629943847656 0.8713105320930481
		 0.10175502300262451 -0.48006629943847656 0.9674917459487915 0.07511342316865921 -0.24149037897586823 0.8713105320930481
		 0.10175502300262451 -0.48006629943847656 0.68644088506698608 0.13132539391517639
		 -0.71522903442382813 0.68644088506698608 0.13132539391517639 -0.71522903442382813 0.8713105320930481
		 0.10175502300262451 -0.48006629943847656 0.68644088506698608 0.13132539391517639
		 -0.71522903442382813 0.38915863633155823 0.1572074294090271 -0.90765702724456787 0.38915863633155823
		 0.1572074294090271 -0.90765702724456787 0.68644088506698608 0.13132539391517639 -0.71522903442382813 0.38915863633155823
		 0.1572074294090271 -0.90765702724456787 -1.5225387173245508e-008 0.16802926361560822
		 -0.9857819676399231 -1.5225387173245508e-008 0.16802926361560822 -0.9857819676399231 0.38915863633155823
		 0.1572074294090271 -0.90765702724456787 -1.5225387173245508e-008 0.16802926361560822
		 -0.9857819676399231 -0.38915917277336121 0.15720738470554352 -0.90765672922134399 -0.38915917277336121
		 0.15720738470554352 -0.90765672922134399 -1.5225387173245508e-008 0.16802926361560822
		 -0.9857819676399231 -0.38915917277336121 0.15720738470554352 -0.90765672922134399 -0.68644118309020996
		 0.13132531940937042 -0.7152288556098938 -0.68644118309020996 0.13132531940937042
		 -0.7152288556098938 -0.38915917277336121 0.15720738470554352 -0.90765672922134399 -0.68644118309020996
		 0.13132531940937042 -0.7152288556098938 -0.87131041288375854 0.10175509750843048
		 -0.48006665706634521 -0.87131041288375854 0.10175509750843048 -0.48006665706634521 -0.68644118309020996
		 0.13132531940937042 -0.7152288556098938 -0.87131041288375854 0.10175509750843048
		 -0.48006665706634521 -0.96749162673950195 0.075113512575626373 -0.24149110913276672 -0.96749162673950195
		 0.075113512575626373 -0.24149110913276672 -0.87131041288375854 0.10175509750843048
		 -0.48006665706634521 -0.96749162673950195 0.075113512575626373 -0.24149110913276672 -0.99854791164398193
		 0.053357724100351334 -0.0074113062582910061 -0.99854791164398193 0.053357724100351334
		 -0.0074113062582910061 -0.96749162673950195 0.075113512575626373 -0.24149110913276672 -0.99854791164398193
		 0.053357724100351334 -0.0074113062582910061 -0.97311866283416748 0.036466874182224274
		 0.22739861905574799 -0.97311866283416748 0.036466874182224274 0.22739861905574799 -0.99854791164398193
		 0.053357724100351334 -0.0074113062582910061 -0.97311866283416748 0.036466874182224274
		 0.22739861905574799 -0.88283228874206543 0.023871393874287605 0.46908113360404968 -0.88283228874206543
		 0.023871393874287605 0.46908113360404968 -0.97311866283416748 0.036466874182224274
		 0.22739861905574799 -0.88283228874206543 0.023871393874287605 0.46908113360404968 -0.70222073793411255
		 0.014992648735642433 0.71180140972137451 -0.70222073793411255 0.014992648735642433
		 0.71180140972137451 -0.88283228874206543 0.023871393874287605 0.46908113360404968 -0.70222073793411255
		 0.014992648735642433 0.71180140972137451 -0.40202343463897705 0.0095460563898086548
		 0.91557955741882324 -0.40202343463897705 0.0095460563898086548 0.91557955741882324 -0.70222073793411255
		 0.014992648735642433 0.71180140972137451 -0.40202343463897705 0.0095460563898086548
		 0.91557955741882324 0 0.0076714185997843742 0.99997049570083618 0 0.0076714185997843742
		 0.99997049570083618 -0.40202343463897705 0.0095460563898086548 0.91557955741882324 0
		 0.0076714185997843742 0.99997049570083618 0.40202352404594421 0.0095460554584860802
		 0.91557955741882324 0.40202352404594421 0.0095460554584860802 0.91557955741882324 0
		 0.0076714185997843742 0.99997049570083618 0.40202352404594421 0.0095460554584860802
		 0.91557955741882324 0.7022208571434021 0.014992642216384411 0.71180129051208496 0.7022208571434021
		 0.014992642216384411 0.71180129051208496 0.40202352404594421 0.0095460554584860802
		 0.91557955741882324 0.7022208571434021 0.014992642216384411 0.71180129051208496 0.88283228874206543
		 0.023871375247836113 0.46908119320869446 0.88283228874206543 0.023871375247836113
		 0.46908119320869446 0.7022208571434021 0.014992642216384411 0.71180129051208496 0.88283228874206543
		 0.023871375247836113 0.46908119320869446 0.97311866283416748 0.036466870456933975
		 0.22739881277084351 0.97311866283416748 0.036466870456933975 0.22739881277084351 0.88283228874206543
		 0.023871375247836113 0.46908119320869446 0.97311866283416748 0.036466870456933975
		 0.22739881277084351 0.99854785203933716 0.05335766077041626 -0.0074106105603277683 0.99854785203933716
		 0.05335766077041626 -0.0074106105603277683 0.97311866283416748 0.036466870456933975
		 0.22739881277084351 0.99854785203933716 0.05335766077041626 -0.0074106105603277683 0.9674917459487915
		 0.07511342316865921 -0.24149037897586823 0.9674917459487915 0.07511342316865921 -0.24149037897586823 0.99854785203933716
		 0.05335766077041626 -0.0074106105603277683 -6.6665421627476462e-007 -0.97493159770965576
		 0.22250452637672424 0 -0.97493118047714233 0.22250655293464661 -3.3332593574186831e-008
		 -0.97493141889572144 0.22250555455684662 -6.6665421627476462e-007 -0.97493159770965576
		 0.2225048840045929 -6.6665421627476462e-007 -0.97493159770965576 0.22250452637672424 -3.3332593574186831e-008
		 -0.97493141889572144 0.22250555455684662 3.3332710813738231e-007 -0.97493135929107666
		 0.22250589728355408 -6.6665421627476462e-007 -0.97493159770965576 0.2225048840045929 -3.3332593574186831e-008
		 -0.97493141889572144 0.22250555455684662 -2.8421709430404007e-014 -0.97493135929107666
		 0.2225058525800705 3.3332710813738231e-007 -0.97493135929107666 0.22250589728355408 -3.3332593574186831e-008
		 -0.97493141889572144 0.22250555455684662 1.9895196601282805e-013 -0.97493153810501099
		 0.22250492870807648 -2.8421709430404007e-014 -0.97493135929107666 0.2225058525800705 -3.3332593574186831e-008
		 -0.97493141889572144 0.22250555455684662 -1.6666339774928929e-007 -0.97493129968643188
		 0.22250592708587646 1.9895196601282805e-013 -0.97493153810501099 0.22250492870807648 -3.3332593574186831e-008
		 -0.97493141889572144 0.22250555455684662 -8.3331372024986194e-008 -0.97493147850036621
		 0.22250530123710632 -1.6666339774928929e-007 -0.97493129968643188 0.22250592708587646 -3.3332593574186831e-008
		 -0.97493141889572144 0.22250555455684662 4.1665936123536085e-007 -0.97493147850036621
		 0.22250533103942871 -8.3331372024986194e-008 -0.97493147850036621 0.22250530123710632 -3.3332593574186831e-008
		 -0.97493141889572144 0.22250555455684662 0 -0.97493112087249756 0.22250661253929138 4.1665936123536085e-007
		 -0.97493147850036621 0.22250533103942871 -3.3332593574186831e-008 -0.97493141889572144
		 0.22250555455684662 -3.3332747761960491e-007 -0.97493171691894531 0.22250404953956604 0
		 -0.97493112087249756 0.22250661253929138 -3.3332593574186831e-008 -0.97493141889572144
		 0.22250555455684662 -2.2737367544323206e-013 -0.97493159770965576 0.22250467538833618 -3.3332747761960491e-007
		 -0.97493171691894531 0.22250404953956604 -3.3332593574186831e-008 -0.97493141889572144
		 0.22250555455684662 -1.1368683772161603e-013 -0.97493112087249756 0.22250664234161377 -2.2737367544323206e-013
		 -0.97493159770965576 0.22250467538833618 -3.3332593574186831e-008 -0.97493141889572144
		 0.22250555455684662 1.4210854715202004e-013 -0.97493135929107666 0.22250586748123169 -1.1368683772161603e-013
		 -0.97493112087249756 0.22250664234161377 -3.3332593574186831e-008 -0.97493141889572144
		 0.22250555455684662 1.1368683772161603e-013 -0.97493135929107666 0.22250588238239288 1.4210854715202004e-013
		 -0.97493135929107666 0.22250586748123169 -3.3332593574186831e-008 -0.97493141889572144
		 0.22250555455684662 1.9895196601282805e-013 -0.97493141889572144 0.22250543534755707 1.1368683772161603e-013
		 -0.97493135929107666 0.22250588238239288 -3.3332593574186831e-008 -0.97493141889572144
		 0.22250555455684662 1.7053025658242404e-013 -0.97493159770965576 0.22250477969646454 1.9895196601282805e-013
		 -0.97493141889572144 0.22250543534755707 -3.3332593574186831e-008 -0.97493141889572144
		 0.22250555455684662 8.3332309941397398e-008 -0.97493171691894531 0.22250416874885559 1.7053025658242404e-013
		 -0.97493159770965576 0.22250477969646454 -3.3332593574186831e-008 -0.97493141889572144
		 0.22250555455684662 4.1665973071758344e-007 -0.97493147850036621 0.22250545024871826 8.3332309941397398e-008
		 -0.97493171691894531 0.22250416874885559 -3.3332593574186831e-008 -0.97493141889572144
		 0.22250555455684662 0 -0.97493088245391846 0.22250792384147644 4.1665973071758344e-007
		 -0.97493147850036621 0.22250545024871826 -3.3332593574186831e-008 -0.97493141889572144
		 0.22250555455684662 0 -0.97493118047714233 0.22250655293464661 0 -0.97493088245391846
		 0.22250792384147644 -3.3332593574186831e-008 -0.97493141889572144 0.22250555455684662 0
		 1 3.2796006053104065e-006 0 1 0 0 1 4.7284157744797994e-007 0 1 0 0 1 3.2795996958157048e-006 0
		 1 4.7284157744797994e-007 0 1 3.2795996958157048e-006 0 1 3.2795996958157048e-006 0
		 1 4.7284157744797994e-007 0 1 3.2795996958157048e-006 0 1 0 0 1 4.7284157744797994e-007 0
		 1 0 0 1 0 0 1 4.7284157744797994e-007 0 1 0 0 0.99999994039535522 0 0 1 4.7284157744797994e-007 0
		 0.99999994039535522 0 0 1 0 0 1 4.7284157744797994e-007 0 1 0 0 1 3.2795996958157048e-006 0
		 1 4.7284157744797994e-007 0 1 3.2795996958157048e-006 0 0.99999994039535522 0 0 1
		 4.7284157744797994e-007 0 0.99999994039535522 0 0 1 -3.2795996958157048e-006 0 1
		 4.7284157744797994e-007 0 1 -3.2795996958157048e-006 0 1 0 0 1 4.7284157744797994e-007 0
		 1 0 0 1 0 0 1 4.7284157744797994e-007 0 1 0 0 1 0 0 1 4.7284157744797994e-007 0 1
		 0 0 1 -1.9098570191999897e-007 0 1 4.7284157744797994e-007 0 1 -1.9098570191999897e-007 0
		 1 -1.83078611826204e-006 0 1 4.7284157744797994e-007 0 1 -1.83078611826204e-006 0
		 1 1.63980246270512e-006 0 1 4.7284157744797994e-007 0 1 1.63980246270512e-006 0 1
		 1.1368683772161603e-012 0 1 4.7284157744797994e-007 0 1 1.1368683772161603e-012 0
		 0.99999994039535522 -3.2796017421787838e-006 0 1 4.7284157744797994e-007 0 0.99999994039535522
		 -3.2796017421787838e-006 0 1 3.2796006053104065e-006 0 1 4.7284157744797994e-007 0
		 1 3.2796006053104065e-006 0 1 3.2796006053104065e-006 0 1 4.7284157744797994e-007 0
		 -0.022547710686922073 0.99974572658538818 0 -0.022547710686922073 0.99974572658538818 0
		 0.070427954196929932 0.99751687049865723 0 0.070427954196929932 0.99751687049865723 0
		 0.070427954196929932 0.99751687049865723 0 0.070427954196929932 0.99751687049865723 0
		 0.16279445588588715 0.98666000366210938 0 0.16279445588588715 0.98666000366210938 0
		 0.9881669282913208 0.15338221192359924 0 0.9881669282913208 0.15338221192359924 0
		 0.9881669282913208 0.15338221192359924 0 0.9881669282913208 0.15338221192359924 0
		 -0.018852211534976959 -0.99982225894927979 0 -0.018852211534976959 -0.99982225894927979 0
		 -0.059458348900079727 -0.99823075532913208 0 -0.059458348900079727 -0.99823075532913208 0
		 -0.059458348900079727 -0.99823075532913208 0 -0.059458348900079727 -0.99823075532913208 0
		 -0.099966302514076233 -0.99499082565307617 0 -0.099966302514076233 -0.99499082565307617 0
		 -1 0 0 -1 0 0 -1 0 0 -1 0 0.99999994039535522 -2.1618871137718543e-009 0 0.99999994039535522
		 -2.1618871137718543e-009 0 1 6.861937684732311e-010 0 1 6.861937684732311e-010 0 1
		 6.861937684732311e-010 0 1 6.861937684732311e-010 0 1 3.5342746507183165e-009 0 1
		 3.5342746507183165e-009 0 -0.99999994039535522 -8.6475484550874171e-009 0 -0.99999994039535522
		 -8.6475484550874171e-009 0 -1 -4.7655586143946493e-009 0 -1 -4.7655586143946493e-009
		 0 -1 -4.7655586143946493e-009 0 -1 -4.7655586143946493e-009 0 -1 -8.8356866267957912e-010
		 0 -1 -8.8356866267957912e-010 0 ;
createNode transform -n "i_heli_crap_i_m_cabin";
	setAttr ".r" -type "double3" 12.157291444336778 0 0 ;
	setAttr ".s" -type "double3" 0.41496719552532679 0.50718522082715978 1.0321282858061136 ;
	setAttr ".rp" -type "double3" 0 2.0032436025600404 -1.147612769536082 ;
	setAttr ".rpt" -type "double3" 0 0.19675639743996037 0.44761276953608164 ;
	setAttr ".sp" -type "double3" 0 3.949727871196612 -1.1118896607312456 ;
	setAttr ".spt" -type "double3" 0 -1.9464842686365709 -0.035723108804836139 ;
createNode transform -n "transform6" -p "i_heli_crap_i_m_cabin";
createNode mesh -n "i_m_cabinShape" -p "|i_heli_crap_i_m_cabin|transform6";
	addAttr -ci true -sn "mso" -ln "miShadingSamplesOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "msh" -ln "miShadingSamples" -min 0 -smx 8 -at "float";
	addAttr -ci true -sn "mdo" -ln "miMaxDisplaceOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "mmd" -ln "miMaxDisplace" -min 0 -smx 1 -at "float";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 4 ".iog[0].og";
	setAttr -av ".iog[0].og[0].gco";
	setAttr -av ".iog[0].og[0].gid";
	setAttr -av ".iog[0].og[1].gco";
	setAttr -av ".iog[0].og[1].gid";
	setAttr -av ".iog[0].og[2].gco";
	setAttr -av ".iog[0].og[2].gid";
	setAttr -av ".iog[0].og[4].gco";
	setAttr -av ".iog[0].og[4].gid";
	setAttr -av ".iog[0].og[11].gco";
	setAttr -av ".iog[0].og[11].gid";
	setAttr -av ".iog[0].og[12].gco";
	setAttr -av ".iog[0].og[12].gid";
	setAttr -av ".iog[0].og[13].gco";
	setAttr -av ".iog[0].og[13].gid";
	setAttr -av ".iog[0].og[14].gco";
	setAttr -av ".iog[0].og[14].gid";
	setAttr -av ".iog[0].og[15].gco";
	setAttr -av ".iog[0].og[15].gid";
	setAttr -av ".iog[0].og[16].gco";
	setAttr -av ".iog[0].og[16].gid";
	setAttr -av ".iog[0].og[21].gco";
	setAttr -av ".iog[0].og[21].gid";
	setAttr -av ".iog[0].og[28].gco";
	setAttr -av ".iog[0].og[28].gid";
	setAttr -av ".iog[0].og[30].gco";
	setAttr -av ".iog[0].og[30].gid";
	setAttr -av ".iog[0].og[34].gco";
	setAttr -av ".iog[0].og[34].gid";
	setAttr -av ".iog[0].og[36].gco";
	setAttr -av ".iog[0].og[36].gid";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 24 ".pt";
	setAttr ".pt[101]" -type "float3" 0.56094182 0.009343667 0.077039868 ;
	setAttr ".pt[103]" -type "float3" -0.57184654 0.0093436595 0.074236751 ;
	setAttr ".pt[118]" -type "float3" 0.57580149 0.0093436632 0.081323892 ;
	setAttr ".pt[120]" -type "float3" -0.57879615 0.0093436595 0.07815332 ;
	setAttr ".pt[135]" -type "float3" 0.58318251 0.0093436632 0.080764532 ;
	setAttr ".pt[137]" -type "float3" -0.58300179 0.0093436595 0.077326417 ;
	setAttr ".pt[152]" -type "float3" 0.58263248 0.0093436614 0.075173765 ;
	setAttr ".pt[154]" -type "float3" -0.58318251 0.0093436576 0.07142169 ;
	setAttr ".pt[169]" -type "float3" 0.57279515 0.0093436604 0.064526476 ;
	setAttr ".pt[171]" -type "float3" -0.57711625 0.0093436595 0.060440894 ;
	setAttr ".pt[189]" -type "float3" 0.55133873 0.0093436595 0.04922625 ;
	setAttr ".pt[191]" -type "float3" -0.5615083 0.0093436576 0.044921324 ;
	setAttr ".pt[209]" -type "float3" 0.51492989 0.0093436576 0.030089408 ;
	setAttr ".pt[211]" -type "float3" -0.53103793 0.0093436595 0.025809258 ;
	setAttr ".pt[229]" -type "float3" 0.46015272 0.0093436558 0.0080032162 ;
	setAttr ".pt[231]" -type "float3" -0.48092136 0.0093436558 0.0040958463 ;
	setAttr ".pt[249]" -type "float3" 0.38657257 0.0093436595 -0.016496405 ;
	setAttr ".pt[251]" -type "float3" -0.41021973 0.0093436595 -0.019699734 ;
	setAttr ".pt[269]" -type "float3" 0.29699728 0.0093436595 -0.043591805 ;
	setAttr ".pt[271]" -type "float3" -0.32143703 0.0093436595 -0.045916919 ;
	setAttr ".pt[289]" -type "float3" 0.19719188 0 0 ;
	setAttr ".pt[291]" -type "float3" -0.19719188 0 0 ;
	setAttr ".pt[309]" -type "float3" 0.075114422 0 0 ;
	setAttr ".pt[311]" -type "float3" -0.07171613 0 0 ;
createNode transform -n "i_m_body1";
	setAttr ".rp" -type "double3" 0 2.2 -0.7 ;
	setAttr ".sp" -type "double3" 0 2.2 -0.7 ;
createNode transform -n "polySurface1" -p "i_m_body1";
createNode transform -n "transform17" -p "polySurface1";
createNode mesh -n "polySurfaceShape1" -p "transform17";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "polySurface2" -p "i_m_body1";
createNode transform -n "transform15" -p "polySurface2";
createNode mesh -n "polySurfaceShape2" -p "transform15";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr ".iog[0].og[1].gcl" -type "componentList" 1 "f[0:315]";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr -s 360 ".uvst[0].uvsp";
	setAttr ".uvst[0].uvsp[0:249]" -type "float2" 0.40000004 0.050000001 0.45000005 
		0.050000001 0.45000005 0.1 0.40000004 0.1 0.50000006 0.050000001 0.50000006 0.1 0.55000007 
		0.050000001 0.55000007 0.1 0.60000008 0.050000001 0.60000008 0.1 0.6500001 0.050000001 
		0.6500001 0.1 0.70000011 0.050000001 0.70000011 0.1 0.75000012 0.050000001 0.75000012 
		0.1 0.80000013 0.050000001 0.80000013 0.1 0.85000014 0.050000001 0.85000014 0.1 0.90000015 
		0.050000001 0.90000015 0.1 0.95000017 0.050000001 0.95000017 0.1 1.0000001 0.050000001 
		1.0000001 0.1 0.45000005 0.15000001 0.40000004 0.15000001 0.50000006 0.15000001 0.55000007 
		0.15000001 0.60000008 0.15000001 0.6500001 0.15000001 0.70000011 0.15000001 0.75000012 
		0.15000001 0.80000013 0.15000001 0.85000014 0.15000001 0.90000015 0.15000001 0.95000017 
		0.15000001 1.0000001 0.15000001 0.45000005 0.2 0.40000004 0.2 0.50000006 0.2 0.55000007 
		0.2 0.60000008 0.2 0.6500001 0.2 0.70000011 0.2 0.75000012 0.2 0.80000013 0.2 0.85000014 
		0.2 0.90000015 0.2 0.95000017 0.2 1.0000001 0.2 0.45000005 0.25 0.40000004 0.25 0.50000006 
		0.25 0.55000007 0.25 0.60000008 0.25 0.6500001 0.25 0.70000011 0.25 0.75000012 0.25 
		0.80000013 0.25 0.85000014 0.25 0.90000015 0.25 0.95000017 0.25 1.0000001 0.25 0.45000005 
		0.30000001 0.40000004 0.30000001 0.50000006 0.30000001 0.55000007 0.30000001 0.60000008 
		0.30000001 0.6500001 0.30000001 0.70000011 0.30000001 0.75000012 0.30000001 0.80000013 
		0.30000001 0.85000014 0.30000001 0.90000015 0.30000001 0.95000017 0.30000001 1.0000001 
		0.30000001 0.45000005 0.35000002 0.40000004 0.35000002 0.50000006 0.35000002 0.55000007 
		0.35000002 0.60000008 0.35000002 0.6500001 0.35000002 0.70000011 0.35000002 0.75000012 
		0.35000002 0.80000013 0.35000002 0.85000014 0.35000002 0.90000015 0.35000002 0.95000017 
		0.35000002 1.0000001 0.35000002 0.45000005 0.40000004 0.40000004 0.40000004 0.50000006 
		0.40000004 0.55000007 0.40000004 0.60000008 0.40000004 0.6500001 0.40000004 0.70000011 
		0.40000004 0.75000012 0.40000004 0.80000013 0.40000004 0.85000014 0.40000004 0.90000015 
		0.40000004 0.95000017 0.40000004 1.0000001 0.40000004 0 0.40000004 0.050000001 0.40000004 
		0.050000001 0.45000005 0 0.45000005 0.1 0.40000004 0.1 0.45000005 0.30000001 0.40000004 
		0.35000002 0.40000004 0.35000002 0.45000005 0.30000001 0.45000005 0.40000004 0.45000005 
		0.45000005 0.45000005 0.50000006 0.45000005 0.55000007 0.45000005 0.60000008 0.45000005 
		0.6500001 0.45000005 0.70000011 0.45000005 0.75000012 0.45000005 0.80000013 0.45000005 
		0.85000014 0.45000005 0.90000015 0.45000005 0.95000017 0.45000005 1.0000001 0.45000005 
		0.050000001 0.50000006 0 0.50000006 0.1 0.50000006 0.35000002 0.50000006 0.30000001 
		0.50000006 0.40000004 0.50000006 0.45000005 0.50000006 0.50000006 0.50000006 0.55000007 
		0.50000006 0.60000008 0.50000006 0.6500001 0.50000006 0.70000011 0.50000006 0.75000012 
		0.50000006 0.80000013 0.50000006 0.85000014 0.50000006 0.90000015 0.50000006 0.95000017 
		0.50000006 1.0000001 0.50000006 0.050000001 0.55000007 0 0.55000007 0.1 0.55000007 
		0.35000002 0.55000007 0.30000001 0.55000007 0.40000004 0.55000007 0.45000005 0.55000007 
		0.50000006 0.55000007 0.55000007 0.55000007 0.60000008 0.55000007 0.6500001 0.55000007 
		0.70000011 0.55000007 0.75000012 0.55000007 0.80000013 0.55000007 0.85000014 0.55000007 
		0.90000015 0.55000007 0.95000017 0.55000007 1.0000001 0.55000007 0.050000001 0.60000008 
		0 0.60000008 0.1 0.60000008 0.35000002 0.60000008 0.30000001 0.60000008 0.40000004 
		0.60000008 0.45000005 0.60000008 0.50000006 0.60000008 0.55000007 0.60000008 0.60000008 
		0.60000008 0.6500001 0.60000008 0.70000011 0.60000008 0.75000012 0.60000008 0.80000013 
		0.60000008 0.85000014 0.60000008 0.90000015 0.60000008 0.95000017 0.60000008 1.0000001 
		0.60000008 0.050000001 0.6500001 0 0.6500001 0.1 0.6500001 0.35000002 0.6500001 0.30000001 
		0.6500001 0.40000004 0.6500001 0.45000005 0.6500001 0.50000006 0.6500001 0.55000007 
		0.6500001 0.60000008 0.6500001 0.6500001 0.6500001 0.70000011 0.6500001 0.75000012 
		0.6500001 0.80000013 0.6500001 0.85000014 0.6500001 0.90000015 0.6500001 0.95000017 
		0.6500001 1.0000001 0.6500001 0.050000001 0.70000011 0 0.70000011 0.1 0.70000011 
		0.15000001 0.6500001 0.15000001 0.70000011 0.2 0.6500001 0.2 0.70000011 0.25 0.6500001 
		0.25 0.70000011 0.30000001 0.70000011 0.35000002 0.70000011 0.40000004 0.70000011 
		0.45000005 0.70000011 0.50000006 0.70000011 0.55000007 0.70000011 0.60000008 0.70000011 
		0.6500001 0.70000011 0.70000011 0.70000011 0.75000012 0.70000011 0.80000013 0.70000011 
		0.85000014 0.70000011 0.90000015 0.70000011 0.95000017 0.70000011 1.0000001 0.70000011 
		0.050000001 0.75000012 0 0.75000012 0.1 0.75000012 0.15000001 0.75000012 0.2 0.75000012 
		0.25 0.75000012 0.30000001 0.75000012 0.35000002 0.75000012 0.40000004 0.75000012 
		0.45000005 0.75000012 0.50000006 0.75000012 0.55000007 0.75000012 0.60000008 0.75000012 
		0.6500001 0.75000012 0.70000011 0.75000012 0.75000012 0.75000012 0.80000013 0.75000012 
		0.85000014 0.75000012 0.90000015 0.75000012 0.95000017 0.75000012 1.0000001 0.75000012 
		0.050000001 0.80000013 0 0.80000013 0.1 0.80000013 0.15000001 0.80000013 0.2 0.80000013 
		0.25 0.80000013;
	setAttr ".uvst[0].uvsp[250:359]" 0.30000001 0.80000013 0.35000002 0.80000013 
		0.40000004 0.80000013 0.45000005 0.80000013 0.50000006 0.80000013 0.55000007 0.80000013 
		0.60000008 0.80000013 0.6500001 0.80000013 0.70000011 0.80000013 0.75000012 0.80000013 
		0.80000013 0.80000013 0.85000014 0.80000013 0.90000015 0.80000013 0.95000017 0.80000013 
		1.0000001 0.80000013 0.050000001 0.85000014 0 0.85000014 0.1 0.85000014 0.15000001 
		0.85000014 0.2 0.85000014 0.25 0.85000014 0.30000001 0.85000014 0.35000002 0.85000014 
		0.40000004 0.85000014 0.45000005 0.85000014 0.50000006 0.85000014 0.55000007 0.85000014 
		0.60000008 0.85000014 0.6500001 0.85000014 0.70000011 0.85000014 0.75000012 0.85000014 
		0.80000013 0.85000014 0.85000014 0.85000014 0.90000015 0.85000014 0.95000017 0.85000014 
		1.0000001 0.85000014 0.050000001 0.90000015 0 0.90000015 0.1 0.90000015 0.15000001 
		0.90000015 0.2 0.90000015 0.25 0.90000015 0.30000001 0.90000015 0.35000002 0.90000015 
		0.40000004 0.90000015 0.45000005 0.90000015 0.50000006 0.90000015 0.55000007 0.90000015 
		0.60000008 0.90000015 0.6500001 0.90000015 0.70000011 0.90000015 0.75000012 0.90000015 
		0.80000013 0.90000015 0.85000014 0.90000015 0.90000015 0.90000015 0.95000017 0.90000015 
		1.0000001 0.90000015 0.050000001 0.95000017 0 0.95000017 0.1 0.95000017 0.15000001 
		0.95000017 0.2 0.95000017 0.25 0.95000017 0.30000001 0.95000017 0.35000002 0.95000017 
		0.40000004 0.95000017 0.45000005 0.95000017 0.50000006 0.95000017 0.55000007 0.95000017 
		0.60000008 0.95000017 0.6500001 0.95000017 0.70000011 0.95000017 0.75000012 0.95000017 
		0.80000013 0.95000017 0.85000014 0.95000017 0.90000015 0.95000017 0.95000017 0.95000017 
		1.0000001 0.95000017 0.42500001 0 0.47500002 0 0.52499998 0 0.57499999 0 0.625 0 
		0.67500001 0 0.72499996 0 0.77499998 0 0.82499999 0 0.875 0 0.92500001 0 0.97499996 
		0 0.025 1 0.075000003 1 0.125 1 0.17500001 1 0.22500001 1 0.27500001 1 0.32500002 
		1 0.375 1 0.42500001 1 0.47500002 1 0.52499998 1 0.57499999 1 0.625 1 0.67500001 
		1 0.72499996 1 0.77499998 1 0.82499999 1 0.875 1 0.92500001 1 0.97499996 1;
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 318 ".vt";
	setAttr ".vt[0:165]"  0.23255864 -0.86930931 -0.4443638 -0.1570819 -0.87397408 
		-0.44247398 -0.16943073 -0.87425292 -0.3331084 -0.16067025 -0.87355864 -0.22182773 
		-0.13048741 -0.87267053 -0.11988834 -0.081667066 -0.87206084 -0.038009297 -0.019504352 
		-0.871562 0.015003255 0.049298726 -0.87078333 0.033361115 0.11748938 -0.86956632 
		0.015001097 0.17773017 -0.86829585 -0.038155932 0.22316177 -0.86763114 -0.12039202 
		0.24848868 -0.86795652 -0.22285205 0.25128016 -0.86881685 -0.33466047 0.39864436 
		-0.8360191 -0.54180431 -0.33945003 -0.84398574 -0.53764486 -0.37436482 -0.85228574 
		-0.32357773 -0.36441788 -0.8579272 -0.10223627 -0.30627972 -0.86449164 0.10288522 
		-0.20878926 -0.87327772 0.26205713 -0.086689018 -0.88072205 0.35906032 0.046613004 
		-0.88228047 0.39141837 0.1796027 -0.87631112 0.35886747 0.30040166 -0.86501122 0.26204118 
		0.39452094 -0.85336238 0.10335277 0.44573998 -0.84574848 -0.10323633 0.44528809 -0.8414734 
		-0.32654822 0.53602272 -0.77379465 -0.62698656 -0.4962205 -0.78286505 -0.62038004 
		-0.5612995 -0.80794179 -0.30850127 -0.55343944 -0.8290844 0.018751498 -0.46429709 
		-0.85341936 0.31308103 -0.31921804 -0.88019055 0.52394474 -0.14600307 -0.90025055 
		0.64810216 0.038364787 -0.90543747 0.68832034 0.22268909 -0.89302951 0.64635426 0.39529875 
		-0.86681014 0.52189243 0.53795147 -0.83574283 0.31243104 0.62058204 -0.8099705 0.018835763 
		0.61581808 -0.79261583 -0.31263757 0.644153 -0.68254602 -0.69799232 -0.62359637 -0.69027388 
		-0.68892002 -0.72117412 -0.73699403 -0.28823796 -0.71435338 -0.77911872 0.13741374 
		-0.59201658 -0.82490104 0.49593428 -0.40535498 -0.86963642 0.74171835 -0.19263764 
		-0.90138143 0.88378119 0.02908764 -0.90995765 0.92876524 0.25083986 -0.89189702 0.8789385 
		0.46271965 -0.8519994 0.73507625 0.64618552 -0.80165112 0.4915624 0.76196837 -0.75442761 
		0.13701391 0.75622761 -0.71917456 -0.29323095 0.72575498 -0.56610793 -0.75332624 
		-0.72148591 -0.57229197 -0.74192876 -0.84754133 -0.63889933 -0.26325199 -0.83945709 
		-0.70248425 0.24560097 -0.68720311 -0.76803464 0.64921403 -0.46864632 -0.82796693 
		0.91937411 -0.22738503 -0.86945313 1.0766789 0.020774066 -0.88119543 1.1262122 0.26916143 
		-0.85899109 1.0676336 0.50918007 -0.80818915 0.90604985 0.72219658 -0.74174356 0.6387648 
		0.86570609 -0.67486262 0.24184026 0.86274749 -0.6212135 -0.26973414 0.78509533 -0.43294424 
		-0.79193014 -0.79234385 -0.43804389 -0.77847868 -0.93735564 -0.51667708 -0.23451932 
		-0.92719018 -0.59872532 0.33920822 -0.75315356 -0.68079144 0.77345514 -0.51333076 
		-0.75291312 1.062713 -0.25242347 -0.80199492 1.2364773 0.013944623 -0.81648272 1.2918277 
		0.28097787 -0.79191995 1.2227737 0.54072833 -0.73319727 1.0414082 0.77235729 -0.65420532 
		0.75502855 0.9338243 -0.57110268 0.33038068 0.93489474 -0.50138783 -0.24365684 0.82635438 
		-0.28783691 -0.81339484 -0.83976477 -0.29123724 -0.79830748 -0.99237162 -0.37605679 
		-0.20380872 -0.98021781 -0.4711484 0.41663057 -0.79518157 -0.56610131 0.87025148 
		-0.54359722 -0.64729106 1.1759083 -0.26993099 -0.70131439 1.3675791 0.0087273065 
		-0.71763659 1.4298234 0.28852609 -0.69263399 1.3494104 0.56157058 -0.6293124 1.1463206 
		0.80300474 -0.54130763 0.84276104 0.97153443 -0.44592327 0.40133277 0.9761942 -0.36360058 
		-0.21627982 0.85249633 -0.13430111 -0.81855369 0.69564873 -0.057644296 -1.3647156 
		0.50541836 0.015919365 -1.8022805 -0.50541824 0.021262564 -1.7968524 -0.69574386 
		-0.053121705 -1.3546848 -0.86736798 -0.13558546 -0.80143291 -1.0169724 -0.22385499 
		-0.17240191 -1.0040628 -0.32472223 0.47709039 -0.81844497 -0.42842099 0.94127804 
		-0.5628702 -0.51582581 1.2604787 -0.049027361 -0.58572352 1.5477513 0.0051620738 
		-0.59136283 1.5379297 0.056244757 -0.57692486 1.5229059 0.57455456 -0.50015628 1.223205 
		0.81906253 -0.40660864 0.90463132 0.985502 -0.30216929 0.45442158 0.9923805 -0.21237744 
		-0.1888127 0.86510998 0.024062796 -0.80827445 0.72244304 0.1028861 -1.3729066 0.52488559 
		0.18098612 -1.8281493 -0.52488548 0.18811727 -1.8228633 -0.72244281 0.1098186 -1.36289 
		-0.8779397 0.025066799 -0.78835219 -1.0170782 -0.064053394 -0.1415008 -1.0053009 
		-0.16691604 0.52043313 -0.82706857 -0.27527884 0.98784673 -0.57358527 -0.3669847 
		1.31589 -0.050408356 -0.44080272 1.6198399 0.0031477541 -0.44651124 1.6105421 0.056890562 
		-0.43167812 1.5915592 0.58150834 -0.3510105 1.2721909 0.82381958 -0.25134876 0.94263524 
		0.98182315 -0.13953763 0.48961332 0.98888046 -0.050765723 -0.16256018 0.86440033 
		0.18364801 -0.78327364 0.73144841 0.26216668 -1.3502086 0.53142834 0.3419722 -1.8116982 
		-0.53142822 0.35068682 -1.8065326 -0.73144811 0.27124342 -1.3403103 -0.87332839 0.18684757 
		-0.7604984 -0.99886119 0.09936408 -0.11216702 -0.98982066 -0.0021310814 0.54686177 
		-0.82377762 -0.11218332 1.0108615 -0.57694936 -0.20543699 1.3408959 -0.05109423 -0.28018987 
		1.6491042 0.0024266981 -0.2863915 1.642134 0.057281408 -0.27093461 1.6182992 0.58320206 
		-0.18601273 1.2920452 0.81943667 -0.079637192 0.95769578 0.96609008 0.035846762 0.50732738 
		0.97106194 0.11978905 -0.13872817 0.85056722 0.34245658 -0.74468142 0.72244304 0.41654101 
		-1.2973413 0.52488559 0.49478352 -1.753188 -0.52488548 0.50473428 -1.7481027 -0.72244281 
		0.42726678 -1.2874058 -0.85572827 0.3459484 -0.71920747 -0.9668268 0.2640326 -0.085410848 
		-0.96120799 0.16891943 0.5562737 -0.80963814 0.059337378 1.0105739 -0.57284439 -0.034187108 
		1.3347037 -0.051043078 -0.10737716 1.6329981 0.0026821687 -0.11414827 1.6296279 0.05729818 
		-0.094647564 1.5999466 0.57941443 -0.0063523292 1.2814575 0.80701995 0.1035569 0.95043832 
		0.94196588 0.21873902 0.50846899 0.94350672 0.29528576 -0.11790066 0.82269353 0.50053293 
		-0.69398326 0.69564873 0.56267929 -1.2178183 0.50541836 0.63598883 -1.654013 -0.50541824 
		0.64667708 -1.6489736 -0.69564849 0.57435739 -1.205811 -0.82531482 0.498698 -0.66608709 
		-0.923087 0.42796713 -0.062189061;
	setAttr ".vt[166:317]" -0.92083263 0.34297198 0.54873264 -0.78417259 0.23821999 
		0.98667121 -0.56012541 0.1471436 1.2970221 -0.050128862 0.078223787 1.5714091 0.0036577585 
		0.07137835 1.5728228 0.056734443 0.096243031 1.5363152 0.56876528 0.18409947 1.2406496 
		0.78592712 0.29309452 0.9217248 0.91000134 0.40151155 0.49442708 0.90808141 0.46817291 
		-0.10000968 0.77739894 0.65401125 -0.63264197 0.65172529 0.69763565 -1.1152561 0.47350609 
		0.76298028 -1.5166624 0.24893685 0.81508726 -1.782347 0 0.83686858 -1.8737464 -0.24893685 
		0.82117939 -1.7802444 -0.473506 0.7737664 -1.5116639 -0.65172505 0.7094273 -1.1005105 
		-0.78025597 0.64284962 -0.60267591 -0.86844856 0.58425361 -0.042921621 -0.87082881 
		0.51206326 0.52464676 -0.74558473 0.41979212 0.93913752 -0.53671563 0.3352758 1.229076 
		-0.048134934 0.27441019 1.467712 0.0051530506 0.26756796 1.4755361 0.055283941 0.29726812 
		1.4315681 0.54876655 0.37985691 1.1718138 0.75488818 0.47975069 0.87273622 0.87273222 
		0.57606614 0.46694598 0.86335665 0.63205236 -0.08442717 0.71486384 0.7960608 -0.56198072 
		0.59243828 0.82163107 -0.99345708 0.42993453 0.87398189 -1.3476328 0.22602995 0.91786689 
		-1.5852331 0 0.93648148 -1.6678345 -0.22602995 0.92368156 -1.5830822 -0.42993444 
		0.88413215 -1.3407848 -0.59304672 0.82985002 -0.97599739 -0.7174384 0.77578652 -0.53082973 
		-0.8072288 0.72961599 -0.027637094 -0.81599754 0.67104065 0.48479456 -0.69969577 
		0.59390098 0.86833292 -0.50250298 0.52281982 1.1331669 -0.045071855 0.47500402 1.3285166 
		0.0068474128 0.46813187 1.3453655 0.053262386 0.50007564 1.2933155 0.51980036 0.56868231 
		1.0784371 0.71574497 0.6547724 0.80467457 0.82364237 0.73738933 0.42778373 0.80511451 
		0.78247434 -0.070274137 0.63620579 0.92248929 -0.48310301 0.52207965 0.93603927 -0.85617524 
		0.37577659 0.96789908 -1.1563739 0.19755742 1.0023084 -1.3557894 0 1.0170791 -1.4252391 
		-0.19755742 1.0074168 -1.3524524 -0.37579998 0.97663474 -1.1470847 -0.52285069 0.93397701 
		-0.83661169 -0.63850009 0.89466268 -0.45224869 -0.73023504 0.86061966 -0.016079269 
		-0.74454391 0.81555021 0.4303273 -0.64263403 0.75417322 0.77470154 -0.46197322 0.69762552 
		1.0114553 -0.045856673 0.66501385 1.1607631 0.0087994449 0.65706855 1.1906592 0.056281377 
		0.68885225 1.1292633 0.48291436 0.74087048 0.9632417 0.66053528 0.81253237 0.71814942 
		0.75269425 0.88018245 0.37830052 0.72900558 0.91494447 -0.056822531 0.54686785 1.0296504 
		-0.39696434 0.44137028 1.0348779 -0.70548993 0.31463212 1.0502087 -0.94923604 0.16440189 
		1.0694027 -1.1079937 7.0074398e-006 1.079461 -1.1622748 -0.16436425 1.0726721 -1.1033391 
		-0.31453684 1.0510035 -0.93808937 -0.44124928 1.0232385 -0.68593097 -0.5480504 0.99741095 
		-0.36838803 -0.63032371 0.97356337 -0.0078807883 -0.64542603 0.94083738 0.36266181 
		-0.56302291 0.89534342 0.65866899 -0.407442 0.85283935 0.86448997 -0.048073284 0.83492696 
		0.96769315 0.010865383 0.82413781 1.0155284 0.060658041 0.8557806 0.94241965 0.42970654 
		0.89124024 0.82591027 0.5805729 0.94775486 0.61279976 0.65378433 0.99944282 0.31922072 
		0.63099754 1.0251399 -0.04379712 0.44343281 1.1136371 -0.30457669 0.35370326 1.1149133 
		-0.54332203 0.24874058 1.1196585 -0.73007053 0.12946188 1.1263781 -0.84900033 0.00051648653 
		1.1294317 -0.88842386 -0.12829909 1.1247137 -0.84367788 -0.24728453 1.1126719 -0.71887571 
		-0.35218358 1.0971165 -0.52560329 -0.44158056 1.0815144 -0.28048205 -0.50383329 1.0651937 
		-0.0027358297 -0.51366878 1.0429283 0.2833285 -0.45189375 1.012669 0.52076781 -0.32962739 
		0.98396307 0.69034815 -0.044828892 0.98053145 0.74574733 0.012434303 0.96563894 0.81596851 
		0.059189435 0.99759305 0.72790563 0.35205698 1.0155635 0.66298795 0.46917152 1.0557909 
		0.48757169 0.52377748 1.091307 0.25072888 0.50814885 1.1095222 -0.03144538 0.32058978 
		1.170817 -0.20723619 0.25795963 1.1718668 -0.37129614 0.18065119 1.1722871 -0.49902067 
		0.093805999 1.1726159 -0.57961065 0.0019104787 1.1716071 -0.60581541 -0.089816846 
		1.1679913 -0.57501501 -0.176186 1.1616633 -0.48978484 -0.25269276 1.1535088 -0.35756153 
		-0.31389171 1.1442516 -0.18969174 -0.35081837 1.1334473 -0.00049936044 -0.35201049 
		1.1198591 0.19387332 -0.30919427 1.1031522 0.36201158 -0.22595063 1.0876229 0.48641402 
		-0.032000042 1.0784739 0.56058937 0.012753288 1.0791407 0.58224452 0.056729823 1.0904324 
		0.55148762 0.24823792 1.1102761 0.4700858 0.32745135 1.1338004 0.34160799 0.36553273 
		1.1542625 0.17286013 0.36019316 1.166187 -0.020379256 0.17499639 1.1990547 -0.10663923 
		0.14467099 1.2008067 -0.19105335 0.10366187 1.2011392 -0.2569198 0.055745665 1.2004457 
		-0.29835284 0.0045123408 1.1988473 -0.31164375 -0.046537742 1.1963278 -0.29556689 
		-0.093895823 1.1928908 -0.25150979 -0.13394213 1.1886073 -0.1833791 -0.16285236 1.1835612 
		-0.097301498 -0.17676018 1.1778432 -0.001181272 -0.17248969 1.1716911 0.096127391 
		-0.14867319 1.1656519 0.18437076 -0.10677093 1.1606934 0.25216311 -0.020126894 1.1583004 
		0.29394966 0.011210454 1.1594999 0.30685085 0.043490581 1.1644341 0.29016989 0.12738863 
		1.1722316 0.24521582 0.1672937 1.1811874 0.17531916 0.18877806 1.189279 0.086108714 
		0.19079065 1.1953226 -0.011334536 0.042840052 -0.87901741 -0.33684576 0.0079720188 
		1.1986388 -0.0048419675;
	setAttr -s 633 ".ed";
	setAttr ".ed[0:165]"  1 2 1 2 3 1 3 4 
		1 4 5 1 5 6 1 6 7 1 7 8 
		1 8 9 1 9 10 1 10 11 1 11 12 
		1 12 0 1 14 15 1 15 16 1 16 17 
		1 17 18 1 18 19 1 19 20 1 20 21 
		1 21 22 1 22 23 1 23 24 1 24 25 
		1 25 13 1 27 28 1 28 29 1 29 30 
		1 30 31 1 31 32 1 32 33 1 33 34 
		1 34 35 1 35 36 1 36 37 1 37 38 
		1 38 26 1 40 41 1 41 42 1 42 43 
		1 43 44 1 44 45 1 45 46 1 46 47 
		1 47 48 1 48 49 1 49 50 1 50 51 
		1 51 39 1 53 54 1 54 55 1 55 56 
		1 56 57 1 57 58 1 58 59 1 59 60 
		1 60 61 1 61 62 1 62 63 1 63 64 
		1 64 52 1 66 67 1 67 68 1 68 69 
		1 69 70 1 70 71 1 71 72 1 72 73 
		1 73 74 1 74 75 1 75 76 1 76 77 
		1 77 65 1 79 80 1 80 81 1 81 82 
		1 82 83 1 83 84 1 84 85 1 85 86 
		1 86 87 1 87 88 1 88 89 1 89 90 
		1 90 78 1 91 92 0 92 93 0 94 95 
		0 95 96 0 96 97 1 97 98 1 98 99 
		1 99 100 1 100 101 1 101 102 1 102 103 
		1 103 104 1 104 105 1 105 106 1 106 107 
		1 107 91 1 108 109 1 109 110 1 111 112 
		1 112 113 1 113 114 1 114 115 1 115 116 
		1 116 117 1 117 118 1 118 119 1 119 120 
		1 120 121 1 121 122 1 122 123 1 123 124 
		1 124 108 1 125 126 1 126 127 1 128 129 
		1 129 130 1 130 131 1 131 132 1 132 133 
		1 133 134 1 134 135 1 135 136 1 136 137 
		1 137 138 1 138 139 1 139 140 1 140 141 
		1 141 125 1 142 143 1 143 144 1 145 146 
		1 146 147 1 147 148 1 148 149 1 149 150 
		1 150 151 1 151 152 1 152 153 1 153 154 
		1 154 155 1 155 156 1 156 157 1 157 158 
		1 158 142 1 159 160 1 160 161 1 162 163 
		1 163 164 1 164 165 1 165 166 1 166 167 
		1 167 168 1 168 169 1 169 170 1 170 171 
		1 171 172 1 172 173 1 173 174 1 174 175 
		1 175 159 1 176 177 1 177 178 1;
	setAttr ".ed[166:331]" 178 179 0 179 180 0 180 181 
		0 181 182 0 182 183 1 183 184 1 184 185 
		1 185 186 1 186 187 1 187 188 1 188 189 
		1 189 190 1 190 191 1 191 192 1 192 193 
		1 193 194 1 194 195 1 195 176 1 196 197 
		1 197 198 1 198 199 1 199 200 1 200 201 
		1 201 202 1 202 203 1 203 204 1 204 205 
		1 205 206 1 206 207 1 207 208 1 208 209 
		1 209 210 1 210 211 1 211 212 1 212 213 
		1 213 214 1 214 215 1 215 196 1 216 217 
		1 217 218 1 218 219 1 219 220 1 220 221 
		1 221 222 1 222 223 1 223 224 1 224 225 
		1 225 226 1 226 227 1 227 228 1 228 229 
		1 229 230 1 230 231 1 231 232 1 232 233 
		1 233 234 1 234 235 1 235 216 1 236 237 
		1 237 238 1 238 239 1 239 240 1 240 241 
		1 241 242 1 242 243 1 243 244 1 244 245 
		1 245 246 1 246 247 1 247 248 1 248 249 
		1 249 250 1 250 251 1 251 252 1 252 253 
		1 253 254 1 254 255 1 255 236 1 256 257 
		1 257 258 1 258 259 1 259 260 1 260 261 
		1 261 262 1 262 263 1 263 264 1 264 265 
		1 265 266 1 266 267 1 267 268 1 268 269 
		1 269 270 1 270 271 1 271 272 1 272 273 
		1 273 274 1 274 275 1 275 256 1 276 277 
		1 277 278 1 278 279 1 279 280 1 280 281 
		1 281 282 1 282 283 1 283 284 1 284 285 
		1 285 286 1 286 287 1 287 288 1 288 289 
		1 289 290 1 290 291 1 291 292 1 292 293 
		1 293 294 1 294 295 1 295 276 1 296 297 
		1 297 298 1 298 299 1 299 300 1 300 301 
		1 301 302 1 302 303 1 303 304 1 304 305 
		1 305 306 1 306 307 1 307 308 1 308 309 
		1 309 310 1 310 311 1 311 312 1 312 313 
		1 313 314 1 314 315 1 315 296 1 0 13 
		0 1 14 0 2 15 1 3 16 1 4 17 
		1 5 18 1 6 19 1 7 20 1 8 21 
		1 9 22 1 10 23 1 11 24 1 12 25 
		1 13 26 0 14 27 0 15 28 1 16 29 
		1 17 30 1 18 31 1 19 32 1 20 33 
		1 21 34 1 22 35 1 23 36 1 24 37 
		1 25 38 1 26 39 0 27 40 0;
	setAttr ".ed[332:497]" 28 41 1 29 42 1 30 43 
		1 31 44 1 32 45 1 33 46 1 34 47 
		1 35 48 1 36 49 1 37 50 1 38 51 
		1 39 52 0 40 53 0 41 54 1 42 55 
		1 43 56 1 44 57 1 45 58 1 46 59 
		1 47 60 1 48 61 1 49 62 1 50 63 
		1 51 64 1 52 65 0 53 66 0 54 67 
		1 55 68 1 56 69 1 57 70 1 58 71 
		1 59 72 1 60 73 1 61 74 1 62 75 
		1 63 76 1 64 77 1 65 78 0 66 79 
		0 67 80 1 68 81 1 69 82 1 70 83 
		1 71 84 1 72 85 1 73 86 1 74 87 
		1 75 88 1 76 89 1 77 90 1 78 91 
		0 79 96 0 80 97 1 81 98 1 82 99 
		1 83 100 1 84 101 1 85 102 1 86 103 
		1 87 104 1 88 105 1 89 106 1 90 107 
		1 91 108 1 92 109 1 93 110 0 94 111 
		0 95 112 1 96 113 1 97 114 1 98 115 
		1 99 116 1 100 117 1 101 118 1 102 119 
		1 103 120 1 104 121 1 105 122 1 106 123 
		1 107 124 1 108 125 1 109 126 1 110 127 
		0 111 128 0 112 129 1 113 130 1 114 131 
		1 115 132 1 116 133 1 117 134 1 118 135 
		1 119 136 1 120 137 1 121 138 1 122 139 
		1 123 140 1 124 141 1 125 142 1 126 143 
		1 127 144 0 128 145 0 129 146 1 130 147 
		1 131 148 1 132 149 1 133 150 1 134 151 
		1 135 152 1 136 153 1 137 154 1 138 155 
		1 139 156 1 140 157 1 141 158 1 142 159 
		1 143 160 1 144 161 0 145 162 0 146 163 
		1 147 164 1 148 165 1 149 166 1 150 167 
		1 151 168 1 152 169 1 153 170 1 154 171 
		1 155 172 1 156 173 1 157 174 1 158 175 
		1 159 176 1 160 177 1 161 178 0 162 182 
		0 163 183 1 164 184 1 165 185 1 166 186 
		1 167 187 1 168 188 1 169 189 1 170 190 
		1 171 191 1 172 192 1 173 193 1 174 194 
		1 175 195 1 176 196 1 177 197 1 178 198 
		1 179 199 1 180 200 1 181 201 1 182 202 
		1 183 203 1 184 204 1 185 205 1 186 206 
		1 187 207 1 188 208 1 189 209 1 190 210 
		1 191 211 1 192 212 1 193 213 1;
	setAttr ".ed[498:632]" 194 214 1 195 215 1 196 216 
		1 197 217 1 198 218 1 199 219 1 200 220 
		1 201 221 1 202 222 1 203 223 1 204 224 
		1 205 225 1 206 226 1 207 227 1 208 228 
		1 209 229 1 210 230 1 211 231 1 212 232 
		1 213 233 1 214 234 1 215 235 1 216 236 
		1 217 237 1 218 238 1 219 239 1 220 240 
		1 221 241 1 222 242 1 223 243 1 224 244 
		1 225 245 1 226 246 1 227 247 1 228 248 
		1 229 249 1 230 250 1 231 251 1 232 252 
		1 233 253 1 234 254 1 235 255 1 236 256 
		1 237 257 1 238 258 1 239 259 1 240 260 
		1 241 261 1 242 262 1 243 263 1 244 264 
		1 245 265 1 246 266 1 247 267 1 248 268 
		1 249 269 1 250 270 1 251 271 1 252 272 
		1 253 273 1 254 274 1 255 275 1 256 276 
		1 257 277 1 258 278 1 259 279 1 260 280 
		1 261 281 1 262 282 1 263 283 1 264 284 
		1 265 285 1 266 286 1 267 287 1 268 288 
		1 269 289 1 270 290 1 271 291 1 272 292 
		1 273 293 1 274 294 1 275 295 1 276 296 
		1 277 297 1 278 298 1 279 299 1 280 300 
		1 281 301 1 282 302 1 283 303 1 284 304 
		1 285 305 1 286 306 1 287 307 1 288 308 
		1 289 309 1 290 310 1 291 311 1 292 312 
		1 293 313 1 294 314 1 295 315 1 316 0 
		0 316 1 0 316 2 1 316 3 1 316 4 
		1 316 5 1 316 6 1 316 7 1 316 8 
		1 316 9 1 316 10 1 316 11 1 316 12 
		1 296 317 1 297 317 1 298 317 1 299 317 
		1 300 317 1 301 317 1 302 317 1 303 317 
		1 304 317 1 305 317 1 306 317 1 307 317 
		1 308 317 1 309 317 1 310 317 1 311 317 
		1 312 317 1 313 317 1 314 317 1 315 317 
		1;
	setAttr -s 316 ".fc[0:315]" -type "polyFaces" 
		f 4 0 306 -13 -306 
		mu 0 4 0 1 2 3 
		f 4 1 307 -14 -307 
		mu 0 4 1 4 5 2 
		f 4 2 308 -15 -308 
		mu 0 4 4 6 7 5 
		f 4 3 309 -16 -309 
		mu 0 4 6 8 9 7 
		f 4 4 310 -17 -310 
		mu 0 4 8 10 11 9 
		f 4 5 311 -18 -311 
		mu 0 4 10 12 13 11 
		f 4 6 312 -19 -312 
		mu 0 4 12 14 15 13 
		f 4 7 313 -20 -313 
		mu 0 4 14 16 17 15 
		f 4 8 314 -21 -314 
		mu 0 4 16 18 19 17 
		f 4 9 315 -22 -315 
		mu 0 4 18 20 21 19 
		f 4 10 316 -23 -316 
		mu 0 4 20 22 23 21 
		f 4 11 304 -24 -317 
		mu 0 4 22 24 25 23 
		f 4 12 319 -25 -319 
		mu 0 4 3 2 26 27 
		f 4 13 320 -26 -320 
		mu 0 4 2 5 28 26 
		f 4 14 321 -27 -321 
		mu 0 4 5 7 29 28 
		f 4 15 322 -28 -322 
		mu 0 4 7 9 30 29 
		f 4 16 323 -29 -323 
		mu 0 4 9 11 31 30 
		f 4 17 324 -30 -324 
		mu 0 4 11 13 32 31 
		f 4 18 325 -31 -325 
		mu 0 4 13 15 33 32 
		f 4 19 326 -32 -326 
		mu 0 4 15 17 34 33 
		f 4 20 327 -33 -327 
		mu 0 4 17 19 35 34 
		f 4 21 328 -34 -328 
		mu 0 4 19 21 36 35 
		f 4 22 329 -35 -329 
		mu 0 4 21 23 37 36 
		f 4 23 317 -36 -330 
		mu 0 4 23 25 38 37 
		f 4 24 332 -37 -332 
		mu 0 4 27 26 39 40 
		f 4 25 333 -38 -333 
		mu 0 4 26 28 41 39 
		f 4 26 334 -39 -334 
		mu 0 4 28 29 42 41 
		f 4 27 335 -40 -335 
		mu 0 4 29 30 43 42 
		f 4 28 336 -41 -336 
		mu 0 4 30 31 44 43 
		f 4 29 337 -42 -337 
		mu 0 4 31 32 45 44 
		f 4 30 338 -43 -338 
		mu 0 4 32 33 46 45 
		f 4 31 339 -44 -339 
		mu 0 4 33 34 47 46 
		f 4 32 340 -45 -340 
		mu 0 4 34 35 48 47 
		f 4 33 341 -46 -341 
		mu 0 4 35 36 49 48 
		f 4 34 342 -47 -342 
		mu 0 4 36 37 50 49 
		f 4 35 330 -48 -343 
		mu 0 4 37 38 51 50 
		f 4 36 345 -49 -345 
		mu 0 4 40 39 52 53 
		f 4 37 346 -50 -346 
		mu 0 4 39 41 54 52 
		f 4 38 347 -51 -347 
		mu 0 4 41 42 55 54 
		f 4 39 348 -52 -348 
		mu 0 4 42 43 56 55 
		f 4 40 349 -53 -349 
		mu 0 4 43 44 57 56 
		f 4 41 350 -54 -350 
		mu 0 4 44 45 58 57 
		f 4 42 351 -55 -351 
		mu 0 4 45 46 59 58 
		f 4 43 352 -56 -352 
		mu 0 4 46 47 60 59 
		f 4 44 353 -57 -353 
		mu 0 4 47 48 61 60 
		f 4 45 354 -58 -354 
		mu 0 4 48 49 62 61 
		f 4 46 355 -59 -355 
		mu 0 4 49 50 63 62 
		f 4 47 343 -60 -356 
		mu 0 4 50 51 64 63 
		f 4 48 358 -61 -358 
		mu 0 4 53 52 65 66 
		f 4 49 359 -62 -359 
		mu 0 4 52 54 67 65 
		f 4 50 360 -63 -360 
		mu 0 4 54 55 68 67 
		f 4 51 361 -64 -361 
		mu 0 4 55 56 69 68 
		f 4 52 362 -65 -362 
		mu 0 4 56 57 70 69 
		f 4 53 363 -66 -363 
		mu 0 4 57 58 71 70 
		f 4 54 364 -67 -364 
		mu 0 4 58 59 72 71 
		f 4 55 365 -68 -365 
		mu 0 4 59 60 73 72 
		f 4 56 366 -69 -366 
		mu 0 4 60 61 74 73 
		f 4 57 367 -70 -367 
		mu 0 4 61 62 75 74 
		f 4 58 368 -71 -368 
		mu 0 4 62 63 76 75 
		f 4 59 356 -72 -369 
		mu 0 4 63 64 77 76 
		f 4 60 371 -73 -371 
		mu 0 4 66 65 78 79 
		f 4 61 372 -74 -372 
		mu 0 4 65 67 80 78 
		f 4 62 373 -75 -373 
		mu 0 4 67 68 81 80 
		f 4 63 374 -76 -374 
		mu 0 4 68 69 82 81 
		f 4 64 375 -77 -375 
		mu 0 4 69 70 83 82 
		f 4 65 376 -78 -376 
		mu 0 4 70 71 84 83 
		f 4 66 377 -79 -377 
		mu 0 4 71 72 85 84 
		f 4 67 378 -80 -378 
		mu 0 4 72 73 86 85 
		f 4 68 379 -81 -379 
		mu 0 4 73 74 87 86 
		f 4 69 380 -82 -380 
		mu 0 4 74 75 88 87 
		f 4 70 381 -83 -381 
		mu 0 4 75 76 89 88 
		f 4 71 369 -84 -382 
		mu 0 4 76 77 90 89 
		f 4 72 384 -89 -384 
		mu 0 4 79 78 91 92 
		f 4 73 385 -90 -385 
		mu 0 4 78 80 93 91 
		f 4 74 386 -91 -386 
		mu 0 4 80 81 94 93 
		f 4 75 387 -92 -387 
		mu 0 4 81 82 95 94 
		f 4 76 388 -93 -388 
		mu 0 4 82 83 96 95 
		f 4 77 389 -94 -389 
		mu 0 4 83 84 97 96 
		f 4 78 390 -95 -390 
		mu 0 4 84 85 98 97 
		f 4 79 391 -96 -391 
		mu 0 4 85 86 99 98 
		f 4 80 392 -97 -392 
		mu 0 4 86 87 100 99 
		f 4 81 393 -98 -393 
		mu 0 4 87 88 101 100 
		f 4 82 394 -99 -394 
		mu 0 4 88 89 102 101 
		f 4 83 382 -100 -395 
		mu 0 4 89 90 103 102 
		f 4 84 396 -101 -396 
		mu 0 4 104 105 106 107 
		f 4 85 397 -102 -397 
		mu 0 4 105 108 109 106 
		f 4 86 399 -103 -399 
		mu 0 4 110 111 112 113 
		f 4 87 400 -104 -400 
		mu 0 4 111 92 114 112 
		f 4 88 401 -105 -401 
		mu 0 4 92 91 115 114 
		f 4 89 402 -106 -402 
		mu 0 4 91 93 116 115 
		f 4 90 403 -107 -403 
		mu 0 4 93 94 117 116 
		f 4 91 404 -108 -404 
		mu 0 4 94 95 118 117 
		f 4 92 405 -109 -405 
		mu 0 4 95 96 119 118 
		f 4 93 406 -110 -406 
		mu 0 4 96 97 120 119 
		f 4 94 407 -111 -407 
		mu 0 4 97 98 121 120 
		f 4 95 408 -112 -408 
		mu 0 4 98 99 122 121 
		f 4 96 409 -113 -409 
		mu 0 4 99 100 123 122 
		f 4 97 410 -114 -410 
		mu 0 4 100 101 124 123 
		f 4 98 411 -115 -411 
		mu 0 4 101 102 125 124 
		f 4 99 395 -116 -412 
		mu 0 4 102 103 126 125 
		f 4 100 413 -117 -413 
		mu 0 4 107 106 127 128 
		f 4 101 414 -118 -414 
		mu 0 4 106 109 129 127 
		f 4 102 416 -119 -416 
		mu 0 4 113 112 130 131 
		f 4 103 417 -120 -417 
		mu 0 4 112 114 132 130 
		f 4 104 418 -121 -418 
		mu 0 4 114 115 133 132 
		f 4 105 419 -122 -419 
		mu 0 4 115 116 134 133 
		f 4 106 420 -123 -420 
		mu 0 4 116 117 135 134 
		f 4 107 421 -124 -421 
		mu 0 4 117 118 136 135 
		f 4 108 422 -125 -422 
		mu 0 4 118 119 137 136 
		f 4 109 423 -126 -423 
		mu 0 4 119 120 138 137 
		f 4 110 424 -127 -424 
		mu 0 4 120 121 139 138 
		f 4 111 425 -128 -425 
		mu 0 4 121 122 140 139 
		f 4 112 426 -129 -426 
		mu 0 4 122 123 141 140 
		f 4 113 427 -130 -427 
		mu 0 4 123 124 142 141 
		f 4 114 428 -131 -428 
		mu 0 4 124 125 143 142 
		f 4 115 412 -132 -429 
		mu 0 4 125 126 144 143 
		f 4 116 430 -133 -430 
		mu 0 4 128 127 145 146 
		f 4 117 431 -134 -431 
		mu 0 4 127 129 147 145 
		f 4 118 433 -135 -433 
		mu 0 4 131 130 148 149 
		f 4 119 434 -136 -434 
		mu 0 4 130 132 150 148 
		f 4 120 435 -137 -435 
		mu 0 4 132 133 151 150 
		f 4 121 436 -138 -436 
		mu 0 4 133 134 152 151 
		f 4 122 437 -139 -437 
		mu 0 4 134 135 153 152 
		f 4 123 438 -140 -438 
		mu 0 4 135 136 154 153 
		f 4 124 439 -141 -439 
		mu 0 4 136 137 155 154 
		f 4 125 440 -142 -440 
		mu 0 4 137 138 156 155 
		f 4 126 441 -143 -441 
		mu 0 4 138 139 157 156 
		f 4 127 442 -144 -442 
		mu 0 4 139 140 158 157 
		f 4 128 443 -145 -443 
		mu 0 4 140 141 159 158 
		f 4 129 444 -146 -444 
		mu 0 4 141 142 160 159 
		f 4 130 445 -147 -445 
		mu 0 4 142 143 161 160 
		f 4 131 429 -148 -446 
		mu 0 4 143 144 162 161 
		f 4 132 447 -149 -447 
		mu 0 4 146 145 163 164 
		f 4 133 448 -150 -448 
		mu 0 4 145 147 165 163 
		f 4 134 450 -151 -450 
		mu 0 4 149 148 166 167 
		f 4 135 451 -152 -451 
		mu 0 4 148 150 168 166 
		f 4 136 452 -153 -452 
		mu 0 4 150 151 169 168 
		f 4 137 453 -154 -453 
		mu 0 4 151 152 170 169 
		f 4 138 454 -155 -454 
		mu 0 4 152 153 171 170 
		f 4 139 455 -156 -455 
		mu 0 4 153 154 172 171 
		f 4 140 456 -157 -456 
		mu 0 4 154 155 173 172 
		f 4 141 457 -158 -457 
		mu 0 4 155 156 174 173 
		f 4 142 458 -159 -458 
		mu 0 4 156 157 175 174 
		f 4 143 459 -160 -459 
		mu 0 4 157 158 176 175 
		f 4 144 460 -161 -460 
		mu 0 4 158 159 177 176 
		f 4 145 461 -162 -461 
		mu 0 4 159 160 178 177 
		f 4 146 462 -163 -462 
		mu 0 4 160 161 179 178 
		f 4 147 446 -164 -463 
		mu 0 4 161 162 180 179 
		f 4 148 464 -165 -464 
		mu 0 4 164 163 181 182 
		f 4 149 465 -166 -465 
		mu 0 4 163 165 183 181 
		f 4 150 467 -171 -467 
		mu 0 4 167 166 184 185 
		f 4 151 468 -172 -468 
		mu 0 4 166 168 186 184 
		f 4 152 469 -173 -469 
		mu 0 4 168 169 187 186 
		f 4 153 470 -174 -470 
		mu 0 4 169 170 188 187 
		f 4 154 471 -175 -471 
		mu 0 4 170 171 189 188 
		f 4 155 472 -176 -472 
		mu 0 4 171 172 190 189 
		f 4 156 473 -177 -473 
		mu 0 4 172 173 191 190 
		f 4 157 474 -178 -474 
		mu 0 4 173 174 192 191 
		f 4 158 475 -179 -475 
		mu 0 4 174 175 193 192 
		f 4 159 476 -180 -476 
		mu 0 4 175 176 194 193 
		f 4 160 477 -181 -477 
		mu 0 4 176 177 195 194 
		f 4 161 478 -182 -478 
		mu 0 4 177 178 196 195 
		f 4 162 479 -183 -479 
		mu 0 4 178 179 197 196 
		f 4 163 463 -184 -480 
		mu 0 4 179 180 198 197 
		f 4 164 481 -185 -481 
		mu 0 4 182 181 199 200 
		f 4 165 482 -186 -482 
		mu 0 4 181 183 201 199 
		f 4 166 483 -187 -483 
		mu 0 4 183 202 203 201 
		f 4 167 484 -188 -484 
		mu 0 4 202 204 205 203 
		f 4 168 485 -189 -485 
		mu 0 4 204 206 207 205 
		f 4 169 486 -190 -486 
		mu 0 4 206 185 208 207 
		f 4 170 487 -191 -487 
		mu 0 4 185 184 209 208 
		f 4 171 488 -192 -488 
		mu 0 4 184 186 210 209 
		f 4 172 489 -193 -489 
		mu 0 4 186 187 211 210 
		f 4 173 490 -194 -490 
		mu 0 4 187 188 212 211 
		f 4 174 491 -195 -491 
		mu 0 4 188 189 213 212 
		f 4 175 492 -196 -492 
		mu 0 4 189 190 214 213 
		f 4 176 493 -197 -493 
		mu 0 4 190 191 215 214 
		f 4 177 494 -198 -494 
		mu 0 4 191 192 216 215 
		f 4 178 495 -199 -495 
		mu 0 4 192 193 217 216 
		f 4 179 496 -200 -496 
		mu 0 4 193 194 218 217 
		f 4 180 497 -201 -497 
		mu 0 4 194 195 219 218 
		f 4 181 498 -202 -498 
		mu 0 4 195 196 220 219 
		f 4 182 499 -203 -499 
		mu 0 4 196 197 221 220 
		f 4 183 480 -204 -500 
		mu 0 4 197 198 222 221 
		f 4 184 501 -205 -501 
		mu 0 4 200 199 223 224 
		f 4 185 502 -206 -502 
		mu 0 4 199 201 225 223 
		f 4 186 503 -207 -503 
		mu 0 4 201 203 226 225 
		f 4 187 504 -208 -504 
		mu 0 4 203 205 227 226 
		f 4 188 505 -209 -505 
		mu 0 4 205 207 228 227 
		f 4 189 506 -210 -506 
		mu 0 4 207 208 229 228 
		f 4 190 507 -211 -507 
		mu 0 4 208 209 230 229 
		f 4 191 508 -212 -508 
		mu 0 4 209 210 231 230 
		f 4 192 509 -213 -509 
		mu 0 4 210 211 232 231 
		f 4 193 510 -214 -510 
		mu 0 4 211 212 233 232 
		f 4 194 511 -215 -511 
		mu 0 4 212 213 234 233 
		f 4 195 512 -216 -512 
		mu 0 4 213 214 235 234 
		f 4 196 513 -217 -513 
		mu 0 4 214 215 236 235 
		f 4 197 514 -218 -514 
		mu 0 4 215 216 237 236 
		f 4 198 515 -219 -515 
		mu 0 4 216 217 238 237 
		f 4 199 516 -220 -516 
		mu 0 4 217 218 239 238 
		f 4 200 517 -221 -517 
		mu 0 4 218 219 240 239 
		f 4 201 518 -222 -518 
		mu 0 4 219 220 241 240 
		f 4 202 519 -223 -519 
		mu 0 4 220 221 242 241 
		f 4 203 500 -224 -520 
		mu 0 4 221 222 243 242 
		f 4 204 521 -225 -521 
		mu 0 4 224 223 244 245 
		f 4 205 522 -226 -522 
		mu 0 4 223 225 246 244 
		f 4 206 523 -227 -523 
		mu 0 4 225 226 247 246 
		f 4 207 524 -228 -524 
		mu 0 4 226 227 248 247 
		f 4 208 525 -229 -525 
		mu 0 4 227 228 249 248 
		f 4 209 526 -230 -526 
		mu 0 4 228 229 250 249 
		f 4 210 527 -231 -527 
		mu 0 4 229 230 251 250 
		f 4 211 528 -232 -528 
		mu 0 4 230 231 252 251 
		f 4 212 529 -233 -529 
		mu 0 4 231 232 253 252 
		f 4 213 530 -234 -530 
		mu 0 4 232 233 254 253 
		f 4 214 531 -235 -531 
		mu 0 4 233 234 255 254 
		f 4 215 532 -236 -532 
		mu 0 4 234 235 256 255 
		f 4 216 533 -237 -533 
		mu 0 4 235 236 257 256 
		f 4 217 534 -238 -534 
		mu 0 4 236 237 258 257 
		f 4 218 535 -239 -535 
		mu 0 4 237 238 259 258 
		f 4 219 536 -240 -536 
		mu 0 4 238 239 260 259 
		f 4 220 537 -241 -537 
		mu 0 4 239 240 261 260 
		f 4 221 538 -242 -538 
		mu 0 4 240 241 262 261 
		f 4 222 539 -243 -539 
		mu 0 4 241 242 263 262 
		f 4 223 520 -244 -540 
		mu 0 4 242 243 264 263 
		f 4 224 541 -245 -541 
		mu 0 4 245 244 265 266 
		f 4 225 542 -246 -542 
		mu 0 4 244 246 267 265 
		f 4 226 543 -247 -543 
		mu 0 4 246 247 268 267 
		f 4 227 544 -248 -544 
		mu 0 4 247 248 269 268 
		f 4 228 545 -249 -545 
		mu 0 4 248 249 270 269 
		f 4 229 546 -250 -546 
		mu 0 4 249 250 271 270 
		f 4 230 547 -251 -547 
		mu 0 4 250 251 272 271 
		f 4 231 548 -252 -548 
		mu 0 4 251 252 273 272 
		f 4 232 549 -253 -549 
		mu 0 4 252 253 274 273 
		f 4 233 550 -254 -550 
		mu 0 4 253 254 275 274 
		f 4 234 551 -255 -551 
		mu 0 4 254 255 276 275 
		f 4 235 552 -256 -552 
		mu 0 4 255 256 277 276 
		f 4 236 553 -257 -553 
		mu 0 4 256 257 278 277 
		f 4 237 554 -258 -554 
		mu 0 4 257 258 279 278 
		f 4 238 555 -259 -555 
		mu 0 4 258 259 280 279 
		f 4 239 556 -260 -556 
		mu 0 4 259 260 281 280 
		f 4 240 557 -261 -557 
		mu 0 4 260 261 282 281 
		f 4 241 558 -262 -558 
		mu 0 4 261 262 283 282 
		f 4 242 559 -263 -559 
		mu 0 4 262 263 284 283 
		f 4 243 540 -264 -560 
		mu 0 4 263 264 285 284 
		f 4 244 561 -265 -561 
		mu 0 4 266 265 286 287 
		f 4 245 562 -266 -562 
		mu 0 4 265 267 288 286 
		f 4 246 563 -267 -563 
		mu 0 4 267 268 289 288 
		f 4 247 564 -268 -564 
		mu 0 4 268 269 290 289 
		f 4 248 565 -269 -565 
		mu 0 4 269 270 291 290 
		f 4 249 566 -270 -566 
		mu 0 4 270 271 292 291 
		f 4 250 567 -271 -567 
		mu 0 4 271 272 293 292 
		f 4 251 568 -272 -568 
		mu 0 4 272 273 294 293 
		f 4 252 569 -273 -569 
		mu 0 4 273 274 295 294 
		f 4 253 570 -274 -570 
		mu 0 4 274 275 296 295 
		f 4 254 571 -275 -571 
		mu 0 4 275 276 297 296 
		f 4 255 572 -276 -572 
		mu 0 4 276 277 298 297 
		f 4 256 573 -277 -573 
		mu 0 4 277 278 299 298 
		f 4 257 574 -278 -574 
		mu 0 4 278 279 300 299 
		f 4 258 575 -279 -575 
		mu 0 4 279 280 301 300 
		f 4 259 576 -280 -576 
		mu 0 4 280 281 302 301 
		f 4 260 577 -281 -577 
		mu 0 4 281 282 303 302 
		f 4 261 578 -282 -578 
		mu 0 4 282 283 304 303 
		f 4 262 579 -283 -579 
		mu 0 4 283 284 305 304 
		f 4 263 560 -284 -580 
		mu 0 4 284 285 306 305 
		f 4 264 581 -285 -581 
		mu 0 4 287 286 307 308 
		f 4 265 582 -286 -582 
		mu 0 4 286 288 309 307 
		f 4 266 583 -287 -583 
		mu 0 4 288 289 310 309 
		f 4 267 584 -288 -584 
		mu 0 4 289 290 311 310 
		f 4 268 585 -289 -585 
		mu 0 4 290 291 312 311 
		f 4 269 586 -290 -586 
		mu 0 4 291 292 313 312 
		f 4 270 587 -291 -587 
		mu 0 4 292 293 314 313 
		f 4 271 588 -292 -588 
		mu 0 4 293 294 315 314 
		f 4 272 589 -293 -589 
		mu 0 4 294 295 316 315 
		f 4 273 590 -294 -590 
		mu 0 4 295 296 317 316 
		f 4 274 591 -295 -591 
		mu 0 4 296 297 318 317 
		f 4 275 592 -296 -592 
		mu 0 4 297 298 319 318 
		f 4 276 593 -297 -593 
		mu 0 4 298 299 320 319 
		f 4 277 594 -298 -594 
		mu 0 4 299 300 321 320 
		f 4 278 595 -299 -595 
		mu 0 4 300 301 322 321 
		f 4 279 596 -300 -596 
		mu 0 4 301 302 323 322 
		f 4 280 597 -301 -597 
		mu 0 4 302 303 324 323 
		f 4 281 598 -302 -598 
		mu 0 4 303 304 325 324 
		f 4 282 599 -303 -599 
		mu 0 4 304 305 326 325 
		f 4 283 580 -304 -600 
		mu 0 4 305 306 327 326 
		f 3 -1 -602 602 
		mu 0 3 1 0 328 
		f 3 -2 -603 603 
		mu 0 3 4 1 329 
		f 3 -3 -604 604 
		mu 0 3 6 4 330 
		f 3 -4 -605 605 
		mu 0 3 8 6 331 
		f 3 -5 -606 606 
		mu 0 3 10 8 332 
		f 3 -6 -607 607 
		mu 0 3 12 10 333 
		f 3 -7 -608 608 
		mu 0 3 14 12 334 
		f 3 -8 -609 609 
		mu 0 3 16 14 335 
		f 3 -9 -610 610 
		mu 0 3 18 16 336 
		f 3 -10 -611 611 
		mu 0 3 20 18 337 
		f 3 -11 -612 612 
		mu 0 3 22 20 338 
		f 3 -12 -613 600 
		mu 0 3 24 22 339 
		f 3 284 614 -614 
		mu 0 3 308 307 340 
		f 3 285 615 -615 
		mu 0 3 307 309 341 
		f 3 286 616 -616 
		mu 0 3 309 310 342 
		f 3 287 617 -617 
		mu 0 3 310 311 343 
		f 3 288 618 -618 
		mu 0 3 311 312 344 
		f 3 289 619 -619 
		mu 0 3 312 313 345 
		f 3 290 620 -620 
		mu 0 3 313 314 346 
		f 3 291 621 -621 
		mu 0 3 314 315 347 
		f 3 292 622 -622 
		mu 0 3 315 316 348 
		f 3 293 623 -623 
		mu 0 3 316 317 349 
		f 3 294 624 -624 
		mu 0 3 317 318 350 
		f 3 295 625 -625 
		mu 0 3 318 319 351 
		f 3 296 626 -626 
		mu 0 3 319 320 352 
		f 3 297 627 -627 
		mu 0 3 320 321 353 
		f 3 298 628 -628 
		mu 0 3 321 322 354 
		f 3 299 629 -629 
		mu 0 3 322 323 355 
		f 3 300 630 -630 
		mu 0 3 323 324 356 
		f 3 301 631 -631 
		mu 0 3 324 325 357 
		f 3 302 632 -632 
		mu 0 3 325 326 358 
		f 3 303 613 -633 
		mu 0 3 326 327 359 ;
	setAttr ".cd" -type "dataPolyComponent" Index_Data Edge 0 ;
	setAttr ".cvd" -type "dataPolyComponent" Index_Data Vertex 0 ;
createNode transform -n "i_pCube1";
	setAttr ".t" -type "double3" 0.9 0.50886943409412877 0.95997608177569616 ;
	setAttr ".r" -type "double3" -7 0 0 ;
createNode transform -n "transform14" -p "i_pCube1";
createNode mesh -n "i_pCubeShape1" -p "transform14";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "i_pCube2";
	setAttr ".t" -type "double3" -0.9 0.50886943409412877 0.95997608177569616 ;
	setAttr ".r" -type "double3" -7 0 0 ;
createNode transform -n "transform16" -p "i_pCube2";
createNode mesh -n "i_pCubeShape2" -p "transform16";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr ".iog[0].og[0].gcl" -type "componentList" 1 "f[0:5]";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr -s 14 ".uvst[0].uvsp[0:13]" -type "float2" 0.375 0 0.625 0 
		0.375 0.25 0.625 0.25 0.375 0.5 0.625 0.5 0.375 0.75 0.625 0.75 0.375 1 0.625 1 0.875 
		0 0.875 0.25 0.125 0 0.125 0.25;
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 8 ".vt[0:7]"  -0.85000002 -0.85000002 0.85000002 0.85000002 
		-0.85000002 0.85000002 -0.85000002 0.85000002 0.85000002 0.85000002 0.85000002 0.85000002 
		-0.85000002 0.85000002 -0.85000002 0.85000002 0.85000002 -0.85000002 -0.85000002 
		-0.85000002 -0.85000002 0.85000002 -0.85000002 -0.85000002;
	setAttr -s 12 ".ed[0:11]"  0 1 0 2 3 0 
		4 5 0 6 7 0 0 2 0 1 3 0 
		2 4 0 3 5 0 4 6 0 5 7 0 
		6 0 0 7 1 0;
	setAttr -s 6 ".fc[0:5]" -type "polyFaces" 
		f 4 0 5 -2 -5 
		mu 0 4 0 1 3 2 
		f 4 1 7 -3 -7 
		mu 0 4 2 3 5 4 
		f 4 2 9 -4 -9 
		mu 0 4 4 5 7 6 
		f 4 3 11 -1 -11 
		mu 0 4 6 7 9 8 
		f 4 -12 -10 -8 -6 
		mu 0 4 1 10 11 3 
		f 4 10 4 6 8 
		mu 0 4 12 0 2 13 ;
	setAttr ".cd" -type "dataPolyComponent" Index_Data Edge 0 ;
	setAttr ".cvd" -type "dataPolyComponent" Index_Data Vertex 0 ;
createNode transform -n "i_pCube3";
	setAttr ".t" -type "double3" -0.9 0.50886943409412877 0.95997608177569616 ;
	setAttr ".r" -type "double3" -7 0 0 ;
createNode transform -n "transform20" -p "i_pCube3";
createNode mesh -n "i_pCubeShape3" -p "transform20";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr ".iog[0].og[0].gcl" -type "componentList" 1 "f[0:5]";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr -s 14 ".uvst[0].uvsp[0:13]" -type "float2" 0.375 0 0.625 0 
		0.375 0.25 0.625 0.25 0.375 0.5 0.625 0.5 0.375 0.75 0.625 0.75 0.375 1 0.625 1 0.875 
		0 0.875 0.25 0.125 0 0.125 0.25;
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 8 ".vt[0:7]"  -0.85000002 -0.85000002 0.85000002 0.85000002 
		-0.85000002 0.85000002 -0.85000002 0.85000002 0.85000002 0.85000002 0.85000002 0.85000002 
		-0.85000002 0.85000002 -0.85000002 0.85000002 0.85000002 -0.85000002 -0.85000002 
		-0.85000002 -0.85000002 0.85000002 -0.85000002 -0.85000002;
	setAttr -s 12 ".ed[0:11]"  0 1 0 2 3 0 
		4 5 0 6 7 0 0 2 0 1 3 0 
		2 4 0 3 5 0 4 6 0 5 7 0 
		6 0 0 7 1 0;
	setAttr -s 6 ".fc[0:5]" -type "polyFaces" 
		f 4 0 5 -2 -5 
		mu 0 4 0 1 3 2 
		f 4 1 7 -3 -7 
		mu 0 4 2 3 5 4 
		f 4 2 9 -4 -9 
		mu 0 4 4 5 7 6 
		f 4 3 11 -1 -11 
		mu 0 4 6 7 9 8 
		f 4 -12 -10 -8 -6 
		mu 0 4 1 10 11 3 
		f 4 10 4 6 8 
		mu 0 4 12 0 2 13 ;
	setAttr ".cd" -type "dataPolyComponent" Index_Data Edge 0 ;
	setAttr ".cvd" -type "dataPolyComponent" Index_Data Vertex 0 ;
createNode transform -n "i_polySurface3";
createNode transform -n "transform19" -p "i_polySurface3";
createNode mesh -n "i_polySurfaceShape3" -p "transform19";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "i_polySurface4";
createNode transform -n "transform18" -p "i_polySurface4";
createNode mesh -n "i_polySurfaceShape4" -p "transform18";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "i_pCube4";
	setAttr ".t" -type "double3" 0.9 0.50886943409412877 0.95997608177569616 ;
	setAttr ".r" -type "double3" -7 0 0 ;
createNode transform -n "transform22" -p "i_pCube4";
createNode mesh -n "i_pCubeShape4" -p "transform22";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr ".iog[0].og[0].gcl" -type "componentList" 1 "f[0:5]";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr -s 14 ".uvst[0].uvsp[0:13]" -type "float2" 0.375 0 0.625 0 
		0.375 0.25 0.625 0.25 0.375 0.5 0.625 0.5 0.375 0.75 0.625 0.75 0.375 1 0.625 1 0.875 
		0 0.875 0.25 0.125 0 0.125 0.25;
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 8 ".vt[0:7]"  -0.85000002 -0.85000002 0.85000002 0.85000002 
		-0.85000002 0.85000002 -0.85000002 0.85000002 0.85000002 0.85000002 0.85000002 0.85000002 
		-0.85000002 0.85000002 -0.85000002 0.85000002 0.85000002 -0.85000002 -0.85000002 
		-0.85000002 -0.85000002 0.85000002 -0.85000002 -0.85000002;
	setAttr -s 12 ".ed[0:11]"  0 1 0 2 3 0 
		4 5 0 6 7 0 0 2 0 1 3 0 
		2 4 0 3 5 0 4 6 0 5 7 0 
		6 0 0 7 1 0;
	setAttr -s 6 ".fc[0:5]" -type "polyFaces" 
		f 4 0 5 -2 -5 
		mu 0 4 0 1 3 2 
		f 4 1 7 -3 -7 
		mu 0 4 2 3 5 4 
		f 4 2 9 -4 -9 
		mu 0 4 4 5 7 6 
		f 4 3 11 -1 -11 
		mu 0 4 6 7 9 8 
		f 4 -12 -10 -8 -6 
		mu 0 4 1 10 11 3 
		f 4 10 4 6 8 
		mu 0 4 12 0 2 13 ;
	setAttr ".cd" -type "dataPolyComponent" Index_Data Edge 0 ;
	setAttr ".cvd" -type "dataPolyComponent" Index_Data Vertex 0 ;
createNode transform -n "polySurface3";
createNode transform -n "transform23" -p "polySurface3";
createNode mesh -n "polySurfaceShape3" -p "transform23";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "polySurface4";
createNode transform -n "transform24" -p "polySurface4";
createNode mesh -n "polySurfaceShape4" -p "transform24";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".pt[2]" -type "float3"  -0.0057735443 3.194809e-005 -3.9339066e-006;
createNode transform -n "polySurface5";
createNode transform -n "transform25" -p "polySurface5";
createNode mesh -n "polySurfaceShape5" -p "transform25";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".pt[2]" -type "float3"  -0.0057735443 3.194809e-005 -3.9339066e-006;
createNode transform -n "i_polySurface6";
createNode transform -n "transform27" -p "i_polySurface6";
createNode mesh -n "i_polySurfaceShape6" -p "transform27";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".pt[1]" -type "float3"  -0.014539033 0.0009047389 0.0073683262;
createNode transform -n "i_polySurface7";
createNode transform -n "transform26" -p "i_polySurface7";
createNode mesh -n "i_polySurfaceShape7" -p "transform26";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "i_m_joystick";
	setAttr ".t" -type "double3" 0.015393762144034762 0.16455848497197506 1.0979699642474623 ;
createNode transform -n "transform28" -p "i_m_joystick";
createNode mesh -n "i_m_joystickShape" -p "transform28";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 26 ".pt[0:25]" -type "float3"  -0.012135806 0.050119452 
		0.019897196 0.012135795 0.050119452 0.019897202 0.024271602 0.050119452 4.2278416e-009 
		0.012135803 0.050119452 -0.019897196 -0.012135799 0.050119452 -0.019897195 -0.024271602 
		0.050119452 8.0425488e-010 -0.012711072 0.22310707 0.013641819 0.011560883 0.22319663 
		0.01347049 0.023631698 0.24542056 -0.0056965896 0.011430553 0.26755509 -0.024692347 
		-0.012841403 0.26746556 -0.024520982 -0.024912212 0.24524145 -0.0053539062 -0.012012896 
		-0.10577255 -0.37749332 0.012259057 -0.10568298 -0.37766883 0.024329865 -0.0834589 
		-0.39616391 0.012128727 -0.061324403 -0.41448331 -0.012143225 -0.061414003 -0.41430774 
		-0.024214037 -0.083638057 -0.39581278 -0.011430556 -0.19019333 -0.44296649 0.012841401 
		-0.18999593 -0.44310606 0.024912212 -0.18376942 -0.46235308 0.01271107 -0.17774007 
		-0.48146087 -0.011560885 -0.17793734 -0.48132139 -0.023631698 -0.18416373 -0.46207428 
		0 0.050119452 8.0425488e-010 0.00064025796 -0.18396662 -0.46221384;
createNode transform -n "i_pCylinder1";
	setAttr ".t" -type "double3" 0 -0.11995608096709082 -2.1447179656692974 ;
	setAttr ".r" -type "double3" 90 0 0 ;
createNode transform -n "transform34" -p "i_pCylinder1";
createNode mesh -n "pCylinderShape1" -p "transform34";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog[0].og";
	setAttr -av ".iog[0].og[1].gco";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "i_m_engine2";
	setAttr ".s" -type "double3" 1 1.0000000000000002 1.0000000000000002 ;
	setAttr ".rp" -type "double3" 0 9.8607613152626476e-032 -2.4651903288156619e-032 ;
	setAttr ".spt" -type "double3" 0 9.8607613152626476e-032 -2.4651903288156619e-032 ;
createNode transform -n "transform33" -p "i_m_engine2";
createNode mesh -n "m_engineShape" -p "transform33";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	addAttr -ci true -sn "rgn" -ln "rgn" -dt "vectorArray";
	setAttr -k off ".v";
	setAttr ".io" yes;
	setAttr -s 2 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 2 ".pt[42:43]" -type "float3"  -0.0062224865 0.0074267387 
		-0.019090176 0.0086095333 -0.0053534508 -0.030076981;
	setAttr ".rgvtx" -type "vectorArray" 44 -0.86736792325973511 -0.13558542728424072
		 -0.80143284797668457 0.8524964451789856 -0.13430112600326538 -0.81855380535125732 -0.50541824102401733
		 0.021262556314468384 -1.7968523502349854 0.50541830062866211 0.015919357538223267
		 -1.8022804260253906 -0.46933048963546753 0.17665152251720428 -2.0473372936248779 0.46933060884475708
		 0.17665155231952667 -2.0473372936248779 -0.41788533329963684 -0.51973211765289307
		 -2.0468640327453613 0.41851180791854858 -0.51981550455093384 -2.0473372936248779 0.04284006729722023
		 -0.87901735305786133 -0.3368457555770874 -0.47350603342056274 0.77376639842987061
		 -1.5116639137268066 0.47350606322288513 0.76298028230667114 -1.5166623592376709 -0.47499999403953552
		 0.86646705865859985 -1.9348816871643066 0.47499999403953552 0.86646705865859985 -1.9348816871643066 0.02247011661529541
		 -0.51981550455093384 -2.0473372936248779 0.82635438442230225 -0.28783684968948364
		 -0.81339478492736816 0.78509527444839478 -0.43294423818588257 -0.79193025827407837 0.72575485706329346
		 -0.56610798835754395 -0.75332629680633545 0.64415299892425537 -0.68254607915878296
		 -0.69799208641052246 0.53602266311645508 -0.77379459142684937 -0.62698650360107422 0.39864432811737061
		 -0.83601903915405273 -0.54180443286895752 0.23255860805511475 -0.86930930614471436
		 -0.44436383247375488 -0.83976471424102783 -0.29123717546463013 -0.79830735921859741 -0.79234397411346436
		 -0.43804389238357544 -0.77847874164581299 -0.72148597240447998 -0.57229191064834595
		 -0.74192863702774048 -0.62359631061553955 -0.69027388095855713 -0.68892008066177368 -0.49622052907943726
		 -0.78286492824554443 -0.62037992477416992 -0.33945003151893616 -0.84398579597473145
		 -0.53764498233795166 -0.15708194673061371 -0.87397408485412598 -0.44247400760650635 -0.69574379920959473
		 -0.053121630102396011 -1.3546848297119141 0.6956486701965332 -0.057644307613372803
		 -1.364715576171875 0 0.86646705865859985 -1.9348816871643066 -4.4703483581542969e-008
		 0.83686864376068115 -1.873746395111084 -3.7670135498046875e-005 0.8664669394493103
		 -1.9348816871643066 -0.24893683195114136 0.82117938995361328 -1.7802443504333496 0
		 0.86646705865859985 -1.9348816871643066 0.24893680214881897 0.81508719921112061 -1.7823469638824463 0.52488565444946289
		 0.18098610639572144 -1.8281493186950684 0.53142833709716797 0.34197220206260681 -1.8116981983184814 0.52488565444946289
		 0.49478352069854736 -1.7531881332397461 -0.52488547563552856 0.18811726570129395
		 -1.8228633403778076 -0.53142833709716797 0.35068681836128235 -1.806532621383667 -0.52488547563552856
		 0.50473427772521973 -1.7481026649475098 -0.50541824102401733 0.64667707681655884
		 -1.6489734649658203 0.50541830062866211 0.63598883152008057 -1.6540131568908691 ;
	setAttr ".rgf" -type "string" "[[4,5,7,13,6],[6,21,0],[1,7,29],[6,28,2,4],[35,10,12,34],[11,32,30,34,12,5,4],[3,5,36],[11,42,9],[13,7,20,8],[14,7,1],[15,7,14],[16,7,15],[17,7,16],[18,7,17],[19,7,18],[20,7,19],[6,22,21],[6,23,22],[6,24,23],[6,25,24],[6,26,25],[6,27,26],[8,27,6,13],[6,0,28],[29,7,5,3],[33,31,30,32],[9,33,32,11],[31,35,34,30],[36,5,37],[37,5,38],[38,5,12,43],[4,2,39],[4,39,40],[4,40,41],[4,41,42,11],[43,12,10]]";
	setAttr ".rgn" -type "vectorArray" 124 -0.0002665319771040231 -0.00018827422172762454
		 -0.99999994039535522 -0.0002665319771040231 -0.00018827422172762454 -0.99999994039535522 -0.0002665319771040231
		 -0.00018827422172762454 -0.99999994039535522 -0.0002665319771040231 -0.00018827422172762454
		 -0.99999994039535522 -0.0002665319771040231 -0.00018827422172762454 -0.99999994039535522 -0.94227933883666992
		 -0.17286175489425659 -0.28675535321235657 -0.94227933883666992 -0.17286175489425659
		 -0.28675535321235657 -0.94227933883666992 -0.17286175489425659 -0.28675535321235657 0.94671672582626343
		 -0.13760165870189667 -0.29119297862052917 0.94671672582626343 -0.13760165870189667
		 -0.29119297862052917 0.94671672582626343 -0.13760165870189667 -0.29119297862052917 -0.94960075616836548
		 -0.0088417986407876015 -0.3133372962474823 -0.94960075616836548 -0.0088417986407876015
		 -0.3133372962474823 -0.94960075616836548 -0.0088417986407876015 -0.3133372962474823 -0.94960075616836548
		 -0.0088417986407876015 -0.3133372962474823 -0.032857757061719894 0.96089106798171997
		 0.27497014403343201 -0.032857757061719894 0.96089106798171997 0.27497014403343201 -0.032857757061719894
		 0.96089106798171997 0.27497014403343201 -0.032857757061719894 0.96089106798171997
		 0.27497014403343201 -1.8061655282508582e-007 0.16089871525764465 -0.98697096109390259 -1.8061655282508582e-007
		 0.16089871525764465 -0.98697096109390259 -1.8061655282508582e-007 0.16089871525764465
		 -0.98697096109390259 -1.8061655282508582e-007 0.16089871525764465 -0.98697096109390259 -1.8061655282508582e-007
		 0.16089871525764465 -0.98697096109390259 -1.8061655282508582e-007 0.16089871525764465
		 -0.98697096109390259 -1.8061655282508582e-007 0.16089871525764465 -0.98697096109390259 0.95898032188415527
		 -0.15072374045848846 -0.24008128046989441 0.95898032188415527 -0.15072374045848846
		 -0.24008128046989441 0.95898032188415527 -0.15072374045848846 -0.24008128046989441 -0.97953289747238159
		 0.19587208330631256 0.046361066401004791 -0.97953289747238159 0.19587208330631256
		 0.046361066401004791 -0.97953289747238159 0.19587208330631256 0.046361066401004791 -0.022378390654921532
		 -0.97740864753723145 -0.2101699560880661 -0.022378390654921532 -0.97740864753723145
		 -0.2101699560880661 -0.022378390654921532 -0.97740864753723145 -0.2101699560880661 -0.022378390654921532
		 -0.97740864753723145 -0.2101699560880661 0.94470983743667603 -0.17026939988136292
		 -0.28023523092269897 0.94470983743667603 -0.17026939988136292 -0.28023523092269897 0.94470983743667603
		 -0.17026939988136292 -0.28023523092269897 0.92144215106964111 -0.29874023795127869
		 -0.24839222431182861 0.92144215106964111 -0.29874023795127869 -0.24839222431182861 0.92144215106964111
		 -0.29874023795127869 -0.24839222431182861 0.86522877216339111 -0.44978383183479309
		 -0.22152596712112427 0.86522877216339111 -0.44978383183479309 -0.22152596712112427 0.86522877216339111
		 -0.44978383183479309 -0.22152596712112427 0.75454157590866089 -0.62455254793167114
		 -0.20149746537208557 0.75454157590866089 -0.62455254793167114 -0.20149746537208557 0.75454157590866089
		 -0.62455254793167114 -0.20149746537208557 0.55697482824325562 -0.80835670232772827
		 -0.19062639772891998 0.55697482824325562 -0.80835670232772827 -0.19062639772891998 0.55697482824325562
		 -0.80835670232772827 -0.19062639772891998 0.30365279316902161 -0.93323707580566406
		 -0.19199852645397186 0.30365279316902161 -0.93323707580566406 -0.19199852645397186 0.30365279316902161
		 -0.93323707580566406 -0.19199852645397186 0.07595592737197876 -0.97602206468582153
		 -0.20398923754692078 0.07595592737197876 -0.97602206468582153 -0.20398923754692078 0.07595592737197876
		 -0.97602206468582153 -0.20398923754692078 -0.91156482696533203 -0.32794448733329773
		 -0.24799573421478271 -0.91156482696533203 -0.32794448733329773 -0.24799573421478271 -0.91156482696533203
		 -0.32794448733329773 -0.24799573421478271 -0.83824652433395386 -0.50102925300598145
		 -0.21520322561264038 -0.83824652433395386 -0.50102925300598145 -0.21520322561264038 -0.83824652433395386
		 -0.50102925300598145 -0.21520322561264038 -0.71090549230575562 -0.67638993263244629
		 -0.19263964891433716 -0.71090549230575562 -0.67638993263244629 -0.19263964891433716 -0.71090549230575562
		 -0.67638993263244629 -0.19263964891433716 -0.51175010204315186 -0.83942997455596924
		 -0.1829461008310318 -0.51175010204315186 -0.83942997455596924 -0.1829461008310318 -0.51175010204315186
		 -0.83942997455596924 -0.1829461008310318 -0.26852375268936157 -0.9445534348487854
		 -0.18898054957389832 -0.26852375268936157 -0.9445534348487854 -0.18898054957389832 -0.26852375268936157
		 -0.9445534348487854 -0.18898054957389832 -0.052532944828271866 -0.97689670324325562
		 -0.20715469121932983 -0.052532944828271866 -0.97689670324325562 -0.20715469121932983 -0.052532944828271866
		 -0.97689670324325562 -0.20715469121932983 0.027082562446594238 -0.97678035497665405
		 -0.21252427995204926 0.027082562446594238 -0.97678035497665405 -0.21252427995204926 0.027082562446594238
		 -0.97678035497665405 -0.21252427995204926 0.027082562446594238 -0.97678035497665405
		 -0.21252427995204926 -0.94532114267349243 -0.10475132614374161 -0.30886110663414001 -0.94532114267349243
		 -0.10475132614374161 -0.30886110663414001 -0.94532114267349243 -0.10475132614374161
		 -0.30886110663414001 0.94826489686965942 -0.0086583495140075684 -0.31736192107200623 0.94826489686965942
		 -0.0086583495140075684 -0.31736192107200623 0.94826489686965942 -0.0086583495140075684
		 -0.31736192107200623 0.94826489686965942 -0.0086583495140075684 -0.31736192107200623 0.1063036248087883
		 0.89498835802078247 0.43323838710784912 0.1063036248087883 0.89498835802078247 0.43323838710784912 0.1063036248087883
		 0.89498835802078247 0.43323838710784912 0.1063036248087883 0.89498835802078247 0.43323838710784912 0.027736246585845947
		 0.96953111886978149 0.24339291453361511 0.027736246585845947 0.96953111886978149
		 0.24339291453361511 0.027736246585845947 0.96953111886978149 0.24339291453361511 0.027736246585845947
		 0.96953111886978149 0.24339291453361511 -0.08097347617149353 0.89710599184036255
		 0.43433189392089844 -0.08097347617149353 0.89710599184036255 0.43433189392089844 -0.08097347617149353
		 0.89710599184036255 0.43433189392089844 -0.08097347617149353 0.89710599184036255
		 0.43433189392089844 0.96931654214859009 -0.014317036606371403 -0.24539844691753387 0.96931654214859009
		 -0.014317036606371403 -0.24539844691753387 0.96931654214859009 -0.014317036606371403
		 -0.24539844691753387 0.91297316551208496 0.17940434813499451 -0.36646172404289246 0.91297316551208496
		 0.17940434813499451 -0.36646172404289246 0.91297316551208496 0.17940434813499451
		 -0.36646172404289246 0.98616307973861694 0.056100565940141678 -0.15599730610847473 0.98616307973861694
		 0.056100565940141678 -0.15599730610847473 0.98616307973861694 0.056100565940141678
		 -0.15599730610847473 0.98616307973861694 0.056100565940141678 -0.15599730610847473 -0.96174174547195435
		 -0.14813366532325745 -0.23045477271080017 -0.96174174547195435 -0.14813366532325745
		 -0.23045477271080017 -0.96174174547195435 -0.14813366532325745 -0.23045477271080017 -0.97078227996826172
		 -0.015013501979410648 -0.23949185013771057 -0.97078227996826172 -0.015013501979410648
		 -0.23949185013771057 -0.97078227996826172 -0.015013501979410648 -0.23949185013771057 -0.91462594270706177
		 0.17676447331905365 -0.36361193656921387 -0.91462594270706177 0.17676447331905365
		 -0.36361193656921387 -0.91462594270706177 0.17676447331905365 -0.36361193656921387 -0.98629492521286011
		 0.055854391306638718 -0.15525028109550476 -0.98629492521286011 0.055854391306638718
		 -0.15525028109550476 -0.98629492521286011 0.055854391306638718 -0.15525028109550476 -0.98629492521286011
		 0.055854391306638718 -0.15525028109550476 0.98020815849304199 0.1913284957408905
		 0.050844904035329819 0.98020815849304199 0.1913284957408905 0.050844904035329819 0.98020815849304199
		 0.1913284957408905 0.050844904035329819 ;
parent -s -nc -r -add "|m_hangar|m_rack|phys_rack1|phys_rackShape1" "phys_rack2";
parent -s -nc -r -add "|m_hangar|m_rack|phys_rack1|phys_rackShape1" "|i_m_rack4|transform8";
parent -s -nc -r -add "|m_hangar|m_rack|phys_rack1|phys_rackShape1" "|i_m_rack3|transform9";
parent -s -nc -r -add "|m_hangar|m_rack|phys_rackPillar1|phys_rackPillar1Shape" "phys_rackPillar2";
parent -s -nc -r -add "|m_hangar|m_rack|phys_rackPillar1|phys_rackPillar1Shape" "phys_rackPillar3";
parent -s -nc -r -add "|m_hangar|m_rack|phys_rackPillar1|phys_rackPillar1Shape" "phys_rackPillar4";
parent -s -nc -r -add "|m_hangar|m_rack|phys_rackPillar1|phys_rackPillar1Shape" "phys_rackPillar5";
parent -s -nc -r -add "|m_hangar|m_rack|phys_rackPillar1|phys_rackPillar1Shape" "phys_rackPillar6";
parent -s -nc -r -add "|m_hangar|m_rack|phys_rackPillar1|phys_rackPillar1Shape" "|i_m_rackPillar12|transform2";
parent -s -nc -r -add "|m_hangar|m_rack|phys_rackPillar1|phys_rackPillar1Shape" "|i_m_rackPillar11|transform3";
parent -s -nc -r -add "|m_hangar|m_rack|phys_rackPillar1|phys_rackPillar1Shape" "|i_m_rackPillar10|transform4";
parent -s -nc -r -add "|m_hangar|m_rack|phys_rackPillar1|phys_rackPillar1Shape" "|i_m_rackPillar9|transform5";
parent -s -nc -r -add "|m_hangar|m_rack|phys_rackPillar1|phys_rackPillar1Shape" "|i_m_rackPillar8|transform6";
parent -s -nc -r -add "|m_hangar|m_rack|phys_rackPillar1|phys_rackPillar1Shape" "|i_m_rackPillar7|transform7";
parent -s -nc -r -add "|m_hangar|phys_wall1|phys_wallShape1" "phys_wall2";
parent -s -nc -r -add "|m_hangar|m_startingpad|phys_startingpad|m_landingpadShape" "phys_landingpad";
parent -s -nc -r -add "|m_hangar|m_startingpad|phys_startingpad|m_landingpadShape" "m_startingpad";
parent -s -nc -r -add "|m_hangar|m_startingpad|phys_startingpad|m_landingpadShape" "m_landingpad";
parent -s -nc -r -add "|m_hangar|phys_pos_path_start|phys_pos_path_Shape1" "phys_pos_path_1";
parent -s -nc -r -add "|m_hangar|phys_pos_path_start|phys_pos_path_Shape1" "phys_pos_path_land";
parent -s -nc -r -add "|m_hangar|phys_pos_path_start|phys_pos_path_Shape1" "phys_pos_path_2";
parent -s -nc -r -add "|m_hangar|phys_pos_path_start|phys_pos_path_Shape1" "phys_pos_path_3";
parent -s -nc -r -add "|m_hangar|phys_pos_path_start|phys_pos_path_Shape1" "phys_pos_path_4";
parent -s -nc -r -add "|i_m_landingbar0|transform12|i_m_landingbarShape0" "transform11";
parent -s -nc -r -add "|i_m_foot0|transform10|i_m_footShape0" "|i_m_foot3|transform7";
parent -s -nc -r -add "|i_m_foot0|transform10|i_m_footShape0" "|i_m_foot1|transform8";
parent -s -nc -r -add "|i_m_foot0|transform10|i_m_footShape0" "|i_m_foot2|transform9";
createNode lightLinker -n "lightLinker1";
	setAttr -s 19 ".lnk";
	setAttr -s 19 ".slnk";
createNode displayLayerManager -n "layerManager";
createNode displayLayer -n "defaultLayer";
createNode renderLayerManager -n "renderLayerManager";
createNode renderLayer -n "defaultRenderLayer";
	setAttr ".g" yes;
createNode polyCube -n "polyCube1";
	setAttr ".uvs" -type "string" "map_hangar";
	setAttr ".w" 50;
	setAttr ".h" 0.5;
	setAttr ".d" 50;
	setAttr ".cuv" 4;
createNode polySplit -n "polySplit1";
	setAttr -s 2 ".e[0:1]"  0.92917764 0.92767543;
	setAttr -s 2 ".d[0:1]"  -2147483641 -2147483642;
createNode polySplit -n "polySplit2";
	setAttr -s 2 ".e[0:1]"  0.93432295 0.086878814;
	setAttr -s 2 ".d[0:1]"  -2147483634 -2147483647;
createNode polySplit -n "polySplit3";
	setAttr -s 2 ".e[0:1]"  0.12380897 0.89100695;
	setAttr -s 2 ".d[0:1]"  -2147483634 -2147483632;
createNode polyExtrudeFace -n "polyExtrudeFace1";
	setAttr ".ics" -type "componentList" 2 "f[1]" "f[6:7]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".ws" yes;
	setAttr ".pvt" -type "float3" 0.35295376 1.0325832 5.4317589 ;
	setAttr ".rs" 45395;
	setAttr ".lt" -type "double3" 0 -1.2896286844418445e-015 18.192028737301406 ;
	setAttr ".c[0]"  0 1 1;
createNode polyTweak -n "polyTweak1";
	setAttr ".uopa" yes;
	setAttr -s 6 ".tk[8:13]" -type "float3"  0 0 -2.2073526 0 0 -2.2073526 
		-1.8370619 0 -2.2073526 -2.8971508 0 0 4.4636126 0 -2.2073526 3.6559286 0 0;
createNode polyExtrudeFace -n "polyExtrudeFace2";
	setAttr ".ics" -type "componentList" 1 "f[6]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".ws" yes;
	setAttr ".pvt" -type "float3" 0.35295376 19.224613 -18.863802 ;
	setAttr ".rs" 33107;
	setAttr ".lt" -type "double3" 0 5.1366850895611846e-015 1.3445134323990235 ;
	setAttr ".c[0]"  0 1 1;
createNode polyExtrudeFace -n "polyExtrudeFace3";
	setAttr ".ics" -type "componentList" 1 "f[19:21]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".ws" yes;
	setAttr ".pvt" -type "float3" 0.35295472 19.89687 -18.196918 ;
	setAttr ".rs" 59595;
	setAttr ".lt" -type "double3" 1.4155343563970746e-015 0 48.590839257918589 ;
	setAttr ".c[0]"  0 1 1;
createNode deleteComponent -n "deleteComponent1";
	setAttr ".dc" -type "componentList" 1 "f[7]";
createNode deleteComponent -n "deleteComponent2";
	setAttr ".dc" -type "componentList" 1 "f[24]";
createNode deleteComponent -n "deleteComponent3";
	setAttr ".dc" -type "componentList" 1 "f[28]";
createNode deleteComponent -n "deleteComponent4";
	setAttr ".dc" -type "componentList" 1 "f[1]";
createNode polyTweak -n "polyTweak2";
	setAttr ".uopa" yes;
	setAttr -s 15 ".tk";
	setAttr ".tk[16:22]" -type "float3" 0 5.9604645e-008 0  0 5.9604645e-008 
		0  0 5.9604645e-008 0  0 5.9604645e-008 0  0 5.9604645e-008 0  0 5.9604645e-008 0  
		0 0 0 ;
	setAttr ".tk[30:37]" -type "float3" -0.11445618 -1.9073486e-006 0.0053768158  
		-0.16565895 -1.9073486e-006 0.00056838989  -0.11445606 0 0  -0.16565895 0 0  -0.19154549 
		-1.9073486e-006 0.067247391  -0.19154549 0 0  -0.31983566 -1.9073486e-006 0.076442719  
		-0.31983566 0 0 ;
createNode deleteComponent -n "deleteComponent5";
	setAttr ".dc" -type "componentList" 0;
createNode polyMergeVert -n "polyMergeVert1";
	setAttr ".ics" -type "componentList" 2 "vtx[22]" "vtx[31]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".d" 0.1652;
createNode polyMergeVert -n "polyMergeVert2";
	setAttr ".ics" -type "componentList" 2 "vtx[23]" "vtx[30]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".d" 0.1652;
createNode polyMergeVert -n "polyMergeVert3";
	setAttr ".ics" -type "componentList" 2 "vtx[14]" "vtx[32]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".d" 0.1652;
createNode polyMergeVert -n "polyMergeVert4";
	setAttr ".ics" -type "componentList" 2 "vtx[15]" "vtx[33]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".d" 0.1652;
createNode deleteComponent -n "deleteComponent6";
	setAttr ".dc" -type "componentList" 1 "e[49]";
createNode deleteComponent -n "deleteComponent7";
	setAttr ".dc" -type "componentList" 1 "e[49]";
createNode deleteComponent -n "deleteComponent8";
	setAttr ".dc" -type "componentList" 1 "e[54]";
createNode deleteComponent -n "deleteComponent9";
	setAttr ".dc" -type "componentList" 1 "e[53]";
createNode deleteComponent -n "deleteComponent10";
	setAttr ".dc" -type "componentList" 1 "e[41]";
createNode deleteComponent -n "deleteComponent11";
	setAttr ".dc" -type "componentList" 1 "e[40]";
createNode deleteComponent -n "deleteComponent12";
	setAttr ".dc" -type "componentList" 1 "e[39]";
createNode deleteComponent -n "deleteComponent13";
	setAttr ".dc" -type "componentList" 1 "e[27]";
createNode deleteComponent -n "deleteComponent14";
	setAttr ".dc" -type "componentList" 1 "e[37]";
createNode deleteComponent -n "deleteComponent15";
	setAttr ".dc" -type "componentList" 1 "e[6]";
createNode deleteComponent -n "deleteComponent16";
	setAttr ".dc" -type "componentList" 1 "e[2]";
createNode deleteComponent -n "deleteComponent17";
	setAttr ".dc" -type "componentList" 1 "e[28]";
createNode deleteComponent -n "deleteComponent18";
	setAttr ".dc" -type "componentList" 1 "e[20]";
createNode deleteComponent -n "deleteComponent19";
	setAttr ".dc" -type "componentList" 1 "e[19]";
createNode deleteComponent -n "deleteComponent20";
	setAttr ".dc" -type "componentList" 1 "e[5]";
createNode deleteComponent -n "deleteComponent21";
	setAttr ".dc" -type "componentList" 1 "e[12]";
createNode deleteComponent -n "deleteComponent22";
	setAttr ".dc" -type "componentList" 1 "e[15]";
createNode deleteComponent -n "deleteComponent23";
	setAttr ".dc" -type "componentList" 1 "e[23]";
createNode polyChipOff -n "polyChipOff1";
	setAttr ".ics" -type "componentList" 1 "f[3]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".ws" yes;
	setAttr ".pvt" -type "float3" 0.35295376 0.78258324 5.4317589 ;
	setAttr ".rs" 42353;
	setAttr ".dup" no;
createNode polySeparate -n "polySeparate1";
	setAttr ".ic" 2;
	setAttr -s 2 ".out";
createNode groupId -n "groupId1";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts1";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:10]";
createNode groupId -n "groupId2";
	setAttr ".ihi" 0;
createNode phong -n "mat_floor";
	setAttr ".dc" 1;
	setAttr ".c" -type "float3" 0.44151199 0.48199999 0.44751096 ;
	setAttr ".sc" -type "float3" 0 0 0 ;
	setAttr ".cp" 2;
createNode shadingEngine -n "phong1SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo1";
createNode groupId -n "groupId4";
	setAttr ".ihi" 0;
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
		+ "                -snapValue \"none\" \n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Trax Editor\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = clipEditorNameFromPanel($panelName);\n            clipEditor -e \n                -displayKeys 0\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"none\" \n                -snapValue \"none\" \n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"hyperGraphPanel\" (localizedPanelLabel(\"Hypergraph Hierarchy1\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"hyperGraphPanel\" -l (localizedPanelLabel(\"Hypergraph Hierarchy1\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = ($panelName+\"HyperGraphEd\");\n"
		+ "            hyperGraph -e \n                -graphLayoutStyle \"hierarchicalLayout\" \n                -orientation \"horiz\" \n                -mergeConnections 1\n                -zoom 0.854524\n                -animateTransition 0\n                -showRelationships 1\n                -showShapes 0\n                -showDeformers 0\n                -showExpressions 0\n                -showConstraints 0\n                -showUnderworld 0\n                -showInvisible 0\n                -transitionFrames 5\n                -currentNode \"phys_rack\" \n                -opaqueContainers 0\n                -dropNode \"m_fixture\" \n                -freeform 1\n                -imagePosition 0 0 \n                -imageScale 1\n                -imageEnabled 0\n                -graphType \"DAG\" \n                -heatMapDisplay 0\n                -updateSelection 1\n                -updateNodeAdded 1\n                -useDrawOverrideColor 0\n                -limitGraphTraversal -1\n                -range 0 0 \n                -iconSize \"largeIcons\" \n"
		+ "                -showCachedConnections 0\n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Hypergraph Hierarchy1\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = ($panelName+\"HyperGraphEd\");\n            hyperGraph -e \n                -graphLayoutStyle \"hierarchicalLayout\" \n                -orientation \"horiz\" \n                -mergeConnections 1\n                -zoom 0.854524\n                -animateTransition 0\n                -showRelationships 1\n                -showShapes 0\n                -showDeformers 0\n                -showExpressions 0\n                -showConstraints 0\n                -showUnderworld 0\n                -showInvisible 0\n                -transitionFrames 5\n                -currentNode \"phys_rack\" \n                -opaqueContainers 0\n                -dropNode \"m_fixture\" \n                -freeform 1\n                -imagePosition 0 0 \n                -imageScale 1\n                -imageEnabled 0\n                -graphType \"DAG\" \n"
		+ "                -heatMapDisplay 0\n                -updateSelection 1\n                -updateNodeAdded 1\n                -useDrawOverrideColor 0\n                -limitGraphTraversal -1\n                -range 0 0 \n                -iconSize \"largeIcons\" \n                -showCachedConnections 0\n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"hyperShadePanel\" (localizedPanelLabel(\"Hypershade\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"hyperShadePanel\" -l (localizedPanelLabel(\"Hypershade\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Hypershade\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"visorPanel\" (localizedPanelLabel(\"Visor\")) `;\n\tif (\"\" == $panelName) {\n"
		+ "\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"visorPanel\" -l (localizedPanelLabel(\"Visor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Visor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"polyTexturePlacementPanel\" (localizedPanelLabel(\"UV Texture Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"polyTexturePlacementPanel\" -l (localizedPanelLabel(\"UV Texture Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"UV Texture Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"multiListerPanel\" (localizedPanelLabel(\"Multilister\")) `;\n"
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
		+ "                -textures 1\n                -strokes 1\n                -shadows 0\n                -displayMode \"centerEye\" \n                -viewColor 0 0 0 1 \n                $editorName;\nstereoCameraView -e -viewSelected 0 $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\tif ($useSceneConfig) {\n        string $configName = `getPanel -cwl (localizedPanelLabel(\"Current Layout\"))`;\n        if (\"\" != $configName) {\n\t\t\tpanelConfiguration -edit -label (localizedPanelLabel(\"Current Layout\")) \n\t\t\t\t-defaultImage \"\"\n\t\t\t\t-image \"\"\n\t\t\t\t-sc false\n\t\t\t\t-configString \"global string $gMainPane; paneLayout -e -cn \\\"horizontal2\\\" -ps 1 100 45 -ps 2 100 55 $gMainPane;\"\n\t\t\t\t-removeAllPanels\n\t\t\t\t-ap false\n\t\t\t\t\t(localizedPanelLabel(\"Persp View\")) \n\t\t\t\t\t\"modelPanel\"\n"
		+ "\t\t\t\t\t\"$panelName = `modelPanel -unParent -l (localizedPanelLabel(\\\"Persp View\\\")) -mbv $menusOkayInPanels `;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera persp` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"smoothShaded\\\" \\n    -activeOnly 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 1\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName\"\n"
		+ "\t\t\t\t\t\"modelPanel -edit -l (localizedPanelLabel(\\\"Persp View\\\")) -mbv $menusOkayInPanels  $panelName;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera persp` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"smoothShaded\\\" \\n    -activeOnly 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 1\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName\"\n"
		+ "\t\t\t\t-ap false\n\t\t\t\t\t(localizedPanelLabel(\"Hypergraph Hierarchy1\")) \n\t\t\t\t\t\"scriptedPanel\"\n\t\t\t\t\t\"$panelName = `scriptedPanel -unParent  -type \\\"hyperGraphPanel\\\" -l (localizedPanelLabel(\\\"Hypergraph Hierarchy1\\\")) -mbv $menusOkayInPanels `;\\n\\n\\t\\t\\t$editorName = ($panelName+\\\"HyperGraphEd\\\");\\n            hyperGraph -e \\n                -graphLayoutStyle \\\"hierarchicalLayout\\\" \\n                -orientation \\\"horiz\\\" \\n                -mergeConnections 1\\n                -zoom 0.854524\\n                -animateTransition 0\\n                -showRelationships 1\\n                -showShapes 0\\n                -showDeformers 0\\n                -showExpressions 0\\n                -showConstraints 0\\n                -showUnderworld 0\\n                -showInvisible 0\\n                -transitionFrames 5\\n                -currentNode \\\"phys_rack\\\" \\n                -opaqueContainers 0\\n                -dropNode \\\"m_fixture\\\" \\n                -freeform 1\\n                -imagePosition 0 0 \\n                -imageScale 1\\n                -imageEnabled 0\\n                -graphType \\\"DAG\\\" \\n                -heatMapDisplay 0\\n                -updateSelection 1\\n                -updateNodeAdded 1\\n                -useDrawOverrideColor 0\\n                -limitGraphTraversal -1\\n                -range 0 0 \\n                -iconSize \\\"largeIcons\\\" \\n                -showCachedConnections 0\\n                $editorName\"\n"
		+ "\t\t\t\t\t\"scriptedPanel -edit -l (localizedPanelLabel(\\\"Hypergraph Hierarchy1\\\")) -mbv $menusOkayInPanels  $panelName;\\n\\n\\t\\t\\t$editorName = ($panelName+\\\"HyperGraphEd\\\");\\n            hyperGraph -e \\n                -graphLayoutStyle \\\"hierarchicalLayout\\\" \\n                -orientation \\\"horiz\\\" \\n                -mergeConnections 1\\n                -zoom 0.854524\\n                -animateTransition 0\\n                -showRelationships 1\\n                -showShapes 0\\n                -showDeformers 0\\n                -showExpressions 0\\n                -showConstraints 0\\n                -showUnderworld 0\\n                -showInvisible 0\\n                -transitionFrames 5\\n                -currentNode \\\"phys_rack\\\" \\n                -opaqueContainers 0\\n                -dropNode \\\"m_fixture\\\" \\n                -freeform 1\\n                -imagePosition 0 0 \\n                -imageScale 1\\n                -imageEnabled 0\\n                -graphType \\\"DAG\\\" \\n                -heatMapDisplay 0\\n                -updateSelection 1\\n                -updateNodeAdded 1\\n                -useDrawOverrideColor 0\\n                -limitGraphTraversal -1\\n                -range 0 0 \\n                -iconSize \\\"largeIcons\\\" \\n                -showCachedConnections 0\\n                $editorName\"\n"
		+ "\t\t\t\t$configName;\n\n            setNamedPanelLayout (localizedPanelLabel(\"Current Layout\"));\n        }\n\n        panelHistory -e -clear mainPanelHistory;\n        setFocus `paneLayout -q -p1 $gMainPane`;\n        sceneUIReplacement -deleteRemaining;\n        sceneUIReplacement -clear;\n\t}\n\n\ngrid -spacing 5 -size 12 -divisions 5 -displayAxes yes -displayGridLines yes -displayDivisionLines yes -displayPerspectiveLabels no -displayOrthographicLabels no -displayAxesBold yes -perspectiveLabelPosition axis -orthographicLabelPosition edge;\nviewManip -drawCompass 0 -compassAngle 0 -frontParameters \"\" -homeParameters \"\" -selectionLockParameters \"\";\n}\n");
	setAttr ".st" 3;
createNode script -n "sceneConfigurationScriptNode";
	setAttr ".b" -type "string" "playbackOptions -min 1 -max 24 -ast 1 -aet 48 ";
	setAttr ".st" 6;
createNode materialInfo -n "materialInfo3";
createNode shadingEngine -n "phong3SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode phong -n "mat_platform";
	setAttr ".dc" 1;
	setAttr ".ambc" -type "float3" 0.41321999 0.41321999 0.41321999 ;
	setAttr ".sc" -type "float3" 0 0 0 ;
	setAttr ".cp" 2;
createNode file -n "file2";
	setAttr ".ftn" -type "string" "C:/RnD/PD/trunk/HeliForce/Data//helipad.png";
createNode place2dTexture -n "place2dTexture2";
createNode materialInfo -n "landingpad_materialInfo1";
createNode shadingEngine -n "landingpad_phong1SG";
	setAttr ".ihi" 0;
	setAttr -s 14 ".dsm";
	setAttr ".ro" yes;
createNode phong -n "mat_phys";
	setAttr ".c" -type "float3" 1 0.42400002 0.98656017 ;
	setAttr ".it" -type "float3" 0.28099999 0.28099999 0.28099999 ;
createNode phong -n "mat_walls";
	setAttr ".dc" 1;
	setAttr ".sc" -type "float3" 0.42148 0.42148 0.42148 ;
	setAttr ".cp" 5.2399997711181641;
createNode shadingEngine -n "phong4SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo4";
createNode file -n "file3";
	setAttr ".ftn" -type "string" "C:/RnD/PD/trunk/HeliForce/Data//sheet_metal.png";
createNode place2dTexture -n "place2dTexture3";
createNode deleteComponent -n "deleteComponent24";
	setAttr ".dc" -type "componentList" 1 "f[8]";
createNode deleteComponent -n "deleteComponent25";
	setAttr ".dc" -type "componentList" 1 "f[8]";
createNode deleteComponent -n "deleteComponent26";
	setAttr ".dc" -type "componentList" 1 "f[0]";
createNode deleteComponent -n "deleteComponent27";
	setAttr ".dc" -type "componentList" 1 "f[0]";
createNode deleteComponent -n "deleteComponent28";
	setAttr ".dc" -type "componentList" 1 "f[4]";
createNode deleteComponent -n "deleteComponent29";
	setAttr ".dc" -type "componentList" 1 "f[0]";
createNode polyTweakUV -n "polyTweakUV1";
	setAttr ".uopa" yes;
	setAttr -s 10 ".uvtk[0:9]" -type "float2" -0.0012913346 0.0063658357 
		-0.0013455749 0.089664519 -0.0013455749 0.089664519 -0.0012913346 0.0063658357 0.0014593005 
		0.0064761043 0.0014593005 0.0064761043 0.001388073 0.089664519 0.001388073 0.089664519 
		0.0014593005 0.0064761043 -0.0012913346 0.0063658357;
	setAttr ".uvs" -type "string" "map_hangar";
createNode polyTweakUV -n "polyTweakUV2";
	setAttr ".uopa" yes;
	setAttr -s 4 ".uvtk[0:3]" -type "float2" 0.001388073 0.089664519 
		-0.0013455749 0.089664519 -0.0012913346 0.0063658357 0.0014593005 0.0064761043;
	setAttr ".uvs" -type "string" "map_hangar";
createNode polyPlanarProj -n "polyPlanarProj3";
	setAttr ".uopa" yes;
	setAttr ".ics" -type "componentList" 1 "f[1]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 2 0 0.35295377438606579 0.78258325556203723 0 1;
	setAttr ".ws" yes;
	setAttr ".pc" -type "double3" 0.4162139892578125 10.128598213195801 -47.253597259521484 ;
	setAttr ".ps" -type "double2" 47.232944488525391 47.232944488525391 ;
	setAttr ".uvs" -type "string" "map_hangar";
	setAttr ".is" -type "double2" 1.7024215950985022 2.7739122501791345 ;
	setAttr ".cam" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
createNode polyPlanarProj -n "polyPlanarProj4";
	setAttr ".uopa" yes;
	setAttr ".ics" -type "componentList" 2 "f[0]" "f[2]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 2 0 0.35295377438606579 0.78258325556203723 0 1;
	setAttr ".ws" yes;
	setAttr ".pc" -type "double3" 0.4162139892578125 10.128598213195801 1.3424568176269531 ;
	setAttr ".ic" -type "double2" 0.5 0.13841171011532794 ;
	setAttr ".ro" -type "double3" 0 90 0 ;
	setAttr ".ps" -type "double2" 97.315086364746094 97.315086364746094 ;
	setAttr ".uvs" -type "string" "map_hangar";
	setAttr ".is" -type "double2" 2.6121013926275309 5.7859811467122828 ;
	setAttr ".cam" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
createNode polyPlanarProj -n "polyPlanarProj5";
	setAttr ".uopa" yes;
	setAttr ".ics" -type "componentList" 1 "f[3]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 2 0 0.35295377438606579 0.78258325556203723 0 1;
	setAttr ".ws" yes;
	setAttr ".pc" -type "double3" 0.4162139892578125 19.224612236022949 1.3424568176269531 ;
	setAttr ".ic" -type "double2" 0.58158458095435694 1.1853099320177565 ;
	setAttr ".ro" -type "double3" -90 0 0 ;
	setAttr ".ps" -type "double2" 97.315086364746094 97.315086364746094 ;
	setAttr ".uvs" -type "string" "map_hangar";
	setAttr ".is" -type "double2" 4.1349289065208241 2.6510853052058221 ;
	setAttr ".ra" -89.927735927630266;
	setAttr ".cam" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
createNode polyCube -n "polyCube4";
	setAttr ".w" 3.0168278623761253;
	setAttr ".h" 40;
	setAttr ".d" 100;
	setAttr ".cuv" 4;
createNode polyCube -n "phys_wall3";
	setAttr ".w" 50;
	setAttr ".h" 3;
	setAttr ".d" 100;
	setAttr ".cuv" 4;
createNode polyCube -n "phys_wall4";
	setAttr ".w" 50;
	setAttr ".h" 40;
	setAttr ".d" 3;
	setAttr ".cuv" 4;
createNode polySphere -n "polySphere1";
	setAttr ".sa" 8;
	setAttr ".sh" 8;
createNode polyPlanarProj -n "polyPlanarProj2";
	setAttr ".uopa" yes;
	setAttr ".ics" -type "componentList" 1 "f[0:5]";
	setAttr ".ix" -type "matrix" 2 0 0 0 0 2 0 0 0 0 2 0 -40.739570387654815 22.360854321750786 -1.9984014443252818e-015 1;
	setAttr ".ws" yes;
	setAttr ".pc" -type "double3" -40.739570617675781 22.360855102539063 -2.384185791015625e-007 ;
	setAttr ".ro" -type "double3" -90 0 0 ;
	setAttr ".ps" -type "double2" 12 12.000000476837158 ;
	setAttr ".uvs" -type "string" "helipad_map";
	setAttr ".cam" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
createNode polyPlanarProj -n "polyPlanarProj1";
	setAttr ".uopa" yes;
	setAttr ".ics" -type "componentList" 1 "f[0:5]";
	setAttr ".ix" -type "matrix" 2 0 0 0 0 2 0 0 0 0 2 0 -40.739570387654815 24.866971310600068 -2.4594056924097862 1;
	setAttr ".ws" yes;
	setAttr ".pc" -type "double3" -40.739570617675781 24.866972923278809 -2.4594058990478516 ;
	setAttr ".ro" -type "double3" -90 0 0 ;
	setAttr ".ps" -type "double2" 16 16 ;
	setAttr ".uvs" -type "string" "helipad_map";
	setAttr ".cam" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
createNode polyCube -n "level_start_kit_polyCube1";
	setAttr ".uvs" -type "string" "helipad_map";
	setAttr ".w" 6;
	setAttr ".h" 4;
	setAttr ".d" 6;
	setAttr ".cuv" 4;
createNode materialInfo -n "level_start_kit_materialInfo3";
createNode shadingEngine -n "level_start_kit_phong3SG";
	setAttr ".ihi" 0;
	setAttr -s 4 ".dsm";
	setAttr ".ro" yes;
createNode phong -n "level_start_kit_mat_platform";
	setAttr ".dc" 1;
createNode file -n "level_start_kit_file2";
	setAttr ".ftn" -type "string" "C:/RnD/PD/trunk/HeliForce/Data//helipad.png";
createNode place2dTexture -n "level_start_kit_place2dTexture2";
createNode polyCube -n "polyCube2";
	setAttr ".w" 0.1;
	setAttr ".h" 0.1;
	setAttr ".cuv" 4;
createNode polyCube -n "level_start_kit_polyCube3";
	setAttr ".w" 6;
	setAttr ".h" 0.5;
	setAttr ".d" 6;
	setAttr ".cuv" 4;
createNode polyCube -n "polyCube5";
	setAttr ".w" 25;
	setAttr ".h" 0.5;
	setAttr ".d" 12;
	setAttr ".cuv" 4;
createNode polyTweakUV -n "polyTweakUV3";
	setAttr ".uopa" yes;
	setAttr -s 16 ".uvtk[0:15]" -type "float2" 0.86325431 -0.87985301 1.662019 
		-0.87985301 1.662019 0.87985301 0.94429803 0 0.86325431 0.87985301 0.94923949 0 1.662019 
		-0.87985301 0.86048079 -0.87985301 1.662019 0 1.662019 0 -0.94923955 -0.60467124 
		0.48110545 -0.60467124 0.48110545 0.24569938 -0.94923955 0.24569938 0.86048079 0.87985301 
		1.662019 0.87985301;
	setAttr ".uvs" -type "string" "map_hangar";
createNode phong -n "mat_rack";
	setAttr ".dc" 1;
	setAttr ".c" -type "float3" 0.30577999 0.30577999 0.30577999 ;
	setAttr ".sc" -type "float3" 0.19008 0.19008 0.19008 ;
	setAttr ".cp" 6.8619999885559082;
createNode shadingEngine -n "phong5SG";
	setAttr ".ihi" 0;
	setAttr -s 7 ".dsm";
	setAttr ".ro" yes;
	setAttr -s 5 ".gn";
createNode materialInfo -n "materialInfo5";
createNode polyCube -n "polyCube6";
	setAttr ".w" 0.5;
	setAttr ".h" 25;
	setAttr ".d" 0.5;
	setAttr ".cuv" 4;
createNode polyUnite -n "polyUnite1";
	setAttr -s 8 ".ip";
	setAttr -s 8 ".im";
createNode groupId -n "groupId5";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts2";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:5]";
createNode groupId -n "groupId6";
	setAttr ".ihi" 0;
createNode groupId -n "groupId7";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts3";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:5]";
createNode groupId -n "groupId8";
	setAttr ".ihi" 0;
createNode groupId -n "groupId9";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts4";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:5]";
createNode groupId -n "groupId10";
	setAttr ".ihi" 0;
createNode groupId -n "groupId11";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts5";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:5]";
createNode groupId -n "groupId12";
	setAttr ".ihi" 0;
createNode groupId -n "groupId13";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts6";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:5]";
createNode groupId -n "groupId14";
	setAttr ".ihi" 0;
createNode groupId -n "groupId15";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts7";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:5]";
createNode groupId -n "groupId16";
	setAttr ".ihi" 0;
createNode groupId -n "groupId17";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts8";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:5]";
createNode groupId -n "groupId18";
	setAttr ".ihi" 0;
createNode groupId -n "groupId19";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts9";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:5]";
createNode groupId -n "groupId20";
	setAttr ".ihi" 0;
createNode groupId -n "groupId21";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts10";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[12:47]";
	setAttr ".irc" -type "componentList" 1 "f[0:11]";
createNode groupId -n "groupId22";
	setAttr ".ihi" 0;
createNode groupId -n "groupId23";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts11";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:11]";
createNode polySphere -n "helicopter_01_polySphere1";
	setAttr ".r" 2.1787730267357865;
createNode polyCylinder -n "polyCylinder1";
	setAttr ".r" 0.35444310804077367;
	setAttr ".h" 6;
	setAttr ".sc" 1;
	setAttr ".cuv" 3;
createNode polyCylinder -n "polyCylinder2";
	setAttr ".r" 0.1;
	setAttr ".h" 5;
	setAttr ".sa" 8;
	setAttr ".sh" 2;
	setAttr ".sc" 1;
	setAttr ".cuv" 3;
createNode polyCylinder -n "polyCylinder3";
	setAttr ".r" 0.03;
	setAttr ".h" 1.2;
	setAttr ".sc" 1;
	setAttr ".cuv" 3;
createNode tweak -n "tweak1";
createNode objectSet -n "tweakSet1";
	setAttr ".ihi" 0;
	setAttr ".vo" yes;
createNode groupId -n "helicopter_01_groupId2";
	setAttr ".ihi" 0;
createNode groupParts -n "helicopter_01_groupParts2";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "vtx[*]";
createNode deleteComponent -n "helicopter_01_deleteComponent1";
	setAttr ".dc" -type "componentList" 0;
createNode deleteComponent -n "helicopter_01_deleteComponent2";
	setAttr ".dc" -type "componentList" 9 "e[82:85]" "e[102:105]" "e[122:125]" "e[142:145]" "e[443:445]" "e[463:465]" "e[483:485]" "e[503:505]" "e[523:525]";
createNode deleteComponent -n "helicopter_01_deleteComponent3";
	setAttr ".dc" -type "componentList" 8 "e[146:149]" "e[166:169]" "e[186:189]" "e[206:209]" "e[512:514]" "e[532:534]" "e[552:554]" "e[572:574]";
createNode deleteComponent -n "helicopter_01_deleteComponent4";
	setAttr ".dc" -type "componentList" 11 "e[41:46]" "e[60:67]" "e[80:83]" "e[96:99]" "e[112:115]" "e[370:374]" "e[389:395]" "e[409:412]" "e[426:429]" "e[443:446]" "e[460:463]";
createNode deleteComponent -n "helicopter_01_deleteComponent5";
	setAttr ".dc" -type "componentList" 6 "e[2:5]" "e[20:27]" "e[40:41]" "e[323:329]" "e[343:344]" "e[630:632]";
createNode deleteComponent -n "helicopter_01_deleteComponent6";
	setAttr ".dc" -type "componentList" 2 "e[1:2]" "e[606:607]";
createNode deleteComponent -n "helicopter_01_deleteComponent7";
	setAttr ".dc" -type "componentList" 2 "e[0:1]" "e[603:604]";
createNode deleteComponent -n "helicopter_01_deleteComponent8";
	setAttr ".dc" -type "componentList" 1 "f[72]";
createNode polyCube -n "helicopter_01_polyCube1";
	setAttr ".w" 1.0769689493549741;
	setAttr ".h" 0.69646703441258373;
	setAttr ".d" 1.2520111630428186;
	setAttr ".cuv" 4;
createNode lambert -n "mat_broken_heli";
	setAttr ".c" -type "float3" 0.17295939 0.14242502 0.67500001 ;
createNode shadingEngine -n "lambert2SG";
	setAttr ".ihi" 0;
	setAttr -s 30 ".dsm";
	setAttr ".ro" yes;
	setAttr -s 16 ".gn";
createNode materialInfo -n "helicopter_01_materialInfo1";
createNode polySplit -n "helicopter_01_polySplit1";
	setAttr -s 2 ".e[0:1]"  0.5 0.5;
	setAttr -s 2 ".d[0:1]"  -2147483648 -2147483647;
createNode polyTweak -n "helicopter_01_polyTweak1";
	setAttr ".uopa" yes;
	setAttr -s 4 ".tk[0:3]" -type "float3"  -0.11148468 -0.072096266 
		0 0.11148468 -0.072096266 0 -0.11148468 0.072096266 0 0.11148468 0.072096266 0;
createNode polySplit -n "helicopter_01_polySplit2";
	setAttr ".e[0]"  0;
	setAttr ".d[0]"  -2147483634;
createNode polyCylinder -n "polyCylinder4";
	setAttr ".r" 0.2;
	setAttr ".h" 0.9;
	setAttr ".sc" 1;
	setAttr ".cuv" 3;
createNode polyCube -n "helicopter_01_polyCube2";
	setAttr ".w" 0.95;
	setAttr ".h" 0.7;
	setAttr ".d" 0.63487493600632572;
	setAttr ".cuv" 4;
createNode polyCube -n "helicopter_01_polyCube4";
	setAttr ".w" 0.02;
	setAttr ".d" 0.3;
	setAttr ".sh" 2;
	setAttr ".cuv" 4;
createNode shadingEngine -n "lambert3SG";
	setAttr ".ihi" 0;
	setAttr -s 19 ".dsm";
	setAttr ".ro" yes;
	setAttr -s 18 ".gn";
createNode materialInfo -n "materialInfo2";
createNode lambert -n "phys";
	setAttr ".c" -type "float3" 1 0.597 0.86606956 ;
	setAttr ".it" -type "float3" 0.57852 0.57852 0.57852 ;
createNode shadingEngine -n "lambert4SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "helicopter_01_materialInfo3";
createNode polyTweak -n "helicopter_01_polyTweak2";
	setAttr ".uopa" yes;
	setAttr -s 2 ".tk[6:7]" -type "float3"  -0.054740861 0.064669482 
		-0.012693347 0.054740861 0.064669482 -0.012693347;
createNode deleteComponent -n "helicopter_01_deleteComponent9";
	setAttr ".dc" -type "componentList" 1 "f[3]";
createNode deleteComponent -n "helicopter_01_deleteComponent10";
	setAttr ".dc" -type "componentList" 1 "f[0]";
createNode polyTweak -n "polyTweak3";
	setAttr ".uopa" yes;
	setAttr -s 10 ".tk[0:9]" -type "float3"  -0.041597076 -0.071080364 
		0 0.067017727 -0.045050852 0 -0.23983945 0 0 0.23663205 0 0 0.069153972 0 -0.32300538 
		-0.069153972 0 -0.32300538 0.11997265 0 -0.32300538 -0.11997265 0 -0.32300538 0.047596578 
		-0.29631454 0.14431594 0 0.11041703 0;
createNode deleteComponent -n "helicopter_01_deleteComponent11";
	setAttr ".dc" -type "componentList" 1 "f[1]";
createNode deleteComponent -n "helicopter_01_deleteComponent12";
	setAttr ".dc" -type "componentList" 1 "f[5]";
createNode deleteComponent -n "helicopter_01_deleteComponent13";
	setAttr ".dc" -type "componentList" 1 "f[0]";
createNode polyUnite -n "helicopter_01_polyUnite1";
	setAttr -s 2 ".ip";
	setAttr -s 2 ".im";
createNode groupId -n "helicopter_01_groupId10";
	setAttr ".ihi" 0;
createNode groupParts -n "helicopter_01_groupParts10";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:3]";
createNode groupId -n "helicopter_01_groupId11";
	setAttr ".ihi" 0;
createNode groupId -n "helicopter_01_groupId12";
	setAttr ".ihi" 0;
createNode groupParts -n "helicopter_01_groupParts11";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:3]";
createNode groupId -n "helicopter_01_groupId13";
	setAttr ".ihi" 0;
createNode groupId -n "helicopter_01_groupId14";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts12";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:7]";
createNode groupId -n "helicopter_01_groupId15";
	setAttr ".ihi" 0;
createNode polyMergeVert -n "helicopter_01_polyMergeVert1";
	setAttr ".ics" -type "componentList" 2 "vtx[5]" "vtx[16]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1.0000000000000002 8.3266726846886741e-017 0
		 0 5.5511151231257827e-017 1.0000000000000002 0 0 1.9721522630525295e-031 -4.9303806576313238e-032 1;
createNode polyMergeVert -n "helicopter_01_polyMergeVert2";
	setAttr ".ics" -type "componentList" 2 "vtx[4]" "vtx[15]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1.0000000000000002 8.3266726846886741e-017 0
		 0 5.5511151231257827e-017 1.0000000000000002 0 0 1.9721522630525295e-031 -4.9303806576313238e-032 1;
createNode polyMergeVert -n "helicopter_01_polyMergeVert3";
	setAttr ".ics" -type "componentList" 2 "vtx[3]" "vtx[10]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1.0000000000000002 8.3266726846886741e-017 0
		 0 5.5511151231257827e-017 1.0000000000000002 0 0 1.9721522630525295e-031 -4.9303806576313238e-032 1;
createNode polyMergeVert -n "helicopter_01_polyMergeVert4";
	setAttr ".ics" -type "componentList" 2 "vtx[2]" "vtx[9]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1.0000000000000002 8.3266726846886741e-017 0
		 0 5.5511151231257827e-017 1.0000000000000002 0 0 1.9721522630525295e-031 -4.9303806576313238e-032 1;
createNode polyUnite -n "polyUnite2";
	setAttr -s 4 ".ip";
	setAttr -s 4 ".im";
createNode groupId -n "helicopter_01_groupId16";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts13";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:315]";
createNode groupId -n "helicopter_01_groupId17";
	setAttr ".ihi" 0;
createNode groupId -n "helicopter_01_groupId18";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts14";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:59]";
createNode groupId -n "helicopter_01_groupId19";
	setAttr ".ihi" 0;
createNode groupId -n "helicopter_01_groupId20";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts15";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:59]";
createNode groupId -n "helicopter_01_groupId21";
	setAttr ".ihi" 0;
createNode groupId -n "helicopter_01_groupId22";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts16";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:9]";
createNode groupId -n "helicopter_01_groupId23";
	setAttr ".ihi" 0;
createNode groupId -n "groupId24";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts17";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:445]";
createNode groupId -n "groupId26";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts18";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:31]";
createNode groupId -n "groupId28";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts19";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:31]";
createNode groupId -n "groupId30";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts20";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:59]";
createNode groupId -n "groupId32";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts21";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:59]";
createNode groupId -n "groupId34";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts22";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:59]";
createNode groupId -n "groupId36";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts23";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:59]";
createNode groupId -n "groupId49";
	setAttr ".ihi" 0;
createNode deleteComponent -n "helicopter_01_deleteComponent19";
	setAttr ".dc" -type "componentList" 1 "f[0]";
createNode deleteComponent -n "helicopter_01_deleteComponent18";
	setAttr ".dc" -type "componentList" 1 "f[1]";
createNode deleteComponent -n "helicopter_01_deleteComponent17";
	setAttr ".dc" -type "componentList" 1 "f[1]";
createNode groupParts -n "groupParts29";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:82]";
createNode polyBoolOp -n "polyBoolOp1";
	setAttr -s 2 ".ip";
	setAttr -s 2 ".im";
	setAttr ".op" 3;
	setAttr ".uth" yes;
createNode polyCube -n "heli_crap_polyCube7";
	setAttr ".w" 1.7;
	setAttr ".h" 1.7;
	setAttr ".d" 1.7;
	setAttr ".cuv" 4;
createNode groupId -n "groupId53";
	setAttr ".ihi" 0;
createNode deleteComponent -n "helicopter_01_deleteComponent16";
	setAttr ".dc" -type "componentList" 1 "f[0]";
createNode deleteComponent -n "helicopter_01_deleteComponent15";
	setAttr ".dc" -type "componentList" 1 "f[0]";
createNode deleteComponent -n "helicopter_01_deleteComponent14";
	setAttr ".dc" -type "componentList" 1 "f[0]";
createNode groupParts -n "groupParts30";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:74]";
createNode polyBoolOp -n "polyBoolOp2";
	setAttr -s 2 ".ip";
	setAttr -s 2 ".im";
	setAttr ".op" 3;
	setAttr ".uth" yes;
createNode polySeparate -n "helicopter_01_polySeparate1";
	setAttr ".ic" 4;
createNode groupId -n "heli_crap_groupId24";
	setAttr ".ihi" 0;
createNode groupId -n "groupId40";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts25";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:445]";
createNode groupParts -n "heli_crap_groupParts17";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:445]";
createNode polyUnite -n "heli_crap_polyUnite2";
	setAttr -s 4 ".ip";
createNode tweak -n "heli_crap_tweak1";
createNode objectSet -n "heli_crap_tweakSet1";
	setAttr ".ihi" 0;
	setAttr ".vo" yes;
createNode groupId -n "heli_crap_groupId2";
	setAttr ".ihi" 0;
createNode groupParts -n "heli_crap_groupParts2";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "vtx[*]";
createNode polySphere -n "heli_crap_polySphere1";
	setAttr ".r" 2.1787730267357865;
createNode deleteComponent -n "heli_crap_deleteComponent3";
	setAttr ".dc" -type "componentList" 8 "e[146:149]" "e[166:169]" "e[186:189]" "e[206:209]" "e[512:514]" "e[532:534]" "e[552:554]" "e[572:574]";
createNode deleteComponent -n "heli_crap_deleteComponent2";
	setAttr ".dc" -type "componentList" 9 "e[82:85]" "e[102:105]" "e[122:125]" "e[142:145]" "e[443:445]" "e[463:465]" "e[483:485]" "e[503:505]" "e[523:525]";
createNode deleteComponent -n "heli_crap_deleteComponent1";
	setAttr ".dc" -type "componentList" 0;
createNode deleteComponent -n "heli_crap_deleteComponent7";
	setAttr ".dc" -type "componentList" 2 "e[0:1]" "e[603:604]";
createNode deleteComponent -n "heli_crap_deleteComponent6";
	setAttr ".dc" -type "componentList" 2 "e[1:2]" "e[606:607]";
createNode deleteComponent -n "heli_crap_deleteComponent5";
	setAttr ".dc" -type "componentList" 6 "e[2:5]" "e[20:27]" "e[40:41]" "e[323:329]" "e[343:344]" "e[630:632]";
createNode deleteComponent -n "heli_crap_deleteComponent4";
	setAttr ".dc" -type "componentList" 11 "e[41:46]" "e[60:67]" "e[80:83]" "e[96:99]" "e[112:115]" "e[370:374]" "e[389:395]" "e[409:412]" "e[426:429]" "e[443:446]" "e[460:463]";
createNode deleteComponent -n "heli_crap_deleteComponent8";
	setAttr ".dc" -type "componentList" 1 "f[72]";
createNode polyCylinder -n "heli_crap_polyCylinder1";
	setAttr ".r" 0.35444310804077367;
	setAttr ".h" 6;
	setAttr ".sc" 1;
	setAttr ".cuv" 3;
createNode polyCylinder -n "heli_crap_polyCylinder4";
	setAttr ".r" 0.2;
	setAttr ".h" 0.9;
	setAttr ".sc" 1;
	setAttr ".cuv" 3;
createNode polyCube -n "heli_crap_polyCube4";
	setAttr ".w" 0.02;
	setAttr ".d" 0.3;
	setAttr ".sh" 2;
	setAttr ".cuv" 4;
createNode materialInfo -n "helicopter_01_materialInfo4";
createNode shadingEngine -n "blinn1SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode polyBoolOp -n "polyBoolOp4";
	setAttr -s 2 ".ip";
	setAttr -s 2 ".im";
	setAttr ".op" 2;
	setAttr ".uth" yes;
createNode groupId -n "groupId57";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts31";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:438]";
createNode polyBoolOp -n "polyBoolOp5";
	setAttr -s 2 ".ip";
	setAttr -s 2 ".im";
	setAttr ".op" 2;
	setAttr ".uth" yes;
createNode groupParts -n "groupParts32";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:435]";
createNode deleteComponent -n "helicopter_01_deleteComponent20";
	setAttr ".dc" -type "componentList" 1 "f[2]";
createNode deleteComponent -n "helicopter_01_deleteComponent21";
	setAttr ".dc" -type "componentList" 1 "f[433]";
createNode polyMergeVert -n "polyMergeVert5";
	setAttr ".ics" -type "componentList" 2 "vtx[4]" "vtx[452]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
createNode polyTweak -n "polyTweak4";
	setAttr ".uopa" yes;
	setAttr -s 2 ".tk";
	setAttr ".tk[4]" -type "float3" -0.049999952 0 0 ;
	setAttr ".tk[452]" -type "float3" 0.049999952 0 0 ;
createNode groupId -n "groupId61";
	setAttr ".ihi" 0;
createNode polyCreateFace -n "polyCreateFace1";
	setAttr -s 3 ".v[0:2]" -type "float3"  0 -0.43838376 0.21990079 
		-0.049999952 1.1905062 0.019898295 0.055773497 1.1961643 0.019203544;
	setAttr ".l[0]"  3;
	setAttr ".tx" 1;
createNode polyCreateFace -n "polyCreateFace2";
	setAttr -s 3 ".v[0:2]" -type "float3"  0 -0.43838376 0.21990079 
		0.064538985 -0.26725692 1.6136172 -0.051089168 -0.26309526 1.647511;
	setAttr ".l[0]"  3;
	setAttr ".tx" 1;
createNode polyUnite -n "polyUnite4";
	setAttr -s 2 ".ip";
	setAttr -s 2 ".im";
createNode groupId -n "groupId63";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts33";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0]";
createNode groupId -n "groupId65";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts34";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:434]";
createNode groupId -n "groupId67";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts35";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0]";
createNode materialInfo -n "pasted__materialInfo8";
createNode shadingEngine -n "pasted__phong2SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode phong -n "mat_black1";
	setAttr ".sc" -type "float3" 0.31404001 0.31404001 0.31404001 ;
	setAttr ".cp" 13.340000152587891;
createNode file -n "pasted__file1";
	setAttr ".ftn" -type "string" "C:/RnD/PD/trunk/Push/Data/blue_eye.png";
createNode place2dTexture -n "pasted__place2dTexture2";
createNode groupId -n "heli_crap_groupId16";
	setAttr ".ihi" 0;
createNode groupParts -n "heli_crap_groupParts13";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:315]";
createNode groupId -n "heli_crap_groupId17";
	setAttr ".ihi" 0;
createNode phong -n "gray";
	setAttr ".dc" 0.90082001686096191;
	setAttr ".c" -type "float3" 0.49700001 0.49700001 0.49700001 ;
createNode shadingEngine -n "helicopter_01_phong1SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "forklift_01_materialInfo1";
createNode phong -n "blue";
	setAttr ".c" -type "float3" 0.53999996 0.89458328 1 ;
createNode shadingEngine -n "phong2SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "forklift_01_materialInfo2";
createNode phong -n "mat_black";
	setAttr ".c" -type "float3" 0.23 0.23 0.23 ;
createNode shadingEngine -n "helicopter_01_phong3SG";
	setAttr ".ihi" 0;
	setAttr -s 2 ".dsm";
	setAttr ".ro" yes;
	setAttr -s 3 ".gn";
createNode materialInfo -n "forklift_01_materialInfo3";
createNode phong -n "grey";
	setAttr ".c" -type "float3" 0.69999999 0.69999999 0.69999999 ;
createNode shadingEngine -n "helicopter_01_phong4SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "forklift_01_materialInfo4";
createNode phong -n "forklift_01_phys";
	setAttr ".c" -type "float3" 0.97582418 0.56700003 1 ;
	setAttr ".it" -type "float3" 0.72728002 0.72728002 0.72728002 ;
createNode shadingEngine -n "helicopter_01_phong5SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "helicopter_01_materialInfo5";
createNode cameraView -n "cameraView1";
	setAttr ".e" -type "double3" 2.3849640200401567 2.4756134310912907 4.3737993218888827 ;
	setAttr ".coi" -type "double3" 0.66950060317122673 0.65905216247416742 0.48432309115794769 ;
	setAttr ".u" -type "double3" -0.15857422993868106 0.91955866809513143 -0.35953590854108336 ;
	setAttr ".tp" -type "double3" 0.76603825958374649 0.95648742017602917 0.93542745012259587 ;
	setAttr ".fl" 34.999999999999979;
createNode cameraView -n "cameraView2";
	setAttr ".e" -type "double3" 2.3849640200401567 2.4756134310912907 4.3737993218888827 ;
	setAttr ".coi" -type "double3" 0.66950060317122673 0.65905216247416742 0.48432309115794769 ;
	setAttr ".u" -type "double3" -0.15857422993868106 0.91955866809513143 -0.35953590854108336 ;
	setAttr ".tp" -type "double3" 0.76603825958374649 0.95648742017602917 0.93542745012259587 ;
	setAttr ".fl" 34.999999999999979;
createNode script -n "forklift_01_rg_export";
	addAttr -ci true -sn "time" -ln "time" -dt "string";
	setAttr ".time" -type "string" "2011-04-10T22:12:38.187000";
createNode polyCylinder -n "polyCylinder6";
	setAttr ".r" 0.053210537144433197;
	setAttr ".h" 1.1338209957577174;
	setAttr ".sa" 6;
	setAttr ".sh" 3;
	setAttr ".sc" 1;
	setAttr ".cuv" 3;
createNode groupId -n "groupId74";
	setAttr ".ihi" 0;
createNode groupId -n "groupId79";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts41";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:29]";
createNode groupId -n "groupId80";
	setAttr ".ihi" 0;
createNode polySplit -n "helicopter_01_polySplit3";
	setAttr -s 2 ".e[0:1]"  0.52684528 0;
	setAttr -s 2 ".d[0:1]"  -2147483646 -2147483637;
createNode polyTweak -n "polyTweak5";
	setAttr ".uopa" yes;
	setAttr -s 13 ".tk";
	setAttr ".tk[0:5]" -type "float3" -0.17580175 0.52740669 -0.32911211  
		0.13550961 0.50266147 -0.34623307  0.38439023 -0.22748524 -1.3245317  -0.38118291 
		-0.23282853 -1.3299598  0 -1.4901161e-008 0  5.9604645e-008 0 0 ;
	setAttr ".tk[8:12]" -type "float3" 0 0 -0.060729317  0.0014939606 0.083752312 
		-0.37184849  -0.0014939308 0.072966218 -0.37684691  0 0.05417281 -0.17207845  0 0.05417281 
		-0.17207845 ;
	setAttr ".tk[15:16]" -type "float3" 0 1.4901161e-008 0  -5.9604645e-008 
		0 0 ;
createNode polySplit -n "polySplit4";
	setAttr -s 2 ".e[0:1]"  1 0.88502955;
	setAttr -s 2 ".d[0:1]"  -2147483640 -2147483637;
createNode polySplit -n "polySplit5";
	setAttr -s 2 ".e[0:1]"  1 0.78654188;
	setAttr -s 2 ".d[0:1]"  -2147483640 -2147483637;
createNode polySplit -n "polySplit6";
	setAttr -s 2 ".e[0:1]"  1 0.73488796;
	setAttr -s 2 ".d[0:1]"  -2147483640 -2147483637;
createNode polySplit -n "polySplit7";
	setAttr -s 2 ".e[0:1]"  1 0.66494304;
	setAttr -s 2 ".d[0:1]"  -2147483640 -2147483637;
createNode polySplit -n "polySplit8";
	setAttr -s 2 ".e[0:1]"  1 0.5951736;
	setAttr -s 2 ".d[0:1]"  -2147483640 -2147483637;
createNode polySplit -n "polySplit9";
	setAttr -s 2 ".e[0:1]"  1 0.5;
	setAttr -s 2 ".d[0:1]"  -2147483640 -2147483637;
createNode polySplit -n "polySplit10";
	setAttr -s 2 ".e[0:1]"  1 0.5;
	setAttr -s 2 ".d[0:1]"  -2147483640 -2147483637;
createNode polyTweak -n "polyTweak6";
	setAttr ".uopa" yes;
	setAttr -s 7 ".tk";
	setAttr ".tk[8]" -type "float3" -0.0047565103 0.0092089176 0.051888347 ;
	setAttr ".tk[14:19]" -type "float3" 0.066397667 -0.066856623 -0.044257522  
		0.17719764 -0.069534898 -0.10399318  0.2663998 -0.063563347 -0.14471149  0.32276046 
		-0.050776124 -0.16304982  0.32546997 -0.038204372 -0.15123314  0.26956969 -0.024110794 
		-0.10956073 ;
createNode polySplit -n "polySplit11";
	setAttr -s 2 ".e[0:1]"  0 0.11064567;
	setAttr -s 2 ".d[0:1]"  -2147483646 -2147483648;
createNode polyTweak -n "polyTweak7";
	setAttr ".uopa" yes;
	setAttr ".tk[20]" -type "float3"  0.011816412 -0.01179111 -0.0050387383;
createNode polySplit -n "polySplit12";
	setAttr -s 2 ".e[0:1]"  0.026858147 0.099963665;
	setAttr -s 2 ".d[0:1]"  -2147483646 -2147483612;
createNode polySplit -n "polySplit13";
	setAttr -s 2 ".e[0:1]"  1 0.12954788;
	setAttr -s 2 ".d[0:1]"  -2147483646 -2147483609;
createNode polySplit -n "polySplit14";
	setAttr -s 4 ".e[0:3]"  0.035543606 0.016897768 0.030959623 0.17330615;
	setAttr -s 4 ".d[0:3]"  -2147483646 -2147483608 -2147483606 -2147483607;
createNode polySplit -n "polySplit15";
	setAttr -s 4 ".e[0:3]"  1 0.69089299 0.53459251 0.21426788;
	setAttr -s 4 ".d[0:3]"  -2147483646 -2147483608 -2147483606 -2147483600;
createNode polySplit -n "polySplit16";
	setAttr -s 4 ".e[0:3]"  1 0.81210899 0.74164218 0.2285561;
	setAttr -s 4 ".d[0:3]"  -2147483646 -2147483608 -2147483606 -2147483594;
createNode polySplit -n "polySplit17";
	setAttr -s 7 ".e[0:6]"  0 0.035279803 0.089322507 0.25042352 0.903844 
		0.86230648 0.25401157;
	setAttr -s 7 ".d[0:6]"  -2147483646 -2147483603 -2147483597 -2147483591 -2147483608 -2147483606 
		-2147483588;
createNode polySplit -n "polySplit18";
	setAttr -s 4 ".e[0:3]"  1 0.83160359 0.79738647 0.46649122;
	setAttr -s 4 ".d[0:3]"  -2147483646 -2147483608 -2147483606 -2147483576;
createNode polyTweak -n "polyTweak8";
	setAttr ".uopa" yes;
	setAttr -s 7 ".tk";
	setAttr ".tk[21]" -type "float3" -0.073107362 -0.073394239 -0.048279047 ;
	setAttr ".tk[23:24]" -type "float3" -0.10660696 -0.1541075 -0.069753647  
		-0.13013458 -0.21126407 -0.081379771 ;
	setAttr ".tk[28]" -type "float3" -0.14215422 -0.23947525 -0.084470987 ;
	setAttr ".tk[31]" -type "float3" -0.12711525 -0.24031276 -0.073269606 ;
	setAttr ".tk[34]" -type "float3" -0.064497352 -0.22453249 -0.038591862 ;
	setAttr ".tk[40]" -type "float3" 0.037147701 -0.1885885 0.015376568 ;
createNode deleteComponent -n "helicopter_01_deleteComponent22";
	setAttr ".dc" -type "componentList" 1 "e[79]";
createNode deleteComponent -n "helicopter_01_deleteComponent23";
	setAttr ".dc" -type "componentList" 1 "vtx[43]";
createNode polyMergeVert -n "polyMergeVert6";
	setAttr ".ics" -type "componentList" 7 "vtx[6]" "vtx[22]" "vtx[25:27]" "vtx[29:30]" "vtx[32:33]" "vtx[35:39]" "vtx[41:42]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1.0000000000000002 0 0 0 0 1.0000000000000002 0
		 0 9.8607613152626476e-032 -2.4651903288156619e-032 1;
createNode polyTweak -n "polyTweak9";
	setAttr ".uopa" yes;
	setAttr -s 17 ".tk";
	setAttr ".tk[6]" -type "float3" 0.00062644482 8.3327293e-005 0.00047278404 ;
	setAttr ".tk[22]" -type "float3" -0.011217505 8.3327293e-005 0.00047278404 ;
	setAttr ".tk[25:27]" -type "float3" 0.00020548701 8.3327293e-005 0.00047278404  
		-0.0065018833 -0.0039025545 -0.02214694  -0.0054997802 -0.0048326254 -0.042461634 ;
	setAttr ".tk[29:30]" -type "float3" -0.0079594851 -0.0026704669 -0.015155077  
		-0.0081608593 -0.0025447011 -0.022479534 ;
	setAttr ".tk[32:33]" -type "float3" -0.0085716248 -0.002153039 -0.012218714  
		-0.0089505613 -0.0018657446 -0.016549587 ;
	setAttr ".tk[35:39]" -type "float3" -3.1143427e-005 -5.7280064e-005 -0.00032520294  
		-0.00052383542 -0.00016266108 -0.00092315674  -0.0019925237 -0.00047671795 -0.002705574  
		-0.0088260174 -0.0019379854 -0.010998249  -0.0092627108 -0.0015973449 -0.014205694 ;
	setAttr ".tk[41:43]" -type "float3" -0.0092287362 -0.0015975833 -0.0090665817  
		-0.0096587837 -0.0012568235 -0.011231661  0 0 0 ;
createNode polyMergeVert -n "polyMergeVert7";
	setAttr ".ics" -type "componentList" 1 "vtx[7]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1.0000000000000002 0 0 0 0 1.0000000000000002 0
		 0 9.8607613152626476e-032 -2.4651903288156619e-032 1;
createNode polySplit -n "polySplit19";
	setAttr -s 2 ".e[0:1]"  1 0.53614199;
	setAttr -s 2 ".d[0:1]"  -2147483642 -2147483646;
createNode polySplit -n "polySplit20";
	setAttr -s 2 ".e[0:1]"  1 0.53527331;
	setAttr -s 2 ".d[0:1]"  -2147483641 -2147483645;
createNode polyTweak -n "polyTweak10";
	setAttr ".uopa" yes;
	setAttr ".tk[28]" -type "float3"  -0.022432327 -0.0016289949 -0.019565821;
createNode polySplit -n "polySplit21";
	setAttr -s 2 ".e[0:1]"  0.5 0.5;
	setAttr -s 2 ".d[0:1]"  -2147483636 -2147483637;
createNode polyTweak -n "polyTweak11";
	setAttr ".uopa" yes;
	setAttr ".tk[29]" -type "float3"  0.028933883 -0.0037521981 -0.019599199;
createNode polySplit -n "polySplit22";
	setAttr -s 2 ".e[0:1]"  0.5 0.44003248;
	setAttr -s 2 ".d[0:1]"  -2147483636 -2147483637;
createNode polySplit -n "polySplit23";
	setAttr -s 2 ".e[0:1]"  0.53286749 0.54458416;
	setAttr -s 2 ".d[0:1]"  -2147483594 -2147483593;
createNode polySplit -n "polySplit24";
	setAttr -s 2 ".e[0:1]"  1 0.2409308;
	setAttr -s 2 ".d[0:1]"  -2147483647 -2147483634;
createNode polyTweak -n "polyTweak12";
	setAttr ".uopa" yes;
	setAttr -s 5 ".tk[31:35]" -type "float3"  -5.9604645e-008 0.068495244 
		-0.35958326 0.23746233 -1.1920929e-007 0 0.016211152 0.049786091 -0.26748061 -0.25311205 
		0 0 -0.0089271069 0.049650818 -0.26682281;
createNode polySplit -n "polySplit25";
	setAttr -s 2 ".e[0:1]"  1 0.2558229;
	setAttr -s 2 ".d[0:1]"  -2147483647 -2147483585;
createNode polySplit -n "polySplit26";
	setAttr -s 2 ".e[0:1]"  1 0.40093401;
	setAttr -s 2 ".d[0:1]"  -2147483647 -2147483583;
createNode polySplit -n "polySplit27";
	setAttr -s 2 ".e[0:1]"  0 0.27910036;
	setAttr -s 2 ".d[0:1]"  -2147483647 -2147483635;
createNode polySplit -n "polySplit28";
	setAttr -s 2 ".e[0:1]"  0 0.20271318;
	setAttr -s 2 ".d[0:1]"  -2147483647 -2147483579;
createNode polySplit -n "polySplit29";
	setAttr -s 2 ".e[0:1]"  0 0.23794708;
	setAttr -s 2 ".d[0:1]"  -2147483647 -2147483577;
createNode polySplit -n "polySplit30";
	setAttr -s 2 ".e[0:1]"  0 0.5;
	setAttr -s 2 ".d[0:1]"  -2147483636 -2147483575;
createNode polyTweak -n "polyTweak13";
	setAttr ".uopa" yes;
	setAttr -s 7 ".tk";
	setAttr ".tk[36:41]" -type "float3" 0.027155995 -0.01492323 -0.094683051  
		0.039895594 0.00099313259 -0.13369536  0.040580451 -0.015390217 -0.13987207  -0.028373957 
		-0.043169379 -0.10560727  -0.039580345 0.0094323754 -0.13095284  -0.037401915 0.060564876 
		-0.11152625 ;
	setAttr ".tk[43]" -type "float3" 8.9406967e-008 0 0 ;
createNode polySplit -n "polySplit31";
	setAttr -s 2 ".e[0:1]"  1 0.54646003;
	setAttr -s 2 ".d[0:1]"  -2147483632 -2147483581;
createNode polyCylinder -n "polyCylinder7";
	setAttr ".r" 0.1;
	setAttr ".h" 0.3;
	setAttr ".sa" 6;
	setAttr ".sc" 1;
	setAttr ".cuv" 3;
createNode groupId -n "groupId83";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts43";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:17]";
createNode groupId -n "groupId66";
	setAttr ".ihi" 0;
createNode groupId -n "groupId64";
	setAttr ".ihi" 0;
createNode groupId -n "groupId62";
	setAttr ".ihi" 0;
createNode groupId -n "groupId58";
	setAttr ".ihi" 0;
createNode groupId -n "groupId60";
	setAttr ".ihi" 0;
createNode groupId -n "groupId59";
	setAttr ".ihi" 0;
createNode groupId -n "groupId54";
	setAttr ".ihi" 0;
createNode groupId -n "groupId50";
	setAttr ".ihi" 0;
createNode groupId -n "groupId56";
	setAttr ".ihi" 0;
createNode groupId -n "groupId55";
	setAttr ".ihi" 0;
createNode groupId -n "groupId52";
	setAttr ".ihi" 0;
createNode groupId -n "groupId51";
	setAttr ".ihi" 0;
createNode groupId -n "groupId48";
	setAttr ".ihi" 0;
createNode groupId -n "groupId47";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts28";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:5]";
createNode groupId -n "groupId37";
	setAttr ".ihi" 0;
createNode groupId -n "groupId35";
	setAttr ".ihi" 0;
createNode groupId -n "groupId33";
	setAttr ".ihi" 0;
createNode groupId -n "groupId31";
	setAttr ".ihi" 0;
createNode groupId -n "groupId29";
	setAttr ".ihi" 0;
createNode groupId -n "groupId27";
	setAttr ".ihi" 0;
createNode groupId -n "groupId25";
	setAttr ".ihi" 0;
createNode script -n "helicopter_01_rg_export";
	addAttr -ci true -sn "time" -ln "time" -dt "string";
	setAttr ".time" -type "string" "2013-05-11T12:54:34.312000";
createNode phong -n "mat_broken_heli_engine";
	setAttr ".dc" 1;
	setAttr ".c" -type "float3" 0.80166 0.80166 0.80166 ;
	setAttr ".sc" -type "float3" 1 1 1 ;
	setAttr ".cp" 9.2899999618530273;
createNode groupId -n "groupId84";
	setAttr ".ihi" 0;
createNode groupId -n "groupId68";
	setAttr ".ihi" 0;
createNode groupId -n "groupId42";
	setAttr ".ihi" 0;
createNode groupId -n "groupId43";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts26";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:315]";
createNode groupId -n "groupId45";
	setAttr ".ihi" 0;
createNode groupId -n "groupId46";
	setAttr ".ihi" 0;
createNode polyCube -n "polyCube7";
	setAttr ".cuv" 4;
createNode phong -n "mat_broken_heli_window";
	setAttr ".c" -type "float3" 0.050999999 0.043247998 0.044209249 ;
	setAttr ".it" -type "float3" 0.38016 0.38016 0.38016 ;
	setAttr ".sc" -type "float3" 1 1 1 ;
	setAttr ".cp" 6.8619999885559082;
createNode phong -n "mat_forklift_path1";
	setAttr ".c" -type "float3" 0 0.024833441 1 ;
createNode shadingEngine -n "phong8SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo6";
createNode deleteComponent -n "deleteComponent30";
	setAttr ".dc" -type "componentList" 1 "f[0:5]";
createNode polyCube -n "polyCube3";
	setAttr ".w" 50;
	setAttr ".h" 3;
	setAttr ".d" 100;
	setAttr ".cuv" 4;
createNode polyCube -n "polyCube8";
	setAttr ".w" 50;
	setAttr ".h" 1.5;
	setAttr ".d" 25;
	setAttr ".cuv" 4;
createNode script -n "rg_export";
	addAttr -ci true -sn "time" -ln "time" -dt "string";
	setAttr ".time" -type "string" "2013-07-30T14:50:19.875000";
select -ne :time1;
	setAttr ".o" 1;
select -ne :renderPartition;
	setAttr -s 19 ".st";
select -ne :renderGlobalsList1;
select -ne :defaultShaderList1;
	setAttr -s 19 ".s";
select -ne :postProcessList1;
	setAttr -s 2 ".p";
select -ne :defaultRenderUtilityList1;
	setAttr -s 4 ".u";
select -ne :lightList1;
select -ne :defaultTextureList1;
	setAttr -s 4 ".tx";
select -ne :initialShadingGroup;
	setAttr -s 32 ".dsm";
	setAttr ".ro" yes;
	setAttr -s 33 ".gn";
select -ne :initialParticleSE;
	setAttr ".ro" yes;
select -ne :hardwareRenderGlobals;
	setAttr ".ctrs" 256;
	setAttr ".btrs" 512;
select -ne :defaultHardwareRenderGlobals;
	setAttr ".fn" -type "string" "im";
	setAttr ".res" -type "string" "ntsc_4d 646 485 1.333";
select -ne :ikSystem;
	setAttr -s 4 ".sol";
select -ne :hyperGraphLayout;
	setAttr -s 66 ".hyp";
	setAttr ".hyp[0].x" 112.40059661865234;
	setAttr ".hyp[0].y" 19.119287490844727;
	setAttr ".hyp[0].isf" yes;
	setAttr ".hyp[1].x" 153.7982177734375;
	setAttr ".hyp[1].y" -156.5198974609375;
	setAttr ".hyp[1].isf" yes;
	setAttr ".hyp[2].x" 118.7982177734375;
	setAttr ".hyp[2].y" -116.51988220214844;
	setAttr ".hyp[2].isf" yes;
	setAttr ".hyp[4].x" 375.25369262695312;
	setAttr ".hyp[4].y" 49;
	setAttr ".hyp[4].isf" yes;
	setAttr ".hyp[5].x" 375.25369262695312;
	setAttr ".hyp[5].y" 10;
	setAttr ".hyp[5].isf" yes;
	setAttr ".hyp[6].x" 374.36581420898437;
	setAttr ".hyp[6].y" -152.02447509765625;
	setAttr ".hyp[6].isf" yes;
	setAttr ".hyp[7].x" 1298.0723876953125;
	setAttr ".hyp[7].y" -182.57672119140625;
	setAttr ".hyp[7].isf" yes;
	setAttr ".hyp[9].x" 961.771240234375;
	setAttr ".hyp[9].y" -446.57647705078125;
	setAttr ".hyp[9].isf" yes;
	setAttr ".hyp[10].x" 1298.0723876953125;
	setAttr ".hyp[10].y" -182.57672119140625;
	setAttr ".hyp[10].isf" yes;
	setAttr ".hyp[11].x" 1298.0723876953125;
	setAttr ".hyp[11].y" -182.57672119140625;
	setAttr ".hyp[11].isf" yes;
	setAttr ".hyp[12].x" 1298.0723876953125;
	setAttr ".hyp[12].y" -182.57672119140625;
	setAttr ".hyp[12].isf" yes;
	setAttr ".hyp[13].x" 1298.0723876953125;
	setAttr ".hyp[13].y" -182.57672119140625;
	setAttr ".hyp[13].isf" yes;
	setAttr ".hyp[18].x" 1298.0723876953125;
	setAttr ".hyp[18].y" -182.57672119140625;
	setAttr ".hyp[18].isf" yes;
	setAttr ".hyp[22].x" 1298.0723876953125;
	setAttr ".hyp[22].y" -182.57672119140625;
	setAttr ".hyp[22].isf" yes;
	setAttr ".hyp[23].x" 1298.0723876953125;
	setAttr ".hyp[23].y" -182.57672119140625;
	setAttr ".hyp[23].isf" yes;
	setAttr ".hyp[24].x" 1298.0723876953125;
	setAttr ".hyp[24].y" -182.57672119140625;
	setAttr ".hyp[24].isf" yes;
	setAttr ".hyp[25].x" 1929.9232177734375;
	setAttr ".hyp[25].y" -4.6538410186767578;
	setAttr ".hyp[25].isf" yes;
	setAttr ".hyp[101].x" 136.16706848144531;
	setAttr ".hyp[101].y" -196.31689453125;
	setAttr ".hyp[101].isf" yes;
	setAttr ".hyp[102].x" 980.91455078125;
	setAttr ".hyp[102].y" -118.70445251464844;
	setAttr ".hyp[102].isf" yes;
	setAttr ".hyp[103].x" 980.91455078125;
	setAttr ".hyp[103].y" -118.70445251464844;
	setAttr ".hyp[103].isf" yes;
	setAttr ".hyp[104].x" 1298.0723876953125;
	setAttr ".hyp[104].y" -182.57672119140625;
	setAttr ".hyp[104].isf" yes;
	setAttr ".hyp[123].x" 1298.0723876953125;
	setAttr ".hyp[123].y" -182.57672119140625;
	setAttr ".hyp[123].isf" yes;
	setAttr ".hyp[138].x" 1428.469970703125;
	setAttr ".hyp[138].y" -128.88360595703125;
	setAttr ".hyp[138].isf" yes;
	setAttr ".hyp[139].x" 176.26983642578125;
	setAttr ".hyp[139].y" -401.18438720703125;
	setAttr ".hyp[139].isf" yes;
	setAttr ".hyp[140].x" 176.26983642578125;
	setAttr ".hyp[140].y" -401.18438720703125;
	setAttr ".hyp[140].isf" yes;
	setAttr ".hyp[141].x" 1221.367919921875;
	setAttr ".hyp[141].y" -289.96295166015625;
	setAttr ".hyp[141].isf" yes;
	setAttr ".hyp[142].x" 1238.62646484375;
	setAttr ".hyp[142].y" -341.73846435546875;
	setAttr ".hyp[142].isf" yes;
	setAttr ".hyp[143].x" 1236.7088623046875;
	setAttr ".hyp[143].y" -243.94027709960937;
	setAttr ".hyp[143].isf" yes;
	setAttr ".hyp[144].x" 1154.2515869140625;
	setAttr ".hyp[144].y" -132.71882629394531;
	setAttr ".hyp[144].isf" yes;
	setAttr ".hyp[145].x" 1786.92724609375;
	setAttr ".hyp[145].y" -252.99986267089844;
	setAttr ".hyp[145].isf" yes;
	setAttr ".hyp[146].x" 993.1722412109375;
	setAttr ".hyp[146].y" -345.57366943359375;
	setAttr ".hyp[146].isf" yes;
	setAttr ".hyp[147].x" 949.067138671875;
	setAttr ".hyp[147].y" -280.37490844726562;
	setAttr ".hyp[147].isf" yes;
	setAttr ".hyp[148].x" 979.74896240234375;
	setAttr ".hyp[148].y" -178.74150085449219;
	setAttr ".hyp[148].isf" yes;
	setAttr ".hyp[149].x" 993.1722412109375;
	setAttr ".hyp[149].y" -107.78988647460937;
	setAttr ".hyp[149].isf" yes;
	setAttr ".hyp[150].x" 1639.308349609375;
	setAttr ".hyp[150].y" -96.692459106445313;
	setAttr ".hyp[150].isf" yes;
	setAttr ".hyp[151].x" 1819.308349609375;
	setAttr ".hyp[151].y" -96.692459106445313;
	setAttr ".hyp[151].isf" yes;
	setAttr ".hyp[153].x" 136.16706848144531;
	setAttr ".hyp[153].y" -196.31689453125;
	setAttr ".hyp[153].isf" yes;
	setAttr ".hyp[160].x" 1964.308349609375;
	setAttr ".hyp[160].y" -96.692459106445313;
	setAttr ".hyp[160].isf" yes;
	setAttr ".hyp[166].x" 136.16706848144531;
	setAttr ".hyp[166].y" -196.31689453125;
	setAttr ".hyp[166].isf" yes;
	setAttr ".hyp[206].x" 830.17529296875;
	setAttr ".hyp[206].y" -130.80122375488281;
	setAttr ".hyp[206].isf" yes;
	setAttr ".hyp[208].x" 136.16706848144531;
	setAttr ".hyp[208].y" -196.31689453125;
	setAttr ".hyp[208].isf" yes;
	setAttr ".hyp[209].x" 136.16706848144531;
	setAttr ".hyp[209].y" -196.31689453125;
	setAttr ".hyp[209].isf" yes;
	setAttr ".hyp[210].x" 136.16706848144531;
	setAttr ".hyp[210].y" -196.31689453125;
	setAttr ".hyp[210].isf" yes;
	setAttr ".hyp[211].x" 136.16706848144531;
	setAttr ".hyp[211].y" -196.31689453125;
	setAttr ".hyp[211].isf" yes;
	setAttr ".hyp[212].x" 136.16706848144531;
	setAttr ".hyp[212].y" -196.31689453125;
	setAttr ".hyp[212].isf" yes;
	setAttr ".hyp[213].x" 136.16706848144531;
	setAttr ".hyp[213].y" -196.31689453125;
	setAttr ".hyp[213].isf" yes;
	setAttr ".hyp[214].x" 136.16706848144531;
	setAttr ".hyp[214].y" -196.31689453125;
	setAttr ".hyp[214].isf" yes;
	setAttr ".hyp[244].x" 153.7982177734375;
	setAttr ".hyp[244].y" -196.5198974609375;
	setAttr ".hyp[244].isf" yes;
	setAttr ".hyp[245].x" 405.85763549804687;
	setAttr ".hyp[245].y" -458.57723999023437;
	setAttr ".hyp[245].isf" yes;
	setAttr ".hyp[246].x" 145.19580078125;
	setAttr ".hyp[246].y" -263.07321166992187;
	setAttr ".hyp[246].isf" yes;
	setAttr ".hyp[247].x" 906.55682373046875;
	setAttr ".hyp[247].y" 61.4593505859375;
	setAttr ".hyp[247].isf" yes;
	setAttr ".hyp[248].x" -146.19102478027344;
	setAttr ".hyp[248].y" 25.004062652587891;
	setAttr ".hyp[248].isf" yes;
	setAttr ".hyp[249].x" 1674.308349609375;
	setAttr ".hyp[249].y" -136.69245910644531;
	setAttr ".hyp[249].isf" yes;
	setAttr ".hyp[250].x" 1809.6160888671875;
	setAttr ".hyp[250].y" 52.730655670166016;
	setAttr ".hyp[250].isf" yes;
	setAttr ".hyp[252].x" 371.80551147460938;
	setAttr ".hyp[252].y" -259.8291015625;
	setAttr ".hyp[252].isf" yes;
	setAttr ".hyp[314].x" -111;
	setAttr ".hyp[314].y" -19.492610931396484;
	setAttr ".hyp[314].isf" yes;
	setAttr ".hyp[315].x" -111;
	setAttr ".hyp[315].y" -99.49261474609375;
	setAttr ".hyp[315].isf" yes;
	setAttr ".hyp[316].x" -111;
	setAttr ".hyp[316].y" -139.49261474609375;
	setAttr ".hyp[316].isf" yes;
	setAttr ".hyp[317].x" -111;
	setAttr ".hyp[317].y" -179.49261474609375;
	setAttr ".hyp[317].isf" yes;
	setAttr ".hyp[318].x" -111;
	setAttr ".hyp[318].y" -219.49261474609375;
	setAttr ".hyp[318].isf" yes;
	setAttr ".hyp[319].x" 147;
	setAttr ".hyp[319].y" -21.373153686523437;
	setAttr ".hyp[319].isf" yes;
	setAttr ".hyp[320].x" 147;
	setAttr ".hyp[320].y" -58.626846313476562;
	setAttr ".hyp[320].isf" yes;
	setAttr ".hyp[321].x" 179.23428344726562;
	setAttr ".hyp[321].y" -313.75698852539062;
	setAttr ".hyp[321].isf" yes;
	setAttr ".hyp[322].x" 374.65866088867187;
	setAttr ".hyp[322].y" -30.074501037597656;
	setAttr ".hyp[322].isf" yes;
	setAttr ".hyp[323].x" 376.95169067382812;
	setAttr ".hyp[323].y" -69.024261474609375;
	setAttr ".hyp[323].isf" yes;
	setAttr ".hyp[324].x" 375.82974243164062;
	setAttr ".hyp[324].y" -109.58522796630859;
	setAttr ".hyp[324].isf" yes;
connectAttr "cameraView1.msg" ":perspShape.b" -na;
connectAttr "cameraView2.msg" ":perspShape.b" -na;
connectAttr "groupId1.id" "i_m_hangarShape.iog.og[0].gid";
connectAttr ":initialShadingGroup.mwc" "i_m_hangarShape.iog.og[0].gco";
connectAttr "groupParts1.og" "i_m_hangarShape.i";
connectAttr "groupId2.id" "i_m_hangarShape.ciog.cog[0].cgid";
connectAttr "polyTweakUV3.out" "m_hangarShape.i";
connectAttr "polyTweakUV3.uvtk[0]" "m_hangarShape.uvst[0].uvtw";
connectAttr "groupId21.id" "m_rackShape.iog.og[0].gid";
connectAttr ":initialShadingGroup.mwc" "m_rackShape.iog.og[0].gco";
connectAttr "groupId23.id" "m_rackShape.iog.og[1].gid";
connectAttr "phong5SG.mwc" "m_rackShape.iog.og[1].gco";
connectAttr "deleteComponent30.og" "m_rackShape.i";
connectAttr "groupId22.id" "m_rackShape.ciog.cog[0].cgid";
connectAttr "groupParts3.og" "|m_hangar|m_rack|phys_rack1|phys_rackShape1.i";
connectAttr "groupId5.id" "|i_m_rack3|transform9|phys_rackShape1.iog.og[0].gid";
connectAttr "phong5SG.mwc" "|i_m_rack3|transform9|phys_rackShape1.iog.og[0].gco"
		;
connectAttr "groupId7.id" "|i_m_rack4|transform8|phys_rackShape1.iog.og[0].gid";
connectAttr "phong5SG.mwc" "|i_m_rack4|transform8|phys_rackShape1.iog.og[0].gco"
		;
connectAttr "groupId6.id" "|i_m_rack3|transform9|phys_rackShape1.ciog.cog[0].cgid"
		;
connectAttr "groupId8.id" "|i_m_rack4|transform8|phys_rackShape1.ciog.cog[0].cgid"
		;
connectAttr "groupParts9.og" "|m_hangar|m_rack|phys_rackPillar1|phys_rackPillar1Shape.i"
		;
connectAttr "groupId9.id" "|i_m_rackPillar7|transform7|phys_rackPillar1Shape.iog.og[0].gid"
		;
connectAttr ":initialShadingGroup.mwc" "|i_m_rackPillar7|transform7|phys_rackPillar1Shape.iog.og[0].gco"
		;
connectAttr "groupId11.id" "|i_m_rackPillar8|transform6|phys_rackPillar1Shape.iog.og[0].gid"
		;
connectAttr ":initialShadingGroup.mwc" "|i_m_rackPillar8|transform6|phys_rackPillar1Shape.iog.og[0].gco"
		;
connectAttr "groupId13.id" "|i_m_rackPillar9|transform5|phys_rackPillar1Shape.iog.og[0].gid"
		;
connectAttr ":initialShadingGroup.mwc" "|i_m_rackPillar9|transform5|phys_rackPillar1Shape.iog.og[0].gco"
		;
connectAttr "groupId15.id" "|i_m_rackPillar10|transform4|phys_rackPillar1Shape.iog.og[0].gid"
		;
connectAttr ":initialShadingGroup.mwc" "|i_m_rackPillar10|transform4|phys_rackPillar1Shape.iog.og[0].gco"
		;
connectAttr "groupId17.id" "|i_m_rackPillar11|transform3|phys_rackPillar1Shape.iog.og[0].gid"
		;
connectAttr ":initialShadingGroup.mwc" "|i_m_rackPillar11|transform3|phys_rackPillar1Shape.iog.og[0].gco"
		;
connectAttr "groupId19.id" "|i_m_rackPillar12|transform2|phys_rackPillar1Shape.iog.og[0].gid"
		;
connectAttr ":initialShadingGroup.mwc" "|i_m_rackPillar12|transform2|phys_rackPillar1Shape.iog.og[0].gco"
		;
connectAttr "groupId10.id" "|i_m_rackPillar7|transform7|phys_rackPillar1Shape.ciog.cog[0].cgid"
		;
connectAttr "groupId12.id" "|i_m_rackPillar8|transform6|phys_rackPillar1Shape.ciog.cog[0].cgid"
		;
connectAttr "groupId14.id" "|i_m_rackPillar9|transform5|phys_rackPillar1Shape.ciog.cog[0].cgid"
		;
connectAttr "groupId16.id" "|i_m_rackPillar10|transform4|phys_rackPillar1Shape.ciog.cog[0].cgid"
		;
connectAttr "groupId18.id" "|i_m_rackPillar11|transform3|phys_rackPillar1Shape.ciog.cog[0].cgid"
		;
connectAttr "groupId20.id" "|i_m_rackPillar12|transform2|phys_rackPillar1Shape.ciog.cog[0].cgid"
		;
connectAttr "polyCube8.out" "phys_rackShape.i";
connectAttr "polyTweakUV2.out" "m_floorShape.i";
connectAttr "polyTweakUV2.uvtk[0]" "m_floorShape.uvst[1].uvtw";
connectAttr "polyCube3.out" "phys_floorShape.i";
connectAttr "polyCube4.out" "|m_hangar|phys_wall1|phys_wallShape1.i";
connectAttr "phys_wall3.out" "phys_ceilingShape.i";
connectAttr "phys_wall4.out" "phys_hangarShape.i";
connectAttr "polyPlanarProj2.out" "|m_hangar|m_startingpad|phys_startingpad|m_landingpadShape.i"
		;
connectAttr "polyCube2.out" "phys_pos_startShape.i";
connectAttr "level_start_kit_polyCube3.out" "phys_trig_landingShape.i";
connectAttr "polyCube7.out" "phys_pos_simShape.i";
connectAttr "polySphere1.out" "|m_hangar|phys_pos_path_start|phys_pos_path_Shape1.i"
		;
connectAttr "tweakSet1.mwc" "|i_m_cabin|transform6|i_m_cabinShape.iog.og[1].gco"
		;
connectAttr "helicopter_01_groupId2.id" "|i_m_cabin|transform6|i_m_cabinShape.iog.og[1].gid"
		;
connectAttr "helicopter_01_groupId16.id" "|i_m_cabin|transform6|i_m_cabinShape.iog.og[37].gid"
		;
connectAttr "lambert2SG.mwc" "|i_m_cabin|transform6|i_m_cabinShape.iog.og[37].gco"
		;
connectAttr "groupParts13.og" "|i_m_cabin|transform6|i_m_cabinShape.i";
connectAttr "helicopter_01_groupId17.id" "|i_m_cabin|transform6|i_m_cabinShape.ciog.cog[0].cgid"
		;
connectAttr "helicopter_01_groupParts10.og" "i_m_engineShape1.i";
connectAttr "helicopter_01_groupId10.id" "i_m_engineShape1.iog.og[2].gid";
connectAttr "lambert3SG.mwc" "i_m_engineShape1.iog.og[2].gco";
connectAttr "helicopter_01_groupId11.id" "i_m_engineShape1.ciog.cog[2].cgid";
connectAttr "helicopter_01_groupParts11.og" "i_m_engineShape0.i";
connectAttr "helicopter_01_groupId12.id" "i_m_engineShape0.iog.og[2].gid";
connectAttr "lambert3SG.mwc" "i_m_engineShape0.iog.og[2].gco";
connectAttr "helicopter_01_groupId13.id" "i_m_engineShape0.ciog.cog[2].cgid";
connectAttr "groupParts14.og" "i_m_tailShape.i";
connectAttr "helicopter_01_groupId18.id" "i_m_tailShape.iog.og[0].gid";
connectAttr "lambert2SG.mwc" "i_m_tailShape.iog.og[0].gco";
connectAttr "helicopter_01_groupId19.id" "i_m_tailShape.ciog.cog[0].cgid";
connectAttr "groupParts15.og" "i_m_towerShape.i";
connectAttr "helicopter_01_groupId20.id" "i_m_towerShape.iog.og[0].gid";
connectAttr "lambert2SG.mwc" "i_m_towerShape.iog.og[0].gco";
connectAttr "helicopter_01_groupId21.id" "i_m_towerShape.ciog.cog[0].cgid";
connectAttr "groupParts16.og" "i_m_tail_finShape.i";
connectAttr "helicopter_01_groupId22.id" "i_m_tail_finShape.iog.og[0].gid";
connectAttr "lambert2SG.mwc" "i_m_tail_finShape.iog.og[0].gco";
connectAttr "helicopter_01_groupId23.id" "i_m_tail_finShape.ciog.cog[0].cgid";
connectAttr "groupParts19.og" "|i_m_landingbar0|transform12|i_m_landingbarShape0.i"
		;
connectAttr "groupId26.id" "|i_m_landingbar0|transform12|i_m_landingbarShape0.iog.og[0].gid"
		;
connectAttr "lambert3SG.mwc" "|i_m_landingbar0|transform12|i_m_landingbarShape0.iog.og[0].gco"
		;
connectAttr "lambert3SG.mwc" "|i_m_landingbar1|transform11|i_m_landingbarShape0.iog.og[0].gco"
		;
connectAttr "groupId28.id" "|i_m_landingbar1|transform11|i_m_landingbarShape0.iog.og[0].gid"
		;
connectAttr "groupId27.id" "|i_m_landingbar0|transform12|i_m_landingbarShape0.ciog.cog[0].cgid"
		;
connectAttr "groupId29.id" "|i_m_landingbar1|transform11|i_m_landingbarShape0.ciog.cog[0].cgid"
		;
connectAttr "groupParts23.og" "|i_m_foot0|transform10|i_m_footShape0.i";
connectAttr "groupId30.id" "|i_m_foot0|transform10|i_m_footShape0.iog.og[0].gid"
		;
connectAttr "lambert3SG.mwc" "|i_m_foot0|transform10|i_m_footShape0.iog.og[0].gco"
		;
connectAttr "lambert3SG.mwc" "|i_m_foot2|transform9|i_m_footShape0.iog.og[0].gco"
		;
connectAttr "groupId32.id" "|i_m_foot2|transform9|i_m_footShape0.iog.og[0].gid";
connectAttr "lambert3SG.mwc" "|i_m_foot1|transform8|i_m_footShape0.iog.og[0].gco"
		;
connectAttr "groupId34.id" "|i_m_foot1|transform8|i_m_footShape0.iog.og[0].gid";
connectAttr "lambert3SG.mwc" "|i_m_foot3|transform7|i_m_footShape0.iog.og[0].gco"
		;
connectAttr "groupId36.id" "|i_m_foot3|transform7|i_m_footShape0.iog.og[0].gid";
connectAttr "groupId31.id" "|i_m_foot0|transform10|i_m_footShape0.ciog.cog[0].cgid"
		;
connectAttr "groupId33.id" "|i_m_foot2|transform9|i_m_footShape0.ciog.cog[0].cgid"
		;
connectAttr "groupId35.id" "|i_m_foot1|transform8|i_m_footShape0.ciog.cog[0].cgid"
		;
connectAttr "groupId37.id" "|i_m_foot3|transform7|i_m_footShape0.ciog.cog[0].cgid"
		;
connectAttr "lambert2SG.mwc" "m_bodyShape.iog.og[0].gco";
connectAttr "groupId24.id" "m_bodyShape.iog.og[0].gid";
connectAttr "groupParts17.og" "m_bodyShape.i";
connectAttr "groupId25.id" "m_bodyShape.ciog.cog[0].cgid";
connectAttr "heli_crap_tweakSet1.mwc" "|i_heli_crap_i_m_cabin|transform6|i_m_cabinShape.iog.og[1].gco"
		;
connectAttr "heli_crap_groupId2.id" "|i_heli_crap_i_m_cabin|transform6|i_m_cabinShape.iog.og[1].gid"
		;
connectAttr "heli_crap_groupId16.id" "|i_heli_crap_i_m_cabin|transform6|i_m_cabinShape.iog.og[37].gid"
		;
connectAttr "heli_crap_groupParts13.og" "|i_heli_crap_i_m_cabin|transform6|i_m_cabinShape.i"
		;
connectAttr "heli_crap_groupId17.id" "|i_heli_crap_i_m_cabin|transform6|i_m_cabinShape.ciog.cog[0].cgid"
		;
connectAttr "groupId43.id" "polySurfaceShape1.iog.og[1].gid";
connectAttr "groupParts26.og" "polySurfaceShape1.i";
connectAttr "groupId42.id" "polySurfaceShape1.ciog.cog[0].cgid";
connectAttr "groupId45.id" "polySurfaceShape2.iog.og[1].gid";
connectAttr "groupId46.id" "polySurfaceShape2.ciog.cog[1].cgid";
connectAttr "groupId47.id" "i_pCubeShape1.iog.og[0].gid";
connectAttr "groupParts28.og" "i_pCubeShape1.i";
connectAttr "groupId48.id" "i_pCubeShape1.ciog.cog[0].cgid";
connectAttr "groupId51.id" "i_pCubeShape2.iog.og[0].gid";
connectAttr "groupId52.id" "i_pCubeShape2.ciog.cog[0].cgid";
connectAttr "groupId55.id" "i_pCubeShape3.iog.og[0].gid";
connectAttr "lambert2SG.mwc" "i_pCubeShape3.iog.og[0].gco";
connectAttr "groupId56.id" "i_pCubeShape3.ciog.cog[0].cgid";
connectAttr "groupId49.id" "i_polySurfaceShape3.iog.og[0].gid";
connectAttr "helicopter_01_deleteComponent19.og" "i_polySurfaceShape3.i";
connectAttr "groupId50.id" "i_polySurfaceShape3.ciog.cog[0].cgid";
connectAttr "groupId53.id" "i_polySurfaceShape4.iog.og[0].gid";
connectAttr "helicopter_01_deleteComponent16.og" "i_polySurfaceShape4.i";
connectAttr "groupId54.id" "i_polySurfaceShape4.ciog.cog[0].cgid";
connectAttr "groupId59.id" "i_pCubeShape4.iog.og[0].gid";
connectAttr "lambert2SG.mwc" "i_pCubeShape4.iog.og[0].gco";
connectAttr "groupId60.id" "i_pCubeShape4.ciog.cog[0].cgid";
connectAttr "groupId57.id" "polySurfaceShape3.iog.og[0].gid";
connectAttr "lambert2SG.mwc" "polySurfaceShape3.iog.og[0].gco";
connectAttr "groupParts31.og" "polySurfaceShape3.i";
connectAttr "groupId58.id" "polySurfaceShape3.ciog.cog[0].cgid";
connectAttr "groupId61.id" "polySurfaceShape4.iog.og[0].gid";
connectAttr "lambert2SG.mwc" "polySurfaceShape4.iog.og[0].gco";
connectAttr "polyMergeVert5.out" "polySurfaceShape4.i";
connectAttr "groupId62.id" "polySurfaceShape4.ciog.cog[0].cgid";
connectAttr "groupId63.id" "polySurfaceShape5.iog.og[1].gid";
connectAttr "lambert2SG.mwc" "polySurfaceShape5.iog.og[1].gco";
connectAttr "groupParts33.og" "polySurfaceShape5.i";
connectAttr "groupId64.id" "polySurfaceShape5.ciog.cog[1].cgid";
connectAttr "groupId67.id" "i_polySurfaceShape6.iog.og[1].gid";
connectAttr "lambert2SG.mwc" "i_polySurfaceShape6.iog.og[1].gco";
connectAttr "groupParts35.og" "i_polySurfaceShape6.i";
connectAttr "groupId68.id" "i_polySurfaceShape6.ciog.cog[1].cgid";
connectAttr "groupId65.id" "i_polySurfaceShape7.iog.og[0].gid";
connectAttr "lambert2SG.mwc" "i_polySurfaceShape7.iog.og[0].gco";
connectAttr "groupParts34.og" "i_polySurfaceShape7.i";
connectAttr "groupId66.id" "i_polySurfaceShape7.ciog.cog[0].cgid";
connectAttr "groupId79.id" "i_m_joystickShape.iog.og[0].gid";
connectAttr "helicopter_01_phong3SG.mwc" "i_m_joystickShape.iog.og[0].gco";
connectAttr "groupParts41.og" "i_m_joystickShape.i";
connectAttr "groupId80.id" "i_m_joystickShape.ciog.cog[0].cgid";
connectAttr "lambert3SG.mwc" "pCylinderShape1.iog.og[1].gco";
connectAttr "groupId83.id" "pCylinderShape1.iog.og[1].gid";
connectAttr "groupParts43.og" "pCylinderShape1.i";
connectAttr "groupId84.id" "pCylinderShape1.ciog.cog[1].cgid";
connectAttr "helicopter_01_groupId14.id" "m_engineShape.iog.og[0].gid";
connectAttr "lambert3SG.mwc" "m_engineShape.iog.og[0].gco";
connectAttr "polySplit31.out" "m_engineShape.i";
connectAttr "helicopter_01_groupId15.id" "m_engineShape.ciog.cog[0].cgid";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[0].llnk";
connectAttr ":initialShadingGroup.msg" "lightLinker1.lnk[0].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[1].llnk";
connectAttr ":initialParticleSE.msg" "lightLinker1.lnk[1].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[2].llnk";
connectAttr "phong1SG.msg" "lightLinker1.lnk[2].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[3].llnk";
connectAttr "landingpad_phong1SG.msg" "lightLinker1.lnk[3].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[4].llnk";
connectAttr "phong3SG.msg" "lightLinker1.lnk[4].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[5].llnk";
connectAttr "phong4SG.msg" "lightLinker1.lnk[5].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[6].llnk";
connectAttr "level_start_kit_phong3SG.msg" "lightLinker1.lnk[6].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[7].llnk";
connectAttr "phong5SG.msg" "lightLinker1.lnk[7].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[8].llnk";
connectAttr "lambert2SG.msg" "lightLinker1.lnk[8].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[9].llnk";
connectAttr "lambert3SG.msg" "lightLinker1.lnk[9].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[10].llnk";
connectAttr "lambert4SG.msg" "lightLinker1.lnk[10].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[11].llnk";
connectAttr "blinn1SG.msg" "lightLinker1.lnk[11].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[12].llnk";
connectAttr "pasted__phong2SG.msg" "lightLinker1.lnk[12].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[13].llnk";
connectAttr "helicopter_01_phong1SG.msg" "lightLinker1.lnk[13].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[14].llnk";
connectAttr "phong2SG.msg" "lightLinker1.lnk[14].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[15].llnk";
connectAttr "helicopter_01_phong3SG.msg" "lightLinker1.lnk[15].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[16].llnk";
connectAttr "helicopter_01_phong4SG.msg" "lightLinker1.lnk[16].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[17].llnk";
connectAttr "helicopter_01_phong5SG.msg" "lightLinker1.lnk[17].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[18].llnk";
connectAttr "phong8SG.msg" "lightLinker1.lnk[18].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[0].sllk";
connectAttr ":initialShadingGroup.msg" "lightLinker1.slnk[0].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[1].sllk";
connectAttr ":initialParticleSE.msg" "lightLinker1.slnk[1].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[2].sllk";
connectAttr "phong1SG.msg" "lightLinker1.slnk[2].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[3].sllk";
connectAttr "phong5SG.msg" "lightLinker1.slnk[3].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[4].sllk";
connectAttr "phong8SG.msg" "lightLinker1.slnk[4].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[5].sllk";
connectAttr "landingpad_phong1SG.msg" "lightLinker1.slnk[5].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[6].sllk";
connectAttr "phong3SG.msg" "lightLinker1.slnk[6].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[7].sllk";
connectAttr "phong4SG.msg" "lightLinker1.slnk[7].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[8].sllk";
connectAttr "level_start_kit_phong3SG.msg" "lightLinker1.slnk[8].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[10].sllk";
connectAttr "lambert2SG.msg" "lightLinker1.slnk[10].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[11].sllk";
connectAttr "lambert3SG.msg" "lightLinker1.slnk[11].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[12].sllk";
connectAttr "lambert4SG.msg" "lightLinker1.slnk[12].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[13].sllk";
connectAttr "blinn1SG.msg" "lightLinker1.slnk[13].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[14].sllk";
connectAttr "helicopter_01_phong1SG.msg" "lightLinker1.slnk[14].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[15].sllk";
connectAttr "phong2SG.msg" "lightLinker1.slnk[15].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[16].sllk";
connectAttr "pasted__phong2SG.msg" "lightLinker1.slnk[16].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[17].sllk";
connectAttr "helicopter_01_phong3SG.msg" "lightLinker1.slnk[17].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[18].sllk";
connectAttr "helicopter_01_phong4SG.msg" "lightLinker1.slnk[18].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[19].sllk";
connectAttr "helicopter_01_phong5SG.msg" "lightLinker1.slnk[19].solk";
connectAttr "layerManager.dli[0]" "defaultLayer.id";
connectAttr "renderLayerManager.rlmi[0]" "defaultRenderLayer.rlid";
connectAttr "polyCube1.out" "polySplit1.ip";
connectAttr "polySplit1.out" "polySplit2.ip";
connectAttr "polySplit2.out" "polySplit3.ip";
connectAttr "polyTweak1.out" "polyExtrudeFace1.ip";
connectAttr "i_m_hangarShape.wm" "polyExtrudeFace1.mp";
connectAttr "polySplit3.out" "polyTweak1.ip";
connectAttr "polyExtrudeFace1.out" "polyExtrudeFace2.ip";
connectAttr "i_m_hangarShape.wm" "polyExtrudeFace2.mp";
connectAttr "polyExtrudeFace2.out" "polyExtrudeFace3.ip";
connectAttr "i_m_hangarShape.wm" "polyExtrudeFace3.mp";
connectAttr "polyExtrudeFace3.out" "deleteComponent1.ig";
connectAttr "deleteComponent1.og" "deleteComponent2.ig";
connectAttr "deleteComponent2.og" "deleteComponent3.ig";
connectAttr "deleteComponent3.og" "deleteComponent4.ig";
connectAttr "deleteComponent4.og" "polyTweak2.ip";
connectAttr "polyTweak2.out" "deleteComponent5.ig";
connectAttr "deleteComponent5.og" "polyMergeVert1.ip";
connectAttr "i_m_hangarShape.wm" "polyMergeVert1.mp";
connectAttr "polyMergeVert1.out" "polyMergeVert2.ip";
connectAttr "i_m_hangarShape.wm" "polyMergeVert2.mp";
connectAttr "polyMergeVert2.out" "polyMergeVert3.ip";
connectAttr "i_m_hangarShape.wm" "polyMergeVert3.mp";
connectAttr "polyMergeVert3.out" "polyMergeVert4.ip";
connectAttr "i_m_hangarShape.wm" "polyMergeVert4.mp";
connectAttr "polyMergeVert4.out" "deleteComponent6.ig";
connectAttr "deleteComponent6.og" "deleteComponent7.ig";
connectAttr "deleteComponent7.og" "deleteComponent8.ig";
connectAttr "deleteComponent8.og" "deleteComponent9.ig";
connectAttr "deleteComponent9.og" "deleteComponent10.ig";
connectAttr "deleteComponent10.og" "deleteComponent11.ig";
connectAttr "deleteComponent11.og" "deleteComponent12.ig";
connectAttr "deleteComponent12.og" "deleteComponent13.ig";
connectAttr "deleteComponent13.og" "deleteComponent14.ig";
connectAttr "deleteComponent14.og" "deleteComponent15.ig";
connectAttr "deleteComponent15.og" "deleteComponent16.ig";
connectAttr "deleteComponent16.og" "deleteComponent17.ig";
connectAttr "deleteComponent17.og" "deleteComponent18.ig";
connectAttr "deleteComponent18.og" "deleteComponent19.ig";
connectAttr "deleteComponent19.og" "deleteComponent20.ig";
connectAttr "deleteComponent20.og" "deleteComponent21.ig";
connectAttr "deleteComponent21.og" "deleteComponent22.ig";
connectAttr "deleteComponent22.og" "deleteComponent23.ig";
connectAttr "deleteComponent23.og" "polyChipOff1.ip";
connectAttr "i_m_hangarShape.wm" "polyChipOff1.mp";
connectAttr "i_m_hangarShape.o" "polySeparate1.ip";
connectAttr "polyChipOff1.out" "groupParts1.ig";
connectAttr "groupId1.id" "groupParts1.gi";
connectAttr "mat_floor.oc" "phong1SG.ss";
connectAttr "m_floorShape.iog" "phong1SG.dsm" -na;
connectAttr "phong1SG.msg" "materialInfo1.sg";
connectAttr "mat_floor.msg" "materialInfo1.m";
connectAttr "phong3SG.msg" "materialInfo3.sg";
connectAttr "mat_platform.msg" "materialInfo3.m";
connectAttr "file2.msg" "materialInfo3.t" -na;
connectAttr "mat_platform.oc" "phong3SG.ss";
connectAttr "file2.oc" "mat_platform.c";
connectAttr "file2.ot" "mat_platform.it";
connectAttr "place2dTexture2.c" "file2.c";
connectAttr "place2dTexture2.tf" "file2.tf";
connectAttr "place2dTexture2.rf" "file2.rf";
connectAttr "place2dTexture2.mu" "file2.mu";
connectAttr "place2dTexture2.mv" "file2.mv";
connectAttr "place2dTexture2.s" "file2.s";
connectAttr "place2dTexture2.wu" "file2.wu";
connectAttr "place2dTexture2.wv" "file2.wv";
connectAttr "place2dTexture2.re" "file2.re";
connectAttr "place2dTexture2.of" "file2.of";
connectAttr "place2dTexture2.r" "file2.ro";
connectAttr "place2dTexture2.n" "file2.n";
connectAttr "place2dTexture2.vt1" "file2.vt1";
connectAttr "place2dTexture2.vt2" "file2.vt2";
connectAttr "place2dTexture2.vt3" "file2.vt3";
connectAttr "place2dTexture2.vc1" "file2.vc1";
connectAttr "place2dTexture2.o" "file2.uv";
connectAttr "place2dTexture2.ofs" "file2.fs";
connectAttr "landingpad_phong1SG.msg" "landingpad_materialInfo1.sg";
connectAttr "mat_phys.msg" "landingpad_materialInfo1.m";
connectAttr "mat_phys.oc" "landingpad_phong1SG.ss";
connectAttr "phys_floorShape.iog" "landingpad_phong1SG.dsm" -na;
connectAttr "|m_hangar|phys_wall1|phys_wallShape1.iog" "landingpad_phong1SG.dsm"
		 -na;
connectAttr "|m_hangar|phys_wall2|phys_wallShape1.iog" "landingpad_phong1SG.dsm"
		 -na;
connectAttr "phys_ceilingShape.iog" "landingpad_phong1SG.dsm" -na;
connectAttr "phys_hangarShape.iog" "landingpad_phong1SG.dsm" -na;
connectAttr "phys_pos_startShape.iog" "landingpad_phong1SG.dsm" -na;
connectAttr "|m_hangar|phys_pos_path_start|phys_pos_path_Shape1.iog" "landingpad_phong1SG.dsm"
		 -na;
connectAttr "|m_hangar|phys_pos_path_1|phys_pos_path_Shape1.iog" "landingpad_phong1SG.dsm"
		 -na;
connectAttr "|m_hangar|phys_pos_path_land|phys_pos_path_Shape1.iog" "landingpad_phong1SG.dsm"
		 -na;
connectAttr "phys_trig_landingShape.iog" "landingpad_phong1SG.dsm" -na;
connectAttr "phys_rackShape.iog" "landingpad_phong1SG.dsm" -na;
connectAttr "|m_hangar|phys_pos_path_2|phys_pos_path_Shape1.iog" "landingpad_phong1SG.dsm"
		 -na;
connectAttr "|m_hangar|phys_pos_path_3|phys_pos_path_Shape1.iog" "landingpad_phong1SG.dsm"
		 -na;
connectAttr "|m_hangar|phys_pos_path_4|phys_pos_path_Shape1.iog" "landingpad_phong1SG.dsm"
		 -na;
connectAttr "file3.oc" "mat_walls.c";
connectAttr "mat_walls.oc" "phong4SG.ss";
connectAttr "m_hangarShape.iog" "phong4SG.dsm" -na;
connectAttr "phong4SG.msg" "materialInfo4.sg";
connectAttr "mat_walls.msg" "materialInfo4.m";
connectAttr "file3.msg" "materialInfo4.t" -na;
connectAttr "place2dTexture3.c" "file3.c";
connectAttr "place2dTexture3.tf" "file3.tf";
connectAttr "place2dTexture3.rf" "file3.rf";
connectAttr "place2dTexture3.mu" "file3.mu";
connectAttr "place2dTexture3.mv" "file3.mv";
connectAttr "place2dTexture3.s" "file3.s";
connectAttr "place2dTexture3.wu" "file3.wu";
connectAttr "place2dTexture3.wv" "file3.wv";
connectAttr "place2dTexture3.re" "file3.re";
connectAttr "place2dTexture3.of" "file3.of";
connectAttr "place2dTexture3.r" "file3.ro";
connectAttr "place2dTexture3.n" "file3.n";
connectAttr "place2dTexture3.vt1" "file3.vt1";
connectAttr "place2dTexture3.vt2" "file3.vt2";
connectAttr "place2dTexture3.vt3" "file3.vt3";
connectAttr "place2dTexture3.vc1" "file3.vc1";
connectAttr "place2dTexture3.o" "file3.uv";
connectAttr "place2dTexture3.ofs" "file3.fs";
connectAttr "polySeparate1.out[0]" "deleteComponent24.ig";
connectAttr "deleteComponent24.og" "deleteComponent25.ig";
connectAttr "deleteComponent25.og" "deleteComponent26.ig";
connectAttr "deleteComponent26.og" "deleteComponent27.ig";
connectAttr "deleteComponent27.og" "deleteComponent28.ig";
connectAttr "deleteComponent28.og" "deleteComponent29.ig";
connectAttr "deleteComponent29.og" "polyTweakUV1.ip";
connectAttr "polySeparate1.out[1]" "polyTweakUV2.ip";
connectAttr "polyTweakUV1.out" "polyPlanarProj3.ip";
connectAttr "m_hangarShape.wm" "polyPlanarProj3.mp";
connectAttr "polyPlanarProj3.out" "polyPlanarProj4.ip";
connectAttr "m_hangarShape.wm" "polyPlanarProj4.mp";
connectAttr "polyPlanarProj4.out" "polyPlanarProj5.ip";
connectAttr "m_hangarShape.wm" "polyPlanarProj5.mp";
connectAttr "polyPlanarProj1.out" "polyPlanarProj2.ip";
connectAttr "|m_hangar|m_startingpad|m_landingpadShape.wm" "polyPlanarProj2.mp";
connectAttr "level_start_kit_polyCube1.out" "polyPlanarProj1.ip";
connectAttr "|m_hangar|m_startingpad|m_landingpadShape.wm" "polyPlanarProj1.mp";
connectAttr "level_start_kit_phong3SG.msg" "level_start_kit_materialInfo3.sg";
connectAttr "level_start_kit_mat_platform.msg" "level_start_kit_materialInfo3.m"
		;
connectAttr "level_start_kit_file2.msg" "level_start_kit_materialInfo3.t" -na;
connectAttr "level_start_kit_mat_platform.oc" "level_start_kit_phong3SG.ss";
connectAttr "|m_hangar|m_startingpad|m_landingpadShape.iog" "level_start_kit_phong3SG.dsm"
		 -na;
connectAttr "|m_hangar|m_startingpad|phys_startingpad|m_landingpadShape.iog" "level_start_kit_phong3SG.dsm"
		 -na;
connectAttr "|m_hangar|m_landingpad|m_landingpadShape.iog" "level_start_kit_phong3SG.dsm"
		 -na;
connectAttr "|m_hangar|m_landingpad|phys_landingpad|m_landingpadShape.iog" "level_start_kit_phong3SG.dsm"
		 -na;
connectAttr "level_start_kit_file2.oc" "level_start_kit_mat_platform.c";
connectAttr "level_start_kit_file2.ot" "level_start_kit_mat_platform.it";
connectAttr "level_start_kit_place2dTexture2.c" "level_start_kit_file2.c";
connectAttr "level_start_kit_place2dTexture2.tf" "level_start_kit_file2.tf";
connectAttr "level_start_kit_place2dTexture2.rf" "level_start_kit_file2.rf";
connectAttr "level_start_kit_place2dTexture2.mu" "level_start_kit_file2.mu";
connectAttr "level_start_kit_place2dTexture2.mv" "level_start_kit_file2.mv";
connectAttr "level_start_kit_place2dTexture2.s" "level_start_kit_file2.s";
connectAttr "level_start_kit_place2dTexture2.wu" "level_start_kit_file2.wu";
connectAttr "level_start_kit_place2dTexture2.wv" "level_start_kit_file2.wv";
connectAttr "level_start_kit_place2dTexture2.re" "level_start_kit_file2.re";
connectAttr "level_start_kit_place2dTexture2.of" "level_start_kit_file2.of";
connectAttr "level_start_kit_place2dTexture2.r" "level_start_kit_file2.ro";
connectAttr "level_start_kit_place2dTexture2.n" "level_start_kit_file2.n";
connectAttr "level_start_kit_place2dTexture2.vt1" "level_start_kit_file2.vt1";
connectAttr "level_start_kit_place2dTexture2.vt2" "level_start_kit_file2.vt2";
connectAttr "level_start_kit_place2dTexture2.vt3" "level_start_kit_file2.vt3";
connectAttr "level_start_kit_place2dTexture2.vc1" "level_start_kit_file2.vc1";
connectAttr "level_start_kit_place2dTexture2.o" "level_start_kit_file2.uv";
connectAttr "level_start_kit_place2dTexture2.ofs" "level_start_kit_file2.fs";
connectAttr "polyPlanarProj5.out" "polyTweakUV3.ip";
connectAttr "mat_rack.oc" "phong5SG.ss";
connectAttr "|m_hangar|m_rack|phys_rack1|phys_rackShape1.iog" "phong5SG.dsm" -na
		;
connectAttr "|m_hangar|m_rack|phys_rack2|phys_rackShape1.iog" "phong5SG.dsm" -na
		;
connectAttr "|i_m_rack3|transform9|phys_rackShape1.iog.og[0]" "phong5SG.dsm" -na
		;
connectAttr "|i_m_rack3|transform9|phys_rackShape1.ciog.cog[0]" "phong5SG.dsm" -na
		;
connectAttr "|i_m_rack4|transform8|phys_rackShape1.iog.og[0]" "phong5SG.dsm" -na
		;
connectAttr "|i_m_rack4|transform8|phys_rackShape1.ciog.cog[0]" "phong5SG.dsm" -na
		;
connectAttr "m_rackShape.iog.og[1]" "phong5SG.dsm" -na;
connectAttr "groupId5.msg" "phong5SG.gn" -na;
connectAttr "groupId6.msg" "phong5SG.gn" -na;
connectAttr "groupId7.msg" "phong5SG.gn" -na;
connectAttr "groupId8.msg" "phong5SG.gn" -na;
connectAttr "groupId23.msg" "phong5SG.gn" -na;
connectAttr "phong5SG.msg" "materialInfo5.sg";
connectAttr "mat_rack.msg" "materialInfo5.m";
connectAttr "|m_hangar|m_rack|phys_rack1|phys_rackShape1.o" "polyUnite1.ip[0]";
connectAttr "|m_hangar|m_rack|phys_rack1|phys_rackShape1.o" "polyUnite1.ip[1]";
connectAttr "|m_hangar|m_rack|phys_rackPillar1|phys_rackPillar1Shape.o" "polyUnite1.ip[2]"
		;
connectAttr "|m_hangar|m_rack|phys_rackPillar1|phys_rackPillar1Shape.o" "polyUnite1.ip[3]"
		;
connectAttr "|m_hangar|m_rack|phys_rackPillar1|phys_rackPillar1Shape.o" "polyUnite1.ip[4]"
		;
connectAttr "|m_hangar|m_rack|phys_rackPillar1|phys_rackPillar1Shape.o" "polyUnite1.ip[5]"
		;
connectAttr "|m_hangar|m_rack|phys_rackPillar1|phys_rackPillar1Shape.o" "polyUnite1.ip[6]"
		;
connectAttr "|m_hangar|m_rack|phys_rackPillar1|phys_rackPillar1Shape.o" "polyUnite1.ip[7]"
		;
connectAttr "|i_m_rack3|transform9|phys_rackShape1.wm" "polyUnite1.im[0]";
connectAttr "|i_m_rack4|transform8|phys_rackShape1.wm" "polyUnite1.im[1]";
connectAttr "|i_m_rackPillar7|transform7|phys_rackPillar1Shape.wm" "polyUnite1.im[2]"
		;
connectAttr "|i_m_rackPillar8|transform6|phys_rackPillar1Shape.wm" "polyUnite1.im[3]"
		;
connectAttr "|i_m_rackPillar9|transform5|phys_rackPillar1Shape.wm" "polyUnite1.im[4]"
		;
connectAttr "|i_m_rackPillar10|transform4|phys_rackPillar1Shape.wm" "polyUnite1.im[5]"
		;
connectAttr "|i_m_rackPillar11|transform3|phys_rackPillar1Shape.wm" "polyUnite1.im[6]"
		;
connectAttr "|i_m_rackPillar12|transform2|phys_rackPillar1Shape.wm" "polyUnite1.im[7]"
		;
connectAttr "polyCube5.out" "groupParts2.ig";
connectAttr "groupId5.id" "groupParts2.gi";
connectAttr "groupParts2.og" "groupParts3.ig";
connectAttr "groupId7.id" "groupParts3.gi";
connectAttr "polyCube6.out" "groupParts4.ig";
connectAttr "groupId9.id" "groupParts4.gi";
connectAttr "groupParts4.og" "groupParts5.ig";
connectAttr "groupId11.id" "groupParts5.gi";
connectAttr "groupParts5.og" "groupParts6.ig";
connectAttr "groupId13.id" "groupParts6.gi";
connectAttr "groupParts6.og" "groupParts7.ig";
connectAttr "groupId15.id" "groupParts7.gi";
connectAttr "groupParts7.og" "groupParts8.ig";
connectAttr "groupId17.id" "groupParts8.gi";
connectAttr "groupParts8.og" "groupParts9.ig";
connectAttr "groupId19.id" "groupParts9.gi";
connectAttr "polyUnite1.out" "groupParts10.ig";
connectAttr "groupId21.id" "groupParts10.gi";
connectAttr "groupParts10.og" "groupParts11.ig";
connectAttr "groupId23.id" "groupParts11.gi";
connectAttr "helicopter_01_groupParts2.og" "tweak1.ip[0].ig";
connectAttr "helicopter_01_groupId2.id" "tweak1.ip[0].gi";
connectAttr "helicopter_01_groupId2.msg" "tweakSet1.gn" -na;
connectAttr "|i_m_cabin|transform6|i_m_cabinShape.iog.og[1]" "tweakSet1.dsm" -na
		;
connectAttr "tweak1.msg" "tweakSet1.ub[0]";
connectAttr "helicopter_01_polySphere1.out" "helicopter_01_groupParts2.ig";
connectAttr "helicopter_01_groupId2.id" "helicopter_01_groupParts2.gi";
connectAttr "tweak1.og[0]" "helicopter_01_deleteComponent1.ig";
connectAttr "helicopter_01_deleteComponent1.og" "helicopter_01_deleteComponent2.ig"
		;
connectAttr "helicopter_01_deleteComponent2.og" "helicopter_01_deleteComponent3.ig"
		;
connectAttr "helicopter_01_deleteComponent3.og" "helicopter_01_deleteComponent4.ig"
		;
connectAttr "helicopter_01_deleteComponent4.og" "helicopter_01_deleteComponent5.ig"
		;
connectAttr "helicopter_01_deleteComponent5.og" "helicopter_01_deleteComponent6.ig"
		;
connectAttr "helicopter_01_deleteComponent6.og" "helicopter_01_deleteComponent7.ig"
		;
connectAttr "helicopter_01_deleteComponent7.og" "helicopter_01_deleteComponent8.ig"
		;
connectAttr "mat_broken_heli.oc" "lambert2SG.ss";
connectAttr "|i_m_cabin|transform6|i_m_cabinShape.iog.og[37]" "lambert2SG.dsm" -na
		;
connectAttr "|i_m_cabin|transform6|i_m_cabinShape.ciog.cog[0]" "lambert2SG.dsm" 
		-na;
connectAttr "i_m_tailShape.iog.og[0]" "lambert2SG.dsm" -na;
connectAttr "i_m_tailShape.ciog.cog[0]" "lambert2SG.dsm" -na;
connectAttr "i_m_towerShape.iog.og[0]" "lambert2SG.dsm" -na;
connectAttr "i_m_towerShape.ciog.cog[0]" "lambert2SG.dsm" -na;
connectAttr "i_m_tail_finShape.iog.og[0]" "lambert2SG.dsm" -na;
connectAttr "i_m_tail_finShape.ciog.cog[0]" "lambert2SG.dsm" -na;
connectAttr "m_bodyShape.iog.og[0]" "lambert2SG.dsm" -na;
connectAttr "|i_heli_crap_i_m_cabin|transform6|i_m_cabinShape.iog.og[37]" "lambert2SG.dsm"
		 -na;
connectAttr "|i_heli_crap_i_m_cabin|transform6|i_m_cabinShape.ciog.cog[0]" "lambert2SG.dsm"
		 -na;
connectAttr "polySurfaceShape1.iog.og[1]" "lambert2SG.dsm" -na;
connectAttr "polySurfaceShape2.iog.og[1]" "lambert2SG.dsm" -na;
connectAttr "i_pCubeShape1.iog.og[0]" "lambert2SG.dsm" -na;
connectAttr "i_pCubeShape1.ciog.cog[0]" "lambert2SG.dsm" -na;
connectAttr "i_polySurfaceShape3.iog.og[0]" "lambert2SG.dsm" -na;
connectAttr "i_pCubeShape2.iog.og[0]" "lambert2SG.dsm" -na;
connectAttr "i_pCubeShape2.ciog.cog[0]" "lambert2SG.dsm" -na;
connectAttr "i_polySurfaceShape4.iog.og[0]" "lambert2SG.dsm" -na;
connectAttr "i_pCubeShape3.iog.og[0]" "lambert2SG.dsm" -na;
connectAttr "i_pCubeShape3.ciog.cog[0]" "lambert2SG.dsm" -na;
connectAttr "polySurfaceShape3.iog.og[0]" "lambert2SG.dsm" -na;
connectAttr "i_pCubeShape4.iog.og[0]" "lambert2SG.dsm" -na;
connectAttr "i_pCubeShape4.ciog.cog[0]" "lambert2SG.dsm" -na;
connectAttr "polySurfaceShape4.iog.og[0]" "lambert2SG.dsm" -na;
connectAttr "polySurfaceShape5.iog.og[1]" "lambert2SG.dsm" -na;
connectAttr "polySurfaceShape5.ciog.cog[1]" "lambert2SG.dsm" -na;
connectAttr "i_polySurfaceShape7.iog.og[0]" "lambert2SG.dsm" -na;
connectAttr "i_polySurfaceShape6.iog.og[1]" "lambert2SG.dsm" -na;
connectAttr "i_polySurfaceShape6.ciog.cog[1]" "lambert2SG.dsm" -na;
connectAttr "helicopter_01_groupId16.msg" "lambert2SG.gn" -na;
connectAttr "helicopter_01_groupId17.msg" "lambert2SG.gn" -na;
connectAttr "helicopter_01_groupId18.msg" "lambert2SG.gn" -na;
connectAttr "helicopter_01_groupId19.msg" "lambert2SG.gn" -na;
connectAttr "helicopter_01_groupId20.msg" "lambert2SG.gn" -na;
connectAttr "helicopter_01_groupId21.msg" "lambert2SG.gn" -na;
connectAttr "helicopter_01_groupId22.msg" "lambert2SG.gn" -na;
connectAttr "helicopter_01_groupId23.msg" "lambert2SG.gn" -na;
connectAttr "groupId55.msg" "lambert2SG.gn" -na;
connectAttr "groupId56.msg" "lambert2SG.gn" -na;
connectAttr "groupId59.msg" "lambert2SG.gn" -na;
connectAttr "groupId60.msg" "lambert2SG.gn" -na;
connectAttr "groupId63.msg" "lambert2SG.gn" -na;
connectAttr "groupId64.msg" "lambert2SG.gn" -na;
connectAttr "groupId67.msg" "lambert2SG.gn" -na;
connectAttr "groupId68.msg" "lambert2SG.gn" -na;
connectAttr "lambert2SG.msg" "helicopter_01_materialInfo1.sg";
connectAttr "mat_broken_heli.msg" "helicopter_01_materialInfo1.m";
connectAttr "helicopter_01_polyTweak1.out" "helicopter_01_polySplit1.ip";
connectAttr "helicopter_01_polyCube1.out" "helicopter_01_polyTweak1.ip";
connectAttr "helicopter_01_polySplit1.out" "helicopter_01_polySplit2.ip";
connectAttr "mat_broken_heli_engine.oc" "lambert3SG.ss";
connectAttr "i_m_engineShape1.iog.og[2]" "lambert3SG.dsm" -na;
connectAttr "i_m_engineShape1.ciog.cog[2]" "lambert3SG.dsm" -na;
connectAttr "i_m_engineShape0.iog.og[2]" "lambert3SG.dsm" -na;
connectAttr "i_m_engineShape0.ciog.cog[2]" "lambert3SG.dsm" -na;
connectAttr "m_engineShape.iog.og[0]" "lambert3SG.dsm" -na;
connectAttr "|i_m_landingbar0|transform12|i_m_landingbarShape0.iog.og[0]" "lambert3SG.dsm"
		 -na;
connectAttr "|i_m_landingbar0|transform12|i_m_landingbarShape0.ciog.cog[0]" "lambert3SG.dsm"
		 -na;
connectAttr "|i_m_landingbar1|transform11|i_m_landingbarShape0.iog.og[0]" "lambert3SG.dsm"
		 -na;
connectAttr "|i_m_landingbar1|transform11|i_m_landingbarShape0.ciog.cog[0]" "lambert3SG.dsm"
		 -na;
connectAttr "|i_m_foot0|transform10|i_m_footShape0.iog.og[0]" "lambert3SG.dsm" -na
		;
connectAttr "|i_m_foot0|transform10|i_m_footShape0.ciog.cog[0]" "lambert3SG.dsm"
		 -na;
connectAttr "|i_m_foot2|transform9|i_m_footShape0.iog.og[0]" "lambert3SG.dsm" -na
		;
connectAttr "|i_m_foot2|transform9|i_m_footShape0.ciog.cog[0]" "lambert3SG.dsm" 
		-na;
connectAttr "|i_m_foot1|transform8|i_m_footShape0.iog.og[0]" "lambert3SG.dsm" -na
		;
connectAttr "|i_m_foot1|transform8|i_m_footShape0.ciog.cog[0]" "lambert3SG.dsm" 
		-na;
connectAttr "|i_m_foot3|transform7|i_m_footShape0.iog.og[0]" "lambert3SG.dsm" -na
		;
connectAttr "|i_m_foot3|transform7|i_m_footShape0.ciog.cog[0]" "lambert3SG.dsm" 
		-na;
connectAttr "pCylinderShape1.iog.og[1]" "lambert3SG.dsm" -na;
connectAttr "pCylinderShape1.ciog.cog[1]" "lambert3SG.dsm" -na;
connectAttr "helicopter_01_groupId10.msg" "lambert3SG.gn" -na;
connectAttr "helicopter_01_groupId11.msg" "lambert3SG.gn" -na;
connectAttr "helicopter_01_groupId12.msg" "lambert3SG.gn" -na;
connectAttr "helicopter_01_groupId13.msg" "lambert3SG.gn" -na;
connectAttr "groupId26.msg" "lambert3SG.gn" -na;
connectAttr "groupId27.msg" "lambert3SG.gn" -na;
connectAttr "groupId28.msg" "lambert3SG.gn" -na;
connectAttr "groupId29.msg" "lambert3SG.gn" -na;
connectAttr "groupId30.msg" "lambert3SG.gn" -na;
connectAttr "groupId31.msg" "lambert3SG.gn" -na;
connectAttr "groupId32.msg" "lambert3SG.gn" -na;
connectAttr "groupId33.msg" "lambert3SG.gn" -na;
connectAttr "groupId34.msg" "lambert3SG.gn" -na;
connectAttr "groupId35.msg" "lambert3SG.gn" -na;
connectAttr "groupId36.msg" "lambert3SG.gn" -na;
connectAttr "groupId37.msg" "lambert3SG.gn" -na;
connectAttr "groupId83.msg" "lambert3SG.gn" -na;
connectAttr "groupId84.msg" "lambert3SG.gn" -na;
connectAttr "lambert3SG.msg" "materialInfo2.sg";
connectAttr "mat_broken_heli_engine.msg" "materialInfo2.m";
connectAttr "phys.oc" "lambert4SG.ss";
connectAttr "lambert4SG.msg" "helicopter_01_materialInfo3.sg";
connectAttr "phys.msg" "helicopter_01_materialInfo3.m";
connectAttr "helicopter_01_polyCube2.out" "helicopter_01_polyTweak2.ip";
connectAttr "helicopter_01_polyTweak2.out" "helicopter_01_deleteComponent9.ig";
connectAttr "helicopter_01_deleteComponent9.og" "helicopter_01_deleteComponent10.ig"
		;
connectAttr "helicopter_01_polySplit2.out" "polyTweak3.ip";
connectAttr "polyTweak3.out" "helicopter_01_deleteComponent11.ig";
connectAttr "helicopter_01_deleteComponent11.og" "helicopter_01_deleteComponent12.ig"
		;
connectAttr "helicopter_01_deleteComponent12.og" "helicopter_01_deleteComponent13.ig"
		;
connectAttr "i_m_engineShape1.o" "helicopter_01_polyUnite1.ip[0]";
connectAttr "i_m_engineShape0.o" "helicopter_01_polyUnite1.ip[1]";
connectAttr "i_m_engineShape1.wm" "helicopter_01_polyUnite1.im[0]";
connectAttr "i_m_engineShape0.wm" "helicopter_01_polyUnite1.im[1]";
connectAttr "helicopter_01_deleteComponent13.og" "helicopter_01_groupParts10.ig"
		;
connectAttr "helicopter_01_groupId10.id" "helicopter_01_groupParts10.gi";
connectAttr "helicopter_01_deleteComponent10.og" "helicopter_01_groupParts11.ig"
		;
connectAttr "helicopter_01_groupId12.id" "helicopter_01_groupParts11.gi";
connectAttr "helicopter_01_polyUnite1.out" "groupParts12.ig";
connectAttr "helicopter_01_groupId14.id" "groupParts12.gi";
connectAttr "groupParts12.og" "helicopter_01_polyMergeVert1.ip";
connectAttr "m_engineShape.wm" "helicopter_01_polyMergeVert1.mp";
connectAttr "helicopter_01_polyMergeVert1.out" "helicopter_01_polyMergeVert2.ip"
		;
connectAttr "m_engineShape.wm" "helicopter_01_polyMergeVert2.mp";
connectAttr "helicopter_01_polyMergeVert2.out" "helicopter_01_polyMergeVert3.ip"
		;
connectAttr "m_engineShape.wm" "helicopter_01_polyMergeVert3.mp";
connectAttr "helicopter_01_polyMergeVert3.out" "helicopter_01_polyMergeVert4.ip"
		;
connectAttr "m_engineShape.wm" "helicopter_01_polyMergeVert4.mp";
connectAttr "|i_m_cabin|transform6|i_m_cabinShape.o" "polyUnite2.ip[0]";
connectAttr "i_m_tailShape.o" "polyUnite2.ip[1]";
connectAttr "i_m_towerShape.o" "polyUnite2.ip[2]";
connectAttr "i_m_tail_finShape.o" "polyUnite2.ip[3]";
connectAttr "|i_m_cabin|transform6|i_m_cabinShape.wm" "polyUnite2.im[0]";
connectAttr "i_m_tailShape.wm" "polyUnite2.im[1]";
connectAttr "i_m_towerShape.wm" "polyUnite2.im[2]";
connectAttr "i_m_tail_finShape.wm" "polyUnite2.im[3]";
connectAttr "helicopter_01_deleteComponent8.og" "groupParts13.ig";
connectAttr "helicopter_01_groupId16.id" "groupParts13.gi";
connectAttr "polyCylinder1.out" "groupParts14.ig";
connectAttr "helicopter_01_groupId18.id" "groupParts14.gi";
connectAttr "polyCylinder4.out" "groupParts15.ig";
connectAttr "helicopter_01_groupId20.id" "groupParts15.gi";
connectAttr "helicopter_01_polyCube4.out" "groupParts16.ig";
connectAttr "helicopter_01_groupId22.id" "groupParts16.gi";
connectAttr "polyUnite2.out" "groupParts17.ig";
connectAttr "groupId24.id" "groupParts17.gi";
connectAttr "polyCylinder2.out" "groupParts18.ig";
connectAttr "groupId26.id" "groupParts18.gi";
connectAttr "groupParts18.og" "groupParts19.ig";
connectAttr "groupId28.id" "groupParts19.gi";
connectAttr "polyCylinder3.out" "groupParts20.ig";
connectAttr "groupId30.id" "groupParts20.gi";
connectAttr "groupParts20.og" "groupParts21.ig";
connectAttr "groupId32.id" "groupParts21.gi";
connectAttr "groupParts21.og" "groupParts22.ig";
connectAttr "groupId34.id" "groupParts22.gi";
connectAttr "groupParts22.og" "groupParts23.ig";
connectAttr "groupId36.id" "groupParts23.gi";
connectAttr "helicopter_01_deleteComponent18.og" "helicopter_01_deleteComponent19.ig"
		;
connectAttr "helicopter_01_deleteComponent17.og" "helicopter_01_deleteComponent18.ig"
		;
connectAttr "groupParts29.og" "helicopter_01_deleteComponent17.ig";
connectAttr "polyBoolOp1.out" "groupParts29.ig";
connectAttr "groupId49.id" "groupParts29.gi";
connectAttr "polySurfaceShape2.o" "polyBoolOp1.ip[0]";
connectAttr "i_pCubeShape1.o" "polyBoolOp1.ip[1]";
connectAttr "polySurfaceShape2.wm" "polyBoolOp1.im[0]";
connectAttr "i_pCubeShape1.wm" "polyBoolOp1.im[1]";
connectAttr "helicopter_01_deleteComponent15.og" "helicopter_01_deleteComponent16.ig"
		;
connectAttr "helicopter_01_deleteComponent14.og" "helicopter_01_deleteComponent15.ig"
		;
connectAttr "groupParts30.og" "helicopter_01_deleteComponent14.ig";
connectAttr "polyBoolOp2.out" "groupParts30.ig";
connectAttr "groupId53.id" "groupParts30.gi";
connectAttr "polySurfaceShape1.o" "polyBoolOp2.ip[0]";
connectAttr "i_pCubeShape2.o" "polyBoolOp2.ip[1]";
connectAttr "polySurfaceShape1.wm" "polyBoolOp2.im[0]";
connectAttr "i_pCubeShape2.wm" "polyBoolOp2.im[1]";
connectAttr "groupParts25.og" "helicopter_01_polySeparate1.ip";
connectAttr "heli_crap_groupParts17.og" "groupParts25.ig";
connectAttr "groupId40.id" "groupParts25.gi";
connectAttr "heli_crap_polyUnite2.out" "heli_crap_groupParts17.ig";
connectAttr "heli_crap_groupId24.id" "heli_crap_groupParts17.gi";
connectAttr "|i_heli_crap_i_m_cabin|transform6|i_m_cabinShape.o" "heli_crap_polyUnite2.ip[0]"
		;
connectAttr "heli_crap_polyCylinder1.out" "heli_crap_polyUnite2.ip[1]";
connectAttr "heli_crap_polyCylinder4.out" "heli_crap_polyUnite2.ip[2]";
connectAttr "heli_crap_polyCube4.out" "heli_crap_polyUnite2.ip[3]";
connectAttr "|i_heli_crap_i_m_cabin|transform6|i_m_cabinShape.wm" "heli_crap_polyUnite2.im[0]"
		;
connectAttr "heli_crap_groupParts2.og" "heli_crap_tweak1.ip[0].ig";
connectAttr "heli_crap_groupId2.id" "heli_crap_tweak1.ip[0].gi";
connectAttr "heli_crap_groupId2.msg" "heli_crap_tweakSet1.gn" -na;
connectAttr "|i_heli_crap_i_m_cabin|transform6|i_m_cabinShape.iog.og[1]" "heli_crap_tweakSet1.dsm"
		 -na;
connectAttr "heli_crap_tweak1.msg" "heli_crap_tweakSet1.ub[0]";
connectAttr "heli_crap_polySphere1.out" "heli_crap_groupParts2.ig";
connectAttr "heli_crap_groupId2.id" "heli_crap_groupParts2.gi";
connectAttr "heli_crap_deleteComponent2.og" "heli_crap_deleteComponent3.ig";
connectAttr "heli_crap_deleteComponent1.og" "heli_crap_deleteComponent2.ig";
connectAttr "heli_crap_tweak1.og[0]" "heli_crap_deleteComponent1.ig";
connectAttr "heli_crap_deleteComponent6.og" "heli_crap_deleteComponent7.ig";
connectAttr "heli_crap_deleteComponent5.og" "heli_crap_deleteComponent6.ig";
connectAttr "heli_crap_deleteComponent4.og" "heli_crap_deleteComponent5.ig";
connectAttr "heli_crap_deleteComponent3.og" "heli_crap_deleteComponent4.ig";
connectAttr "heli_crap_deleteComponent7.og" "heli_crap_deleteComponent8.ig";
connectAttr "blinn1SG.msg" "helicopter_01_materialInfo4.sg";
connectAttr "mat_broken_heli_window.msg" "helicopter_01_materialInfo4.m";
connectAttr "mat_broken_heli_window.oc" "blinn1SG.ss";
connectAttr "m_bodyShape.o" "polyBoolOp4.ip[0]";
connectAttr "i_pCubeShape3.o" "polyBoolOp4.ip[1]";
connectAttr "m_bodyShape.wm" "polyBoolOp4.im[0]";
connectAttr "i_pCubeShape3.wm" "polyBoolOp4.im[1]";
connectAttr "polyBoolOp4.out" "groupParts31.ig";
connectAttr "groupId57.id" "groupParts31.gi";
connectAttr "polySurfaceShape3.o" "polyBoolOp5.ip[0]";
connectAttr "i_pCubeShape4.o" "polyBoolOp5.ip[1]";
connectAttr "polySurfaceShape3.wm" "polyBoolOp5.im[0]";
connectAttr "i_pCubeShape4.wm" "polyBoolOp5.im[1]";
connectAttr "polyBoolOp5.out" "groupParts32.ig";
connectAttr "groupId61.id" "groupParts32.gi";
connectAttr "groupParts32.og" "helicopter_01_deleteComponent20.ig";
connectAttr "helicopter_01_deleteComponent20.og" "helicopter_01_deleteComponent21.ig"
		;
connectAttr "polyTweak4.out" "polyMergeVert5.ip";
connectAttr "polySurfaceShape4.wm" "polyMergeVert5.mp";
connectAttr "helicopter_01_deleteComponent21.og" "polyTweak4.ip";
connectAttr "polySurfaceShape5.o" "polyUnite4.ip[0]";
connectAttr "polySurfaceShape4.o" "polyUnite4.ip[1]";
connectAttr "polySurfaceShape5.wm" "polyUnite4.im[0]";
connectAttr "polySurfaceShape4.wm" "polyUnite4.im[1]";
connectAttr "polyCreateFace1.out" "groupParts33.ig";
connectAttr "groupId63.id" "groupParts33.gi";
connectAttr "polyUnite4.out" "groupParts34.ig";
connectAttr "groupId65.id" "groupParts34.gi";
connectAttr "polyCreateFace2.out" "groupParts35.ig";
connectAttr "groupId67.id" "groupParts35.gi";
connectAttr "pasted__phong2SG.msg" "pasted__materialInfo8.sg";
connectAttr "mat_black1.msg" "pasted__materialInfo8.m";
connectAttr "pasted__file1.msg" "pasted__materialInfo8.t" -na;
connectAttr "mat_black1.oc" "pasted__phong2SG.ss";
connectAttr "pasted__file1.oc" "mat_black1.c";
connectAttr "pasted__file1.ot" "mat_black1.it";
connectAttr "pasted__place2dTexture2.c" "pasted__file1.c";
connectAttr "pasted__place2dTexture2.tf" "pasted__file1.tf";
connectAttr "pasted__place2dTexture2.rf" "pasted__file1.rf";
connectAttr "pasted__place2dTexture2.mu" "pasted__file1.mu";
connectAttr "pasted__place2dTexture2.mv" "pasted__file1.mv";
connectAttr "pasted__place2dTexture2.s" "pasted__file1.s";
connectAttr "pasted__place2dTexture2.wu" "pasted__file1.wu";
connectAttr "pasted__place2dTexture2.wv" "pasted__file1.wv";
connectAttr "pasted__place2dTexture2.re" "pasted__file1.re";
connectAttr "pasted__place2dTexture2.of" "pasted__file1.of";
connectAttr "pasted__place2dTexture2.r" "pasted__file1.ro";
connectAttr "pasted__place2dTexture2.n" "pasted__file1.n";
connectAttr "pasted__place2dTexture2.vt1" "pasted__file1.vt1";
connectAttr "pasted__place2dTexture2.vt2" "pasted__file1.vt2";
connectAttr "pasted__place2dTexture2.vt3" "pasted__file1.vt3";
connectAttr "pasted__place2dTexture2.vc1" "pasted__file1.vc1";
connectAttr "pasted__place2dTexture2.o" "pasted__file1.uv";
connectAttr "pasted__place2dTexture2.ofs" "pasted__file1.fs";
connectAttr "heli_crap_deleteComponent8.og" "heli_crap_groupParts13.ig";
connectAttr "heli_crap_groupId16.id" "heli_crap_groupParts13.gi";
connectAttr "gray.oc" "helicopter_01_phong1SG.ss";
connectAttr "helicopter_01_phong1SG.msg" "forklift_01_materialInfo1.sg";
connectAttr "gray.msg" "forklift_01_materialInfo1.m";
connectAttr "blue.oc" "phong2SG.ss";
connectAttr "phong2SG.msg" "forklift_01_materialInfo2.sg";
connectAttr "blue.msg" "forklift_01_materialInfo2.m";
connectAttr "mat_black.oc" "helicopter_01_phong3SG.ss";
connectAttr "i_m_joystickShape.iog.og[0]" "helicopter_01_phong3SG.dsm" -na;
connectAttr "i_m_joystickShape.ciog.cog[0]" "helicopter_01_phong3SG.dsm" -na;
connectAttr "groupId74.msg" "helicopter_01_phong3SG.gn" -na;
connectAttr "groupId79.msg" "helicopter_01_phong3SG.gn" -na;
connectAttr "groupId80.msg" "helicopter_01_phong3SG.gn" -na;
connectAttr "helicopter_01_phong3SG.msg" "forklift_01_materialInfo3.sg";
connectAttr "mat_black.msg" "forklift_01_materialInfo3.m";
connectAttr "grey.oc" "helicopter_01_phong4SG.ss";
connectAttr "helicopter_01_phong4SG.msg" "forklift_01_materialInfo4.sg";
connectAttr "grey.msg" "forklift_01_materialInfo4.m";
connectAttr "forklift_01_phys.oc" "helicopter_01_phong5SG.ss";
connectAttr "helicopter_01_phong5SG.msg" "helicopter_01_materialInfo5.sg";
connectAttr "forklift_01_phys.msg" "helicopter_01_materialInfo5.m";
connectAttr "polyCylinder6.out" "groupParts41.ig";
connectAttr "groupId79.id" "groupParts41.gi";
connectAttr "polyTweak5.out" "helicopter_01_polySplit3.ip";
connectAttr "helicopter_01_polyMergeVert4.out" "polyTweak5.ip";
connectAttr "helicopter_01_polySplit3.out" "polySplit4.ip";
connectAttr "polySplit4.out" "polySplit5.ip";
connectAttr "polySplit5.out" "polySplit6.ip";
connectAttr "polySplit6.out" "polySplit7.ip";
connectAttr "polySplit7.out" "polySplit8.ip";
connectAttr "polySplit8.out" "polySplit9.ip";
connectAttr "polyTweak6.out" "polySplit10.ip";
connectAttr "polySplit9.out" "polyTweak6.ip";
connectAttr "polyTweak7.out" "polySplit11.ip";
connectAttr "polySplit10.out" "polyTweak7.ip";
connectAttr "polySplit11.out" "polySplit12.ip";
connectAttr "polySplit12.out" "polySplit13.ip";
connectAttr "polySplit13.out" "polySplit14.ip";
connectAttr "polySplit14.out" "polySplit15.ip";
connectAttr "polySplit15.out" "polySplit16.ip";
connectAttr "polySplit16.out" "polySplit17.ip";
connectAttr "polySplit17.out" "polySplit18.ip";
connectAttr "polySplit18.out" "polyTweak8.ip";
connectAttr "polyTweak8.out" "helicopter_01_deleteComponent22.ig";
connectAttr "helicopter_01_deleteComponent22.og" "helicopter_01_deleteComponent23.ig"
		;
connectAttr "polyTweak9.out" "polyMergeVert6.ip";
connectAttr "m_engineShape.wm" "polyMergeVert6.mp";
connectAttr "helicopter_01_deleteComponent23.og" "polyTweak9.ip";
connectAttr "polyMergeVert6.out" "polyMergeVert7.ip";
connectAttr "m_engineShape.wm" "polyMergeVert7.mp";
connectAttr "polyMergeVert7.out" "polySplit19.ip";
connectAttr "polyTweak10.out" "polySplit20.ip";
connectAttr "polySplit19.out" "polyTweak10.ip";
connectAttr "polyTweak11.out" "polySplit21.ip";
connectAttr "polySplit20.out" "polyTweak11.ip";
connectAttr "polySplit21.out" "polySplit22.ip";
connectAttr "polySplit22.out" "polySplit23.ip";
connectAttr "polyTweak12.out" "polySplit24.ip";
connectAttr "polySplit23.out" "polyTweak12.ip";
connectAttr "polySplit24.out" "polySplit25.ip";
connectAttr "polySplit25.out" "polySplit26.ip";
connectAttr "polySplit26.out" "polySplit27.ip";
connectAttr "polySplit27.out" "polySplit28.ip";
connectAttr "polySplit28.out" "polySplit29.ip";
connectAttr "polyTweak13.out" "polySplit30.ip";
connectAttr "polySplit29.out" "polyTweak13.ip";
connectAttr "polySplit30.out" "polySplit31.ip";
connectAttr "polyCylinder7.out" "groupParts43.ig";
connectAttr "groupId83.id" "groupParts43.gi";
connectAttr "heli_crap_polyCube7.out" "groupParts28.ig";
connectAttr "groupId47.id" "groupParts28.gi";
connectAttr "helicopter_01_polySeparate1.out[0]" "groupParts26.ig";
connectAttr "groupId43.id" "groupParts26.gi";
connectAttr "mat_forklift_path1.oc" "phong8SG.ss";
connectAttr "phong8SG.msg" "materialInfo6.sg";
connectAttr "mat_forklift_path1.msg" "materialInfo6.m";
connectAttr "groupParts11.og" "deleteComponent30.ig";
connectAttr "phong1SG.pa" ":renderPartition.st" -na;
connectAttr "landingpad_phong1SG.pa" ":renderPartition.st" -na;
connectAttr "phong3SG.pa" ":renderPartition.st" -na;
connectAttr "phong4SG.pa" ":renderPartition.st" -na;
connectAttr "level_start_kit_phong3SG.pa" ":renderPartition.st" -na;
connectAttr "phong5SG.pa" ":renderPartition.st" -na;
connectAttr "lambert2SG.pa" ":renderPartition.st" -na;
connectAttr "lambert3SG.pa" ":renderPartition.st" -na;
connectAttr "lambert4SG.pa" ":renderPartition.st" -na;
connectAttr "blinn1SG.pa" ":renderPartition.st" -na;
connectAttr "pasted__phong2SG.pa" ":renderPartition.st" -na;
connectAttr "helicopter_01_phong1SG.pa" ":renderPartition.st" -na;
connectAttr "phong2SG.pa" ":renderPartition.st" -na;
connectAttr "helicopter_01_phong3SG.pa" ":renderPartition.st" -na;
connectAttr "helicopter_01_phong4SG.pa" ":renderPartition.st" -na;
connectAttr "helicopter_01_phong5SG.pa" ":renderPartition.st" -na;
connectAttr "phong8SG.pa" ":renderPartition.st" -na;
connectAttr "mat_floor.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_phys.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_platform.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_walls.msg" ":defaultShaderList1.s" -na;
connectAttr "level_start_kit_mat_platform.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_rack.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_broken_heli.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_broken_heli_engine.msg" ":defaultShaderList1.s" -na;
connectAttr "phys.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_broken_heli_window.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_black1.msg" ":defaultShaderList1.s" -na;
connectAttr "gray.msg" ":defaultShaderList1.s" -na;
connectAttr "blue.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_black.msg" ":defaultShaderList1.s" -na;
connectAttr "grey.msg" ":defaultShaderList1.s" -na;
connectAttr "forklift_01_phys.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_forklift_path1.msg" ":defaultShaderList1.s" -na;
connectAttr "place2dTexture2.msg" ":defaultRenderUtilityList1.u" -na;
connectAttr "place2dTexture3.msg" ":defaultRenderUtilityList1.u" -na;
connectAttr "level_start_kit_place2dTexture2.msg" ":defaultRenderUtilityList1.u"
		 -na;
connectAttr "pasted__place2dTexture2.msg" ":defaultRenderUtilityList1.u" -na;
connectAttr "lightLinker1.msg" ":lightList1.ln" -na;
connectAttr "file2.msg" ":defaultTextureList1.tx" -na;
connectAttr "file3.msg" ":defaultTextureList1.tx" -na;
connectAttr "level_start_kit_file2.msg" ":defaultTextureList1.tx" -na;
connectAttr "pasted__file1.msg" ":defaultTextureList1.tx" -na;
connectAttr "i_m_hangarShape.iog.og[0]" ":initialShadingGroup.dsm" -na;
connectAttr "i_m_hangarShape.ciog.cog[0]" ":initialShadingGroup.dsm" -na;
connectAttr "|m_hangar|m_rack|phys_rackPillar1|phys_rackPillar1Shape.iog" ":initialShadingGroup.dsm"
		 -na;
connectAttr "|m_hangar|m_rack|phys_rackPillar2|phys_rackPillar1Shape.iog" ":initialShadingGroup.dsm"
		 -na;
connectAttr "|m_hangar|m_rack|phys_rackPillar3|phys_rackPillar1Shape.iog" ":initialShadingGroup.dsm"
		 -na;
connectAttr "|m_hangar|m_rack|phys_rackPillar4|phys_rackPillar1Shape.iog" ":initialShadingGroup.dsm"
		 -na;
connectAttr "|m_hangar|m_rack|phys_rackPillar5|phys_rackPillar1Shape.iog" ":initialShadingGroup.dsm"
		 -na;
connectAttr "|m_hangar|m_rack|phys_rackPillar6|phys_rackPillar1Shape.iog" ":initialShadingGroup.dsm"
		 -na;
connectAttr "|i_m_rackPillar7|transform7|phys_rackPillar1Shape.iog.og[0]" ":initialShadingGroup.dsm"
		 -na;
connectAttr "|i_m_rackPillar7|transform7|phys_rackPillar1Shape.ciog.cog[0]" ":initialShadingGroup.dsm"
		 -na;
connectAttr "|i_m_rackPillar8|transform6|phys_rackPillar1Shape.iog.og[0]" ":initialShadingGroup.dsm"
		 -na;
connectAttr "|i_m_rackPillar8|transform6|phys_rackPillar1Shape.ciog.cog[0]" ":initialShadingGroup.dsm"
		 -na;
connectAttr "|i_m_rackPillar9|transform5|phys_rackPillar1Shape.iog.og[0]" ":initialShadingGroup.dsm"
		 -na;
connectAttr "|i_m_rackPillar9|transform5|phys_rackPillar1Shape.ciog.cog[0]" ":initialShadingGroup.dsm"
		 -na;
connectAttr "|i_m_rackPillar10|transform4|phys_rackPillar1Shape.iog.og[0]" ":initialShadingGroup.dsm"
		 -na;
connectAttr "|i_m_rackPillar10|transform4|phys_rackPillar1Shape.ciog.cog[0]" ":initialShadingGroup.dsm"
		 -na;
connectAttr "|i_m_rackPillar11|transform3|phys_rackPillar1Shape.iog.og[0]" ":initialShadingGroup.dsm"
		 -na;
connectAttr "|i_m_rackPillar11|transform3|phys_rackPillar1Shape.ciog.cog[0]" ":initialShadingGroup.dsm"
		 -na;
connectAttr "|i_m_rackPillar12|transform2|phys_rackPillar1Shape.iog.og[0]" ":initialShadingGroup.dsm"
		 -na;
connectAttr "|i_m_rackPillar12|transform2|phys_rackPillar1Shape.ciog.cog[0]" ":initialShadingGroup.dsm"
		 -na;
connectAttr "m_rackShape.iog.og[0]" ":initialShadingGroup.dsm" -na;
connectAttr "m_rackShape.ciog.cog[0]" ":initialShadingGroup.dsm" -na;
connectAttr "m_engineShape.ciog.cog[0]" ":initialShadingGroup.dsm" -na;
connectAttr "m_bodyShape.ciog.cog[0]" ":initialShadingGroup.dsm" -na;
connectAttr "polySurfaceShape1.ciog.cog[0]" ":initialShadingGroup.dsm" -na;
connectAttr "polySurfaceShape2.ciog.cog[1]" ":initialShadingGroup.dsm" -na;
connectAttr "i_polySurfaceShape3.ciog.cog[0]" ":initialShadingGroup.dsm" -na;
connectAttr "i_polySurfaceShape4.ciog.cog[0]" ":initialShadingGroup.dsm" -na;
connectAttr "polySurfaceShape3.ciog.cog[0]" ":initialShadingGroup.dsm" -na;
connectAttr "polySurfaceShape4.ciog.cog[0]" ":initialShadingGroup.dsm" -na;
connectAttr "i_polySurfaceShape7.ciog.cog[0]" ":initialShadingGroup.dsm" -na;
connectAttr "phys_pos_simShape.iog" ":initialShadingGroup.dsm" -na;
connectAttr "groupId1.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId2.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId9.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId10.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId11.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId12.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId13.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId14.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId15.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId16.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId17.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId18.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId19.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId20.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId21.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId22.msg" ":initialShadingGroup.gn" -na;
connectAttr "helicopter_01_groupId14.msg" ":initialShadingGroup.gn" -na;
connectAttr "helicopter_01_groupId15.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId24.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId25.msg" ":initialShadingGroup.gn" -na;
connectAttr "heli_crap_groupId24.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId42.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId46.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId49.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId50.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId53.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId54.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId57.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId58.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId61.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId62.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId65.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId66.msg" ":initialShadingGroup.gn" -na;
connectAttr "m_startingpad.msg" ":hyperGraphLayout.hyp[0].dn";
connectAttr "phys_landingpad.msg" ":hyperGraphLayout.hyp[1].dn";
connectAttr "m_landingpad.msg" ":hyperGraphLayout.hyp[2].dn";
connectAttr "phys_pos_path_start.msg" ":hyperGraphLayout.hyp[4].dn";
connectAttr "phys_pos_path_1.msg" ":hyperGraphLayout.hyp[5].dn";
connectAttr "phys_pos_path_land.msg" ":hyperGraphLayout.hyp[6].dn";
connectAttr "i_m_landingbar1.msg" ":hyperGraphLayout.hyp[7].dn";
connectAttr "i_m_foot2.msg" ":hyperGraphLayout.hyp[9].dn";
connectAttr "i_m_foot3.msg" ":hyperGraphLayout.hyp[10].dn";
connectAttr "i_m_foot0.msg" ":hyperGraphLayout.hyp[11].dn";
connectAttr "i_m_foot1.msg" ":hyperGraphLayout.hyp[12].dn";
connectAttr "i_m_tower.msg" ":hyperGraphLayout.hyp[13].dn";
connectAttr "i_m_cabin.msg" ":hyperGraphLayout.hyp[18].dn";
connectAttr "i_m_tail_fin.msg" ":hyperGraphLayout.hyp[22].dn";
connectAttr "i_m_tail.msg" ":hyperGraphLayout.hyp[23].dn";
connectAttr "i_m_engine1.msg" ":hyperGraphLayout.hyp[24].dn";
connectAttr "i_m_engine0.msg" ":hyperGraphLayout.hyp[25].dn";
connectAttr "|i_m_engine0|transform1.msg" ":hyperGraphLayout.hyp[101].dn";
connectAttr "|i_m_engine1|transform2.msg" ":hyperGraphLayout.hyp[102].dn";
connectAttr "i_m_engine2.msg" ":hyperGraphLayout.hyp[103].dn";
connectAttr "i_m_body_whatever.msg" ":hyperGraphLayout.hyp[104].dn";
connectAttr "i_heli_crap_i_m_cabin.msg" ":hyperGraphLayout.hyp[123].dn";
connectAttr "i_m_body1.msg" ":hyperGraphLayout.hyp[138].dn";
connectAttr "polySurface1.msg" ":hyperGraphLayout.hyp[139].dn";
connectAttr "polySurface2.msg" ":hyperGraphLayout.hyp[140].dn";
connectAttr "i_pCube1.msg" ":hyperGraphLayout.hyp[141].dn";
connectAttr "i_pCube2.msg" ":hyperGraphLayout.hyp[142].dn";
connectAttr "i_pCube3.msg" ":hyperGraphLayout.hyp[143].dn";
connectAttr "i_polySurface3.msg" ":hyperGraphLayout.hyp[144].dn";
connectAttr "i_polySurface4.msg" ":hyperGraphLayout.hyp[145].dn";
connectAttr "i_pCube4.msg" ":hyperGraphLayout.hyp[146].dn";
connectAttr "polySurface3.msg" ":hyperGraphLayout.hyp[147].dn";
connectAttr "polySurface4.msg" ":hyperGraphLayout.hyp[148].dn";
connectAttr "polySurface5.msg" ":hyperGraphLayout.hyp[149].dn";
connectAttr "i_polySurface6.msg" ":hyperGraphLayout.hyp[150].dn";
connectAttr "i_polySurface7.msg" ":hyperGraphLayout.hyp[151].dn";
connectAttr "i_transform21.msg" ":hyperGraphLayout.hyp[153].dn";
connectAttr "i_m_joystick.msg" ":hyperGraphLayout.hyp[160].dn";
connectAttr "transform28.msg" ":hyperGraphLayout.hyp[166].dn";
connectAttr "i_pCylinder1.msg" ":hyperGraphLayout.hyp[206].dn";
connectAttr "transform34.msg" ":hyperGraphLayout.hyp[208].dn";
connectAttr ":side.msg" ":hyperGraphLayout.hyp[209].dn";
connectAttr ":front.msg" ":hyperGraphLayout.hyp[210].dn";
connectAttr "transform12.msg" ":hyperGraphLayout.hyp[211].dn";
connectAttr "transform11.msg" ":hyperGraphLayout.hyp[212].dn";
connectAttr "transform33.msg" ":hyperGraphLayout.hyp[213].dn";
connectAttr "transform10.msg" ":hyperGraphLayout.hyp[214].dn";
connectAttr "phys_trig_landing.msg" ":hyperGraphLayout.hyp[244].dn";
connectAttr "phys_rack1.msg" ":hyperGraphLayout.hyp[245].dn";
connectAttr "m_rack.msg" ":hyperGraphLayout.hyp[246].dn";
connectAttr "i_m_hangar.msg" ":hyperGraphLayout.hyp[247].dn";
connectAttr "m_hangar.msg" ":hyperGraphLayout.hyp[248].dn";
connectAttr "transform27.msg" ":hyperGraphLayout.hyp[249].dn";
connectAttr "i_m_landingbar0.msg" ":hyperGraphLayout.hyp[250].dn";
connectAttr "phys_pos_sim.msg" ":hyperGraphLayout.hyp[252].dn";
connectAttr "m_floor.msg" ":hyperGraphLayout.hyp[314].dn";
connectAttr "phys_wall1.msg" ":hyperGraphLayout.hyp[315].dn";
connectAttr "phys_wall2.msg" ":hyperGraphLayout.hyp[316].dn";
connectAttr "phys_ceiling.msg" ":hyperGraphLayout.hyp[317].dn";
connectAttr "phys_hangar.msg" ":hyperGraphLayout.hyp[318].dn";
connectAttr "phys_startingpad.msg" ":hyperGraphLayout.hyp[319].dn";
connectAttr "phys_pos_start.msg" ":hyperGraphLayout.hyp[320].dn";
connectAttr "phys_rack.msg" ":hyperGraphLayout.hyp[321].dn";
connectAttr "phys_pos_path_2.msg" ":hyperGraphLayout.hyp[322].dn";
connectAttr "phys_pos_path_3.msg" ":hyperGraphLayout.hyp[323].dn";
connectAttr "phys_pos_path_4.msg" ":hyperGraphLayout.hyp[324].dn";
// End of level_07.ma
