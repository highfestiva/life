//Maya ASCII 2009 scene
//Name: level_05.ma
//Last modified: Tue, Jun 25, 2013 12:09:33 AM
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
	setAttr ".t" -type "double3" 31.996131078065229 77.143649275920652 373.09005263663732 ;
	setAttr ".r" -type "double3" -9.3383527295139892 3.7999999999985894 0 ;
	setAttr ".rp" -type "double3" 3.5527136788005009e-015 4.4408920985006262e-016 0 ;
	setAttr ".rpt" -type "double3" 6.8982178454696156e-014 -1.3666564467236926e-016 
		1.1096891300127576e-013 ;
createNode camera -s -n "perspShape" -p "persp";
	setAttr -k off ".v" no;
	setAttr ".fl" 34.999999999999979;
	setAttr ".coi" 421.27665565384916;
	setAttr ".imn" -type "string" "persp";
	setAttr ".den" -type "string" "persp_depth";
	setAttr ".man" -type "string" "persp_mask";
	setAttr ".tp" -type "double3" -28.924751388582884 3.5235580852338444 0.49129717586774291 ;
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
	setAttr ".t" -type "double3" 0 0 100.1 ;
createNode camera -s -n "frontShape" -p "front";
	setAttr -k off ".v" no;
	setAttr ".rnd" no;
	setAttr ".coi" 100.1;
	setAttr ".ow" 30;
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
createNode transform -n "m_terrain";
createNode mesh -n "m_terrainShape" -p "m_terrain";
	addAttr -ci true -sn "mso" -ln "miShadingSamplesOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "msh" -ln "miShadingSamples" -min 0 -smx 8 -at "float";
	addAttr -ci true -sn "mdo" -ln "miMaxDisplaceOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "mmd" -ln "miMaxDisplace" -min 0 -smx 1 -at "float";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	addAttr -ci true -sn "rgn" -ln "rgn" -dt "vectorArray";
	setAttr -k off ".v";
	setAttr -s 2 ".iog";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".rgvtx" -type "vectorArray" 8 -5000 -1.5 5000 5000 -1.5 5000 -5000
		 1.5 5000 5000 1.5 5000 -5000 1.5 -5000 5000 1.5 -5000 -5000 -1.5 -5000 5000 -1.5
		 -5000 ;
	setAttr ".rgf" -type "string" "[[0,1,3,2],[2,3,5,4],[4,5,7,6],[6,7,1,0],[1,7,5,3],[6,0,2,4]]";
	setAttr ".rgn" -type "vectorArray" 24 0 0 1 0 0 1 0 0 1 0 0 1 0 1 0 0 1 0 0 1
		 0 0 1 0 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 -1 0 0 -1 0 0 -1 0 0 -1 0 1 0 0 1 0 0 1 0 0 1
		 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 0 ;
createNode transform -n "phys_terrain" -p "m_terrain";
createNode transform -n "m_landingpad" -p "m_terrain";
	setAttr ".t" -type "double3" 30 -1.5 -1.9984014443252818e-015 ;
	setAttr ".s" -type "double3" 2 2 2 ;
	setAttr ".rp" -type "double3" -30 1.5 1.9984014443252818e-015 ;
	setAttr ".sp" -type "double3" -15 0.75 9.9920072216264089e-016 ;
	setAttr ".spt" -type "double3" -15 0.75 9.9920072216264089e-016 ;
createNode transform -n "phys_landingpad" -p "m_landingpad";
	setAttr ".t" -type "double3" 0 1.7763568394002505e-015 6.6613381477509392e-016 ;
	setAttr ".rp" -type "double3" -15 0.74999999999999822 3.3306690738754696e-016 ;
	setAttr ".sp" -type "double3" -15 0.74999999999999822 3.3306690738754696e-016 ;
createNode mesh -n "m_landingpadShape" -p "phys_landingpad";
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
createNode transform -n "phys_trig_landing" -p "m_landingpad";
	setAttr ".t" -type "double3" -0.10202169147094509 2.1139647063773754 9.9920072216264089e-016 ;
	setAttr ".s" -type "double3" 0.5 0.5 0.5 ;
	setAttr ".rp" -type "double3" -14.897978308529055 -1.3639647063773754 0 ;
	setAttr ".sp" -type "double3" -29.79595661705811 -2.7279294127547509 0 ;
	setAttr ".spt" -type "double3" 14.897978308529055 1.3639647063773754 0 ;
createNode mesh -n "phys_trig_landingShape" -p "phys_trig_landing";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "phys_pos_path_start" -p "m_terrain";
	setAttr ".t" -type "double3" -31.427406978177689 13.066542783588012 0 ;
createNode mesh -n "phys_pos_path_Shape1" -p "phys_pos_path_start";
	addAttr -ci true -sn "mso" -ln "miShadingSamplesOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "msh" -ln "miShadingSamples" -min 0 -smx 8 -at "float";
	addAttr -ci true -sn "mdo" -ln "miMaxDisplaceOverride" -min 0 -max 1 -at "bool";
	addAttr -ci true -sn "mmd" -ln "miMaxDisplace" -min 0 -smx 1 -at "float";
	setAttr -k off ".v";
	setAttr -s 3 ".iog";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "phys_pos_path_1" -p "m_terrain";
	setAttr ".t" -type "double3" 26.557415263908663 18.808811234487251 0 ;
createNode transform -n "phys_pos_path_land" -p "m_terrain";
	setAttr ".t" -type "double3" 28.430127445444832 3.7052211892180704 0 ;
createNode transform -n "m_startingpad" -p "m_terrain";
	setAttr ".t" -type "double3" -30 -1.5083852415511227 -1.9984014443252818e-015 ;
	setAttr ".s" -type "double3" 2 2 2 ;
	setAttr ".rp" -type "double3" 30 -4.5 1.9984014443252818e-015 ;
	setAttr ".sp" -type "double3" 15 -2.25 9.9920072216264089e-016 ;
	setAttr ".spt" -type "double3" 15 -2.25 9.9920072216264089e-016 ;
createNode transform -n "phys_startingpad" -p "m_startingpad";
	setAttr ".t" -type "double3" 0 0 6.6613381477509392e-016 ;
	setAttr ".rp" -type "double3" 15 -2.25 3.3306690738754696e-016 ;
	setAttr ".sp" -type "double3" 15 -2.25 3.3306690738754696e-016 ;
createNode transform -n "phys_pos_start" -p "m_startingpad";
	addAttr -ci true -sn "nts" -ln "notes" -dt "string";
	setAttr ".t" -type "double3" 0 2.9016894284948709 1.6069320361872788e-014 ;
	setAttr ".r" -type "double3" 0 -89.999999999999986 0 ;
	setAttr ".s" -type "double3" 2.5 2.5 2.5 ;
	setAttr ".rp" -type "double3" -1.1517405960909646e-014 -5.1516894284948718 -15 ;
	setAttr ".rpt" -type "double3" 15.000000000000011 0 14.999999999999986 ;
	setAttr ".sp" -type "double3" -4.6069623843638584e-015 -2.060675771397948 -6 ;
	setAttr ".spt" -type "double3" -6.9104435765457876e-015 -3.0910136570969211 -9 ;
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
createNode transform -n "m_jump3" -p "m_terrain";
	setAttr ".t" -type "double3" -14.903241330624382 7.0030119319701756 0 ;
	setAttr ".rp" -type "double3" 14.903241330624382 -7.0030119319701756 0 ;
	setAttr ".sp" -type "double3" 14.903241330624382 -7.0030119319701756 0 ;
