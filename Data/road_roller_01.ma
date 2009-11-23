//Maya ASCII 2009 scene
//Name: road_roller_01.ma
//Last modified: Sun, Nov 15, 2009 07:58:01 PM
//Codeset: 1252
requires maya "2009";
currentUnit -l centimeter -a degree -t film;
fileInfo "application" "maya";
fileInfo "product" "Maya Unlimited 2009";
fileInfo "version" "2009 Service Pack 1a";
fileInfo "cutIdentifier" "200904080023-749524";
fileInfo "osv" "Microsoft Windows XP Service Pack 3 (Build 2600)\n";
createNode transform -s -n "persp";
	setAttr ".v" no;
	setAttr ".t" -type "double3" 3.3603840270409893 0.73413476983819381 1.817363585758095 ;
	setAttr ".r" -type "double3" -3.9383527296469989 87.00000000000162 -7.5964855469084246e-015 ;
createNode camera -s -n "perspShape" -p "persp";
	setAttr -k off ".v" no;
	setAttr ".fl" 34.999999999999993;
	setAttr ".coi" 3.1663336735796936;
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
	setAttr ".ow" 30;
	setAttr ".imn" -type "string" "top";
	setAttr ".den" -type "string" "top_depth";
	setAttr ".man" -type "string" "top_mask";
	setAttr ".hc" -type "string" "viewSet -t %camera";
	setAttr ".o" yes;
createNode transform -s -n "front";
	setAttr ".v" no;
	setAttr ".t" -type "double3" 0.054672486838485357 0.66700433942951876 100.1 ;
createNode camera -s -n "frontShape" -p "front";
	setAttr -k off ".v" no;
	setAttr ".rnd" no;
	setAttr ".coi" 100.1;
	setAttr ".ow" 4.3409954549757197;
	setAttr ".imn" -type "string" "front";
	setAttr ".den" -type "string" "front_depth";
	setAttr ".man" -type "string" "front_mask";
	setAttr ".hc" -type "string" "viewSet -f %camera";
	setAttr ".o" yes;
createNode transform -s -n "side";
	setAttr ".v" no;
	setAttr ".t" -type "double3" 100.1 0 0 ;
	setAttr ".r" -type "double3" 0 89.999999999999986 0 ;
createNode camera -s -n "sideShape" -p "side";
	setAttr -k off ".v" no;
	setAttr ".rnd" no;
	setAttr ".coi" 100.1;
	setAttr ".ow" 30;
	setAttr ".imn" -type "string" "side";
	setAttr ".den" -type "string" "side_depth";
	setAttr ".man" -type "string" "side_mask";
	setAttr ".hc" -type "string" "viewSet -s %camera";
	setAttr ".o" yes;
createNode transform -n "m_blob1";
	setAttr ".s" -type "double3" 1 1.3505910070627392 1 ;
createNode mesh -n "m_blob1Shape" -p "m_blob1";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 8 ".pt[0:7]" -type "float3"  0.0011098094 0 -0.025803005 
		-0.0011098094 0 -0.025803005 0.080914408 0 0.060405865 -0.082138076 0 0.060405865 
		0.02207166 0 -0.060405865 -0.023295321 0 -0.060405865 0.0011098094 0 0.025803005 
		-0.0011098094 0 0.025803005;
	setAttr ".rgvtx" -type "vectorArray" 8 -0.3481326699256897 -0.2455446720123291
		 0.30228444933891296 0.3481326699256897 -0.2455446720123291 0.30228444933891296 -0.26832807064056396
		 0.2455446720123291 0.38849329948425293 0.2671043872833252 0.2455446720123291 0.38849329948425293 -0.32717081904411316
		 0.2455446720123291 -0.38849329948425293 0.32594716548919678 0.2455446720123291 -0.38849329948425293 -0.3481326699256897
		 -0.2455446720123291 -0.30228444933891296 0.3481326699256897 -0.2455446720123291 -0.30228444933891296 ;
	setAttr ".rgf" -type "string" "[[0,1,3,2],[2,3,5,4],[4,5,7,6],[6,7,1,0],[1,7,5,3],[6,0,2,4]]";
createNode transform -n "m_blob2" -p "m_blob1";
	setAttr ".t" -type "double3" 0.0019587686646520552 -0.029890138725062132 -0.85946261285465453 ;
	setAttr ".s" -type "double3" 1 0.85255082681312899 1.1270626377053563 ;
	setAttr ".rp" -type "double3" -0.0019587686646520557 0.029912768945396514 0.85951119820225697 ;
	setAttr ".sp" -type "double3" -0.0019587686646520557 0.035086200147399667 0.76261173908859159 ;
	setAttr ".spt" -type "double3" 0 -0.005173431202003153 0.096899459113665445 ;
createNode mesh -n "m_blobShape2" -p "m_blob2";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".rgvtx" -type "vectorArray" 8 -0.13912564516067505 -0.25595393776893616
		 0.5087619423866272 0.13912564516067505 -0.25595393776893616 0.5087619423866272 -0.13912564516067505
		 0.25595393776893616 0.5087619423866272 0.13912564516067505 0.25595393776893616 0.5087619423866272 -0.13912564516067505
		 0.25595393776893616 -0.5087619423866272 0.13912564516067505 0.25595393776893616 -0.5087619423866272 -0.13912564516067505
		 -0.25595393776893616 -0.5087619423866272 0.13912564516067505 -0.25595393776893616
		 -0.5087619423866272 ;
	setAttr ".rgf" -type "string" "[[0,1,3,2],[2,3,5,4],[4,5,7,6],[6,7,1,0],[1,7,5,3],[6,0,2,4]]";
createNode transform -n "m_hut" -p "m_blob1";
	setAttr ".t" -type "double3" 0 0.31900866956724555 -0.9101390479586301 ;
	setAttr ".s" -type "double3" 1.3548575074563876 0.74041659893382283 1.6058234812216212 ;
	setAttr ".rp" -type "double3" 0 -0.31898603934691122 0.91018763330623231 ;
	setAttr ".sp" -type "double3" 0 -0.43081967612049932 0.56680428699038088 ;
	setAttr ".spt" -type "double3" 0 0.11183363677358817 0.34338334631585143 ;
createNode mesh -n "m_hutShape" -p "m_hut";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 9 ".pt";
	setAttr ".pt[2:5]" -type "float3" 0 -2.9802322e-008 0  0 -2.9802322e-008 
		0  0 -2.9802322e-008 0  0 -2.9802322e-008 0 ;
	setAttr ".pt[8:11]" -type "float3" 0.15129864 0.70390147 -0.099056758  
		-0.13256708 0.70390147 -0.099056758  -0.13256708 0.70390147 0.15729937  0.15129864 
		0.70390147 0.15729937 ;
	setAttr ".rgvtx" -type "vectorArray" 12 -0.36532342433929443 -0.21355301141738892
		 0.32991969585418701 0.36532342433929443 -0.21355301141738892 0.32991969585418701 -0.36532342433929443
		 0.2135530412197113 0.32991969585418701 0.36532342433929443 0.2135530412197113 0.32991969585418701 -0.36532342433929443
		 0.2135530412197113 -0.32991966605186462 0.36532342433929443 0.2135530412197113 -0.32991966605186462 -0.36532342433929443
		 -0.21355301141738892 -0.32991966605186462 0.36532342433929443 -0.21355301141738892
		 -0.32991966605186462 -0.21402478218078613 0.91745454072952271 0.23086294531822205 0.2327563464641571
		 0.91745454072952271 0.23086294531822205 0.2327563464641571 0.91745454072952271 -0.17262029647827148 -0.21402478218078613
		 0.91745454072952271 -0.17262029647827148 ;
	setAttr ".rgf" -type "string" "[[0,1,3,2],[8,9,10,11],[4,5,7,6],[6,7,1,0],[1,7,5,3],[6,0,2,4],[2,3,9,8],[3,5,10,9],[5,4,11,10],[4,2,8,11]]";
createNode transform -n "m_rear_wheel0" -p "m_blob1";
	setAttr ".t" -type "double3" 0.4 -0.050251656847272175 -0.91885141465239772 ;
	setAttr ".r" -type "double3" 0 0 -89.999999999999986 ;
	setAttr ".s" -type "double3" 0.74041659893382283 1 1 ;
