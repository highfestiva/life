//Maya ASCII 2009 scene
//Name: level_07.ma
//Last modified: Sat, Sep 28, 2013 10:32:40 PM
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
	setAttr ".t" -type "double3" 45.166536990034977 29.050432253660993 101.74833475009007 ;
	setAttr ".r" -type "double3" -12.338352729773886 -4.5999999999993335 1.6293189686538829e-013 ;
	setAttr ".rp" -type "double3" -7.1054273576010019e-015 -3.5527136788005009e-015 
		1.4210854715202004e-014 ;
	setAttr ".rpt" -type "double3" -5.8567893484649228e-015 2.0096005677986135e-015 
		-1.4461412033693613e-014 ;
createNode camera -s -n "perspShape" -p "persp";
	setAttr -k off ".v";
	setAttr ".fl" 34.999999999999979;
	setAttr ".fcp" 3000;
	setAttr ".coi" 94.479085267443892;
	setAttr ".imn" -type "string" "persp";
	setAttr ".den" -type "string" "persp_depth";
	setAttr ".man" -type "string" "persp_mask";
	setAttr ".tp" -type "double3" 51.99664029339943 6.8250849708899182 10.149661069435012 ;
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
	setAttr -s 16 ".uvst[0].uvsp[0:15]" -type "float2" 2.6693053 -1.2822554 
		0.85596848 -1.2822554 0.85596848 1.5590785 2.8468087 2.1904428 2.6693053 1.5590785 
		2.8576314 0.18351546 0.85596848 -1.2822554 2.6632307 -1.2822554 0.9193275 0.18017194 
		0.91679645 2.1871035 -1.3004503 -0.63886499 1.8323162 -0.63886499 1.8323162 1.2798934 
		-1.3004503 1.2798934 2.6632307 1.5590785 0.85596848 1.5590785;
	setAttr ".cuvs" -type "string" "map_hangar";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 6 ".pt";
	setAttr ".pt[0]" -type "float3" 0 0 17.127069 ;
	setAttr ".pt[2]" -type "float3" 0 0 17.127069 ;
	setAttr ".pt[4:7]" -type "float3" 0 20.53385 0  0 20.53385 17.127069  
		0 20.53385 17.127069  0 20.53385 0 ;
	setAttr -s 8 ".vt[0:7]"  -23.553211 0.25 -23.596054 -23.553209 0.25 
		25 23.679733 0.25 -23.657543 23.679733 0.25 25 23.679733 18.442028 25 23.679733 18.44203 
		-23.657543 -23.553211 18.44203 -23.596054 -23.553209 18.442028 25;
	setAttr -s 11 ".ed[0:10]"  0 1 0 2 0 0 
		2 3 0 3 4 0 2 5 0 5 4 0 
		0 6 0 5 6 0 1 7 0 6 7 0 
		4 7 0;
	setAttr -s 4 ".fc[0:3]" -type "polyFaces" 
		f 4 2 3 -6 -5 
		mu 0 4 0 1 2 4 
		f 4 -2 4 7 -7 
		mu 0 4 10 11 12 13 
		f 4 -1 6 9 -9 
		mu 0 4 6 7 14 15 
		f 4 -8 5 10 -10 
		mu 0 4 3 5 8 9 ;
	setAttr ".cd" -type "dataPolyComponent" Index_Data Edge 0 ;
	setAttr ".cvd" -type "dataPolyComponent" Index_Data Vertex 0 ;
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
	setAttr -s 2 ".iog[0].og";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr -s 98 ".uvst[0].uvsp[0:97]" -type "float2" 0.375 0 0.625 0 
		0.625 0.25 0.375 0.25 0.625 0.5 0.375 0.5 0.625 0.75 0.375 0.75 0.625 1 0.375 1 0.875 
		0 0.875 0.25 0.125 0 0.125 0.25 0.375 0 0.625 0 0.625 0.25 0.375 0.25 0.625 0.5 0.375 
		0.5 0.625 0.75 0.375 0.75 0.625 1 0.375 1 0.875 0 0.875 0.25 0.125 0 0.125 0.25 0.375 
		0 0.625 0 0.625 0.25 0.375 0.25 0.625 0.5 0.375 0.5 0.625 0.75 0.375 0.75 0.625 1 
		0.375 1 0.875 0 0.875 0.25 0.125 0 0.125 0.25 0.375 0 0.625 0 0.625 0.25 0.375 0.25 
		0.625 0.5 0.375 0.5 0.625 0.75 0.375 0.75 0.625 1 0.375 1 0.875 0 0.875 0.25 0.125 
		0 0.125 0.25 0.375 0 0.625 0 0.625 0.25 0.375 0.25 0.625 0.5 0.375 0.5 0.625 0.75 
		0.375 0.75 0.625 1 0.375 1 0.875 0 0.875 0.25 0.125 0 0.125 0.25 0.375 0 0.625 0 
		0.625 0.25 0.375 0.25 0.625 0.5 0.375 0.5 0.625 0.75 0.375 0.75 0.625 1 0.375 1 0.875 
		0 0.875 0.25 0.125 0 0.125 0.25 0.375 0 0.625 0 0.625 0.25 0.375 0.25 0.625 0.5 0.375 
		0.5 0.625 0.75 0.375 0.75 0.625 1 0.375 1 0.875 0 0.875 0.25 0.125 0 0.125 0.25;
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 56 ".vt[0:55]"  -20.624794 19.75 -20.812262 4.375206 19.75 
		-20.812262 -20.624794 20.25 -20.812262 4.375206 20.25 -20.812262 -20.624794 20.25 
		-32.812263 4.375206 20.25 -32.812263 -20.624794 19.75 -32.812263 4.375206 19.75 -32.812263 
		4.0283923 0.021775246 -20.697964 4.5283923 0.021775246 -20.697964 4.0283923 25.021774 
		-20.697964 4.5283923 25.021774 -20.697964 4.0283923 25.021774 -21.197964 4.5283923 
		25.021774 -21.197964 4.0283923 0.021775246 -21.197964 4.5283923 0.021775246 -21.197964 
		4.0283923 0.021775246 -32.412857 4.5283923 0.021775246 -32.412857 4.0283923 25.021774 
		-32.412857 4.5283923 25.021774 -32.412857 4.0283923 25.021774 -32.912857 4.5283923 
		25.021774 -32.912857 4.0283923 0.021775246 -32.912857 4.5283923 0.021775246 -32.912857 
		-8.0660305 0.021775246 -20.697964 -7.5660305 0.021775246 -20.697964 -8.0660305 25.021774 
		-20.697964 -7.5660305 25.021774 -20.697964 -8.0660305 25.021774 -21.197964 -7.5660305 
		25.021774 -21.197964 -8.0660305 0.021775246 -21.197964 -7.5660305 0.021775246 -21.197964 
		-8.0660305 0.021775246 -32.412857 -7.5660305 0.021775246 -32.412857 -8.0660305 25.021774 
		-32.412857 -7.5660305 25.021774 -32.412857 -8.0660305 25.021774 -32.912857 -7.5660305 
		25.021774 -32.912857 -8.0660305 0.021775246 -32.912857 -7.5660305 0.021775246 -32.912857 
		-20.758083 0.021775246 -20.697964 -20.258083 0.021775246 -20.697964 -20.758083 25.021774 
		-20.697964 -20.258083 25.021774 -20.697964 -20.758083 25.021774 -21.197964 -20.258083 
		25.021774 -21.197964 -20.758083 0.021775246 -21.197964 -20.258083 0.021775246 -21.197964 
		-20.758083 0.021775246 -32.412857 -20.258083 0.021775246 -32.412857 -20.758083 25.021774 
		-32.412857 -20.258083 25.021774 -32.412857 -20.758083 25.021774 -32.912857 -20.258083 
		25.021774 -32.912857 -20.758083 0.021775246 -32.912857 -20.258083 0.021775246 -32.912857;
	setAttr -s 84 ".ed[0:83]"  0 1 0 2 3 0 
		4 5 0 6 7 0 0 2 0 1 3 0 
		2 4 0 3 5 0 4 6 0 5 7 0 
		6 0 0 7 1 0 8 9 0 10 11 0 
		12 13 0 14 15 0 8 10 0 9 11 0 
		10 12 0 11 13 0 12 14 0 13 15 0 
		14 8 0 15 9 0 16 17 0 18 19 0 
		20 21 0 22 23 0 16 18 0 17 19 0 
		18 20 0 19 21 0 20 22 0 21 23 0 
		22 16 0 23 17 0 24 25 0 26 27 0 
		28 29 0 30 31 0 24 26 0 25 27 0 
		26 28 0 27 29 0 28 30 0 29 31 0 
		30 24 0 31 25 0 32 33 0 34 35 0 
		36 37 0 38 39 0 32 34 0 33 35 0 
		34 36 0 35 37 0 36 38 0 37 39 0 
		38 32 0 39 33 0 40 41 0 42 43 0 
		44 45 0 46 47 0 40 42 0 41 43 0 
		42 44 0 43 45 0 44 46 0 45 47 0 
		46 40 0 47 41 0 48 49 0 50 51 0 
		52 53 0 54 55 0 48 50 0 49 51 0 
		50 52 0 51 53 0 52 54 0 53 55 0 
		54 48 0 55 49 0;
	setAttr -s 42 ".fc[0:41]" -type "polyFaces" 
		f 4 0 5 -2 -5 
		mu 0 4 0 1 2 3 
		f 4 1 7 -3 -7 
		mu 0 4 3 2 4 5 
		f 4 2 9 -4 -9 
		mu 0 4 5 4 6 7 
		f 4 3 11 -1 -11 
		mu 0 4 7 6 8 9 
		f 4 -12 -10 -8 -6 
		mu 0 4 1 10 11 2 
		f 4 10 4 6 8 
		mu 0 4 12 0 3 13 
		f 4 12 17 -14 -17 
		mu 0 4 14 15 16 17 
		f 4 13 19 -15 -19 
		mu 0 4 17 16 18 19 
		f 4 14 21 -16 -21 
		mu 0 4 19 18 20 21 
		f 4 15 23 -13 -23 
		mu 0 4 21 20 22 23 
		f 4 -24 -22 -20 -18 
		mu 0 4 15 24 25 16 
		f 4 22 16 18 20 
		mu 0 4 26 14 17 27 
		f 4 24 29 -26 -29 
		mu 0 4 28 29 30 31 
		f 4 25 31 -27 -31 
		mu 0 4 31 30 32 33 
		f 4 26 33 -28 -33 
		mu 0 4 33 32 34 35 
		f 4 27 35 -25 -35 
		mu 0 4 35 34 36 37 
		f 4 -36 -34 -32 -30 
		mu 0 4 29 38 39 30 
		f 4 34 28 30 32 
		mu 0 4 40 28 31 41 
		f 4 36 41 -38 -41 
		mu 0 4 42 43 44 45 
		f 4 37 43 -39 -43 
		mu 0 4 45 44 46 47 
		f 4 38 45 -40 -45 
		mu 0 4 47 46 48 49 
		f 4 39 47 -37 -47 
		mu 0 4 49 48 50 51 
		f 4 -48 -46 -44 -42 
		mu 0 4 43 52 53 44 
		f 4 46 40 42 44 
		mu 0 4 54 42 45 55 
		f 4 48 53 -50 -53 
		mu 0 4 56 57 58 59 
		f 4 49 55 -51 -55 
		mu 0 4 59 58 60 61 
		f 4 50 57 -52 -57 
		mu 0 4 61 60 62 63 
		f 4 51 59 -49 -59 
		mu 0 4 63 62 64 65 
		f 4 -60 -58 -56 -54 
		mu 0 4 57 66 67 58 
		f 4 58 52 54 56 
		mu 0 4 68 56 59 69 
		f 4 60 65 -62 -65 
		mu 0 4 70 71 72 73 
		f 4 61 67 -63 -67 
		mu 0 4 73 72 74 75 
		f 4 62 69 -64 -69 
		mu 0 4 75 74 76 77 
		f 4 63 71 -61 -71 
		mu 0 4 77 76 78 79 
		f 4 -72 -70 -68 -66 
		mu 0 4 71 80 81 72 
		f 4 70 64 66 68 
		mu 0 4 82 70 73 83 
		f 4 72 77 -74 -77 
		mu 0 4 84 85 86 87 
		f 4 73 79 -75 -79 
		mu 0 4 87 86 88 89 
		f 4 74 81 -76 -81 
		mu 0 4 89 88 90 91 
		f 4 75 83 -73 -83 
		mu 0 4 91 90 92 93 
		f 4 -84 -82 -80 -78 
		mu 0 4 85 94 95 86 
		f 4 82 76 78 80 
		mu 0 4 96 84 87 97 ;
	setAttr ".cd" -type "dataPolyComponent" Index_Data Edge 0 ;
	setAttr ".cvd" -type "dataPolyComponent" Index_Data Vertex 0 ;
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
	setAttr ".t" -type "double3" 17.603931752246009 0.031698116491645167 -4.9960036108132054e-016 ;
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
	setAttr ".t" -type "double3" -5.5864175861165499 0 0 ;
	setAttr ".r" -type "double3" 0 90 0 ;
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
	setAttr ".t" -type "double3" 17.648959131631738 2.1913978358993083 0 ;
	setAttr ".s" -type "double3" 0.5 1 0.5 ;