createNode mesh -n "m_phys_jumpShape" -p "m_jump3";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	addAttr -ci true -sn "rgn" -ln "rgn" -dt "vectorArray";
	setAttr -k off ".v";
	setAttr -s 8 ".iog";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr -s 5 ".pt[3:7]" -type "float3"  0 -1.7036294 0 0 -7.5247598 
		0 0 -1.7036294 0 0 -7.5247598 0 0 0 0;
	setAttr ".rgvtx" -type "vectorArray" 10 -8.0128984451293945 -5.6861653327941895
		 7.4500017166137695 0 -5.6861653327941895 7.4500017166137695 8.0128984451293945 -5.6861653327941895
		 7.4500017166137695 -8.0128984451293945 3.9825358390808105 7.4500017166137695 0 -1.8385944366455078
		 7.4500017166137695 -8.0128984451293945 3.9825358390808105 -7.4500017166137695 0 -1.8385944366455078
		 -7.4500017166137695 8.0128984451293945 -5.6861653327941895 -7.4500017166137695 -8.0128984451293945
		 -5.6861653327941895 -7.4500017166137695 0 -5.6861653327941895 -7.4500017166137695 ;
	setAttr ".rgf" -type "string" "[[0,1,4,3],[1,2,4],[3,4,6,5],[4,2,7,6],[5,6,9,8],[6,7,9],[8,9,1,0],[9,7,2,1],[8,0,3,5]]";
	setAttr ".rgn" -type "vectorArray" 34 -1.4088783473198419e-007 0 0.99999994039535522 -7.0443917365992093e-008
		 0 0.99999994039535522 -7.0443917365992093e-008 0 0.99999994039535522 -1.4088783473198419e-007
		 0 0.99999994039535522 -7.0443917365992093e-008 0 0.99999994039535522 0 0 0.99999994039535522 -7.0443917365992093e-008
		 0 0.99999994039535522 0.58774691820144653 0.80904489755630493 0 0.5123898983001709
		 0.85875290632247925 0 0.5123898983001709 0.85875290632247925 0 0.58774691820144653
		 0.80904489755630493 0 0.5123898983001709 0.85875290632247925 0 0.4328572154045105
		 0.90146249532699585 0 0.4328572154045105 0.90146249532699585 0 0.5123898983001709
		 0.85875290632247925 0 -1.4088783473198419e-007 0 -0.99999994039535522 -7.0443917365992093e-008
		 0 -0.99999994039535522 -7.0443917365992093e-008 0 -0.99999994039535522 -1.4088783473198419e-007
		 0 -0.99999994039535522 -7.0443917365992093e-008 0 -0.99999994039535522 0 0 -0.99999994039535522 -7.0443917365992093e-008
		 0 -0.99999994039535522 0 -1 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 0 -1 0 -1
		 0 0 -1 0 0 -1 0 0 -1 0 0 ;
createNode transform -n "phys_jump3" -p "m_jump3";
	setAttr ".rp" -type "double3" 14.903241330624382 -7.0030119319701756 0 ;
	setAttr ".sp" -type "double3" 14.903241330624382 -7.0030119319701756 0 ;
createNode transform -n "m_jump4" -p "m_terrain";
	setAttr ".t" -type "double3" -45.385110762870866 7.0030119319701756 0 ;
	setAttr ".r" -type "double3" 0 180 0 ;
	setAttr ".rp" -type "double3" -45.385110762870866 -7.0030119319701756 5.5580730624697896e-015 ;
	setAttr ".rpt" -type "double3" 90.770221525741732 0 -5.5580730624697881e-015 ;
	setAttr ".sp" -type "double3" -45.385110762870866 -7.0030119319701756 5.5580730624697896e-015 ;
createNode transform -n "phys_jump4" -p "m_jump4";
	setAttr ".rp" -type "double3" -45.385110762870866 -7.0030119319701756 5.5580730624697912e-015 ;
	setAttr ".sp" -type "double3" -45.385110762870866 -7.0030119319701756 5.5580730624697912e-015 ;
createNode transform -n "m_jump1" -p "m_terrain";
	setAttr ".t" -type "double3" 45.593111327471689 7.0030119319701756 0 ;
	setAttr ".rp" -type "double3" -45.593111327471689 -7.0030119319701756 0 ;
	setAttr ".sp" -type "double3" -45.593111327471689 -7.0030119319701756 0 ;
createNode transform -n "phys_jump1" -p "m_jump1";
	setAttr ".rp" -type "double3" -45.593111327471689 -7.0030119319701756 0 ;
	setAttr ".sp" -type "double3" -45.593111327471689 -7.0030119319701756 0 ;
createNode transform -n "m_jump2" -p "m_terrain";
	setAttr ".t" -type "double3" 14.414171215510063 7.0030119319701756 0 ;
	setAttr ".r" -type "double3" 0 180 0 ;
	setAttr ".rp" -type "double3" 14.414171215510063 -7.0030119319701756 -1.7652268641436312e-015 ;
	setAttr ".rpt" -type "double3" -28.828342431020126 0 1.7652268641436314e-015 ;
	setAttr ".sp" -type "double3" 14.414171215510063 -7.0030119319701756 -1.7652268641436312e-015 ;
createNode transform -n "phys_jump2" -p "m_jump2";
	setAttr ".rp" -type "double3" 14.414171215510063 -7.0030119319701756 -1.765226864143631e-015 ;
	setAttr ".sp" -type "double3" 14.414171215510063 -7.0030119319701756 -1.765226864143631e-015 ;
createNode transform -n "m_garage2" -p "m_terrain";
	setAttr ".t" -type "double3" -124.66028366968214 2.8471871374007058 0 ;
	setAttr ".r" -type "double3" 0 0 -86.607862913355063 ;
	setAttr ".rp" -type "double3" 10.218270544956694 124.27340723691078 0 ;
	setAttr ".rpt" -type "double3" 114.44201312472545 -127.12059437431149 0 ;
	setAttr ".sp" -type "double3" 10.218270544956694 124.27340723691078 0 ;