createNode mesh -n "m_rear_wheelShape0" -p "m_rear_wheel0";
	addAttr -ci true -sn "mso" -ln "miShadingSamplesOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "msh" -ln "miShadingSamples" -min 0 -smx 8 -at "float";
	addAttr -ci true -sn "mdo" -ln "miMaxDisplaceOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "mmd" -ln "miMaxDisplace" -min 0 -smx 1 -at "float";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".rgvtx" -type "vectorArray" 42 0.47552856802940369 -0.20000000298023224
		 -0.15450859069824219 0.40450876951217651 -0.20000000298023224 -0.29389280080795288 0.29389280080795288
		 -0.20000000298023224 -0.40450873970985413 0.15450857579708099 -0.20000000298023224
		 -0.47552850842475891 0 -0.20000000298023224 -0.5000002384185791 -0.15450857579708099
		 -0.20000000298023224 -0.47552847862243652 -0.29389274120330811 -0.20000000298023224
		 -0.40450865030288696 -0.40450862050056458 -0.20000000298023224 -0.29389271140098572 -0.47552838921546936
		 -0.20000000298023224 -0.15450853109359741 -0.50000011920928955 -0.20000000298023224
		 0 -0.47552838921546936 -0.20000000298023224 0.15450853109359741 -0.40450859069824219
		 -0.20000000298023224 0.29389268159866333 -0.29389268159866333 -0.20000000298023224
		 0.4045085608959198 -0.15450853109359741 -0.20000000298023224 0.47552832961082458 -1.4901161193847656e-008
		 -0.20000000298023224 0.50000005960464478 0.15450848639011383 -0.20000000298023224
		 0.4755282998085022 0.29389262199401855 -0.20000000298023224 0.40450853109359741 0.40450850129127502
		 -0.20000000298023224 0.29389265179634094 0.47552827000617981 -0.20000000298023224
		 0.15450850129127502 0.5 -0.20000000298023224 0 0.47552856802940369 0.20000000298023224
		 -0.15450859069824219 0.40450876951217651 0.20000000298023224 -0.29389280080795288 0.29389280080795288
		 0.20000000298023224 -0.40450873970985413 0.15450857579708099 0.20000000298023224
		 -0.47552850842475891 0 0.20000000298023224 -0.5000002384185791 -0.15450857579708099
		 0.20000000298023224 -0.47552847862243652 -0.29389274120330811 0.20000000298023224
		 -0.40450865030288696 -0.40450862050056458 0.20000000298023224 -0.29389271140098572 -0.47552838921546936
		 0.20000000298023224 -0.15450853109359741 -0.50000011920928955 0.20000000298023224
		 0 -0.47552838921546936 0.20000000298023224 0.15450853109359741 -0.40450859069824219
		 0.20000000298023224 0.29389268159866333 -0.29389268159866333 0.20000000298023224
		 0.4045085608959198 -0.15450853109359741 0.20000000298023224 0.47552832961082458 -1.4901161193847656e-008
		 0.20000000298023224 0.50000005960464478 0.15450848639011383 0.20000000298023224 0.4755282998085022 0.29389262199401855
		 0.20000000298023224 0.40450853109359741 0.40450850129127502 0.20000000298023224 0.29389265179634094 0.47552827000617981
		 0.20000000298023224 0.15450850129127502 0.5 0.20000000298023224 0 0 -0.20000000298023224
		 0 0 0.20000000298023224 0 ;
	setAttr ".rgf" -type "string" "[[0,1,21,20],[1,2,22,21],[2,3,23,22],[3,4,24,23],[4,5,25,24],[5,6,26,25],[6,7,27,26],[7,8,28,27],[8,9,29,28],[9,10,30,29],[10,11,31,30],[11,12,32,31],[12,13,33,32],[13,14,34,33],[14,15,35,34],[15,16,36,35],[16,17,37,36],[17,18,38,37],[18,19,39,38],[19,0,20,39],[1,0,40],[2,1,40],[3,2,40],[4,3,40],[5,4,40],[6,5,40],[7,6,40],[8,7,40],[9,8,40],[10,9,40],[11,10,40],[12,11,40],[13,12,40],[14,13,40],[15,14,40],[16,15,40],[17,16,40],[18,17,40],[19,18,40],[0,19,40],[20,21,41],[21,22,41],[22,23,41],[23,24,41],[24,25,41],[25,26,41],[26,27,41],[27,28,41],[28,29,41],[29,30,41],[30,31,41],[31,32,41],[32,33,41],[33,34,41],[34,35,41],[35,36,41],[36,37,41],[37,38,41],[38,39,41],[39,20,41]]";
createNode transform -n "phys_rear_wheel0" -p "m_rear_wheel0";
	setAttr ".r" -type "double3" 0 0 89.999999999999986 ;
createNode mesh -n "phys_rear_wheelShape0" -p "phys_rear_wheel0";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "m_rear_wheel1" -p "m_blob1";
	setAttr ".t" -type "double3" -0.4 -0.050251656847272175 -0.91885141465239772 ;
	setAttr ".r" -type "double3" 0 0 -89.999999999999986 ;
	setAttr ".s" -type "double3" 0.74041659893382283 1 1 ;
createNode mesh -n "m_rear_wheelShape1" -p "m_rear_wheel1";
	addAttr -ci true -sn "mso" -ln "miShadingSamplesOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "msh" -ln "miShadingSamples" -min 0 -smx 8 -at "float";
	addAttr -ci true -sn "mdo" -ln "miMaxDisplaceOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "mmd" -ln "miMaxDisplace" -min 0 -smx 1 -at "float";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".rgvtx" -type "vectorArray" 42 0.47552856802940369 -0.20000000298023224
		 -0.15450859069824219 0.40450876951217651 -0.20000000298023224 -0.29389280080795288 0.29389280080795288
		 -0.20000000298023224 -0.40450873970985413 0.15450857579708099 -0.20000000298023224
		 -0.47552850842475891 0 -0.20000000298023224 -0.5000002384185791 -0.15450857579708099
		 -0.20000000298023224 -0.47552847862243652 -0.29389274120330811 -0.20000000298023224
		 -0.40450865030288696 -0.40450862050056458 -0.20000000298023224 -0.29389271140098572 -0.47552838921546936
		 -0.20000000298023224 -0.15450853109359741 -0.50000011920928955 -0.20000000298023224
		 0 -0.47552838921546936 -0.20000000298023224 0.15450853109359741 -0.40450859069824219
		 -0.20000000298023224 0.29389268159866333 -0.29389268159866333 -0.20000000298023224
		 0.4045085608959198 -0.15450853109359741 -0.20000000298023224 0.47552832961082458 -1.4901161193847656e-008
		 -0.20000000298023224 0.50000005960464478 0.15450848639011383 -0.20000000298023224
		 0.4755282998085022 0.29389262199401855 -0.20000000298023224 0.40450853109359741 0.40450850129127502
		 -0.20000000298023224 0.29389265179634094 0.47552827000617981 -0.20000000298023224
		 0.15450850129127502 0.5 -0.20000000298023224 0 0.47552856802940369 0.20000000298023224
		 -0.15450859069824219 0.40450876951217651 0.20000000298023224 -0.29389280080795288 0.29389280080795288
		 0.20000000298023224 -0.40450873970985413 0.15450857579708099 0.20000000298023224
		 -0.47552850842475891 0 0.20000000298023224 -0.5000002384185791 -0.15450857579708099
		 0.20000000298023224 -0.47552847862243652 -0.29389274120330811 0.20000000298023224
		 -0.40450865030288696 -0.40450862050056458 0.20000000298023224 -0.29389271140098572 -0.47552838921546936
		 0.20000000298023224 -0.15450853109359741 -0.50000011920928955 0.20000000298023224
		 0 -0.47552838921546936 0.20000000298023224 0.15450853109359741 -0.40450859069824219
		 0.20000000298023224 0.29389268159866333 -0.29389268159866333 0.20000000298023224
		 0.4045085608959198 -0.15450853109359741 0.20000000298023224 0.47552832961082458 -1.4901161193847656e-008
		 0.20000000298023224 0.50000005960464478 0.15450848639011383 0.20000000298023224 0.4755282998085022 0.29389262199401855
		 0.20000000298023224 0.40450853109359741 0.40450850129127502 0.20000000298023224 0.29389265179634094 0.47552827000617981
		 0.20000000298023224 0.15450850129127502 0.5 0.20000000298023224 0 0 -0.20000000298023224
		 0 0 0.20000000298023224 0 ;
	setAttr ".rgf" -type "string" "[[0,1,21,20],[1,2,22,21],[2,3,23,22],[3,4,24,23],[4,5,25,24],[5,6,26,25],[6,7,27,26],[7,8,28,27],[8,9,29,28],[9,10,30,29],[10,11,31,30],[11,12,32,31],[12,13,33,32],[13,14,34,33],[14,15,35,34],[15,16,36,35],[16,17,37,36],[17,18,38,37],[18,19,39,38],[19,0,20,39],[1,0,40],[2,1,40],[3,2,40],[4,3,40],[5,4,40],[6,5,40],[7,6,40],[8,7,40],[9,8,40],[10,9,40],[11,10,40],[12,11,40],[13,12,40],[14,13,40],[15,14,40],[16,15,40],[17,16,40],[18,17,40],[19,18,40],[0,19,40],[20,21,41],[21,22,41],[22,23,41],[23,24,41],[24,25,41],[25,26,41],[26,27,41],[27,28,41],[28,29,41],[29,30,41],[30,31,41],[31,32,41],[32,33,41],[33,34,41],[34,35,41],[35,36,41],[36,37,41],[37,38,41],[38,39,41],[39,20,41]]";