createNode transform -n "phys_pos_path_2" -p "m_hangar";
	setAttr ".t" -type "double3" -1.5493179608338452 18.996169459761344 0 ;
	setAttr ".s" -type "double3" 0.5 1 0.5 ;
createNode transform -n "phys_pos_path_3" -p "m_hangar";
	setAttr ".t" -type "double3" 4.1888776688469935 12.732181235487708 0 ;
	setAttr ".s" -type "double3" 0.5 1 0.5 ;
createNode transform -n "phys_pos_path_4" -p "m_hangar";
	setAttr ".t" -type "double3" 14.547848658999778 8.7628452203097957 0 ;
	setAttr ".s" -type "double3" 0.5 1 0.5 ;
createNode transform -n "i_m_rack3";
	setAttr ".t" -type "double3" -8.124794128558019 10 -26.812262098134255 ;
createNode transform -n "transform9" -p "i_m_rack3";
createNode mesh -n "phys_rackShape1" -p "transform9";
	setAttr -k off ".v";
	setAttr ".io" yes;
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
	setAttr -s 8 ".vt[0:7]"  -12.5 -0.25 6 12.5 -0.25 6 -12.5 0.25 6 
		12.5 0.25 6 -12.5 0.25 -6 12.5 0.25 -6 -12.5 -0.25 -6 12.5 -0.25 -6;
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
createNode transform -n "i_m_rack4";
	setAttr ".t" -type "double3" -8.124794128558019 20 -26.812262098134255 ;
createNode transform -n "transform8" -p "i_m_rack4";
createNode transform -n "i_m_rackPillar7";
	setAttr ".t" -type "double3" 4.2783923983104666 12.52177498900233 -20.947963825898274 ;