createNode mesh -n "m_garage2Shape" -p "m_garage2";
	addAttr -ci true -sn "rgvtx" -ln "rgvtx" -dt "vectorArray";
	addAttr -ci true -sn "rgf" -ln "rgf" -dt "string";
	addAttr -ci true -sn "rgn" -ln "rgn" -dt "vectorArray";
	setAttr -k off ".v";
	setAttr -s 2 ".iog";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
	setAttr ".rgvtx" -type "vectorArray" 44 15.909343719482422 -62.068401336669922
		 -7.8183126449584961 11.899655342102051 -62.068401336669922 -9.7492780685424805 7.4492368698120117
		 -62.068401336669922 -9.7492780685424805 3.4395484924316406 -62.068401336669922 -7.8183150291442871 0.66475772857666016
		 -62.068401336669922 -4.3388385772705078 -0.32555484771728516 -62.068401336669922
		 -1.7881393432617188e-006 0.66475582122802734 -62.068401336669922 4.3388357162475586 3.4395456314086914
		 -62.068401336669922 7.8183131217956543 7.4492340087890625 -62.068401336669922 9.7492780685424805 11.899652481079102
		 -62.068401336669922 9.7492790222167969 15.909341812133789 -62.068401336669922 7.8183150291442871 6.2348990440368652
		 0 -7.8183126449584961 2.2252111434936523 0 -9.7492780685424805 -2.2252070903778076
		 0 -9.7492780685424805 -6.2348957061767578 0 -7.8183150291442871 -9.0096864700317383
		 0 -4.3388385772705078 -9.9999990463256836 0 -1.7881393432617188e-006 -9.0096883773803711
		 0 4.3388357162475586 -6.234898567199707 0 7.8183131217956543 -2.225210428237915 0
		 9.7492780685424805 2.2252082824707031 0 9.7492790222167969 6.2348976135253906 0 7.8183150291442871 6.2348990440368652
		 25 -7.8183126449584961 2.2252111434936523 25 -9.7492780685424805 -2.2252070903778076
		 25 -9.7492780685424805 -6.2348957061767578 25 -7.8183150291442871 -9.0096864700317383
		 25 -4.3388385772705078 -9.9999990463256836 25 -1.7881393432617188e-006 -9.0096883773803711
		 25 4.3388357162475586 -6.234898567199707 25 7.8183131217956543 -2.225210428237915
		 25 9.7492780685424805 2.2252082824707031 25 9.7492790222167969 6.2348976135253906
		 25 7.8183150291442871 3.6646194458007812 -1.7481765747070312 -6.0967864990234375 0.53782886266708374
		 -1.7481765747070312 -7.6025676727294922 -2.9326448440551758 -1.7481765747070312 -7.6025676727294922 -6.0594367980957031
		 -1.7481765747070312 -6.0967888832092285 -8.2232427597045898 -1.7481765747070312 -3.3834624290466309 -8.9954948425292969
		 -1.7481765747070312 -1.5420584986713948e-006 -8.2232446670532227 -1.7481765747070312
		 3.3834600448608398 -6.0594387054443359 -1.7481765747070312 6.0967864990234375 -2.9326472282409668
		 -1.7481765747070312 7.6025667190551758 0.53782683610916138 -1.7481765747070312 7.6025676727294922 3.6646180152893066
		 -1.7481765747070312 6.0967884063720703 ;
	setAttr ".rgf" -type "string" "[[0,1,12,11],[1,2,13,12],[2,3,14,13],[3,4,15,14],[4,5,16,15],[5,6,17,16],[6,7,18,17],[7,8,19,18],[8,9,20,19],[9,10,21,20],[11,12,23,22],[12,13,24,23],[13,14,25,24],[14,15,26,25],[15,16,27,26],[16,17,28,27],[17,18,29,28],[18,19,30,29],[19,20,31,30],[20,21,32,31],[22,23,34,33],[23,24,35,34],[24,25,36,35],[25,26,37,36],[26,27,38,37],[27,28,39,38],[28,29,40,39],[29,30,41,40],[30,31,42,41],[31,32,43,42]]";
	setAttr ".rgn" -type "vectorArray" 120 0.4328952431678772 0.067474275827407837
		 -0.89891541004180908 0.22213396430015564 0.034623455256223679 -0.97440111637115479 0.22236092388629913
		 0.017309645190834999 -0.97481071949005127 0.43363675475120544 0.033756375312805176
		 -0.90045517683029175 0.22213396430015564 0.034623455256223679 -0.97440111637115479 -0.22213374078273773
		 -0.034623414278030396 -0.97440123558044434 -0.22236065566539764 -0.017309624701738358
		 -0.97481077909469604 0.22236092388629913 0.017309645190834999 -0.97481071949005127 -0.22213374078273773
		 -0.034623414278030396 -0.97440123558044434 -0.62011837959289551 -0.096656262874603271
		 -0.77853113412857056 -0.62253332138061523 -0.04838167130947113 -0.78109639883041382 -0.22236065566539764
		 -0.017309624701738358 -0.97481077909469604 -0.62011837959289551 -0.096656262874603271
		 -0.77853113412857056 -0.89201951026916504 -0.13903681933879852 -0.43008136749267578 -0.89867818355560303
		 -0.069700725376605988 -0.43303510546684265 -0.62253332138061523 -0.04838167130947113
		 -0.78109639883041382 -0.89201951026916504 -0.13903681933879852 -0.43008136749267578 -0.98806959390640259
		 -0.15400791168212891 -1.9858011057749536e-007 -0.99701118469238281 -0.077257350087165833
		 -2.1839066732809442e-007 -0.89867818355560303 -0.069700725376605988 -0.43303510546684265 -0.98806959390640259
		 -0.15400791168212891 -1.9858011057749536e-007 -0.89201962947845459 -0.13903684914112091
		 0.43008109927177429 -0.89867836236953735 -0.069700740277767181 0.43303480744361877 -0.99701118469238281
		 -0.077257350087165833 -2.1839066732809442e-007 -0.89201962947845459 -0.13903684914112091
		 0.43008109927177429 -0.62011861801147461 -0.096656307578086853 0.77853095531463623 -0.62253355979919434
		 -0.048381693661212921 0.78109622001647949 -0.89867836236953735 -0.069700740277767181
		 0.43303480744361877 -0.62011861801147461 -0.096656307578086853 0.77853095531463623 -0.22213400900363922
		 -0.034623458981513977 0.97440117597579956 -0.22236095368862152 -0.017309645190834999
		 0.97481065988540649 -0.62253355979919434 -0.048381693661212921 0.78109622001647949 -0.22213400900363922
		 -0.034623458981513977 0.97440117597579956 0.22213368117809296 0.034623406827449799
		 0.97440117597579956 0.22236062586307526 0.017309622839093208 0.97481077909469604 -0.22236095368862152
		 -0.017309645190834999 0.97481065988540649 0.22213368117809296 0.034623406827449799
		 0.97440117597579956 0.43289488554000854 0.067474216222763062 0.89891558885574341 0.4336363673210144
		 0.03375634178519249 0.9004552960395813 0.22236062586307526 0.017309622839093208 0.97481077909469604 0.43363675475120544
		 0.033756375312805176 -0.90045517683029175 0.22236092388629913 0.017309645190834999
		 -0.97481071949005127 0.22252108156681061 0 -0.97492784261703491 0.43388402462005615
		 0 -0.90096867084503174 0.22236092388629913 0.017309645190834999 -0.97481071949005127 -0.22236065566539764
		 -0.017309624701738358 -0.97481077909469604 -0.22252082824707031 0 -0.97492796182632446 0.22252108156681061
		 0 -0.97492784261703491 -0.22236065566539764 -0.017309624701738358 -0.97481077909469604 -0.62253332138061523
		 -0.04838167130947113 -0.78109639883041382 -0.6234896183013916 0 -0.78183156251907349 -0.22252082824707031
		 0 -0.97492796182632446 -0.62253332138061523 -0.04838167130947113 -0.78109639883041382 -0.89867818355560303
		 -0.069700725376605988 -0.43303510546684265 -0.90096879005432129 0 -0.43388393521308899 -0.6234896183013916
		 0 -0.78183156251907349 -0.89867818355560303 -0.069700725376605988 -0.43303510546684265 -0.99701118469238281
		 -0.077257350087165833 -2.1839066732809442e-007 -1 0 -2.3690778050422523e-007 -0.90096879005432129
		 0 -0.43388393521308899 -0.99701118469238281 -0.077257350087165833 -2.1839066732809442e-007 -0.89867836236953735
		 -0.069700740277767181 0.43303480744361877 -0.90096890926361084 0 0.43388360738754272 -1
		 0 -2.3690778050422523e-007 -0.89867836236953735 -0.069700740277767181 0.43303480744361877 -0.62253355979919434
		 -0.048381693661212921 0.78109622001647949 -0.62348991632461548 0 0.78183138370513916 -0.90096890926361084
		 0 0.43388360738754272 -0.62253355979919434 -0.048381693661212921 0.78109622001647949 -0.22236095368862152
		 -0.017309645190834999 0.97481065988540649 -0.22252117097377777 0 0.97492790222167969 -0.62348991632461548
		 0 0.78183138370513916 -0.22236095368862152 -0.017309645190834999 0.97481065988540649 0.22236062586307526
		 0.017309622839093208 0.97481077909469604 0.22252078354358673 0 0.97492796182632446 -0.22252117097377777
		 0 0.97492790222167969 0.22236062586307526 0.017309622839093208 0.97481077909469604 0.4336363673210144
		 0.03375634178519249 0.9004552960395813 0.43388369679450989 0 0.90096890926361084 0.22252078354358673
		 0 0.97492796182632446 -0.43174409866333008 0.099187977612018585 0.89652591943740845 -0.22138760983943939
		 0.091882660984992981 0.97084766626358032 -0.22138760983943939 0.091882660984992981
		 0.97084766626358032 -0.43174409866333008 0.099187977612018585 0.89652591943740845 -0.22138760983943939
		 0.091882660984992981 0.97084766626358032 0.22209174931049347 0.07216344028711319
		 0.97235167026519775 0.22209174931049347 0.07216344028711319 0.97235167026519775 -0.22138760983943939
		 0.091882660984992981 0.97084766626358032 0.22209174931049347 0.07216344028711319
		 0.97235167026519775 0.6226421594619751 0.054325375705957413 0.78061860799789429 0.6226421594619751
		 0.054325375705957413 0.78061860799789429 0.22209174931049347 0.07216344028711319
		 0.97235167026519775 0.6226421594619751 0.054325375705957413 0.78061860799789429 0.90019321441650391
		 0.0419495590031147 0.43346560001373291 0.90019321441650391 0.0419495590031147 0.43346560001373291 0.6226421594619751
		 0.054325375705957413 0.78061860799789429 0.90019321441650391 0.0419495590031147 0.43346560001373291 0.99929559230804443
		 0.037527628242969513 2.6746718617687293e-007 0.99929559230804443 0.037527628242969513
		 2.6746718617687293e-007 0.90019321441650391 0.0419495590031147 0.43346560001373291 0.99929559230804443
		 0.037527628242969513 2.6746718617687293e-007 0.90019339323043823 0.041949592530727386
		 -0.43346530199050903 0.90019339323043823 0.041949592530727386 -0.43346530199050903 0.99929559230804443
		 0.037527628242969513 2.6746718617687293e-007 0.90019339323043823 0.041949592530727386
		 -0.43346530199050903 0.62264245748519897 0.054325386881828308 -0.78061848878860474 0.62264245748519897
		 0.054325386881828308 -0.78061848878860474 0.90019339323043823 0.041949592530727386
		 -0.43346530199050903 0.62264245748519897 0.054325386881828308 -0.78061848878860474 0.22209200263023376
		 0.07216346263885498 -0.9723515510559082 0.22209200263023376 0.07216346263885498 -0.9723515510559082 0.62264245748519897
		 0.054325386881828308 -0.78061848878860474 0.22209200263023376 0.07216346263885498
		 -0.9723515510559082 -0.22138723731040955 0.091882660984992981 -0.9708477258682251 -0.22138723731040955
		 0.091882660984992981 -0.9708477258682251 0.22209200263023376 0.07216346263885498
		 -0.9723515510559082 -0.22138723731040955 0.091882660984992981 -0.9708477258682251 -0.43174362182617188
		 0.099187977612018585 -0.89652621746063232 -0.43174362182617188 0.099187977612018585
		 -0.89652621746063232 -0.22138723731040955 0.091882660984992981 -0.9708477258682251 ;