createNode transform -n "phys_rear_wheel1" -p "m_rear_wheel1";
	setAttr ".r" -type "double3" 0 0 89.999999999999986 ;
createNode mesh -n "phys_rear_wheelShape1" -p "phys_rear_wheel1";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "phys_blob1" -p "m_blob1";
	setAttr ".t" -type "double3" 0 -0.02985871920737454 -0.0010886739145398172 ;
	setAttr ".s" -type "double3" 1 0.46719713663884588 1.1254082247880413 ;
	setAttr ".rp" -type "double3" 0 0.029881349427708898 0.0011372592621421028 ;
	setAttr ".sp" -type "double3" 0 0.06395875977041332 0.0010105304342842292 ;
	setAttr ".spt" -type "double3" 0 -0.03407741034270443 0.0001267288278578735 ;
createNode mesh -n "phys_blobShape1" -p "phys_blob1";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "phys_hut" -p "m_blob1";
	setAttr ".t" -type "double3" 0 0.58188099331072229 -0.89539860925657633 ;
	setAttr ".s" -type "double3" 1.096833580360298 0.61539909986837682 1 ;
	setAttr ".rp" -type "double3" -6.018531076210112e-036 -0.58185836309038796 0.89544719460417854 ;
	setAttr ".sp" -type "double3" -4.9424187584107139e-020 -0.94549758557468944 0.89544719460417854 ;
	setAttr ".spt" -type "double3" 4.9424187584107133e-020 0.36363922248430142 0 ;
createNode mesh -n "phys_hutShape" -p "phys_hut";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "m_fender" -p "m_blob1";
	setAttr ".t" -type "double3" 0 -0.16131414668734562 0.85114858534760229 ;
	setAttr ".r" -type "double3" 0 0 -89.999999999999986 ;
	setAttr ".s" -type "double3" 0.74041659893382283 1 1 ;
createNode mesh -n "m_fenderShape" -p "m_fender";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".rgvtx" -type "vectorArray" 44 0 -0.11545225977897644 -0.39999997615814209 -0.12360679358243942
		 -0.11545225977897644 -0.38042259216308594 -0.23511409759521484 -0.11545225977897644
		 -0.32360678911209106 -0.32360678911209106 -0.11545225977897644 -0.23511409759521484 -0.38042259216308594
		 -0.11545225977897644 -0.12360679358243942 -0.39999997615814209 -0.11545225977897644
		 -2.5635551410324906e-017 -0.38042259216308594 -0.11545225977897644 0.12360679358243942 -0.32360678911209106
		 -0.11545225977897644 0.23511409759521484 -0.23511409759521484 -0.11545225977897644
		 0.32360678911209106 -0.12360679358243942 -0.11545225977897644 0.38042259216308594 0
		 -0.11545225977897644 0.39999997615814209 0 0.11545225977897644 -0.39999997615814209 -0.12360679358243942
		 0.11545225977897644 -0.38042259216308594 -0.23511409759521484 0.11545225977897644
		 -0.32360678911209106 -0.32360678911209106 0.11545225977897644 -0.23511409759521484 -0.38042259216308594
		 0.11545225977897644 -0.12360679358243942 -0.39999997615814209 0.11545225977897644
		 2.5635551410324906e-017 -0.38042259216308594 0.11545225977897644 0.12360679358243942 -0.32360678911209106
		 0.11545225977897644 0.23511409759521484 -0.23511409759521484 0.11545225977897644
		 0.32360678911209106 -0.12360679358243942 0.11545225977897644 0.38042259216308594 0
		 0.11545225977897644 0.39999997615814209 -1.4901161193847656e-008 0.11545225977897644
		 -0.45000004768371582 -0.13905768096446991 0.11545225977897644 -0.42797547578811646 -0.26450341939926147
		 0.11545225977897644 -0.36405768990516663 -0.36405771970748901 0.11545225977897644
		 -0.26450338959693909 -0.42797553539276123 0.11545225977897644 -0.13905765116214752 -0.4500001072883606
		 0.11545225977897644 1.4901161193847656e-008 -0.42797553539276123 0.11545225977897644
		 0.1390576958656311 -0.3640577495098114 0.11545225977897644 0.26450341939926147 -0.26450344920158386
		 0.11545225977897644 0.3640577495098114 -0.1390576958656311 0.11545225977897644 0.42797556519508362 0
		 0.11545225977897644 0.45000016689300537 -1.4901161193847656e-008 -0.11545225977897644
		 -0.45000004768371582 -0.13905768096446991 -0.11545225977897644 -0.42797547578811646 -0.26450341939926147
		 -0.11545225977897644 -0.36405768990516663 -0.36405771970748901 -0.11545225977897644
		 -0.26450338959693909 -0.42797553539276123 -0.11545225977897644 -0.13905765116214752 -0.4500001072883606
		 -0.11545225977897644 1.4901161193847656e-008 -0.42797553539276123 -0.11545225977897644
		 0.1390576958656311 -0.3640577495098114 -0.11545225977897644 0.26450341939926147 -0.26450344920158386
		 -0.11545225977897644 0.3640577495098114 -0.1390576958656311 -0.11545225977897644
		 0.42797556519508362 0 -0.11545225977897644 0.45000016689300537 ;
	setAttr ".rgf" -type "string" "[[1,0,11,12],[2,1,12,13],[3,2,13,14],[4,3,14,15],[5,4,15,16],[6,5,16,17],[7,6,17,18],[8,7,18,19],[9,8,19,20],[10,9,20,21],[12,11,22,23],[13,12,23,24],[14,13,24,25],[15,14,25,26],[16,15,26,27],[17,16,27,28],[18,17,28,29],[19,18,29,30],[20,19,30,31],[21,20,31,32],[23,22,33,34],[24,23,34,35],[25,24,35,36],[26,25,36,37],[27,26,37,38],[28,27,38,39],[29,28,39,40],[30,29,40,41],[31,30,41,42],[32,31,42,43],[34,33,0,1],[35,34,1,2],[36,35,2,3],[37,36,3,4],[38,37,4,5],[39,38,5,6],[40,39,6,7],[41,40,7,8],[42,41,8,9],[43,42,9,10]]";
createNode transform -n "m_fender1" -p "m_fender";
	setAttr ".t" -type "double3" 0 0 0.42196612130210553 ;
	setAttr ".r" -type "double3" 0 0 89.999999999999986 ;
	setAttr ".rp" -type "double3" 0 0 -0.42196612130210553 ;
	setAttr ".sp" -type "double3" 0 0 -0.42196612130210553 ;
createNode mesh -n "m_fenderShape1" -p "m_fender1";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".rgvtx" -type "vectorArray" 8 -0.34999999403953552 -0.035000000149011612
		 0.014999999664723873 0.34999999403953552 -0.035000000149011612 0.014999999664723873 -0.34999999403953552
		 0.035000000149011612 0.014999999664723873 0.34999999403953552 0.035000000149011612
		 0.014999999664723873 -0.34999999403953552 0.035000000149011612 -0.014999999664723873 0.34999999403953552
		 0.035000000149011612 -0.014999999664723873 -0.34999999403953552 -0.035000000149011612
		 -0.014999999664723873 0.34999999403953552 -0.035000000149011612 -0.014999999664723873 ;
	setAttr ".rgf" -type "string" "[[0,1,3,2],[2,3,5,4],[4,5,7,6],[6,7,1,0],[1,7,5,3],[6,0,2,4]]";