createNode transform -n "transform7" -p "i_m_rackPillar7";
createNode mesh -n "phys_rackPillar1Shape" -p "transform7";
	setAttr -k off ".v";
	setAttr ".io" yes;
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
	setAttr -s 8 ".vt[0:7]"  -0.25 -12.5 0.25 0.25 -12.5 0.25 -0.25 12.5 
		0.25 0.25 12.5 0.25 -0.25 12.5 -0.25 0.25 12.5 -0.25 -0.25 -12.5 -0.25 0.25 -12.5 
		-0.25;
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
parent -s -nc -r -add "|m_hangar|phys_wall1|phys_wallShape1" "phys_wall2";
parent -s -nc -r -add "|m_hangar|m_startingpad|phys_startingpad|m_landingpadShape" "phys_landingpad";
parent -s -nc -r -add "|m_hangar|m_startingpad|phys_startingpad|m_landingpadShape" "m_startingpad";
parent -s -nc -r -add "|m_hangar|m_startingpad|phys_startingpad|m_landingpadShape" "m_landingpad";
parent -s -nc -r -add "|m_hangar|phys_pos_path_start|phys_pos_path_Shape1" "phys_pos_path_1";
parent -s -nc -r -add "|m_hangar|phys_pos_path_start|phys_pos_path_Shape1" "phys_pos_path_land";
parent -s -nc -r -add "|m_hangar|phys_pos_path_start|phys_pos_path_Shape1" "phys_pos_path_2";
parent -s -nc -r -add "|m_hangar|phys_pos_path_start|phys_pos_path_Shape1" "phys_pos_path_3";
parent -s -nc -r -add "|m_hangar|phys_pos_path_start|phys_pos_path_Shape1" "phys_pos_path_4";
parent -s -nc -r -add "|i_m_rack3|transform9|phys_rackShape1" "transform8";
parent -s -nc -r -add "|i_m_rackPillar7|transform7|phys_rackPillar1Shape" "transform2";
parent -s -nc -r -add "|i_m_rackPillar7|transform7|phys_rackPillar1Shape" "transform3";
parent -s -nc -r -add "|i_m_rackPillar7|transform7|phys_rackPillar1Shape" "transform4";
parent -s -nc -r -add "|i_m_rackPillar7|transform7|phys_rackPillar1Shape" "transform5";
parent -s -nc -r -add "|i_m_rackPillar7|transform7|phys_rackPillar1Shape" "transform6";
createNode lightLinker -n "lightLinker1";
	setAttr -s 19 ".lnk";
	setAttr -s 19 ".slnk";