createNode transform -n "phys_garage2_1" -p "m_garage2";
	setAttr ".t" -type "double3" -9.4766204778089467 12.311544777325125 0 ;
	setAttr ".r" -type "double3" 0 0 90.442150518262196 ;
	setAttr ".rp" -type "double3" 111.80654509241479 -20.55830373147564 0 ;
	setAttr ".rpt" -type "double3" -92.111654069649219 132.52016619106132 0 ;
	setAttr ".sp" -type "double3" 111.80654509241479 -20.55830373147564 0 ;
createNode mesh -n "phys_garage2_Shape1" -p "phys_garage2_1";
	setAttr -k off ".v";
	setAttr -s 2 ".iog";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "phys_garage2_2" -p "m_garage2";
	setAttr ".t" -type "double3" -4.7816137020608842 -29.813851208938885 0 ;
	setAttr ".r" -type "double3" 0 0 98.928415309686969 ;
	setAttr ".s" -type "double3" 0.99999999999999989 0.99999999999999989 1 ;
	setAttr ".rp" -type "double3" 149.89219868277053 -38.732525309187047 0 ;
	setAttr ".rpt" -type "double3" -134.89231443575292 192.81978375503658 0 ;
	setAttr ".sp" -type "double3" 149.89219868277056 -38.732525309187054 0 ;
	setAttr ".spt" -type "double3" -2.8421709430404004e-014 7.1054273576010011e-015 
		0 ;
createNode mesh -n "phys_garage2_Shape2" -p "phys_garage2_2";
	setAttr -k off ".v";
	setAttr -s 2 ".iog";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "m_garage3" -p "m_terrain";
	setAttr ".t" -type "double3" 122.96570082503533 2.8471871374007058 0 ;
	setAttr ".r" -type "double3" 180 0 -93.392 ;
	setAttr ".rp" -type "double3" 10.117709836386654 122.58181758178557 1.5011943053519843e-014 ;
	setAttr ".rpt" -type "double3" -133.08341066142199 -125.4290047191863 -1.501194305351984e-014 ;
	setAttr ".sp" -type "double3" 10.117709836386654 122.58181758178557 1.5011943053519843e-014 ;
createNode transform -n "phys_garage2_3" -p "m_garage3";
	setAttr ".t" -type "double3" -9.380325677326903 12.535449922627564 1.5351498623617846e-015 ;
	setAttr ".r" -type "double3" 6.962580446895813e-015 -7.0165005129028553e-015 90.442 ;
	setAttr ".s" -type "double3" 0.99999999999999933 0.99999999999999956 1 ;
	setAttr ".rp" -type "double3" 109.89267989383704 -20.346383698811607 1.3476793191158066e-014 ;
	setAttr ".rpt" -type "double3" -90.394644380123509 130.39275135796959 0 ;
	setAttr ".sp" -type "double3" 109.89267989383711 -20.346383698811618 1.3476793191158066e-014 ;
	setAttr ".spt" -type "double3" -7.1054273576009968e-014 1.0658141036401498e-014 
		0 ;
createNode transform -n "phys_garage2_4" -p "m_garage3";
	setAttr ".t" -type "double3" -4.6402916391318296 -29.547473151078137 -3.6185218417360264e-015 ;
	setAttr ".r" -type "double3" -180 6.931676838606383e-015 -81.071000000000012 ;
	setAttr ".s" -type "double3" 0.99999999999999956 0.99999999999999967 1 ;
	setAttr ".rp" -type "double3" -147.99510619387411 -38.191206436816124 -1.3953391023413608e-014 ;
	setAttr ".rpt" -type "double3" 162.75310766939262 190.32049716967984 3.258385591866948e-014 ;
	setAttr ".sp" -type "double3" -147.99510619387416 -38.191206436816138 -1.3953391023413608e-014 ;
	setAttr ".spt" -type "double3" 5.684341886080799e-014 1.4210854715201999e-014 0 ;
createNode transform -n "phys_trig_car_eater" -p "m_garage3";
	setAttr ".t" -type "double3" 14.246639524688181 266.42155257139189 3.262723015804234e-014 ;
	setAttr ".r" -type "double3" 0 180 86.608 ;
	setAttr ".s" -type "double3" 0.99999999999999978 0.99999999999999967 1 ;
	setAttr ".rp" -type "double3" 143.83203835484372 -4.3888682985351419 9.4256591262215284e-019 ;
	setAttr ".rpt" -type "double3" -147.9609680431453 -139.45086669107121 -1.7616229670435114e-014 ;
	setAttr ".sp" -type "double3" 143.83203835484375 -4.3888682985351437 9.4256591262215284e-019 ;
	setAttr ".spt" -type "double3" -2.8421709430404001e-014 1.7763568394002499e-015 
		0 ;
createNode mesh -n "phys_trig_car_eaterShape" -p "phys_trig_car_eater";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
createNode transform -n "phys_pos_car_start" -p "m_terrain";
	setAttr ".t" -type "double3" 101.35528615769951 4.0022906836822933 0 ;
	setAttr ".r" -type "double3" 0 25 0 ;
	setAttr ".s" -type "double3" 1 1.0000000000000007 1.0000000000000007 ;
	setAttr ".rpt" -type "double3" 0 0 -1.0023759600130701e-015 ;
	setAttr ".spt" -type "double3" 0 -1.1832913578315177e-029 -3.0292258760486853e-028 ;
createNode mesh -n "phys_pos_car_startShape" -p "phys_pos_car_start";
	setAttr -k off ".v";
	setAttr ".vir" yes;
	setAttr ".vif" yes;
	setAttr ".uvst[0].uvsn" -type "string" "map1";
	setAttr ".cuvs" -type "string" "map1";
	setAttr ".dcc" -type "string" "Ambient+Diffuse";
	setAttr ".covm[0]"  0 1 1;
	setAttr ".cdvm[0]"  0 1 1;