createNode transform -n "m_fender2" -p "m_fender";
	setAttr ".t" -type "double3" 0 0 -0.42149315519162167 ;
	setAttr ".r" -type "double3" 0 0 89.999999999999986 ;
	setAttr ".rp" -type "double3" 0 0 0.42149315519162167 ;
	setAttr ".sp" -type "double3" 0 0 0.42149315519162167 ;
createNode mesh -n "m_fenderShape2" -p "m_fender2";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".rgvtx" -type "vectorArray" 8 -0.34999999403953552 -0.035000000149011612
		 0.014999999664723873 0.34999999403953552 -0.035000000149011612 0.014999999664723873 -0.34999999403953552
		 0.035000000149011612 0.014999999664723873 0.34999999403953552 0.035000000149011612
		 0.014999999664723873 -0.34999999403953552 0.035000000149011612 -0.014999999664723873 0.34999999403953552
		 0.035000000149011612 -0.014999999664723873 -0.34999999403953552 -0.035000000149011612
		 -0.014999999664723873 0.34999999403953552 -0.035000000149011612 -0.014999999664723873 ;
	setAttr ".rgf" -type "string" "[[0,1,3,2],[2,3,5,4],[4,5,7,6],[6,7,1,0],[1,7,5,3],[6,0,2,4]]";
createNode transform -n "m_fender3" -p "m_fender";
	setAttr ".t" -type "double3" 0.003972905183467268 -0.33552265608120913 0.002892991937208933 ;
	setAttr ".r" -type "double3" 0 0 89.999999999999986 ;
	setAttr ".rp" -type "double3" 0.33552265608120913 0.0039729051834673235 -0.002892991937208933 ;
	setAttr ".rpt" -type "double3" -0.33949556126467639 0.3315497508977418 0 ;
	setAttr ".sp" -type "double3" 0.33552265608120913 0.0039729051834673235 -0.002892991937208933 ;
createNode mesh -n "m_fenderShape3" -p "m_fender3";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".rgvtx" -type "vectorArray" 8 -0.014999999664723873 -0.035000000149011612
		 0.42500001192092896 0.014999999664723873 -0.035000000149011612 0.42500001192092896 -0.014999999664723873
		 0.035000000149011612 0.42500001192092896 0.014999999664723873 0.035000000149011612
		 0.42500001192092896 -0.014999999664723873 0.035000000149011612 -0.42500001192092896 0.014999999664723873
		 0.035000000149011612 -0.42500001192092896 -0.014999999664723873 -0.035000000149011612
		 -0.42500001192092896 0.014999999664723873 -0.035000000149011612 -0.42500001192092896 ;
	setAttr ".rgf" -type "string" "[[0,1,3,2],[2,3,5,4],[4,5,7,6],[6,7,1,0],[1,7,5,3],[6,0,2,4]]";
createNode transform -n "m_fender4" -p "m_fender";
	setAttr ".t" -type "double3" 0.003972905183467379 0.33328177704247786 0.002892991937208933 ;
	setAttr ".r" -type "double3" 0 0 89.999999999999986 ;
	setAttr ".rp" -type "double3" -0.33328177704247786 0.0039729051834673235 -0.002892991937208933 ;
	setAttr ".rpt" -type "double3" 0.32930887185901048 -0.33725468222594518 0 ;
	setAttr ".sp" -type "double3" -0.33328177704247786 0.0039729051834673235 -0.002892991937208933 ;
createNode mesh -n "m_fenderShape4" -p "m_fender4";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".rgvtx" -type "vectorArray" 8 -0.014999999664723873 -0.035000000149011612
		 0.42500001192092896 0.014999999664723873 -0.035000000149011612 0.42500001192092896 -0.014999999664723873
		 0.035000000149011612 0.42500001192092896 0.014999999664723873 0.035000000149011612
		 0.42500001192092896 -0.014999999664723873 0.035000000149011612 -0.42500001192092896 0.014999999664723873
		 0.035000000149011612 -0.42500001192092896 -0.014999999664723873 -0.035000000149011612
		 -0.42500001192092896 0.014999999664723873 -0.035000000149011612 -0.42500001192092896 ;
	setAttr ".rgf" -type "string" "[[0,1,3,2],[2,3,5,4],[4,5,7,6],[6,7,1,0],[1,7,5,3],[6,0,2,4]]";
createNode transform -n "phys_fender" -p "m_fender";
	setAttr ".t" -type "double3" -0.2197799986330794 3.6029245035121157e-017 -0.34125554066833641 ;
	setAttr ".r" -type "double3" 0 0 89.999999999999986 ;
	setAttr ".s" -type "double3" 1 5.2479817436855765 17.424151940223179 ;
	setAttr ".rp" -type "double3" 6.8283964424535015e-017 -0.21977999863307934 0.34125554066833635 ;
	setAttr ".rpt" -type "double3" 0.21977999863307929 0.21977999863307929 0 ;
	setAttr ".sp" -type "double3" 9.6039540040163929e-017 -0.041719735833360611 -9.9000557152478796e-005 ;
	setAttr ".spt" -type "double3" 0 -0.17806026279971873 0.34135454122548886 ;
createNode mesh -n "phys_fenderShape" -p "phys_fender";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "m_front_wheel" -p "m_fender";
	setAttr ".t" -type "double3" 0 0 0.00040100042301505212 ;
	setAttr ".r" -type "double3" 0 0 3.3024768059650058e-015 ;
createNode mesh -n "m_front_wheelShape" -p "m_front_wheel";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".rgvtx" -type "vectorArray" 42 0.3328700065612793 -0.30000001192092896
		 -0.10815601050853729 0.2831561267375946 -0.30000001192092896 -0.20572495460510254 0.20572495460510254
		 -0.30000001192092896 -0.2831561267375946 0.1081560030579567 -0.30000001192092896
		 -0.33286994695663452 0 -0.30000001192092896 -0.35000017285346985 -0.1081560030579567
		 -0.30000001192092896 -0.33286991715431213 -0.20572490990161896 -0.30000001192092896
		 -0.28315603733062744 -0.28315603733062744 -0.30000001192092896 -0.20572489500045776 -0.33286985754966736
		 -0.30000001192092896 -0.10815597325563431 -0.35000008344650269 -0.30000001192092896
		 0 -0.33286985754966736 -0.30000001192092896 0.10815597325563431 -0.28315600752830505
		 -0.30000001192092896 0.20572488009929657 -0.20572488009929657 -0.30000001192092896
		 0.28315597772598267 -0.10815597325563431 -0.30000001192092896 0.33286982774734497 -1.0430812658057675e-008
		 -0.30000001192092896 0.35000002384185791 0.10815593600273132 -0.30000001192092896
		 0.33286979794502258 0.20572483539581299 -0.30000001192092896 0.28315597772598267 0.28315594792366028
		 -0.30000001192092896 0.20572485029697418 0.33286979794502258 -0.30000001192092896
		 0.10815595090389252 0.34999999403953552 -0.30000001192092896 0 0.3328700065612793
		 0.30000001192092896 -0.10815601050853729 0.2831561267375946 0.30000001192092896 -0.20572495460510254 0.20572495460510254
		 0.30000001192092896 -0.2831561267375946 0.1081560030579567 0.30000001192092896 -0.33286994695663452 0
		 0.30000001192092896 -0.35000017285346985 -0.1081560030579567 0.30000001192092896
		 -0.33286991715431213 -0.20572490990161896 0.30000001192092896 -0.28315603733062744 -0.28315603733062744
		 0.30000001192092896 -0.20572489500045776 -0.33286985754966736 0.30000001192092896
		 -0.10815597325563431 -0.35000008344650269 0.30000001192092896 0 -0.33286985754966736
		 0.30000001192092896 0.10815597325563431 -0.28315600752830505 0.30000001192092896
		 0.20572488009929657 -0.20572488009929657 0.30000001192092896 0.28315597772598267 -0.10815597325563431
		 0.30000001192092896 0.33286982774734497 -1.0430812658057675e-008 0.30000001192092896
		 0.35000002384185791 0.10815593600273132 0.30000001192092896 0.33286979794502258 0.20572483539581299
		 0.30000001192092896 0.28315597772598267 0.28315594792366028 0.30000001192092896 0.20572485029697418 0.33286979794502258
		 0.30000001192092896 0.10815595090389252 0.34999999403953552 0.30000001192092896 0 0
		 -0.30000001192092896 0 0 0.30000001192092896 0 ;
	setAttr ".rgf" -type "string" "[[0,1,21,20],[1,2,22,21],[2,3,23,22],[3,4,24,23],[4,5,25,24],[5,6,26,25],[6,7,27,26],[7,8,28,27],[8,9,29,28],[9,10,30,29],[10,11,31,30],[11,12,32,31],[12,13,33,32],[13,14,34,33],[14,15,35,34],[15,16,36,35],[16,17,37,36],[17,18,38,37],[18,19,39,38],[19,0,20,39],[1,0,40],[2,1,40],[3,2,40],[4,3,40],[5,4,40],[6,5,40],[7,6,40],[8,7,40],[9,8,40],[10,9,40],[11,10,40],[12,11,40],[13,12,40],[14,13,40],[15,14,40],[16,15,40],[17,16,40],[18,17,40],[19,18,40],[0,19,40],[20,21,41],[21,22,41],[22,23,41],[23,24,41],[24,25,41],[25,26,41],[26,27,41],[27,28,41],[28,29,41],[29,30,41],[30,31,41],[31,32,41],[32,33,41],[33,34,41],[34,35,41],[35,36,41],[36,37,41],[37,38,41],[38,39,41],[39,20,41]]";