createNode displayLayerManager -n "layerManager";
createNode displayLayer -n "defaultLayer";
createNode renderLayerManager -n "renderLayerManager";
createNode renderLayer -n "defaultRenderLayer";
	setAttr ".g" yes;
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
		+ "                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                $editorName;\n\n\t\t\t$editorName = ($panelName+\"GraphEd\");\n            animCurveEditor -e \n                -displayKeys 1\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 1\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"integer\" \n                -snapValue \"none\" \n                -showResults \"off\" \n                -showBufferCurves \"off\" \n                -smoothness \"fine\" \n                -resultSamples 1\n                -resultScreenSamples 0\n                -resultUpdate \"delayed\" \n                -constrainDrag 0\n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Graph Editor\")) -mbv $menusOkayInPanels  $panelName;\n"
		+ "\t\t\t$editorName = ($panelName+\"OutlineEd\");\n            outlinerEditor -e \n                -showShapes 1\n                -showAttributes 1\n                -showConnected 1\n                -showAnimCurvesOnly 1\n                -showMuteInfo 0\n                -organizeByLayer 1\n                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n                -autoExpand 1\n                -showDagOnly 0\n                -showAssets 1\n                -showContainedOnly 0\n                -showPublishedAsConnected 0\n                -showContainerContents 0\n                -ignoreDagHierarchy 0\n                -expandConnections 1\n                -showUnitlessCurves 1\n                -showCompounds 0\n                -showLeafs 1\n                -showNumericAttrsOnly 1\n                -highlightActive 0\n                -autoSelectNewObjects 1\n                -doNotSelectNewObjects 0\n                -dropIsParent 1\n                -transmitFilters 1\n                -setFilter \"0\" \n                -showSetMembers 0\n"
		+ "                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -displayMode \"DAG\" \n                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n                -showAttrValues 0\n                -highlightSecondary 0\n                -showUVAttrsOnly 0\n                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                $editorName;\n\n\t\t\t$editorName = ($panelName+\"GraphEd\");\n            animCurveEditor -e \n                -displayKeys 1\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 1\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"integer\" \n                -snapValue \"none\" \n"
		+ "                -showResults \"off\" \n                -showBufferCurves \"off\" \n                -smoothness \"fine\" \n                -resultSamples 1\n                -resultScreenSamples 0\n                -resultUpdate \"delayed\" \n                -constrainDrag 0\n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"dopeSheetPanel\" (localizedPanelLabel(\"Dope Sheet\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"dopeSheetPanel\" -l (localizedPanelLabel(\"Dope Sheet\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = ($panelName+\"OutlineEd\");\n            outlinerEditor -e \n                -showShapes 1\n                -showAttributes 1\n                -showConnected 1\n                -showAnimCurvesOnly 1\n                -showMuteInfo 0\n                -organizeByLayer 1\n                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n                -autoExpand 0\n"
		+ "                -showDagOnly 0\n                -showAssets 1\n                -showContainedOnly 0\n                -showPublishedAsConnected 0\n                -showContainerContents 0\n                -ignoreDagHierarchy 0\n                -expandConnections 1\n                -showUnitlessCurves 0\n                -showCompounds 1\n                -showLeafs 1\n                -showNumericAttrsOnly 1\n                -highlightActive 0\n                -autoSelectNewObjects 0\n                -doNotSelectNewObjects 1\n                -dropIsParent 1\n                -transmitFilters 0\n                -setFilter \"0\" \n                -showSetMembers 0\n                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -displayMode \"DAG\" \n                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n                -showAttrValues 0\n                -highlightSecondary 0\n                -showUVAttrsOnly 0\n"
		+ "                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                $editorName;\n\n\t\t\t$editorName = ($panelName+\"DopeSheetEd\");\n            dopeSheetEditor -e \n                -displayKeys 1\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"integer\" \n                -snapValue \"none\" \n                -outliner \"dopeSheetPanel1OutlineEd\" \n                -showSummary 1\n                -showScene 0\n                -hierarchyBelow 0\n                -showTicks 1\n                -selectionWindow 0 0 0 0 \n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Dope Sheet\")) -mbv $menusOkayInPanels  $panelName;\n"
		+ "\t\t\t$editorName = ($panelName+\"OutlineEd\");\n            outlinerEditor -e \n                -showShapes 1\n                -showAttributes 1\n                -showConnected 1\n                -showAnimCurvesOnly 1\n                -showMuteInfo 0\n                -organizeByLayer 1\n                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n                -autoExpand 0\n                -showDagOnly 0\n                -showAssets 1\n                -showContainedOnly 0\n                -showPublishedAsConnected 0\n                -showContainerContents 0\n                -ignoreDagHierarchy 0\n                -expandConnections 1\n                -showUnitlessCurves 0\n                -showCompounds 1\n                -showLeafs 1\n                -showNumericAttrsOnly 1\n                -highlightActive 0\n                -autoSelectNewObjects 0\n                -doNotSelectNewObjects 1\n                -dropIsParent 1\n                -transmitFilters 0\n                -setFilter \"0\" \n                -showSetMembers 0\n"
		+ "                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -displayMode \"DAG\" \n                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n                -showAttrValues 0\n                -highlightSecondary 0\n                -showUVAttrsOnly 0\n                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                $editorName;\n\n\t\t\t$editorName = ($panelName+\"DopeSheetEd\");\n            dopeSheetEditor -e \n                -displayKeys 1\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"integer\" \n                -snapValue \"none\" \n"
		+ "                -outliner \"dopeSheetPanel1OutlineEd\" \n                -showSummary 1\n                -showScene 0\n                -hierarchyBelow 0\n                -showTicks 1\n                -selectionWindow 0 0 0 0 \n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"clipEditorPanel\" (localizedPanelLabel(\"Trax Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"clipEditorPanel\" -l (localizedPanelLabel(\"Trax Editor\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = clipEditorNameFromPanel($panelName);\n            clipEditor -e \n                -displayKeys 0\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"none\" \n                -snapValue \"none\" \n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n"
		+ "\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Trax Editor\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = clipEditorNameFromPanel($panelName);\n            clipEditor -e \n                -displayKeys 0\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"none\" \n                -snapValue \"none\" \n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"hyperGraphPanel\" (localizedPanelLabel(\"Hypergraph Hierarchy1\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"hyperGraphPanel\" -l (localizedPanelLabel(\"Hypergraph Hierarchy1\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = ($panelName+\"HyperGraphEd\");\n            hyperGraph -e \n                -graphLayoutStyle \"hierarchicalLayout\" \n                -orientation \"horiz\" \n"
		+ "                -mergeConnections 1\n                -zoom 0.9475\n                -animateTransition 0\n                -showRelationships 1\n                -showShapes 0\n                -showDeformers 0\n                -showExpressions 0\n                -showConstraints 0\n                -showUnderworld 0\n                -showInvisible 0\n                -transitionFrames 5\n                -currentNode \"phys_pos_path_4\" \n                -opaqueContainers 0\n                -freeform 1\n                -imagePosition 0 0 \n                -imageScale 1\n                -imageEnabled 0\n                -graphType \"DAG\" \n                -heatMapDisplay 0\n                -updateSelection 1\n                -updateNodeAdded 1\n                -useDrawOverrideColor 0\n                -limitGraphTraversal -1\n                -range 0 0 \n                -iconSize \"largeIcons\" \n                -showCachedConnections 0\n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Hypergraph Hierarchy1\")) -mbv $menusOkayInPanels  $panelName;\n"
		+ "\t\t\t$editorName = ($panelName+\"HyperGraphEd\");\n            hyperGraph -e \n                -graphLayoutStyle \"hierarchicalLayout\" \n                -orientation \"horiz\" \n                -mergeConnections 1\n                -zoom 0.9475\n                -animateTransition 0\n                -showRelationships 1\n                -showShapes 0\n                -showDeformers 0\n                -showExpressions 0\n                -showConstraints 0\n                -showUnderworld 0\n                -showInvisible 0\n                -transitionFrames 5\n                -currentNode \"phys_pos_path_4\" \n                -opaqueContainers 0\n                -freeform 1\n                -imagePosition 0 0 \n                -imageScale 1\n                -imageEnabled 0\n                -graphType \"DAG\" \n                -heatMapDisplay 0\n                -updateSelection 1\n                -updateNodeAdded 1\n                -useDrawOverrideColor 0\n                -limitGraphTraversal -1\n                -range 0 0 \n                -iconSize \"largeIcons\" \n"
		+ "                -showCachedConnections 0\n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"hyperShadePanel\" (localizedPanelLabel(\"Hypershade\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"hyperShadePanel\" -l (localizedPanelLabel(\"Hypershade\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Hypershade\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"visorPanel\" (localizedPanelLabel(\"Visor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"visorPanel\" -l (localizedPanelLabel(\"Visor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Visor\")) -mbv $menusOkayInPanels  $panelName;\n"
		+ "\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"polyTexturePlacementPanel\" (localizedPanelLabel(\"UV Texture Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"polyTexturePlacementPanel\" -l (localizedPanelLabel(\"UV Texture Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"UV Texture Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"multiListerPanel\" (localizedPanelLabel(\"Multilister\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"multiListerPanel\" -l (localizedPanelLabel(\"Multilister\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Multilister\")) -mbv $menusOkayInPanels  $panelName;\n"
		+ "\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"renderWindowPanel\" (localizedPanelLabel(\"Render View\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"renderWindowPanel\" -l (localizedPanelLabel(\"Render View\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Render View\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"blendShapePanel\" (localizedPanelLabel(\"Blend Shape\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\tblendShapePanel -unParent -l (localizedPanelLabel(\"Blend Shape\")) -mbv $menusOkayInPanels ;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tblendShapePanel -edit -l (localizedPanelLabel(\"Blend Shape\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n"
		+ "\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"dynRelEdPanel\" (localizedPanelLabel(\"Dynamic Relationships\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"dynRelEdPanel\" -l (localizedPanelLabel(\"Dynamic Relationships\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Dynamic Relationships\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"devicePanel\" (localizedPanelLabel(\"Devices\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\tdevicePanel -unParent -l (localizedPanelLabel(\"Devices\")) -mbv $menusOkayInPanels ;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tdevicePanel -edit -l (localizedPanelLabel(\"Devices\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n"
		+ "\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"relationshipPanel\" (localizedPanelLabel(\"Relationship Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"relationshipPanel\" -l (localizedPanelLabel(\"Relationship Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Relationship Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"referenceEditorPanel\" (localizedPanelLabel(\"Reference Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"referenceEditorPanel\" -l (localizedPanelLabel(\"Reference Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Reference Editor\")) -mbv $menusOkayInPanels  $panelName;\n"
		+ "\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"componentEditorPanel\" (localizedPanelLabel(\"Component Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"componentEditorPanel\" -l (localizedPanelLabel(\"Component Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Component Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"dynPaintScriptedPanelType\" (localizedPanelLabel(\"Paint Effects\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"dynPaintScriptedPanelType\" -l (localizedPanelLabel(\"Paint Effects\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Paint Effects\")) -mbv $menusOkayInPanels  $panelName;\n"
		+ "\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"webBrowserPanel\" (localizedPanelLabel(\"Web Browser\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"webBrowserPanel\" -l (localizedPanelLabel(\"Web Browser\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Web Browser\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"scriptEditorPanel\" (localizedPanelLabel(\"Script Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"scriptEditorPanel\" -l (localizedPanelLabel(\"Script Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Script Editor\")) -mbv $menusOkayInPanels  $panelName;\n"
		+ "\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"Stereo\" (localizedPanelLabel(\"Stereo\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"Stereo\" -l (localizedPanelLabel(\"Stereo\")) -mbv $menusOkayInPanels `;\nstring $editorName = ($panelName+\"Editor\");\n            stereoCameraView -e \n                -camera \"persp\" \n                -useInteractiveMode 0\n                -displayLights \"default\" \n                -displayAppearance \"wireframe\" \n                -activeOnly 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n                -activeComponentsXray 0\n                -displayTextures 0\n                -smoothWireframe 0\n"
		+ "                -lineWidth 1\n                -textureAnisotropic 0\n                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n                -colorResolution 4 4 \n                -bumpResolution 4 4 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n                -lowQualityLighting 0\n                -maximumNumHardwareLights 0\n                -occlusionCulling 0\n                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n"
		+ "                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -dimensions 1\n                -handles 1\n                -pivots 1\n                -textures 1\n                -strokes 1\n                -shadows 0\n                -displayMode \"centerEye\" \n                -viewColor 0 0 0 1 \n"
		+ "                $editorName;\nstereoCameraView -e -viewSelected 0 $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Stereo\")) -mbv $menusOkayInPanels  $panelName;\nstring $editorName = ($panelName+\"Editor\");\n            stereoCameraView -e \n                -camera \"persp\" \n                -useInteractiveMode 0\n                -displayLights \"default\" \n                -displayAppearance \"wireframe\" \n                -activeOnly 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n                -activeComponentsXray 0\n                -displayTextures 0\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n                -textureHilight 1\n                -textureSampling 2\n"
		+ "                -textureDisplay \"modulate\" \n                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n                -colorResolution 4 4 \n                -bumpResolution 4 4 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n                -lowQualityLighting 0\n                -maximumNumHardwareLights 0\n                -occlusionCulling 0\n                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n"
		+ "                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -dimensions 1\n                -handles 1\n                -pivots 1\n                -textures 1\n                -strokes 1\n                -shadows 0\n                -displayMode \"centerEye\" \n                -viewColor 0 0 0 1 \n                $editorName;\nstereoCameraView -e -viewSelected 0 $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n"
		+ "\t\t}\n\t}\n\n\n\tif ($useSceneConfig) {\n        string $configName = `getPanel -cwl (localizedPanelLabel(\"Current Layout\"))`;\n        if (\"\" != $configName) {\n\t\t\tpanelConfiguration -edit -label (localizedPanelLabel(\"Current Layout\")) \n\t\t\t\t-defaultImage \"\"\n\t\t\t\t-image \"\"\n\t\t\t\t-sc false\n\t\t\t\t-configString \"global string $gMainPane; paneLayout -e -cn \\\"horizontal2\\\" -ps 1 100 45 -ps 2 100 55 $gMainPane;\"\n\t\t\t\t-removeAllPanels\n\t\t\t\t-ap false\n\t\t\t\t\t(localizedPanelLabel(\"Persp View\")) \n\t\t\t\t\t\"modelPanel\"\n"
		+ "\t\t\t\t\t\"$panelName = `modelPanel -unParent -l (localizedPanelLabel(\\\"Persp View\\\")) -mbv $menusOkayInPanels `;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera persp` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"smoothShaded\\\" \\n    -activeOnly 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 1\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName\"\n"
		+ "\t\t\t\t\t\"modelPanel -edit -l (localizedPanelLabel(\\\"Persp View\\\")) -mbv $menusOkayInPanels  $panelName;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera persp` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"smoothShaded\\\" \\n    -activeOnly 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 1\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName\"\n"
		+ "\t\t\t\t-ap false\n\t\t\t\t\t(localizedPanelLabel(\"Hypergraph Hierarchy1\")) \n\t\t\t\t\t\"scriptedPanel\"\n\t\t\t\t\t\"$panelName = `scriptedPanel -unParent  -type \\\"hyperGraphPanel\\\" -l (localizedPanelLabel(\\\"Hypergraph Hierarchy1\\\")) -mbv $menusOkayInPanels `;\\n\\n\\t\\t\\t$editorName = ($panelName+\\\"HyperGraphEd\\\");\\n            hyperGraph -e \\n                -graphLayoutStyle \\\"hierarchicalLayout\\\" \\n                -orientation \\\"horiz\\\" \\n                -mergeConnections 1\\n                -zoom 0.9475\\n                -animateTransition 0\\n                -showRelationships 1\\n                -showShapes 0\\n                -showDeformers 0\\n                -showExpressions 0\\n                -showConstraints 0\\n                -showUnderworld 0\\n                -showInvisible 0\\n                -transitionFrames 5\\n                -currentNode \\\"phys_pos_path_4\\\" \\n                -opaqueContainers 0\\n                -freeform 1\\n                -imagePosition 0 0 \\n                -imageScale 1\\n                -imageEnabled 0\\n                -graphType \\\"DAG\\\" \\n                -heatMapDisplay 0\\n                -updateSelection 1\\n                -updateNodeAdded 1\\n                -useDrawOverrideColor 0\\n                -limitGraphTraversal -1\\n                -range 0 0 \\n                -iconSize \\\"largeIcons\\\" \\n                -showCachedConnections 0\\n                $editorName\"\n"
		+ "\t\t\t\t\t\"scriptedPanel -edit -l (localizedPanelLabel(\\\"Hypergraph Hierarchy1\\\")) -mbv $menusOkayInPanels  $panelName;\\n\\n\\t\\t\\t$editorName = ($panelName+\\\"HyperGraphEd\\\");\\n            hyperGraph -e \\n                -graphLayoutStyle \\\"hierarchicalLayout\\\" \\n                -orientation \\\"horiz\\\" \\n                -mergeConnections 1\\n                -zoom 0.9475\\n                -animateTransition 0\\n                -showRelationships 1\\n                -showShapes 0\\n                -showDeformers 0\\n                -showExpressions 0\\n                -showConstraints 0\\n                -showUnderworld 0\\n                -showInvisible 0\\n                -transitionFrames 5\\n                -currentNode \\\"phys_pos_path_4\\\" \\n                -opaqueContainers 0\\n                -freeform 1\\n                -imagePosition 0 0 \\n                -imageScale 1\\n                -imageEnabled 0\\n                -graphType \\\"DAG\\\" \\n                -heatMapDisplay 0\\n                -updateSelection 1\\n                -updateNodeAdded 1\\n                -useDrawOverrideColor 0\\n                -limitGraphTraversal -1\\n                -range 0 0 \\n                -iconSize \\\"largeIcons\\\" \\n                -showCachedConnections 0\\n                $editorName\"\n"
		+ "\t\t\t\t$configName;\n\n            setNamedPanelLayout (localizedPanelLabel(\"Current Layout\"));\n        }\n\n        panelHistory -e -clear mainPanelHistory;\n        setFocus `paneLayout -q -p1 $gMainPane`;\n        sceneUIReplacement -deleteRemaining;\n        sceneUIReplacement -clear;\n\t}\n\n\ngrid -spacing 5 -size 12 -divisions 5 -displayAxes yes -displayGridLines yes -displayDivisionLines yes -displayPerspectiveLabels no -displayOrthographicLabels no -displayAxesBold yes -perspectiveLabelPosition axis -orthographicLabelPosition edge;\nviewManip -drawCompass 0 -compassAngle 0 -frontParameters \"\" -homeParameters \"\" -selectionLockParameters \"\";\n}\n");
	setAttr ".st" 3;