parent -s -nc -r -add "|m_terrain|m_terrainShape" "phys_terrain";
parent -s -nc -r -add "|m_terrain|m_landingpad|phys_landingpad|m_landingpadShape" "phys_startingpad";
parent -s -nc -r -add "|m_terrain|m_landingpad|phys_landingpad|m_landingpadShape" "m_startingpad";
parent -s -nc -r -add "|m_terrain|m_landingpad|phys_landingpad|m_landingpadShape" "m_landingpad";
parent -s -nc -r -add "|m_terrain|phys_pos_path_start|phys_pos_path_Shape1" "phys_pos_path_1";
parent -s -nc -r -add "|m_terrain|phys_pos_path_start|phys_pos_path_Shape1" "phys_pos_path_land";
parent -s -nc -r -add "|m_terrain|m_jump3|m_phys_jumpShape" "m_jump4";
parent -s -nc -r -add "|m_terrain|m_jump3|m_phys_jumpShape" "m_jump1";
parent -s -nc -r -add "|m_terrain|m_jump3|m_phys_jumpShape" "m_jump2";
parent -s -nc -r -add "|m_terrain|m_jump3|m_phys_jumpShape" "phys_jump1";
parent -s -nc -r -add "|m_terrain|m_jump3|m_phys_jumpShape" "phys_jump2";
parent -s -nc -r -add "|m_terrain|m_jump3|m_phys_jumpShape" "phys_jump3";
parent -s -nc -r -add "|m_terrain|m_jump3|m_phys_jumpShape" "phys_jump4";
parent -s -nc -r -add "|m_terrain|m_garage2|m_garage2Shape" "m_garage3";
parent -s -nc -r -add "|m_terrain|m_garage2|phys_garage2_1|phys_garage2_Shape1" "phys_garage2_3";
parent -s -nc -r -add "|m_terrain|m_garage2|phys_garage2_2|phys_garage2_Shape2" "phys_garage2_4";
createNode lightLinker -n "lightLinker1";
	setAttr -s 9 ".lnk";
	setAttr -s 9 ".slnk";
createNode displayLayerManager -n "layerManager";
createNode displayLayer -n "defaultLayer";
createNode renderLayerManager -n "renderLayerManager";
createNode renderLayer -n "defaultRenderLayer";
	setAttr ".g" yes;
createNode phong -n "mat_phys";
	setAttr ".c" -type "float3" 1 0.42400002 0.98656017 ;
	setAttr ".it" -type "float3" 0.28099999 0.28099999 0.28099999 ;
createNode shadingEngine -n "phong1SG";
	setAttr ".ihi" 0;
	setAttr -s 16 ".dsm";
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo1";
createNode phong -n "mat_ground";
	setAttr ".ambc" -type "float3" 0.40496001 0.40496001 0.40496001 ;
	setAttr ".sc" -type "float3" 0 0 0 ;
	setAttr ".cp" 100;
createNode shadingEngine -n "phong2SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo2";
createNode file -n "file1";
	setAttr ".ftn" -type "string" "C:/RnD/PD/trunk/HeliForce/Data//rock.png";
createNode place2dTexture -n "place2dTexture1";
createNode phong -n "mat_platform";
	setAttr ".dc" 1;
	setAttr ".ambc" -type "float3" 0.41321999 0.41321999 0.41321999 ;
createNode shadingEngine -n "phong3SG";
	setAttr ".ihi" 0;
	setAttr -s 4 ".dsm";
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo3";
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
		+ "\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Trax Editor\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = clipEditorNameFromPanel($panelName);\n            clipEditor -e \n                -displayKeys 0\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -autoFit 0\n                -snapTime \"none\" \n                -snapValue \"none\" \n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"hyperGraphPanel\" (localizedPanelLabel(\"Hypergraph Hierarchy\")) `;\n\tif (\"\" == $panelName) {\n\t\tif ($useSceneConfig) {\n\t\t\t$panelName = `scriptedPanel -unParent  -type \"hyperGraphPanel\" -l (localizedPanelLabel(\"Hypergraph Hierarchy\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = ($panelName+\"HyperGraphEd\");\n            hyperGraph -e \n                -graphLayoutStyle \"hierarchicalLayout\" \n                -orientation \"horiz\" \n"
		+ "                -mergeConnections 1\n                -zoom 0.763825\n                -animateTransition 0\n                -showRelationships 1\n                -showShapes 0\n                -showDeformers 0\n                -showExpressions 0\n                -showConstraints 0\n                -showUnderworld 0\n                -showInvisible 0\n                -transitionFrames 5\n                -currentNode \"m_garage2\" \n                -opaqueContainers 0\n                -dropTargetNode \"m_terrain\" \n                -dropNode \"phys_pos_car_start\" \n                -freeform 1\n                -imagePosition 0 0 \n                -imageScale 1\n                -imageEnabled 0\n                -graphType \"DAG\" \n                -heatMapDisplay 0\n                -updateSelection 1\n                -updateNodeAdded 1\n                -useDrawOverrideColor 0\n                -limitGraphTraversal -1\n                -range 0 0 \n                -iconSize \"largeIcons\" \n                -showCachedConnections 0\n                $editorName;\n"
		+ "\t\t}\n\t} else {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Hypergraph Hierarchy\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = ($panelName+\"HyperGraphEd\");\n            hyperGraph -e \n                -graphLayoutStyle \"hierarchicalLayout\" \n                -orientation \"horiz\" \n                -mergeConnections 1\n                -zoom 0.763825\n                -animateTransition 0\n                -showRelationships 1\n                -showShapes 0\n                -showDeformers 0\n                -showExpressions 0\n                -showConstraints 0\n                -showUnderworld 0\n                -showInvisible 0\n                -transitionFrames 5\n                -currentNode \"m_garage2\" \n                -opaqueContainers 0\n                -dropTargetNode \"m_terrain\" \n                -dropNode \"phys_pos_car_start\" \n                -freeform 1\n                -imagePosition 0 0 \n                -imageScale 1\n                -imageEnabled 0\n                -graphType \"DAG\" \n"
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
		+ "                -textures 1\n                -strokes 1\n                -shadows 0\n                -displayMode \"centerEye\" \n                -viewColor 0 0 0 1 \n                $editorName;\nstereoCameraView -e -viewSelected 0 $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\tif ($useSceneConfig) {\n        string $configName = `getPanel -cwl (localizedPanelLabel(\"Current Layout\"))`;\n        if (\"\" != $configName) {\n\t\t\tpanelConfiguration -edit -label (localizedPanelLabel(\"Current Layout\")) \n\t\t\t\t-defaultImage \"\"\n\t\t\t\t-image \"\"\n\t\t\t\t-sc false\n\t\t\t\t-configString \"global string $gMainPane; paneLayout -e -cn \\\"horizontal2\\\" -ps 1 100 46 -ps 2 100 54 $gMainPane;\"\n\t\t\t\t-removeAllPanels\n\t\t\t\t-ap false\n\t\t\t\t\t(localizedPanelLabel(\"Persp View\")) \n\t\t\t\t\t\"modelPanel\"\n"
		+ "\t\t\t\t\t\"$panelName = `modelPanel -unParent -l (localizedPanelLabel(\\\"Persp View\\\")) -mbv $menusOkayInPanels `;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera persp` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"smoothShaded\\\" \\n    -activeOnly 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 1\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName\"\n"
		+ "\t\t\t\t\t\"modelPanel -edit -l (localizedPanelLabel(\\\"Persp View\\\")) -mbv $menusOkayInPanels  $panelName;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera persp` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"smoothShaded\\\" \\n    -activeOnly 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 0\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 1\\n    -backfaceCulling 0\\n    -xray 0\\n    -jointXray 0\\n    -activeComponentsXray 0\\n    -displayTextures 1\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 8192\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -maxConstantTransparency 1\\n    -rendererName \\\"base_OpenGL_Renderer\\\" \\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -shadows 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName\"\n"
		+ "\t\t\t\t-ap false\n\t\t\t\t\t(localizedPanelLabel(\"Hypergraph Hierarchy\")) \n\t\t\t\t\t\"scriptedPanel\"\n\t\t\t\t\t\"$panelName = `scriptedPanel -unParent  -type \\\"hyperGraphPanel\\\" -l (localizedPanelLabel(\\\"Hypergraph Hierarchy\\\")) -mbv $menusOkayInPanels `;\\n\\n\\t\\t\\t$editorName = ($panelName+\\\"HyperGraphEd\\\");\\n            hyperGraph -e \\n                -graphLayoutStyle \\\"hierarchicalLayout\\\" \\n                -orientation \\\"horiz\\\" \\n                -mergeConnections 1\\n                -zoom 0.763825\\n                -animateTransition 0\\n                -showRelationships 1\\n                -showShapes 0\\n                -showDeformers 0\\n                -showExpressions 0\\n                -showConstraints 0\\n                -showUnderworld 0\\n                -showInvisible 0\\n                -transitionFrames 5\\n                -currentNode \\\"m_garage2\\\" \\n                -opaqueContainers 0\\n                -dropTargetNode \\\"m_terrain\\\" \\n                -dropNode \\\"phys_pos_car_start\\\" \\n                -freeform 1\\n                -imagePosition 0 0 \\n                -imageScale 1\\n                -imageEnabled 0\\n                -graphType \\\"DAG\\\" \\n                -heatMapDisplay 0\\n                -updateSelection 1\\n                -updateNodeAdded 1\\n                -useDrawOverrideColor 0\\n                -limitGraphTraversal -1\\n                -range 0 0 \\n                -iconSize \\\"largeIcons\\\" \\n                -showCachedConnections 0\\n                $editorName\"\n"
		+ "\t\t\t\t\t\"scriptedPanel -edit -l (localizedPanelLabel(\\\"Hypergraph Hierarchy\\\")) -mbv $menusOkayInPanels  $panelName;\\n\\n\\t\\t\\t$editorName = ($panelName+\\\"HyperGraphEd\\\");\\n            hyperGraph -e \\n                -graphLayoutStyle \\\"hierarchicalLayout\\\" \\n                -orientation \\\"horiz\\\" \\n                -mergeConnections 1\\n                -zoom 0.763825\\n                -animateTransition 0\\n                -showRelationships 1\\n                -showShapes 0\\n                -showDeformers 0\\n                -showExpressions 0\\n                -showConstraints 0\\n                -showUnderworld 0\\n                -showInvisible 0\\n                -transitionFrames 5\\n                -currentNode \\\"m_garage2\\\" \\n                -opaqueContainers 0\\n                -dropTargetNode \\\"m_terrain\\\" \\n                -dropNode \\\"phys_pos_car_start\\\" \\n                -freeform 1\\n                -imagePosition 0 0 \\n                -imageScale 1\\n                -imageEnabled 0\\n                -graphType \\\"DAG\\\" \\n                -heatMapDisplay 0\\n                -updateSelection 1\\n                -updateNodeAdded 1\\n                -useDrawOverrideColor 0\\n                -limitGraphTraversal -1\\n                -range 0 0 \\n                -iconSize \\\"largeIcons\\\" \\n                -showCachedConnections 0\\n                $editorName\"\n"
		+ "\t\t\t\t$configName;\n\n            setNamedPanelLayout (localizedPanelLabel(\"Current Layout\"));\n        }\n\n        panelHistory -e -clear mainPanelHistory;\n        setFocus `paneLayout -q -p1 $gMainPane`;\n        sceneUIReplacement -deleteRemaining;\n        sceneUIReplacement -clear;\n\t}\n\n\ngrid -spacing 5 -size 12 -divisions 5 -displayAxes yes -displayGridLines yes -displayDivisionLines yes -displayPerspectiveLabels no -displayOrthographicLabels no -displayAxesBold yes -perspectiveLabelPosition axis -orthographicLabelPosition edge;\nviewManip -drawCompass 0 -compassAngle 0 -frontParameters \"\" -homeParameters \"\" -selectionLockParameters \"\";\n}\n");
	setAttr ".st" 3;