createNode transform -n "phys_front_wheel" -p "m_front_wheel";
	setAttr ".t" -type "double3" 0 0 -1.0004230150961746e-006 ;
	setAttr ".r" -type "double3" 0 0 89.999999999999986 ;
createNode mesh -n "phys_front_wheelShape" -p "phys_front_wheel";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "phys_hud" -p "m_blob1";
	setAttr ".t" -type "double3" -0.00027443634900869984 0.3122538872380401 0.22418521344331011 ;
	setAttr ".s" -type "double3" 0.82894706693732045 0.74041659893382283 1 ;
	setAttr ".rp" -type "double3" 0.00027443634900869984 -0.31223125701770571 -0.22413662809570778 ;
	setAttr ".sp" -type "double3" 0.00033106619222702551 -0.42169672785200807 -0.22413662809570778 ;
	setAttr ".spt" -type "double3" -5.6629843218325645e-005 0.1094654708343024 0 ;
createNode mesh -n "phys_hudShape" -p "phys_hud";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "m_hud" -p "m_blob1";
	setAttr ".t" -type "double3" 0 0.31480046915991539 0.36370178271966547 ;
	setAttr ".s" -type "double3" 1 0.74041659893382283 0.78821673537888148 ;
	setAttr ".rp" -type "double3" 0 -0.31477783893958095 -0.3636531973720632 ;
	setAttr ".sp" -type "double3" 0 -0.42513611849444122 -0.46136193390674674 ;
	setAttr ".spt" -type "double3" 0 0.11035827955486027 0.097708736534683496 ;
createNode mesh -n "m_hudShape" -p "m_hud";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 4 ".pt[0:3]" -type "float3"  0.11437031 0.068279698 -0.022687212 
		-0.11437031 0.068279698 -0.022687212 0.11437031 -0.052321251 0.02268721 -0.11437031 
		-0.052321251 0.02268721;
	setAttr ".rgvtx" -type "vectorArray" 8 -0.21532189846038818 -0.098667517304420471
		 0.93835341930389404 0.21532189846038818 -0.098667517304420471 0.93835341930389404 -0.21532189846038818
		 0.1146259605884552 0.98372781276702881 0.21532189846038818 0.1146259605884552 0.98372781276702881 -0.32969221472740173
		 0.16694721579551697 -0.96104061603546143 0.32969221472740173 0.16694721579551697
		 -0.96104061603546143 -0.32969221472740173 -0.16694721579551697 -0.96104061603546143 0.32969221472740173
		 -0.16694721579551697 -0.96104061603546143 ;
	setAttr ".rgf" -type "string" "[[0,1,3,2],[2,3,5,4],[4,5,7,6],[6,7,1,0],[1,7,5,3],[6,0,2,4]]";
createNode lightLinker -n "lightLinker1";
	setAttr -s 5 ".lnk";
	setAttr -s 7 ".slnk";
createNode displayLayerManager -n "layerManager";
createNode displayLayer -n "defaultLayer";
createNode renderLayerManager -n "renderLayerManager";
createNode renderLayer -n "defaultRenderLayer";
	setAttr ".g" yes;
createNode polyCylinder -n "polyCylinder1";
	setAttr ".r" 0.5;
	setAttr ".h" 0.4;
	setAttr ".sc" 1;
	setAttr ".cuv" 3;
createNode polyCylinder -n "pasted__polyCylinder1";
	setAttr ".r" 0.5;
	setAttr ".h" 0.4;
	setAttr ".sc" 1;
	setAttr ".cuv" 3;
createNode polyCylinder -n "polyCylinder2";
	setAttr ".r" 0.35;
	setAttr ".h" 0.6;
	setAttr ".sc" 1;
	setAttr ".cuv" 3;
createNode polyCube -n "polyCube1";
	setAttr ".w" 0.65938440787443842;
	setAttr ".h" 0.33389442315863954;
	setAttr ".d" 1.9220812656785951;
	setAttr ".cuv" 4;
createNode polyCube -n "polyCube2";
	setAttr ".w" 0.69848493590832916;
	setAttr ".h" 0.49108934464820625;
	setAttr ".d" 0.65617491954110729;
	setAttr ".cuv" 4;
createNode polyCube -n "polyCube3";
	setAttr ".w" 0.73064683485190862;
	setAttr ".h" 0.42710604245502337;
	setAttr ".d" 0.6598393257826114;
	setAttr ".cuv" 4;
createNode polyExtrudeFace -n "polyExtrudeFace1";
	setAttr ".ics" -type "componentList" 1 "f[1]";
	setAttr ".ix" -type "matrix" 1.3548575074563876 0 0 0 0 1 0 0 0 0 1.6058234812216212 0
		 0 0.99871967612049928 0.0087123666937675981 1;
	setAttr ".ws" yes;
	setAttr ".pvt" -type "float3" 0 1.2122726 0.0087123672 ;
	setAttr ".rs" 42963;
	setAttr ".c[0]"  0 1 1;
createNode polyPipe -n "polyPipe1";
	setAttr ".r" 0.45;
	setAttr ".h" 0.46180903669909729;
	setAttr ".t" 0.05;
createNode deleteComponent -n "deleteComponent1";
	setAttr ".dc" -type "componentList" 0;
createNode deleteComponent -n "deleteComponent2";
	setAttr ".dc" -type "componentList" 0;
createNode deleteComponent -n "deleteComponent3";
	setAttr ".dc" -type "componentList" 5 "f[0:4]" "f[15:24]" "f[35:44]" "f[55:64]" "f[75:79]";
createNode polyCube -n "polyCube4";
	setAttr ".w" 0.7;
	setAttr ".h" 0.07;
	setAttr ".d" 0.03;
	setAttr ".cuv" 4;
createNode polyCube -n "pasted__polyCube4";
	setAttr ".w" 0.7;
	setAttr ".h" 0.07;
	setAttr ".d" 0.03;
	setAttr ".cuv" 4;
createNode polyCube -n "polyCube5";
	setAttr ".w" 0.03;
	setAttr ".h" 0.07;
	setAttr ".d" 0.85;
	setAttr ".cuv" 4;
createNode polyCube -n "pasted__polyCube5";
	setAttr ".w" 0.03;
	setAttr ".h" 0.07;
	setAttr ".d" 0.85;
	setAttr ".cuv" 4;
createNode lambert -n "lambert2";
	setAttr ".c" -type "float3" 0.085999995 0.39739704 0.5 ;
