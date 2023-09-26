// Created by Crowbar 0.72

$modelname "ppm/c_arms_pony.mdl"

$bodygroup "arms"
{
	studio "c_arms_hooves_reference.smd"
	blank
}


$surfaceprop "default"

$contents "solid"

$illumposition -0.012 -1.24 -11.394

$ambientboost

$cdmaterials "models\ppm2\base\"

$cbox 0 0 0 0 0 0

$bbox -5.061 -27.491 33.546 1.78 27.414 60.957

$definebone "ValveBiped.Bip01_Spine4" "" 0.000006 0 0 -79.749938 -89.999921 -90.000037 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Clavicle" "ValveBiped.Bip01_Spine4" 2.033348 1.000767 1.937659 -73.711859 168.619394 91.480426 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_UpperArm" "ValveBiped.Bip01_L_Clavicle" 6.028142 0.000005 0.000002 2.738796 -33.527865 -90.504986 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Forearm" "ValveBiped.Bip01_L_UpperArm" 11.692561 0.000013 -0.000003 -0.000014 -3.459 -0.000404 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Hand" "ValveBiped.Bip01_L_Forearm" 11.48168 0.000006 -0.000037 -6.108712 2.532073 90.136975 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger4" "ValveBiped.Bip01_L_Hand" 3.859705 -0.142418 -1.191975 8.682493 -50.431607 -26.821813 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger41" "ValveBiped.Bip01_L_Finger4" 1.312557 -0.000015 0.000004 -0.000176 -25.194258 0.000012 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger42" "ValveBiped.Bip01_L_Finger41" 0.729364 -0.000008 -0.000004 -0.000172 -13.997587 0.000068 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger3" "ValveBiped.Bip01_L_Hand" 3.942324 0.046752 -0.431444 4.194173 -46.46634 -8.696352 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger31" "ValveBiped.Bip01_L_Finger3" 1.539097 0.00001 -0.000006 0.000344 -14.197371 -0.000114 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger32" "ValveBiped.Bip01_L_Finger31" 1.196323 -0.000008 0.000003 -0.0004 -24.683723 0.000055 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger2" "ValveBiped.Bip01_L_Hand" 3.883688 -0.046781 0.431443 1.251056 -33.682152 4.921824 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger21" "ValveBiped.Bip01_L_Finger2" 1.719578 -0.000004 0.000009 0.315937 -20.997728 0.000116 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger22" "ValveBiped.Bip01_L_Finger21" 1.209187 -0.000008 0.000003 0.17711 -12.398451 0 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger1" "ValveBiped.Bip01_L_Hand" 3.859854 -0.137959 1.332467 -2.449451 -30.465581 20.308497 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger11" "ValveBiped.Bip01_L_Finger1" 1.719435 0.000004 -0.000007 0.233719 -20.79858 -0.000219 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger12" "ValveBiped.Bip01_L_Finger11" 1.099672 0 0.000003 0.148697 -13.999188 0.000302 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger0" "ValveBiped.Bip01_L_Hand" 0.806046 -0.348927 1.321259 -38.916232 -45.196017 -70.054634 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger01" "ValveBiped.Bip01_L_Finger0" 1.789783 0.000004 -0.000011 -0.000097 13.093467 -0.000106 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger02" "ValveBiped.Bip01_L_Finger01" 1.207008 -0.000001 -0.000002 -0.000095 20.790467 0.000108 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Ulna" "ValveBiped.Bip01_L_Forearm" 5.740852 0.000003 -0.000001 -0.000014 0.000403 0.039418 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Wrist" "ValveBiped.Bip01_L_Forearm" 11.48171 0.000006 -0.000003 -0.000015 -0.000398 0.073164 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Clavicle" "ValveBiped.Bip01_Spine4" 2.033356 1.000772 -1.93761 73.712098 168.61882 -95.596496 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_UpperArm" "ValveBiped.Bip01_R_Clavicle" 6.028142 0.000001 -0.000001 -0.461919 -33.623799 93.935175 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Forearm" "ValveBiped.Bip01_R_UpperArm" 11.692556 0.000031 -0.000001 0.000007 -3.459407 -0.000001 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Hand" "ValveBiped.Bip01_R_Forearm" 11.481707 0.000105 0.000002 6.107667 2.532588 -89.664285 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger4" "ValveBiped.Bip01_R_Hand" 3.859676 -0.132545 1.193109 -9.056629 -50.352489 26.713244 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger41" "ValveBiped.Bip01_R_Finger4" 1.312563 0 -0.000001 0.000071 -25.191455 0.000113 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger42" "ValveBiped.Bip01_R_Finger41" 0.729364 0.000002 -0.000001 0.000179 -13.995975 -0.000003 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger3" "ValveBiped.Bip01_R_Hand" 3.942295 0.050344 0.431044 -4.537251 -46.420974 8.59878 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger31" "ValveBiped.Bip01_R_Finger3" 1.539104 -0.000002 0.000002 -0.000171 -14.195983 0.000177 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger32" "ValveBiped.Bip01_R_Finger31" 1.19632 -0.000006 -0.000003 0.000061 -24.68096 -0.000169 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger2" "ValveBiped.Bip01_R_Hand" 3.883659 -0.050303 -0.431042 -1.512608 -33.672208 -4.921988 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger21" "ValveBiped.Bip01_R_Finger2" 1.719585 0.000004 0.000006 -0.456763 -20.995349 0.000113 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger22" "ValveBiped.Bip01_R_Finger21" 1.209185 0 0.000006 -0.256057 -12.397092 0.000115 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger1" "ValveBiped.Bip01_R_Hand" 3.859833 -0.148916 -1.331282 2.209956 -30.481328 -20.306934 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger11" "ValveBiped.Bip01_R_Finger1" 1.719431 0.000008 0.00001 -0.37879 -20.79662 0.000167 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger12" "ValveBiped.Bip01_R_Finger11" 1.099657 0 0.000007 -0.241342 -13.997963 0.000225 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger0" "ValveBiped.Bip01_R_Hand" 0.806023 -0.359831 -1.318334 38.596163 -45.467169 69.981415 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger01" "ValveBiped.Bip01_R_Finger0" 1.789783 -0.000008 -0.00001 -0.000045 13.095007 -0.000116 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger02" "ValveBiped.Bip01_R_Finger01" 1.207005 -0.000004 0.000002 -0.000104 20.793208 0.000235 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Ulna" "ValveBiped.Bip01_R_Forearm" 5.740852 0.000052 0.000001 0.000002 0.000399 0.004639 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Wrist" "ValveBiped.Bip01_R_Forearm" 11.481707 0.000105 0.000002 0.000002 0.0008 0.008708 0 0 0 0 0 0