createNode script -n "sceneConfigurationScriptNode";
	setAttr ".b" -type "string" "playbackOptions -min 1 -max 24 -ast 1 -aet 48 ";
	setAttr ".st" 6;
createNode file -n "file2";
	setAttr ".ftn" -type "string" "C:/RnD/PD/trunk/HeliForce/Data//helipad.png";
createNode place2dTexture -n "place2dTexture2";
createNode phong -n "phong4";
	setAttr ".c" -type "float3" 0.097354583 0.34400001 0.091159999 ;
createNode shadingEngine -n "phong4SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo4";
createNode blinn -n "mat_grass";
	setAttr ".c" -type "float3" 0.1232 0.34999999 0.13401082 ;
createNode shadingEngine -n "blinn1SG";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo5";
createNode polyCube -n "polyCube5";
	setAttr ".w" 16.025797086642783;
	setAttr ".h" 11.372330892039264;
	setAttr ".d" 14.900003263997746;
	setAttr ".sw" 2;
	setAttr ".cuv" 4;
createNode polyCube -n "polyCube4";
	setAttr ".w" 10000;
	setAttr ".h" 3;
	setAttr ".d" 10000;
	setAttr ".cuv" 4;
createNode polyCube -n "polyCube3";
	setAttr ".w" 6;
	setAttr ".h" 0.5;
	setAttr ".d" 6;
	setAttr ".cuv" 4;
createNode polySphere -n "polySphere1";
	setAttr ".sa" 8;
	setAttr ".sh" 8;
createNode polyCube -n "polyCube2";
	setAttr ".w" 0.05;
	setAttr ".h" 0.01;
	setAttr ".d" 0.1;
	setAttr ".cuv" 4;
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
createNode polyCube -n "polyCube1";
	setAttr ".uvs" -type "string" "helipad_map";
	setAttr ".w" 6;
	setAttr ".h" 4;
	setAttr ".d" 6;
	setAttr ".cuv" 4;
createNode polyMergeVert -n "polyMergeVert1";
	setAttr ".ics" -type "componentList" 2 "vtx[2]" "vtx[5]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 -14.903241330624382 5.6861654460196318 0.16303187492982829 1;
createNode polyTweak -n "polyTweak1";
	setAttr ".uopa" yes;
	setAttr -s 2 ".tk";
	setAttr ".tk[5]" -type "float3" 1.7763568e-015 -11.372331 0 ;
	setAttr ".tk[8]" -type "float3" 1.7763568e-015 -11.372331 0 ;
createNode polyMergeVert -n "polyMergeVert2";
	setAttr ".ics" -type "componentList" 2 "vtx[7]" "vtx[10]";
	setAttr ".ix" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 -14.903241330624382 5.6861654460196318 0.16303187492982829 1;
createNode phong -n "mat_jump";
	setAttr ".c" -type "float3" 0.54000002 0.43269122 0.2484 ;
	setAttr ".sc" -type "float3" 0.066119999 0.066119999 0.066119999 ;
createNode shadingEngine -n "phong5SG";
	setAttr ".ihi" 0;
	setAttr -s 4 ".dsm";
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo6";
createNode polyCylinder -n "polyCylinder1";
	setAttr ".r" 10;
	setAttr ".h" 50;
	setAttr ".sa" 14;
	setAttr ".sh" 2;
	setAttr ".sc" 2;
	setAttr ".cuv" 3;
createNode polyTweak -n "polyTweak2";
	setAttr ".uopa" yes;
	setAttr -s 44 ".tk";
	setAttr ".tk[0:27]" -type "float3" 9.6744442 -37.068401 0  9.6744442 -37.068401 
		0  9.6744442 -37.068401 0  9.6744442 -37.068401 0  9.6744442 -37.068401 0  9.6744442 
		-37.068401 0  9.6744442 -37.068401 0  9.6744442 -37.068401 0  9.6744442 -37.068401 
		0  9.6744442 -37.068401 0  9.6744442 -37.068401 0  9.6744442 -37.068401 0  9.6744442 
		-37.068401 0  9.6744442 -37.068401 0  9.6744442 -37.068401 0  9.6744442 -37.068401 
		0  9.6744442 -37.068401 0  9.6744442 -37.068401 0  9.6744442 -37.068401 0  9.6744442 
		-37.068401 0  9.6744442 -37.068401 0  9.6744442 -37.068401 0  9.6744442 -37.068401 
		0  9.6744442 -37.068401 0  9.6744442 -37.068401 0  9.6744442 -37.068401 0  9.6744442 
		-37.068401 0  9.6744442 -37.068401 0 ;
	setAttr ".tk[56:71]" -type "float3" 1.3235769 -26.748177 -1.2140423  0.5471698 
		-26.748177 -2.1876304  -0.57477671 -26.748177 -2.7279284  -1.8200414 -26.748177 -2.7279284  
		-2.9419892 -26.748177 -2.1876314  -3.7183995 -26.748177 -1.2140433  -3.9954951 -26.748177 
		-6.4798888e-007  -3.7184005 -26.748177 1.2140423  -2.9419892 -26.748177 2.1876297  
		-1.8200419 -26.748177 2.7279279  -0.57477731 -26.748177 2.7279284  0.54716933 -26.748177 
		2.1876307  1.3235769 -26.748177 1.2140425  1.6006751 -26.748177 -1.4765257e-007  
		9.6744442 -37.068401 0  -1.1974095 -26.748177 -1.4765257e-007 ;