createNode shadingEngine -n "lambert2SG";
	setAttr ".ihi" 0;
	setAttr -s 2 ".dsm";
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo1";
createNode polyCube -n "polyCube6";
	setAttr ".w" 0.27825129779506685;
	setAttr ".h" 0.51190790528382135;
	setAttr ".d" 1.0175238413738543;
	setAttr ".cuv" 4;
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
		+ "                -displayLights \"default\" \n                -displayAppearance \"smoothShaded\" \n                -activeOnly 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n                -activeComponentsXray 0\n                -displayTextures 0\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n                -textureMaxSize 8192\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -maxConstantTransparency 1\n"
		+ "                -rendererName \"base_OpenGL_Renderer\" \n                -colorResolution 256 256 \n                -bumpResolution 512 512 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n                -lowQualityLighting 0\n                -maximumNumHardwareLights 1\n                -occlusionCulling 0\n                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n"
		+ "                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -dimensions 1\n                -handles 1\n                -pivots 1\n                -textures 1\n                -strokes 1\n                -shadows 0\n                $editorName;\nmodelEditor -e -viewSelected 0 $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tmodelPanel -edit -l (localizedPanelLabel(\"Front View\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        modelEditor -e \n            -camera \"front\" \n            -useInteractiveMode 0\n            -displayLights \"default\" \n            -displayAppearance \"smoothShaded\" \n            -activeOnly 0\n            -wireframeOnShaded 0\n"
		+ "            -headsUpDisplay 1\n            -selectionHiliteDisplay 1\n            -useDefaultMaterial 0\n            -bufferMode \"double\" \n            -twoSidedLighting 1\n            -backfaceCulling 0\n            -xray 0\n            -jointXray 0\n            -activeComponentsXray 0\n            -displayTextures 0\n            -smoothWireframe 0\n            -lineWidth 1\n            -textureAnisotropic 0\n            -textureHilight 1\n            -textureSampling 2\n            -textureDisplay \"modulate\" \n            -textureMaxSize 8192\n            -fogging 0\n            -fogSource \"fragment\" \n            -fogMode \"linear\" \n            -fogStart 0\n            -fogEnd 100\n            -fogDensity 0.1\n            -fogColor 0.5 0.5 0.5 1 \n            -maxConstantTransparency 1\n            -rendererName \"base_OpenGL_Renderer\" \n            -colorResolution 256 256 \n            -bumpResolution 512 512 \n            -textureCompression 0\n            -transparencyAlgorithm \"frontAndBackCull\" \n            -transpInShadows 0\n            -cullingOverride \"none\" \n"
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
		+ "            hyperGraph -e \n                -graphLayoutStyle \"hierarchicalLayout\" \n                -orientation \"horiz\" \n                -mergeConnections 1\n                -zoom 0.507775\n                -animateTransition 0\n                -showRelationships 1\n                -showShapes 0\n                -showDeformers 0\n                -showExpressions 0\n                -showConstraints 0\n                -showUnderworld 0\n                -showInvisible 0\n                -transitionFrames 5\n                -currentNode \"m_blob1\" \n                -opaqueContainers 0\n                -dropTargetNode \"m_blob1\" \n                -dropNode \"m_hud\" \n                -freeform 1\n                -imagePosition 0 0 \n                -imageScale 1\n                -imageEnabled 0\n                -graphType \"DAG\" \n                -heatMapDisplay 0\n                -updateSelection 1\n                -updateNodeAdded 1\n                -useDrawOverrideColor 0\n                -limitGraphTraversal -1\n                -range 0 0 \n                -iconSize \"largeIcons\" \n"
		+ "                -showCachedConnections 0\n                $editorName;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Hypergraph Hierarchy\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = ($panelName+\"HyperGraphEd\");\n            hyperGraph -e \n                -graphLayoutStyle \"hierarchicalLayout\" \n                -orientation \"horiz\" \n                -mergeConnections 1\n                -zoom 0.507775\n                -animateTransition 0\n                -showRelationships 1\n                -showShapes 0\n                -showDeformers 0\n                -showExpressions 0\n                -showConstraints 0\n                -showUnderworld 0\n                -showInvisible 0\n                -transitionFrames 5\n                -currentNode \"m_blob1\" \n                -opaqueContainers 0\n                -dropTargetNode \"m_blob1\" \n                -dropNode \"m_hud\" \n                -freeform 1\n                -imagePosition 0 0 \n                -imageScale 1\n"
		+ "                -imageEnabled 0\n                -graphType \"DAG\" \n                -heatMapDisplay 0\n                -updateSelection 1\n                -updateNodeAdded 1\n                -useDrawOverrideColor 0\n                -limitGraphTraversal -1\n                -range 0 0 \n                -iconSize \"largeIcons\" \n                -showCachedConnections 0\n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"hyperShadePanel\" (localizedPanelLabel(\"Hypershade\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"hyperShadePanel\" -l (localizedPanelLabel(\"Hypershade\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Hypershade\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"visorPanel\" (localizedPanelLabel(\"Visor\")) `;\n"
		+ "\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"visorPanel\" -l (localizedPanelLabel(\"Visor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Visor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"polyTexturePlacementPanel\" (localizedPanelLabel(\"UV Texture Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"polyTexturePlacementPanel\" -l (localizedPanelLabel(\"UV Texture Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"UV Texture Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"multiListerPanel\" (localizedPanelLabel(\"Multilister\")) `;\n"
		+ "\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"multiListerPanel\" -l (localizedPanelLabel(\"Multilister\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Multilister\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"renderWindowPanel\" (localizedPanelLabel(\"Render View\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"renderWindowPanel\" -l (localizedPanelLabel(\"Render View\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Render View\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"blendShapePanel\" (localizedPanelLabel(\"Blend Shape\")) `;\n"
		+ "\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\tblendShapePanel -unParent -l (localizedPanelLabel(\"Blend Shape\")) -mbv $menusOkayInPanels ;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tblendShapePanel -edit -l (localizedPanelLabel(\"Blend Shape\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"dynRelEdPanel\" (localizedPanelLabel(\"Dynamic Relationships\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"dynRelEdPanel\" -l (localizedPanelLabel(\"Dynamic Relationships\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Dynamic Relationships\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"devicePanel\" (localizedPanelLabel(\"Devices\")) `;\n\tif (\"\" == $panelName) {\n"
		+ "\t\tif ($useSceneConfig) {\n\t\t\tdevicePanel -unParent -l (localizedPanelLabel(\"Devices\")) -mbv $menusOkayInPanels ;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tdevicePanel -edit -l (localizedPanelLabel(\"Devices\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"relationshipPanel\" (localizedPanelLabel(\"Relationship Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"relationshipPanel\" -l (localizedPanelLabel(\"Relationship Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Relationship Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"referenceEditorPanel\" (localizedPanelLabel(\"Reference Editor\")) `;\n\tif (\"\" == $panelName) {\n"
		+ "\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"referenceEditorPanel\" -l (localizedPanelLabel(\"Reference Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Reference Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"componentEditorPanel\" (localizedPanelLabel(\"Component Editor\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"componentEditorPanel\" -l (localizedPanelLabel(\"Component Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Component Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"dynPaintScriptedPanelType\" (localizedPanelLabel(\"Paint Effects\")) `;\n"
		+ "\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"dynPaintScriptedPanelType\" -l (localizedPanelLabel(\"Paint Effects\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Paint Effects\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"webBrowserPanel\" (localizedPanelLabel(\"Web Browser\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"webBrowserPanel\" -l (localizedPanelLabel(\"Web Browser\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Web Browser\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"scriptEditorPanel\" (localizedPanelLabel(\"Script Editor\")) `;\n"
		+ "\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"scriptEditorPanel\" -l (localizedPanelLabel(\"Script Editor\")) -mbv $menusOkayInPanels `;\n\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Script Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\tif ($useSceneConfig) {\n        string $configName = `getPanel -cwl (localizedPanelLabel(\"Current Layout\"))`;\n        if (\"\" != $configName) {\n\t\t\tpanelConfiguration -edit -label (localizedPanelLabel(\"Current Layout\")) \n\t\t\t\t-defaultImage \"\"\n\t\t\t\t-image \"\"\n\t\t\t\t-sc false\n\t\t\t\t-configString \"global string $gMainPane; paneLayout -e -cn \\\"horizontal2\\\" -ps 1 100 46 -ps 2 100 54 $gMainPane;\"\n\t\t\t\t-removeAllPanels\n\t\t\t\t-ap false\n\t\t\t\t\t(localizedPanelLabel(\"Persp View\")) \n\t\t\t\t\t\"modelPanel\"\n"
		+ "\t\t\t\t\t\"$panelName = `modelPanel -unParent -l (localizedPanelLabel(\\\"Persp View\\\")) -mbv $menusOkayInPanels `;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera persp` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"smoothShaded\\\" \\n    -activeOnly 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 0\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName\"\n"
		+ "\t\t\t\t\t\"modelPanel -edit -l (localizedPanelLabel(\\\"Persp View\\\")) -mbv $menusOkayInPanels  $panelName;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera persp` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"smoothShaded\\\" \\n    -activeOnly 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 0\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName\"\n"
		+ "\t\t\t\t-ap false\n\t\t\t\t\t(localizedPanelLabel(\"Hypergraph Hierarchy\")) \n\t\t\t\t\t\"scriptedPanel\"\n\t\t\t\t\t\"$panelName = `scriptedPanel -unParent  -type \\\"hyperGraphPanel\\\" -l (localizedPanelLabel(\\\"Hypergraph Hierarchy\\\")) -mbv $menusOkayInPanels `;\\n\\n\\t\\t\\t$editorName = ($panelName+\\\"HyperGraphEd\\\");\\n            hyperGraph -e \\n                -graphLayoutStyle \\\"hierarchicalLayout\\\" \\n                -orientation \\\"horiz\\\" \\n                -mergeConnections 1\\n                -zoom 0.507775\\n                -animateTransition 0\\n                -showRelationships 1\\n                -showShapes 0\\n                -showDeformers 0\\n                -showExpressions 0\\n                -showConstraints 0\\n                -showUnderworld 0\\n                -showInvisible 0\\n                -transitionFrames 5\\n                -currentNode \\\"m_blob1\\\" \\n                -opaqueContainers 0\\n                -dropTargetNode \\\"m_blob1\\\" \\n                -dropNode \\\"m_hud\\\" \\n                -freeform 1\\n                -imagePosition 0 0 \\n                -imageScale 1\\n                -imageEnabled 0\\n                -graphType \\\"DAG\\\" \\n                -heatMapDisplay 0\\n                -updateSelection 1\\n                -updateNodeAdded 1\\n                -useDrawOverrideColor 0\\n                -limitGraphTraversal -1\\n                -range 0 0 \\n                -iconSize \\\"largeIcons\\\" \\n                -showCachedConnections 0\\n                $editorName\"\n"
		+ "\t\t\t\t\t\"scriptedPanel -edit -l (localizedPanelLabel(\\\"Hypergraph Hierarchy\\\")) -mbv $menusOkayInPanels  $panelName;\\n\\n\\t\\t\\t$editorName = ($panelName+\\\"HyperGraphEd\\\");\\n            hyperGraph -e \\n                -graphLayoutStyle \\\"hierarchicalLayout\\\" \\n                -orientation \\\"horiz\\\" \\n                -mergeConnections 1\\n                -zoom 0.507775\\n                -animateTransition 0\\n                -showRelationships 1\\n                -showShapes 0\\n                -showDeformers 0\\n                -showExpressions 0\\n                -showConstraints 0\\n                -showUnderworld 0\\n                -showInvisible 0\\n                -transitionFrames 5\\n                -currentNode \\\"m_blob1\\\" \\n                -opaqueContainers 0\\n                -dropTargetNode \\\"m_blob1\\\" \\n                -dropNode \\\"m_hud\\\" \\n                -freeform 1\\n                -imagePosition 0 0 \\n                -imageScale 1\\n                -imageEnabled 0\\n                -graphType \\\"DAG\\\" \\n                -heatMapDisplay 0\\n                -updateSelection 1\\n                -updateNodeAdded 1\\n                -useDrawOverrideColor 0\\n                -limitGraphTraversal -1\\n                -range 0 0 \\n                -iconSize \\\"largeIcons\\\" \\n                -showCachedConnections 0\\n                $editorName\"\n"
		+ "\t\t\t\t$configName;\n\n            setNamedPanelLayout (localizedPanelLabel(\"Current Layout\"));\n        }\n\n        panelHistory -e -clear mainPanelHistory;\n        setFocus `paneLayout -q -p1 $gMainPane`;\n        sceneUIReplacement -deleteRemaining;\n        sceneUIReplacement -clear;\n\t}\n\n\ngrid -spacing 5 -size 12 -divisions 5 -displayAxes yes -displayGridLines yes -displayDivisionLines yes -displayPerspectiveLabels no -displayOrthographicLabels no -displayAxesBold yes -perspectiveLabelPosition axis -orthographicLabelPosition edge;\nviewManip -drawCompass 0 -compassAngle 0 -frontParameters \"\" -homeParameters \"\" -selectionLockParameters \"\";\n}\n");
	setAttr ".st" 3;