createNode script -n "sceneConfigurationScriptNode";
	setAttr ".b" -type "string" "playbackOptions -min 1 -max 24 -ast 1 -aet 48 ";
	setAttr ".st" 6;
createNode file -n "file2";
	setAttr ".ftn" -type "string" "C:/RnD/PD/trunk/HeliForce/Data//helipad.png";
createNode place2dTexture -n "place2dTexture2";
createNode file -n "file3";
	setAttr ".ftn" -type "string" "C:/RnD/PD/trunk/HeliForce/Data//sheet_metal.png";
createNode place2dTexture -n "place2dTexture3";
createNode file -n "level_start_kit_file2";
	setAttr ".ftn" -type "string" "C:/RnD/PD/trunk/HeliForce/Data//helipad.png";
createNode place2dTexture -n "level_start_kit_place2dTexture2";
createNode groupId -n "groupId22";
	setAttr ".ihi" 0;
createNode file -n "pasted__file1";
	setAttr ".ftn" -type "string" "C:/RnD/PD/trunk/Push/Data/blue_eye.png";
createNode place2dTexture -n "pasted__place2dTexture2";
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
createNode script -n "helicopter_01_rg_export";
	addAttr -ci true -sn "time" -ln "time" -dt "string";
	setAttr ".time" -type "string" "2013-05-11T12:54:34.312000";
createNode groupId -n "groupId75";
	setAttr ".ihi" 0;
createNode polySphere -n "polySphere1";
	setAttr ".sa" 8;
	setAttr ".sh" 8;
createNode polyCube -n "polyCube7";
	setAttr ".d" 2;
	setAttr ".cuv" 4;
createNode polyCube -n "level_start_kit_polyCube3";
	setAttr ".w" 6;
	setAttr ".h" 0.5;
	setAttr ".d" 6;
	setAttr ".cuv" 4;
createNode polyCube -n "polyCube2";
	setAttr ".w" 0.1;
	setAttr ".h" 0.1;
	setAttr ".cuv" 4;
createNode polyCube -n "level_start_kit_polyCube1";
	setAttr ".uvs" -type "string" "helipad_map";
	setAttr ".w" 6;
	setAttr ".h" 4;
	setAttr ".d" 6;
	setAttr ".cuv" 4;
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
createNode polyCube -n "phys_wall4";
	setAttr ".w" 50;
	setAttr ".h" 40;
	setAttr ".d" 3;
	setAttr ".cuv" 4;
createNode polyCube -n "phys_wall3";
	setAttr ".w" 50;
	setAttr ".h" 3;
	setAttr ".d" 100;
	setAttr ".cuv" 4;