createNode deleteComponent -n "deleteComponent1";
	setAttr ".dc" -type "componentList" 4 "f[14]" "f[25:28]" "f[39:42]" "f[53:55]";
createNode deleteComponent -n "deleteComponent2";
	setAttr ".dc" -type "componentList" 3 "f[0:8]" "f[10:13]" "f[44:57]";
createNode deleteComponent -n "deleteComponent3";
	setAttr ".dc" -type "componentList" 1 "f[0]";
createNode deleteComponent -n "deleteComponent4";
	setAttr ".dc" -type "componentList" 1 "f[30:43]";
createNode polyCube -n "polyCube6";
	setAttr ".w" 25;
	setAttr ".d" 7;
	setAttr ".cuv" 4;
createNode polyCube -n "polyCube7";
	setAttr ".w" 60;
	setAttr ".d" 7;
	setAttr ".cuv" 4;
createNode polySphere -n "polySphere2";
	setAttr ".r" 3;
createNode polyCube -n "polyCube8";
	setAttr ".w" 0.1;
	setAttr ".h" 0.1;
	setAttr ".cuv" 4;
createNode phong -n "mat_garage";
	setAttr ".c" -type "float3" 0.414 0.414 0.414 ;
	setAttr ".sc" -type "float3" 0 0 0 ;
createNode shadingEngine -n "phong6SG";
	setAttr ".ihi" 0;
	setAttr -s 2 ".dsm";
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo7";
createNode script -n "rg_export";
	addAttr -ci true -sn "time" -ln "time" -dt "string";
	setAttr ".time" -type "string" "2013-06-25T00:09:32.890000";
select -ne :time1;
	setAttr ".o" 1;
select -ne :renderPartition;
	setAttr -s 9 ".st";
select -ne :renderGlobalsList1;
select -ne :defaultShaderList1;
	setAttr -s 9 ".s";
select -ne :postProcessList1;
	setAttr -s 2 ".p";
select -ne :defaultRenderUtilityList1;
	setAttr -s 2 ".u";
select -ne :lightList1;
select -ne :defaultTextureList1;
	setAttr -s 2 ".tx";
select -ne :initialShadingGroup;
	setAttr ".ro" yes;
select -ne :initialParticleSE;
	setAttr ".ro" yes;
select -ne :hardwareRenderGlobals;
	setAttr ".ctrs" 256;
	setAttr ".btrs" 512;
select -ne :defaultHardwareRenderGlobals;
	setAttr ".fn" -type "string" "im";
	setAttr ".res" -type "string" "ntsc_4d 646 485 1.333";
select -ne :hyperGraphLayout;
	setAttr -s 15 ".hyp";
	setAttr ".hyp[0].x" 108.41239929199219;
	setAttr ".hyp[0].y" 1.0026954412460327;
	setAttr ".hyp[0].isf" yes;
	setAttr ".hyp[1].x" 144.78436279296875;
	setAttr ".hyp[1].y" -191.89488220214844;
	setAttr ".hyp[1].isf" yes;
	setAttr ".hyp[2].x" 109.15633392333984;
	setAttr ".hyp[2].y" -113;
	setAttr ".hyp[2].isf" yes;
	setAttr ".hyp[3].x" 73.15093994140625;
	setAttr ".hyp[3].y" 79.814018249511719;
	setAttr ".hyp[3].isf" yes;
	setAttr ".hyp[4].x" 355.02383422851562;
	setAttr ".hyp[4].y" 79.462211608886719;
	setAttr ".hyp[4].isf" yes;
	setAttr ".hyp[5].x" 355.02383422851562;
	setAttr ".hyp[5].y" 39.462215423583984;
	setAttr ".hyp[5].isf" yes;
	setAttr ".hyp[6].x" 355.02383422851562;
	setAttr ".hyp[6].y" -0.53778642416000366;
	setAttr ".hyp[6].isf" yes;
	setAttr ".hyp[7].x" 560.983154296875;
	setAttr ".hyp[7].y" 77.2427978515625;
	setAttr ".hyp[7].isf" yes;
	setAttr ".hyp[8].x" 560.983154296875;
	setAttr ".hyp[8].y" -2.7572042942047119;
	setAttr ".hyp[8].isf" yes;
	setAttr ".hyp[9].x" 560.983154296875;
	setAttr ".hyp[9].y" -82.7572021484375;
	setAttr ".hyp[9].isf" yes;
	setAttr ".hyp[10].x" 560.983154296875;
	setAttr ".hyp[10].y" -162.7572021484375;
	setAttr ".hyp[10].isf" yes;
	setAttr ".hyp[11].x" 816.618408203125;
	setAttr ".hyp[11].y" 85.618400573730469;
	setAttr ".hyp[11].isf" yes;
	setAttr ".hyp[12].x" 817;
	setAttr ".hyp[12].y" -83.835601806640625;
	setAttr ".hyp[12].isf" yes;
	setAttr ".hyp[13].x" 852.186767578125;
	setAttr ".hyp[13].y" -199.28961181640625;
	setAttr ".hyp[13].isf" yes;
	setAttr ".hyp[14].x" 379.6878662109375;
	setAttr ".hyp[14].y" -172.08444213867187;
	setAttr ".hyp[14].isf" yes;
connectAttr "polyCube4.out" "|m_terrain|m_terrainShape.i";
connectAttr "polyPlanarProj2.out" "|m_terrain|m_landingpad|phys_landingpad|m_landingpadShape.i"
		;
connectAttr "polyCube3.out" "phys_trig_landingShape.i";
connectAttr "polySphere1.out" "|m_terrain|phys_pos_path_start|phys_pos_path_Shape1.i"
		;
connectAttr "polyCube2.out" "phys_pos_startShape.i";
connectAttr "polyMergeVert2.out" "|m_terrain|m_jump3|m_phys_jumpShape.i";
connectAttr "deleteComponent4.og" "|m_terrain|m_garage2|m_garage2Shape.i";
connectAttr "polyCube6.out" "|m_terrain|m_garage2|phys_garage2_1|phys_garage2_Shape1.i"
		;
connectAttr "polyCube7.out" "|m_terrain|m_garage2|phys_garage2_2|phys_garage2_Shape2.i"
		;
connectAttr "polySphere2.out" "phys_trig_car_eaterShape.i";
connectAttr "polyCube8.out" "phys_pos_car_startShape.i";
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
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[5].llnk";
connectAttr "phong4SG.msg" "lightLinker1.lnk[5].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[6].llnk";
connectAttr "blinn1SG.msg" "lightLinker1.lnk[6].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[7].llnk";
connectAttr "phong5SG.msg" "lightLinker1.lnk[7].olnk";
connectAttr ":defaultLightSet.msg" "lightLinker1.lnk[8].llnk";
connectAttr "phong6SG.msg" "lightLinker1.lnk[8].olnk";
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
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[5].sllk";
connectAttr "phong4SG.msg" "lightLinker1.slnk[5].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[6].sllk";
connectAttr "blinn1SG.msg" "lightLinker1.slnk[6].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[7].sllk";
connectAttr "phong5SG.msg" "lightLinker1.slnk[7].solk";
connectAttr ":defaultLightSet.msg" "lightLinker1.slnk[8].sllk";
connectAttr "phong6SG.msg" "lightLinker1.slnk[8].solk";
connectAttr "layerManager.dli[0]" "defaultLayer.id";
connectAttr "renderLayerManager.rlmi[0]" "defaultRenderLayer.rlid";
connectAttr "mat_phys.oc" "phong1SG.ss";
connectAttr "phys_pos_startShape.iog" "phong1SG.dsm" -na;
connectAttr "phys_trig_landingShape.iog" "phong1SG.dsm" -na;
connectAttr "|m_terrain|phys_terrain|m_terrainShape.iog" "phong1SG.dsm" -na;
connectAttr "|m_terrain|phys_pos_path_start|phys_pos_path_Shape1.iog" "phong1SG.dsm"
		 -na;
connectAttr "|m_terrain|phys_pos_path_1|phys_pos_path_Shape1.iog" "phong1SG.dsm"
		 -na;
connectAttr "|m_terrain|phys_pos_path_land|phys_pos_path_Shape1.iog" "phong1SG.dsm"
		 -na;