$proceduralbones "c_arms_pony.vrd"

$sequence "idle" {
	"c_arms_pony_anims\idle.smd"
	fadein 0.2
	fadeout 0.2
	fps 30
}

$sectionframes 30 121

$animation "a_fists_idle_01" "c_arms_pony_anims\a_fists_idle_01.smd" {
	fps 30
	loop
}

$animation "a_fists_idle_02" "c_arms_pony_anims\a_fists_idle_02.smd" {
	fps 30
	loop
}

$sequence "reference" {
	"c_arms_pony_anims\reference.smd"
	fadein 0.2
	fadeout 0.2
	fps 1
	loop
}

$sequence "seq_admire" {
	"c_arms_pony_anims\seq_admire.smd"
	fadein 0.2
	fadeout 0.2
	fps 30
}

$sequence "fists_draw" {
	"c_arms_pony_anims\fists_draw.smd"
	activity "ACT_VM_DRAW" 1
	fadein 0.2
	fadeout 0.2
	fps 30
}

$sequence "fists_right" {
	"c_arms_pony_anims\fists_right.smd"
	activity "ACT_VM_HITCENTER" 1
	fadein 0.1
	fadeout 0.3
	fps 33
}

$sequence "fists_left" {
	"c_arms_pony_anims\fists_left.smd"
	activity "ACT_VM_HITCENTER" 1
	fadein 0.1
	fadeout 0.3
	fps 33
}

$sequence "fists_right_miss" {
	"c_arms_pony_anims\fists_right.smd"
	activity "ACT_VM_MISSCENTER" 1
	fadein 0.1
	fadeout 0.3
	fps 33
}

$sequence "fists_left_miss" {
	"c_arms_pony_anims\fists_left.smd"
	activity "ACT_VM_MISSCENTER" 1
	fadein 0.1
	fadeout 0.3
	fps 33
}

$sequence "fists_uppercut" {
	"c_arms_pony_anims\fists_uppercut.smd"
	activity "ACT_VM_HITKILL" 1
	fadein 0.1
	fadeout 0.3
	fps 33
}

$sequence "fists_holster" {
	"c_arms_pony_anims\fists_holster.smd"
	activity "ACT_VM_HOLSTER" 1
	fadein 0.2
	fadeout 0.2
	fps 26
}

$sequence "fists_idle_01" {
	"a_fists_idle_01"
	activity "ACT_VM_IDLE" 1
	fadein 0.2
	fadeout 0.2
}

$sequence "fists_idle_02" {
	"a_fists_idle_02"
	activity "ACT_VM_IDLE" 1
	fadein 0.2
	fadeout 0.2
}