createNode polyCube -n "polyCube4";
	setAttr ".w" 3.0168278623761253;
	setAttr ".h" 40;
	setAttr ".d" 100;
	setAttr ".cuv" 4;
createNode polyCube -n "polyCube3";
	setAttr ".w" 50;
	setAttr ".h" 3;
	setAttr ".d" 100;
	setAttr ".cuv" 4;
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
createNode polyTweak -n "polyTweak1";
	setAttr ".uopa" yes;
	setAttr -s 6 ".tk[8:13]" -type "float3"  0 0 -2.2073526 0 0 -2.2073526 
		-1.8370619 0 -2.2073526 -2.8971508 0 0 4.4636126 0 -2.2073526 3.6559286 0 0;
createNode polyExtrudeFace -n "polyExtrudeFace1";
	setAttr ".ics" -type "componentList" 2 "f[1]" "f[6:7]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".ws" yes;
	setAttr ".mp" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".pvt" -type "float3" 0.35295376 1.0325832 5.4317589 ;
	setAttr ".rs" 45395;
	setAttr ".lt" -type "double3" 0 -1.2896286844418445e-015 18.192028737301406 ;
	setAttr ".c[0]"  0 1 1;
createNode polyExtrudeFace -n "polyExtrudeFace2";
	setAttr ".ics" -type "componentList" 1 "f[6]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".ws" yes;
	setAttr ".mp" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".pvt" -type "float3" 0.35295376 19.224613 -18.863802 ;
	setAttr ".rs" 33107;
	setAttr ".lt" -type "double3" 0 5.1366850895611846e-015 1.3445134323990235 ;
	setAttr ".c[0]"  0 1 1;
createNode polyExtrudeFace -n "polyExtrudeFace3";
	setAttr ".ics" -type "componentList" 1 "f[19:21]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".ws" yes;
	setAttr ".mp" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
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
	setAttr ".mp" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".d" 0.1652;
createNode polyMergeVert -n "polyMergeVert2";
	setAttr ".ics" -type "componentList" 2 "vtx[23]" "vtx[30]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".mp" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".d" 0.1652;
createNode polyMergeVert -n "polyMergeVert3";
	setAttr ".ics" -type "componentList" 2 "vtx[14]" "vtx[32]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".mp" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".d" 0.1652;
createNode polyMergeVert -n "polyMergeVert4";
	setAttr ".ics" -type "componentList" 2 "vtx[15]" "vtx[33]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".mp" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
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
	setAttr ".mp" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0.35295377438606579 0.78258325556203723 5.4317589722171711 1;
	setAttr ".pvt" -type "float3" 0.35295376 0.78258324 5.4317589 ;
	setAttr ".rs" 42353;
	setAttr ".dup" no;
createNode groupId -n "groupId1";
	setAttr ".ihi" 0;
createNode groupParts -n "groupParts1";
	setAttr ".ihi" 0;
	setAttr ".ic" -type "componentList" 1 "f[0:10]";
createNode polySeparate -n "polySeparate1";
	setAttr ".ic" 2;
createNode polyTweakUV -n "polyTweakUV2";
	setAttr ".uopa" yes;
	setAttr -s 4 ".uvtk[0:3]" -type "float2" 0.001388073 0.089664519 
		-0.0013455749 0.089664519 -0.0012913346 0.0063658357 0.0014593005 0.0064761043;
	setAttr ".uvs" -type "string" "map_hangar";
createNode polyCube -n "polyCube8";
	setAttr ".w" 50;
	setAttr ".h" 1.5;
	setAttr ".d" 25;
	setAttr ".cuv" 4;
createNode groupId -n "groupId5";
	setAttr ".ihi" 0;
createNode phong -n "mat_forklift_path1";
	setAttr ".c" -type "float3" 0 0.024833441 1 ;
createNode phong -n "forklift_01_phys";
	setAttr ".c" -type "float3" 0.97582418 0.56700003 1 ;
	setAttr ".it" -type "float3" 0.72728002 0.72728002 0.72728002 ;
createNode phong -n "grey";
	setAttr ".c" -type "float3" 0.69999999 0.69999999 0.69999999 ;
createNode phong -n "mat_black";
	setAttr ".c" -type "float3" 0.23 0.23 0.23 ;
createNode phong -n "blue";
	setAttr ".c" -type "float3" 0.53999996 0.89458328 1 ;
createNode phong -n "gray";
	setAttr ".dc" 0.90082001686096191;
	setAttr ".c" -type "float3" 0.49700001 0.49700001 0.49700001 ;
createNode phong -n "mat_black1";
	setAttr ".sc" -type "float3" 0.31404001 0.31404001 0.31404001 ;
	setAttr ".cp" 13.340000152587891;
createNode phong -n "mat_broken_heli_window";
	setAttr ".c" -type "float3" 0.050999999 0.043247998 0.044209249 ;
	setAttr ".it" -type "float3" 0.38016 0.38016 0.38016 ;
	setAttr ".sc" -type "float3" 1 1 1 ;
	setAttr ".cp" 6.8619999885559082;
createNode lambert -n "phys";
	setAttr ".c" -type "float3" 1 0.597 0.86606956 ;
	setAttr ".it" -type "float3" 0.57852 0.57852 0.57852 ;
createNode phong -n "mat_broken_heli_engine";
	setAttr ".dc" 1;
	setAttr ".c" -type "float3" 0.80166 0.80166 0.80166 ;
	setAttr ".sc" -type "float3" 1 1 1 ;
	setAttr ".cp" 9.2899999618530273;
createNode lambert -n "mat_broken_heli";
	setAttr ".c" -type "float3" 0.17295939 0.14242502 0.67500001 ;
createNode phong -n "mat_rack";
	setAttr ".dc" 1;
	setAttr ".c" -type "float3" 0.30577999 0.30577999 0.30577999 ;
	setAttr ".sc" -type "float3" 0.19008 0.19008 0.19008 ;
	setAttr ".cp" 6.8619999885559082;
createNode phong -n "level_start_kit_mat_platform";
	setAttr ".dc" 1;
	setAttr ".ambc" -type "float3" 0.9752 0.9752 0.9752 ;
	setAttr ".ic" -type "float3" 0.9752 0.9752 0.9752 ;
createNode phong -n "mat_walls";
	setAttr ".dc" 1;
	setAttr ".sc" -type "float3" 0.42148 0.42148 0.42148 ;
	setAttr ".cp" 5.2399997711181641;
createNode phong -n "mat_platform";
	setAttr ".dc" 1;
	setAttr ".ambc" -type "float3" 0.41321999 0.41321999 0.41321999 ;
	setAttr ".sc" -type "float3" 0 0 0 ;
	setAttr ".cp" 2;
createNode phong -n "mat_phys";
	setAttr ".c" -type "float3" 1 0.42400002 0.98656017 ;
	setAttr ".it" -type "float3" 0.28099999 0.28099999 0.28099999 ;
createNode phong -n "mat_floor";
	setAttr ".dc" 1;
	setAttr ".c" -type "float3" 0.44151199 0.48199999 0.44751096 ;
	setAttr ".sc" -type "float3" 0 0 0 ;
	setAttr ".cp" 2;
createNode shadingEngine -n "phong8SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo6";
createNode shadingEngine -n "phong5SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
	setAttr -s 3 ".gn";
createNode groupId -n "groupId8";
	setAttr ".ihi" 0;
createNode groupId -n "groupId6";
	setAttr ".ihi" 0;
createNode groupId -n "groupId76";
	setAttr ".ihi" 0;
createNode materialInfo -n "materialInfo5";
createNode shadingEngine -n "phong4SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo4";
createNode shadingEngine -n "phong3SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo3";
createNode shadingEngine -n "phong2SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "forklift_01_materialInfo2";
createNode shadingEngine -n "phong1SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo1";
createNode shadingEngine -n "pasted__phong2SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "pasted__materialInfo8";
createNode shadingEngine -n "level_start_kit_phong3SG";
	setAttr ".ihi" 0;
	setAttr -s 4 ".dsm";
	setAttr ".ro" yes;