createNode script -n "sceneConfigurationScriptNode";
	setAttr ".b" -type "string" "playbackOptions -min 1 -max 24 -ast 1 -aet 48 ";
	setAttr ".st" 6;
createNode polyCube -n "polyCube7";
	setAttr ".w" 0.72458980249834926;
	setAttr ".h" 0.92563662594794749;
	setAttr ".d" 0.54899392559308158;
	setAttr ".cuv" 4;
createNode lambert -n "lambert3";
	setAttr ".it" -type "float3" 0.28926 0.28926 0.28926 ;
createNode shadingEngine -n "lambert3SG";
	setAttr ".ihi" 0;
	setAttr -s 6 ".dsm";
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo2";
createNode polyCube -n "polyCube8";
	setAttr ".w" 0.59380885725433386;
	setAttr ".h" 0.24197850627449213;
	setAttr ".d" 1.8574946059331379;
	setAttr ".cuv" 4;
createNode polyCube -n "polyCube9";
	setAttr ".w" 0.89010369952067947;
	setAttr ".h" 1.3027444857465154;
	setAttr ".d" 1.116163536582305;
	setAttr ".cuv" 4;
createNode polySphere -n "polySphere1";
	setAttr ".r" 0.35;
createNode polySphere -n "polySphere2";
	setAttr ".r" 0.5;
createNode polySphere -n "pasted__polySphere2";
	setAttr ".r" 0.5;
createNode materialInfo -n "pasted__materialInfo2";
createNode shadingEngine -n "pasted__lambert3SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode lambert -n "pasted__lambert3";
	setAttr ".it" -type "float3" 0.28926 0.28926 0.28926 ;
createNode polyCube -n "polyCube10";
	setAttr ".w" 0.7;
	setAttr ".h" 0.1;
	setAttr ".d" 0.05;
	setAttr ".cuv" 4;
createNode script -n "rg_export";
	addAttr -ci true -sn "time" -ln "time" -dt "string";
	setAttr ".time" -type "string" "2009-11-15T19:58:01.109000";
select -ne :time1;
	setAttr ".o" 1;
select -ne :renderPartition;
	setAttr -s 5 ".st";
select -ne :renderGlobalsList1;
select -ne :defaultShaderList1;
	setAttr -s 5 ".s";
select -ne :postProcessList1;
	setAttr -s 2 ".p";
select -ne :lightList1;
select -ne :lambert1;
	setAttr ".c" -type "float3" 0.97399998 0.71072739 0.060387976 ;
select -ne :initialShadingGroup;
	setAttr -s 10 ".dsm";
	setAttr ".ro" yes;
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
	setAttr -s 19 ".hyp";
	setAttr ".hyp[0].x" 70.747512817382812;
	setAttr ".hyp[0].y" 205.29191589355469;
	setAttr ".hyp[0].isf" yes;
	setAttr ".hyp[1].x" 102;
	setAttr ".hyp[1].y" 153.96937561035156;
	setAttr ".hyp[1].isf" yes;
	setAttr ".hyp[2].x" 107.84417724609375;
	setAttr ".hyp[2].y" 92;
	setAttr ".hyp[2].isf" yes;
	setAttr ".hyp[3].x" 111.24391937255859;
	setAttr ".hyp[3].y" 37.959880828857422;
	setAttr ".hyp[3].isf" yes;
	setAttr ".hyp[4].x" 107;
	setAttr ".hyp[4].y" -373.32632446289062;
	setAttr ".hyp[4].isf" yes;
	setAttr ".hyp[5].x" 105.03062438964844;
	setAttr ".hyp[5].y" -321;
	setAttr ".hyp[5].isf" yes;
	setAttr ".hyp[6].x" 138.61245727539062;
	setAttr ".hyp[6].y" -261.26397705078125;
	setAttr ".hyp[6].isf" yes;
	setAttr ".hyp[7].x" 136.97021484375;
	setAttr ".hyp[7].y" -65;
	setAttr ".hyp[7].isf" yes;
	setAttr ".hyp[8].x" 135.97021484375;
	setAttr ".hyp[8].y" -114;
	setAttr ".hyp[8].isf" yes;
	setAttr ".hyp[9].x" 138.97021484375;
	setAttr ".hyp[9].y" -164;
	setAttr ".hyp[9].isf" yes;
	setAttr ".hyp[10].x" 139.97021484375;
	setAttr ".hyp[10].y" -212;
	setAttr ".hyp[10].isf" yes;
	setAttr ".hyp[11].x" 106.15311431884766;
	setAttr ".hyp[11].y" -16;
	setAttr ".hyp[11].isf" yes;
	setAttr ".hyp[12].x" 358.74737548828125;
	setAttr ".hyp[12].y" 191.54736328125;
	setAttr ".hyp[12].isf" yes;
	setAttr ".hyp[13].x" 364.54736328125;
	setAttr ".hyp[13].y" 141.45263671875;
	setAttr ".hyp[13].isf" yes;
	setAttr ".hyp[14].x" 365.03158569335938;
	setAttr ".hyp[14].y" 92.210533142089844;
	setAttr ".hyp[14].isf" yes;
	setAttr ".hyp[15].x" 374.90814208984375;
	setAttr ".hyp[15].y" -279;
	setAttr ".hyp[15].isf" yes;
	setAttr ".hyp[16].x" 373;
	setAttr ".hyp[16].y" -340;
	setAttr ".hyp[16].isf" yes;
	setAttr ".hyp[17].x" 376.273681640625;
	setAttr ".hyp[17].y" -386;
	setAttr ".hyp[17].isf" yes;
	setAttr ".hyp[18].x" 367;
	setAttr ".hyp[18].y" -33.030624389648438;
	setAttr ".hyp[18].isf" yes;