connectAttr "|m_terrain|m_jump2|phys_jump2|m_phys_jumpShape.iog" "phong1SG.dsm" 
		-na;
connectAttr "|m_terrain|m_jump1|phys_jump1|m_phys_jumpShape.iog" "phong1SG.dsm" 
		-na;
connectAttr "|m_terrain|m_jump4|phys_jump4|m_phys_jumpShape.iog" "phong1SG.dsm" 
		-na;
connectAttr "|m_terrain|m_jump3|phys_jump3|m_phys_jumpShape.iog" "phong1SG.dsm" 
		-na;
connectAttr "|m_terrain|m_garage2|phys_garage2_1|phys_garage2_Shape1.iog" "phong1SG.dsm"
		 -na;
connectAttr "|m_terrain|m_garage2|phys_garage2_2|phys_garage2_Shape2.iog" "phong1SG.dsm"
		 -na;
connectAttr "|m_terrain|m_garage3|phys_garage2_3|phys_garage2_Shape1.iog" "phong1SG.dsm"
		 -na;
connectAttr "|m_terrain|m_garage3|phys_garage2_4|phys_garage2_Shape2.iog" "phong1SG.dsm"
		 -na;
connectAttr "phys_trig_car_eaterShape.iog" "phong1SG.dsm" -na;
connectAttr "phys_pos_car_startShape.iog" "phong1SG.dsm" -na;
connectAttr "phong1SG.msg" "materialInfo1.sg";
connectAttr "mat_phys.msg" "materialInfo1.m";
connectAttr "file1.oc" "mat_ground.c";
connectAttr "file1.ot" "mat_ground.it";
connectAttr "mat_ground.oc" "phong2SG.ss";
connectAttr "phong2SG.msg" "materialInfo2.sg";
connectAttr "mat_ground.msg" "materialInfo2.m";
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
connectAttr "file2.oc" "mat_platform.c";
connectAttr "file2.ot" "mat_platform.it";
connectAttr "mat_platform.oc" "phong3SG.ss";
connectAttr "|m_terrain|m_startingpad|m_landingpadShape.iog" "phong3SG.dsm" -na;
connectAttr "|m_terrain|m_startingpad|phys_startingpad|m_landingpadShape.iog" "phong3SG.dsm"
		 -na;
connectAttr "|m_terrain|m_landingpad|m_landingpadShape.iog" "phong3SG.dsm" -na;
connectAttr "|m_terrain|m_landingpad|phys_landingpad|m_landingpadShape.iog" "phong3SG.dsm"
		 -na;
connectAttr "phong3SG.msg" "materialInfo3.sg";
connectAttr "mat_platform.msg" "materialInfo3.m";
connectAttr "file2.msg" "materialInfo3.t" -na;
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
connectAttr "phong4.oc" "phong4SG.ss";
connectAttr "phong4SG.msg" "materialInfo4.sg";
connectAttr "phong4.msg" "materialInfo4.m";
connectAttr "mat_grass.oc" "blinn1SG.ss";
connectAttr "|m_terrain|m_terrainShape.iog" "blinn1SG.dsm" -na;
connectAttr "blinn1SG.msg" "materialInfo5.sg";
connectAttr "mat_grass.msg" "materialInfo5.m";
connectAttr "polyPlanarProj1.out" "polyPlanarProj2.ip";
connectAttr "|m_terrain|m_startingpad|m_landingpadShape.wm" "polyPlanarProj2.mp"
		;
connectAttr "polyCube1.out" "polyPlanarProj1.ip";
connectAttr "|m_terrain|m_startingpad|m_landingpadShape.wm" "polyPlanarProj1.mp"
		;
connectAttr "polyTweak1.out" "polyMergeVert1.ip";
connectAttr "|m_terrain|m_jump3|m_phys_jumpShape.wm" "polyMergeVert1.mp";
connectAttr "polyCube5.out" "polyTweak1.ip";
connectAttr "polyMergeVert1.out" "polyMergeVert2.ip";
connectAttr "|m_terrain|m_jump3|m_phys_jumpShape.wm" "polyMergeVert2.mp";
connectAttr "mat_jump.oc" "phong5SG.ss";
connectAttr "|m_terrain|m_jump3|m_phys_jumpShape.iog" "phong5SG.dsm" -na;
connectAttr "|m_terrain|m_jump4|m_phys_jumpShape.iog" "phong5SG.dsm" -na;
connectAttr "|m_terrain|m_jump1|m_phys_jumpShape.iog" "phong5SG.dsm" -na;
connectAttr "|m_terrain|m_jump2|m_phys_jumpShape.iog" "phong5SG.dsm" -na;
connectAttr "phong5SG.msg" "materialInfo6.sg";
connectAttr "mat_jump.msg" "materialInfo6.m";
connectAttr "polyCylinder1.out" "polyTweak2.ip";
connectAttr "polyTweak2.out" "deleteComponent1.ig";
connectAttr "deleteComponent1.og" "deleteComponent2.ig";
connectAttr "deleteComponent2.og" "deleteComponent3.ig";
connectAttr "deleteComponent3.og" "deleteComponent4.ig";
connectAttr "mat_garage.oc" "phong6SG.ss";
connectAttr "|m_terrain|m_garage2|m_garage2Shape.iog" "phong6SG.dsm" -na;
connectAttr "|m_terrain|m_garage3|m_garage2Shape.iog" "phong6SG.dsm" -na;
connectAttr "phong6SG.msg" "materialInfo7.sg";
connectAttr "mat_garage.msg" "materialInfo7.m";
connectAttr "phong1SG.pa" ":renderPartition.st" -na;
connectAttr "phong2SG.pa" ":renderPartition.st" -na;
connectAttr "phong3SG.pa" ":renderPartition.st" -na;
connectAttr "phong4SG.pa" ":renderPartition.st" -na;
connectAttr "blinn1SG.pa" ":renderPartition.st" -na;
connectAttr "phong5SG.pa" ":renderPartition.st" -na;
connectAttr "phong6SG.pa" ":renderPartition.st" -na;
connectAttr "mat_phys.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_ground.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_platform.msg" ":defaultShaderList1.s" -na;
connectAttr "phong4.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_grass.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_jump.msg" ":defaultShaderList1.s" -na;
connectAttr "mat_garage.msg" ":defaultShaderList1.s" -na;
connectAttr "place2dTexture1.msg" ":defaultRenderUtilityList1.u" -na;
connectAttr "place2dTexture2.msg" ":defaultRenderUtilityList1.u" -na;
connectAttr "lightLinker1.msg" ":lightList1.ln" -na;
connectAttr "file1.msg" ":defaultTextureList1.tx" -na;
connectAttr "file2.msg" ":defaultTextureList1.tx" -na;
connectAttr "m_startingpad.msg" ":hyperGraphLayout.hyp[0].dn";
connectAttr "phys_landingpad.msg" ":hyperGraphLayout.hyp[1].dn";
connectAttr "m_landingpad.msg" ":hyperGraphLayout.hyp[2].dn";
connectAttr "m_terrain.msg" ":hyperGraphLayout.hyp[3].dn";
connectAttr "phys_pos_path_start.msg" ":hyperGraphLayout.hyp[4].dn";
connectAttr "phys_pos_path_1.msg" ":hyperGraphLayout.hyp[5].dn";
connectAttr "phys_pos_path_land.msg" ":hyperGraphLayout.hyp[6].dn";
connectAttr "m_jump3.msg" ":hyperGraphLayout.hyp[7].dn";
connectAttr "m_jump4.msg" ":hyperGraphLayout.hyp[8].dn";
connectAttr "m_jump1.msg" ":hyperGraphLayout.hyp[9].dn";
connectAttr "m_jump2.msg" ":hyperGraphLayout.hyp[10].dn";
connectAttr "m_garage2.msg" ":hyperGraphLayout.hyp[11].dn";
connectAttr "m_garage3.msg" ":hyperGraphLayout.hyp[12].dn";
connectAttr "phys_trig_car_eater.msg" ":hyperGraphLayout.hyp[13].dn";
connectAttr "phys_pos_car_start.msg" ":hyperGraphLayout.hyp[14].dn";
// End of level_05.ma