createNode materialInfo -n "level_start_kit_materialInfo3";
createNode shadingEngine -n "landingpad_phong1SG";
	setAttr ".ihi" 0;
	setAttr -s 14 ".dsm";
	setAttr ".ro" yes;
createNode materialInfo -n "landingpad_materialInfo1";
createNode shadingEngine -n "lambert4SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "helicopter_01_materialInfo3";
createNode shadingEngine -n "lambert3SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo2";
createNode shadingEngine -n "lambert2SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "helicopter_01_materialInfo1";
createNode shadingEngine -n "helicopter_01_phong5SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "helicopter_01_materialInfo5";
createNode shadingEngine -n "helicopter_01_phong4SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "forklift_01_materialInfo4";
createNode shadingEngine -n "helicopter_01_phong3SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode groupId -n "groupId74";
	setAttr ".ihi" 0;
createNode materialInfo -n "forklift_01_materialInfo3";
createNode shadingEngine -n "helicopter_01_phong1SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "forklift_01_materialInfo1";
createNode shadingEngine -n "blinn1SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "helicopter_01_materialInfo4";
createNode script -n "rg_export";
	addAttr -ci true -sn "time" -ln "time" -dt "string";
	setAttr ".time" -type "string" "2013-09-28T22:32:40.343000";
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
	setAttr -s 2 ".dsm";
	setAttr ".ro" yes;
	setAttr -s 3 ".gn";
select -ne :initialParticleSE;
	setAttr ".ro" yes;
select -ne :hardwareRenderGlobals;
	setAttr ".ctrs" 256;
	setAttr ".btrs" 512;
select -ne :defaultHardwareRenderGlobals;
	setAttr ".fn" -type "string" "im";
	setAttr ".res" -type "string" "ntsc_4d 646 485 1.333";
select -ne :hyperGraphLayout;
	setAttr -s 34 ".hyp";
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
	setAttr ".hyp[209].x" 136.16706848144531;
	setAttr ".hyp[209].y" -196.31689453125;
	setAttr ".hyp[209].isf" yes;
	setAttr ".hyp[210].x" 136.16706848144531;
	setAttr ".hyp[210].y" -196.31689453125;
	setAttr ".hyp[210].isf" yes;
	setAttr ".hyp[244].x" 153.7982177734375;
	setAttr ".hyp[244].y" -196.5198974609375;
	setAttr ".hyp[244].isf" yes;
	setAttr ".hyp[246].x" 145.19580078125;
	setAttr ".hyp[246].y" -263.07321166992187;
	setAttr ".hyp[246].isf" yes;
	setAttr ".hyp[248].x" -146.19102478027344;
	setAttr ".hyp[248].y" 25.004062652587891;
	setAttr ".hyp[248].isf" yes;
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
	setAttr ".hyp[321].isc" yes;
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
	setAttr ".hyp[325].x" 619.03741455078125;
	setAttr ".hyp[325].y" -27.031745910644531;
	setAttr ".hyp[325].isf" yes;
	setAttr ".hyp[326].x" 764.03741455078125;
	setAttr ".hyp[326].y" -27.031745910644531;
	setAttr ".hyp[326].isf" yes;
	setAttr ".hyp[327].x" 909.03741455078125;
	setAttr ".hyp[327].y" -27.031745910644531;
	setAttr ".hyp[327].isf" yes;
	setAttr ".hyp[328].x" 1054.037353515625;
	setAttr ".hyp[328].y" -27.031745910644531;
	setAttr ".hyp[328].isf" yes;
	setAttr ".hyp[329].x" 622.03741455078125;
	setAttr ".hyp[329].y" -80.031745910644531;
	setAttr ".hyp[329].isf" yes;
	setAttr ".hyp[330].x" 767.03741455078125;
	setAttr ".hyp[330].y" -80.031745910644531;
	setAttr ".hyp[330].isf" yes;
	setAttr ".hyp[331].x" 912.03741455078125;
	setAttr ".hyp[331].y" -80.031745910644531;
	setAttr ".hyp[331].isf" yes;
	setAttr ".hyp[332].x" 1057.037353515625;
	setAttr ".hyp[332].y" -80.031745910644531;
	setAttr ".hyp[332].isf" yes;
	setAttr ".hyp[333].x" 854.41973876953125;
	setAttr ".hyp[333].y" 52.860816955566406;
	setAttr ".hyp[333].isf" yes;
	setAttr ".hyp[334].x" 853.03216552734375;
	setAttr ".hyp[334].y" -27.625267028808594;
	setAttr ".hyp[334].isf" yes;
	setAttr ".hyp[337].isc" yes;
connectAttr "cameraView1.msg" ":perspShape.b" -na;
connectAttr "cameraView2.msg" ":perspShape.b" -na;
connectAttr "groupId75.id" "m_rackShape.iog.og[0].gid";
connectAttr ":initialShadingGroup.mwc" "m_rackShape.iog.og[0].gco";
connectAttr "groupId76.id" "m_rackShape.iog.og[1].gid";
connectAttr "phong5SG.mwc" "m_rackShape.iog.og[1].gco";
connectAttr "groupId22.id" "m_rackShape.ciog.cog[0].cgid";
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
connectAttr "level_start_kit_polyCube1.out" "polyPlanarProj1.ip";
connectAttr "|m_hangar|m_startingpad|m_landingpadShape.wm" "polyPlanarProj1.mp";
connectAttr "polyPlanarProj1.out" "polyPlanarProj2.ip";
connectAttr "|m_hangar|m_startingpad|m_landingpadShape.wm" "polyPlanarProj2.mp";
connectAttr "polyCube1.out" "polySplit1.ip";
connectAttr "polySplit1.out" "polySplit2.ip";
connectAttr "polySplit2.out" "polySplit3.ip";
connectAttr "polySplit3.out" "polyTweak1.ip";
connectAttr "polyTweak1.out" "polyExtrudeFace1.ip";
connectAttr "polyExtrudeFace1.out" "polyExtrudeFace2.ip";
connectAttr "polyExtrudeFace2.out" "polyExtrudeFace3.ip";
connectAttr "polyExtrudeFace3.out" "deleteComponent1.ig";
connectAttr "deleteComponent1.og" "deleteComponent2.ig";
connectAttr "deleteComponent2.og" "deleteComponent3.ig";
connectAttr "deleteComponent3.og" "deleteComponent4.ig";
connectAttr "deleteComponent4.og" "polyTweak2.ip";
connectAttr "polyTweak2.out" "deleteComponent5.ig";
connectAttr "deleteComponent5.og" "polyMergeVert1.ip";
connectAttr "polyMergeVert1.out" "polyMergeVert2.ip";
connectAttr "polyMergeVert2.out" "polyMergeVert3.ip";
connectAttr "polyMergeVert3.out" "polyMergeVert4.ip";
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
connectAttr "polyChipOff1.out" "groupParts1.ig";
connectAttr "groupId1.id" "groupParts1.gi";
connectAttr "groupParts1.og" "polySeparate1.ip";
connectAttr "polySeparate1.out[1]" "polyTweakUV2.ip";
connectAttr "pasted__file1.oc" "mat_black1.c";
connectAttr "pasted__file1.ot" "mat_black1.it";
connectAttr "level_start_kit_file2.oc" "level_start_kit_mat_platform.c";
connectAttr "level_start_kit_file2.ot" "level_start_kit_mat_platform.it";
connectAttr "file3.oc" "mat_walls.c";
connectAttr "file2.oc" "mat_platform.c";
connectAttr "file2.ot" "mat_platform.it";
connectAttr "mat_forklift_path1.oc" "phong8SG.ss";
connectAttr "phong8SG.msg" "materialInfo6.sg";
connectAttr "mat_forklift_path1.msg" "materialInfo6.m";
connectAttr "mat_rack.oc" "phong5SG.ss";
connectAttr "m_rackShape.iog.og[1]" "phong5SG.dsm" -na;
connectAttr "groupId76.msg" "phong5SG.gn" -na;
connectAttr "groupId6.msg" "phong5SG.gn" -na;
connectAttr "groupId8.msg" "phong5SG.gn" -na;
connectAttr "phong5SG.msg" "materialInfo5.sg";
connectAttr "mat_rack.msg" "materialInfo5.m";
connectAttr "mat_walls.oc" "phong4SG.ss";
connectAttr "m_hangarShape.iog" "phong4SG.dsm" -na;
connectAttr "phong4SG.msg" "materialInfo4.sg";
connectAttr "mat_walls.msg" "materialInfo4.m";
connectAttr "file3.msg" "materialInfo4.t" -na;
connectAttr "mat_platform.oc" "phong3SG.ss";
connectAttr "phong3SG.msg" "materialInfo3.sg";
connectAttr "mat_platform.msg" "materialInfo3.m";
connectAttr "file2.msg" "materialInfo3.t" -na;
connectAttr "blue.oc" "phong2SG.ss";
connectAttr "phong2SG.msg" "forklift_01_materialInfo2.sg";
connectAttr "blue.msg" "forklift_01_materialInfo2.m";
connectAttr "mat_floor.oc" "phong1SG.ss";
connectAttr "m_floorShape.iog" "phong1SG.dsm" -na;
connectAttr "phong1SG.msg" "materialInfo1.sg";
connectAttr "mat_floor.msg" "materialInfo1.m";
connectAttr "mat_black1.oc" "pasted__phong2SG.ss";
connectAttr "pasted__phong2SG.msg" "pasted__materialInfo8.sg";
connectAttr "mat_black1.msg" "pasted__materialInfo8.m";
connectAttr "pasted__file1.msg" "pasted__materialInfo8.t" -na;
connectAttr "level_start_kit_mat_platform.oc" "level_start_kit_phong3SG.ss";
connectAttr "|m_hangar|m_startingpad|m_landingpadShape.iog" "level_start_kit_phong3SG.dsm"
		 -na;