connectAttr "polyCube2.out" "m_blob1Shape.i";
connectAttr "polyCube6.out" "m_blobShape2.i";
connectAttr "polyExtrudeFace1.out" "m_hutShape.i";
connectAttr "pasted__polyCylinder1.out" "m_rear_wheelShape0.i";
connectAttr "polySphere2.out" "phys_rear_wheelShape0.i";
connectAttr "polyCylinder1.out" "m_rear_wheelShape1.i";
connectAttr "pasted__polySphere2.out" "phys_rear_wheelShape1.i";
connectAttr "polyCube7.out" "phys_blobShape1.i";
connectAttr "polyCube9.out" "phys_hutShape.i";
connectAttr "deleteComponent3.og" "m_fenderShape.i";
connectAttr "polyCube4.out" "m_fenderShape1.i";
connectAttr "pasted__polyCube4.out" "m_fenderShape2.i";
connectAttr "polyCube5.out" "m_fenderShape3.i";
connectAttr "pasted__polyCube5.out" "m_fenderShape4.i";
connectAttr "polyCube10.out" "phys_fenderShape.i";
connectAttr "polyCylinder2.out" "m_front_wheelShape.i";
connectAttr "polySphere1.out" "phys_front_wheelShape.i";
connectAttr "polyCube8.out" "phys_hudShape.i";
connectAttr "polyCube1.out" "m_hudShape.i";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[0].llnk";
connectAttr ":initialShadingGroup.msg" "lightLinker1.lnk[0].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[1].llnk";
connectAttr ":initialParticleSE.msg" "lightLinker1.lnk[1].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[2].llnk";
connectAttr "lambert2SG.msg" "lightLinker1.lnk[2].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[3].llnk";
connectAttr "lambert3SG.msg" "lightLinker1.lnk[3].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[4].llnk";
connectAttr "pasted__lambert3SG.msg" "lightLinker1.lnk[4].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[0].sllk";
connectAttr ":initialShadingGroup.msg" "lightLinker1.slnk[0].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[1].sllk";
connectAttr ":initialParticleSE.msg" "lightLinker1.slnk[1].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[2].sllk";
connectAttr "lambert2SG.msg" "lightLinker1.slnk[2].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[3].sllk";
connectAttr "lambert3SG.msg" "lightLinker1.slnk[3].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[6].sllk";
connectAttr "pasted__lambert3SG.msg" "lightLinker1.slnk[6].solk";
connectAttr "layerManager.dli[0]" "defaultLayer.id";
connectAttr "renderLayerManager.rlmi[0]" "defaultRenderLayer.rlid";
connectAttr "polyCube3.out" "polyExtrudeFace1.ip";
connectAttr "m_hutShape.wm" "polyExtrudeFace1.mp";
connectAttr "polyPipe1.out" "deleteComponent1.ig";
connectAttr "deleteComponent1.og" "deleteComponent2.ig";
connectAttr "deleteComponent2.og" "deleteComponent3.ig";
connectAttr "lambert2.oc" "lambert2SG.ss";
connectAttr "m_rear_wheelShape0.iog" "lambert2SG.dsm" -na;
connectAttr "m_rear_wheelShape1.iog" "lambert2SG.dsm" -na;
connectAttr "lambert2SG.msg" "materialInfo1.sg";
connectAttr "lambert2.msg" "materialInfo1.m";
connectAttr "lambert3.oc" "lambert3SG.ss";
connectAttr "phys_blobShape1.iog" "lambert3SG.dsm" -na;
connectAttr "phys_hudShape.iog" "lambert3SG.dsm" -na;
connectAttr "phys_hutShape.iog" "lambert3SG.dsm" -na;
connectAttr "phys_front_wheelShape.iog" "lambert3SG.dsm" -na;
connectAttr "phys_rear_wheelShape0.iog" "lambert3SG.dsm" -na;
connectAttr "phys_fenderShape.iog" "lambert3SG.dsm" -na;
connectAttr "lambert3SG.msg" "materialInfo2.sg";
connectAttr "lambert3.msg" "materialInfo2.m";
connectAttr "pasted__lambert3SG.msg" "pasted__materialInfo2.sg";
connectAttr "pasted__lambert3.msg" "pasted__materialInfo2.m";
connectAttr "pasted__lambert3.oc" "pasted__lambert3SG.ss";
connectAttr "phys_rear_wheelShape1.iog" "pasted__lambert3SG.dsm" -na;
connectAttr "lambert2SG.pa" ":renderPartition.st" -na;
connectAttr "lambert3SG.pa" ":renderPartition.st" -na;
connectAttr "pasted__lambert3SG.pa" ":renderPartition.st" -na;
connectAttr "lambert2.msg" ":defaultShaderList1.s" -na;
connectAttr "lambert3.msg" ":defaultShaderList1.s" -na;
connectAttr "pasted__lambert3.msg" ":defaultShaderList1.s" -na;
connectAttr "lightLinker1.msg" ":lightList1.ln" -na;
connectAttr "m_front_wheelShape.iog" ":initialShadingGroup.dsm" -na;
connectAttr "m_hudShape.iog" ":initialShadingGroup.dsm" -na;
connectAttr "m_blob1Shape.iog" ":initialShadingGroup.dsm" -na;
connectAttr "m_hutShape.iog" ":initialShadingGroup.dsm" -na;
connectAttr "m_fenderShape.iog" ":initialShadingGroup.dsm" -na;
connectAttr "m_fenderShape1.iog" ":initialShadingGroup.dsm" -na;
connectAttr "m_fenderShape2.iog" ":initialShadingGroup.dsm" -na;
connectAttr "m_fenderShape3.iog" ":initialShadingGroup.dsm" -na;
connectAttr "m_fenderShape4.iog" ":initialShadingGroup.dsm" -na;
connectAttr "m_blobShape2.iog" ":initialShadingGroup.dsm" -na;
connectAttr "m_blob1.msg" ":hyperGraphLayout.hyp[0].dn";
connectAttr "m_blob2.msg" ":hyperGraphLayout.hyp[1].dn";
connectAttr "m_hut.msg" ":hyperGraphLayout.hyp[2].dn";
connectAttr "m_hud.msg" ":hyperGraphLayout.hyp[3].dn";
connectAttr "m_rear_wheel1.msg" ":hyperGraphLayout.hyp[4].dn";
connectAttr "m_rear_wheel0.msg" ":hyperGraphLayout.hyp[5].dn";
connectAttr "m_front_wheel.msg" ":hyperGraphLayout.hyp[6].dn";
connectAttr "m_fender1.msg" ":hyperGraphLayout.hyp[7].dn";
connectAttr "m_fender2.msg" ":hyperGraphLayout.hyp[8].dn";
connectAttr "m_fender3.msg" ":hyperGraphLayout.hyp[9].dn";
connectAttr "m_fender4.msg" ":hyperGraphLayout.hyp[10].dn";
connectAttr "m_fender.msg" ":hyperGraphLayout.hyp[11].dn";
connectAttr "phys_blob1.msg" ":hyperGraphLayout.hyp[12].dn";
connectAttr "phys_hud.msg" ":hyperGraphLayout.hyp[13].dn";
connectAttr "phys_hut.msg" ":hyperGraphLayout.hyp[14].dn";
connectAttr "phys_front_wheel.msg" ":hyperGraphLayout.hyp[15].dn";
connectAttr "phys_rear_wheel0.msg" ":hyperGraphLayout.hyp[16].dn";
connectAttr "phys_rear_wheel1.msg" ":hyperGraphLayout.hyp[17].dn";
connectAttr "phys_fender.msg" ":hyperGraphLayout.hyp[18].dn";
// End of road_roller_01.ma