connectAttr "|m_hangar|m_startingpad|phys_startingpad|m_landingpadShape.iog" "level_start_kit_phong3SG.dsm"
		 -na;
connectAttr "|m_hangar|m_landingpad|m_landingpadShape.iog" "level_start_kit_phong3SG.dsm"
		 -na;
connectAttr "|m_hangar|m_landingpad|phys_landingpad|m_landingpadShape.iog" "level_start_kit_phong3SG.dsm"
		 -na;
connectAttr "level_start_kit_phong3SG.msg" "level_start_kit_materialInfo3.sg";
connectAttr "level_start_kit_mat_platform.msg" "level_start_kit_materialInfo3.m"
		;
connectAttr "level_start_kit_file2.msg" "level_start_kit_materialInfo3.t" -na;
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
connectAttr "landingpad_phong1SG.msg" "landingpad_materialInfo1.sg";
connectAttr "mat_phys.msg" "landingpad_materialInfo1.m";
connectAttr "phys.oc" "lambert4SG.ss";
connectAttr "phys_pos_simShape.iog" "lambert4SG.dsm" -na;
connectAttr "lambert4SG.msg" "helicopter_01_materialInfo3.sg";
connectAttr "phys.msg" "helicopter_01_materialInfo3.m";
connectAttr "mat_broken_heli_engine.oc" "lambert3SG.ss";
connectAttr "lambert3SG.msg" "materialInfo2.sg";
connectAttr "mat_broken_heli_engine.msg" "materialInfo2.m";
connectAttr "mat_broken_heli.oc" "lambert2SG.ss";
connectAttr "lambert2SG.msg" "helicopter_01_materialInfo1.sg";
connectAttr "mat_broken_heli.msg" "helicopter_01_materialInfo1.m";
connectAttr "forklift_01_phys.oc" "helicopter_01_phong5SG.ss";
connectAttr "helicopter_01_phong5SG.msg" "helicopter_01_materialInfo5.sg";
connectAttr "forklift_01_phys.msg" "helicopter_01_materialInfo5.m";
connectAttr "grey.oc" "helicopter_01_phong4SG.ss";
connectAttr "helicopter_01_phong4SG.msg" "forklift_01_materialInfo4.sg";
connectAttr "grey.msg" "forklift_01_materialInfo4.m";
connectAttr "mat_black.oc" "helicopter_01_phong3SG.ss";
connectAttr "groupId74.msg" "helicopter_01_phong3SG.gn" -na;
connectAttr "helicopter_01_phong3SG.msg" "forklift_01_materialInfo3.sg";
connectAttr "mat_black.msg" "forklift_01_materialInfo3.m";
connectAttr "gray.oc" "helicopter_01_phong1SG.ss";
connectAttr "helicopter_01_phong1SG.msg" "forklift_01_materialInfo1.sg";
connectAttr "gray.msg" "forklift_01_materialInfo1.m";
connectAttr "mat_broken_heli_window.oc" "blinn1SG.ss";
connectAttr "blinn1SG.msg" "helicopter_01_materialInfo4.sg";
connectAttr "mat_broken_heli_window.msg" "helicopter_01_materialInfo4.m";
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
connectAttr "m_rackShape.ciog.cog[0]" ":initialShadingGroup.dsm" -na;
connectAttr "m_rackShape.iog.og[0]" ":initialShadingGroup.dsm" -na;
connectAttr "groupId1.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId22.msg" ":initialShadingGroup.gn" -na;
connectAttr "groupId75.msg" ":initialShadingGroup.gn" -na;
connectAttr "m_startingpad.msg" ":hyperGraphLayout.hyp[0].dn";
connectAttr "phys_landingpad.msg" ":hyperGraphLayout.hyp[1].dn";
connectAttr "m_landingpad.msg" ":hyperGraphLayout.hyp[2].dn";
connectAttr "phys_pos_path_start.msg" ":hyperGraphLayout.hyp[4].dn";
connectAttr "phys_pos_path_1.msg" ":hyperGraphLayout.hyp[5].dn";
connectAttr "phys_pos_path_land.msg" ":hyperGraphLayout.hyp[6].dn";
connectAttr ":side.msg" ":hyperGraphLayout.hyp[209].dn";
connectAttr ":front.msg" ":hyperGraphLayout.hyp[210].dn";
connectAttr "phys_trig_landing.msg" ":hyperGraphLayout.hyp[244].dn";
connectAttr "m_rack.msg" ":hyperGraphLayout.hyp[246].dn";
connectAttr "m_hangar.msg" ":hyperGraphLayout.hyp[248].dn";
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
connectAttr "i_m_rack3.msg" ":hyperGraphLayout.hyp[325].dn";
connectAttr "i_m_rack4.msg" ":hyperGraphLayout.hyp[326].dn";
connectAttr "i_m_rackPillar7.msg" ":hyperGraphLayout.hyp[327].dn";
connectAttr "i_m_rackPillar8.msg" ":hyperGraphLayout.hyp[328].dn";
connectAttr "i_m_rackPillar9.msg" ":hyperGraphLayout.hyp[329].dn";
connectAttr "i_m_rackPillar10.msg" ":hyperGraphLayout.hyp[330].dn";
connectAttr "i_m_rackPillar11.msg" ":hyperGraphLayout.hyp[331].dn";
connectAttr "i_m_rackPillar12.msg" ":hyperGraphLayout.hyp[332].dn";
connectAttr ":top.msg" ":hyperGraphLayout.hyp[333].dn";
connectAttr ":persp.msg" ":hyperGraphLayout.hyp[334].dn";
connectAttr "transform8.msg" ":hyperGraphLayout.hyp[337].dn";
// End of level_07.ma
