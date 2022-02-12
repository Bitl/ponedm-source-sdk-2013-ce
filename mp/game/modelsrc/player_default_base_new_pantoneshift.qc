// Created by Crowbar 0.71

$modelname "ppm/player_default_base_new_pantoneshift.mdl"

$model "model" "ref_body_01_new_skeleton.smd" {

	eyeball "eye_right" "LrigScull" -2.999986 -13.999986 41.000068 "eye_r" 8 13 "iris_unused" 8
	eyeball "eye_left" "LrigScull" 3.000007 -13.999982 41.000068 "eye_l" 8 -13 "iris_unused" 8


	flexfile "player_default_base_new_pantoneshift_01.vta" 
	{
		defaultflex frame 0
		flex "JawOpen" frame 1
		flex "JawClose" frame 2
		flex "Pucker" frame 3
		flex "Grin" frame 4
		flex "Tongue_Out" frame 5
		flex "Tongue_Up" frame 6
		flex "Tongue_Down" frame 7
		flex "Male" frame 8
		flex "angry_eyes" frame 9
		flex "sad_eyes" frame 10
		flex "Male_2" frame 11
		flex "Duck" frame 12
		flex "FatButt" frame 13
		flex "Buff_Body" frame 14
		flex "Fatbutt2" frame 15
		flex "Smirk" frame 16
		flex "Frown" frame 17
		flex "Stretch" frame 18
		flex "CatFace" frame 19
		flex "Mouth_O" frame 20
		flex "Mouth_O2" frame 21
		flex "Mouth_Full" frame 22
		flex "Eyes_Blink" frame 23
		flex "Left_Blink" frame 24
		flex "Right_Blink" frame 25
		flex "Scrunch" frame 26
		flex "Stomach_Out" frame 27
		flex "Stomach_In" frame 28
		flex "Throat_Bulge" frame 29
		flex "Hoof_Fluffers" frame 30
		flex "o3o" frame 31
		flex "Ear_Fluffers" frame 32
		flex "Fang_Test" frame 33
		flex "Eyes_Blink_Lower" frame 34
		flex "Manliest_Chin" frame 35
		flex "Lowerlid_Raise" frame 36
		flex "Happy_Eyes" frame 37
		flex "NoEyelashes" frame 38
		flex "Fangs" frame 39
		flex "Claw_Teeth" frame 40
	}

	flexcontroller eyes range -45 45 "eyes_updown"
	flexcontroller eyes range -45 45 "eyes_rightleft"
	flexcontroller phoneme range 0 1 "JawOpen"
	flexcontroller phoneme range 0 1 "JawClose"
	flexcontroller phoneme range 0 1 "Smirk"
	flexcontroller phoneme range 0 1 "Frown"
	flexcontroller phoneme range 0 1 "Stretch"
	flexcontroller phoneme range 0 1 "Pucker"
	flexcontroller phoneme range 0 1 "Grin"
	flexcontroller phoneme range 0 1 "CatFace"
	flexcontroller phoneme range 0 1 "Mouth_O"
	flexcontroller phoneme range 0 1 "Mouth_O2"
	flexcontroller phoneme range 0 1 "Mouth_Full"
	flexcontroller phoneme range 0 1 "Tongue_Out"
	flexcontroller phoneme range 0 1 "Tongue_Up"
	flexcontroller phoneme range 0 1 "Tongue_Down"
	flexcontroller phoneme range 0 1 "NoEyelashes"
	flexcontroller phoneme range 0 1 "Eyes_Blink"
	flexcontroller phoneme range 0 1 "Left_Blink"
	flexcontroller phoneme range 0 1 "Right_Blink"
	flexcontroller phoneme range 0 1 "Scrunch"
	flexcontroller phoneme range 0 1 "FatButt"
	flexcontroller phoneme range 0 1 "Stomach_Out"
	flexcontroller phoneme range 0 1 "Stomach_In"
	flexcontroller phoneme range 0 1 "Throat_Bulge"
	flexcontroller phoneme range 0 1 "Male"
	flexcontroller phoneme range 0 1 "Hoof_Fluffers"
	flexcontroller phoneme range 0 1 "o3o"
	flexcontroller phoneme range 0 1 "Ear_Fluffers"
	flexcontroller phoneme range 0 1 "Fangs"
	flexcontroller phoneme range 0 1 "Claw_Teeth"
	flexcontroller phoneme range 0 1 "Fang_Test"
	flexcontroller phoneme range 0 1 "angry_eyes"
	flexcontroller phoneme range 0 1 "sad_eyes"
	flexcontroller phoneme range 0 1 "Eyes_Blink_Lower"
	flexcontroller phoneme range 0 1 "Male_2"
	flexcontroller phoneme range 0 1 "Buff_Body"
	flexcontroller phoneme range 0 1 "Manliest_Chin"
	flexcontroller phoneme range 0 1 "Lowerlid_Raise"
	flexcontroller phoneme range 0 1 "Happy_Eyes"
	flexcontroller phoneme range 0 1 "Duck"
	flexcontroller phoneme range 0 1 "Fatbutt2"

	%JawOpen = JawOpen
	%JawClose = JawClose
	%Smirk = Smirk
	%Frown = Frown
	%Stretch = Stretch
	%Pucker = Pucker
	%Grin = Grin
	%CatFace = CatFace
	%Mouth_O = Mouth_O
	%Mouth_O2 = Mouth_O2
	%Mouth_Full = Mouth_Full
	%Tongue_Out = Tongue_Out
	%Tongue_Up = Tongue_Up
	%Tongue_Down = Tongue_Down
	%NoEyelashes = NoEyelashes
	%Eyes_Blink = Eyes_Blink
	%Left_Blink = Left_Blink
	%Right_Blink = Right_Blink
	%Scrunch = Scrunch
	%FatButt = FatButt
	%Stomach_Out = Stomach_Out
	%Stomach_In = Stomach_In
	%Throat_Bulge = Throat_Bulge
	%Male = Male
	%Hoof_Fluffers = Hoof_Fluffers
	%o3o = o3o
	%Ear_Fluffers = Ear_Fluffers
	%Fangs = Fangs
	%Claw_Teeth = Claw_Teeth
	%Fang_Test = Fang_Test
	%angry_eyes = angry_eyes
	%sad_eyes = sad_eyes
	%Eyes_Blink_Lower = Eyes_Blink_Lower
	%Male_2 = Male_2
	%Buff_Body = Buff_Body
	%Manliest_Chin = Manliest_Chin
	%Lowerlid_Raise = Lowerlid_Raise
	%Happy_Eyes = Happy_Eyes
	%Duck = Duck
	%Fatbutt2 = Fatbutt2
}
$bodygroup "UpperMane"
{
	studio "uppermane_01.smd"
	studio "uppermane_02.smd"
	studio "uppermane_03.smd"
	studio "uppermane_04.smd"
	studio "uppermane_05.smd"
	studio "uppermane_06.smd"
	studio "uppermane_07.smd"
	studio "uppermane_08.smd"
	studio "uppermane_09.smd"
	studio "uppermane_10.smd"
	studio "uppermane_11.smd"
	studio "uppermane_12.smd"
	studio "uppermane_13.smd"
	studio "uppermane_14.smd"
	studio "uppermane_15.smd"
	studio "Mane_timid.smd"
	studio "mane_filly.smd"
	studio "mane_mechanic.smd"
	studio "mane_moon.smd"
	studio "mane_cloud_upper.smd"
	studio "mane_drunk.smd"
	studio "mane_emo.smd"
	blank
}
$bodygroup "LowerMane"
{
	studio "lowermane_01.smd"
	studio "lowermane_02.smd"
	studio "lowermane_03.smd"
	studio "lowermane_04.smd"
	studio "lowermane_05.smd"
	studio "lowermane_06.smd"
	studio "lowermane_07.smd"
	studio "lowermane_08.smd"
	studio "lowermane_09.smd"
	studio "lowermane_10.smd"
	studio "lowermane_11.smd"
	studio "lowermane_12.smd"
	studio "Mane_timid_Lower.smd"
	studio "mane_moon_lower.smd"
	studio "mane_bun_lower.smd"
	studio "mane_cloud_lower.smd"
	studio "mane_emo_lower.smd"
	blank
}
$bodygroup "Tail"
{
	studio "tail_01.smd"
	studio "tail_02.smd"
	studio "tail_03.smd"
	studio "tail_04.smd"
	studio "tail_05.smd"
	studio "tail_06.smd"
	studio "tail_07.smd"
	studio "tail_08.smd"
	studio "tail_09.smd"
	studio "tail_10.smd"
	studio "tail_11.smd"
	studio "tail_12.smd"
	studio "tail_13.smd"
	studio "tail_14.smd"
	blank
}
$bodygroup "Horn"
{
	blank
	studio "ref_horn_01.smd"
}
$bodygroup "Wings"
{
	blank
	studio "ref_wings_folded_hr.smd"
	studio "wing_bat_closed_hr.smd"
}

$surfaceprop "flesh"

$contents "solid"

$illumposition -0.001 3 26.503

$eyeposition 0 0 70

$mostlyopaque

$cdmaterials "models\ppm2\base\"

$texturegroup "skinfamilies"
{
	{ "body"       "eye_r"       "eye_l"       "hair_color_1"       "hair_color_2"       "tail_color_1"       "tail_color_2" 	"horn" 	"wings" "wing_bat_skin" 	"wing_bat"}
	{ "body"       "eye_r_dead"  "eye_l_dead"  "hair_color_1"       "hair_color_2"       "tail_color_1"       "tail_color_2" 	"horn" 	"wings" "wing_bat_skin" 	"wing_bat"}
	{ "body"       "eye_r_dazed" "eye_l_dazed" "hair_color_1"       "hair_color_2"       "tail_color_1"       "tail_color_2" 	"horn" 	"wings" "wing_bat_skin" 	"wing_bat"}
	{ "body_local" "eye_r"       "eye_l"       "hair_color_1_local" "hair_color_2_local" "tail_color_1_local" "tail_color_2_local" "horn_local"  "wings_local" "wing_bat_skin_local"	"wing_bat_local"}
}

$attachment "forward" "LrigScull" 0 0 0 rotate 0 -10 90
$attachment "anim_attachment_RH" "Lrigweaponbone" 0 0 0 rotate 0 90 0
$attachment "anim_attachment_LH" "Lrigweaponbone" 0 0 0 rotate 0 90 0
$attachment "eyes" "LrigScull" 6.3 0 0 rotate 0 -10 90
$attachment "righteye" "LrigScull" 5 0 -4 rotate 0 -10 90
$attachment "lefteye" "LrigScull" 5 0 4 rotate 0 -10 90
$attachment "weapon_bone" "Lrigweaponbone" 0 0 0 rotate 0 90 0
$attachment "righthand" "right_hand" 0 0 0 rotate 0 0 0
$attachment "physgun_attachment" "right_hand" 0 0 0 rotate 0 0 0
$attachment "lefthand" "right_hand" 0 0 0 rotate 0 0 0
$attachment "Mane01" "Mane01" 0 0 0 rotate 0 0 0
$attachment "Mane02" "Mane02" 0 0 0 rotate 0 0 0
$attachment "Mane03" "Mane03" 0 0 0 rotate 0 0 0
$attachment "Mane03_tip" "Mane03_tip" 0 0 0 rotate 0 0 0
$attachment "Mane04" "Mane04" 0 0 0 rotate 0 0 0
$attachment "Mane05" "Mane05" 0 0 0 rotate 0 0 0
$attachment "Mane06" "Mane06" 0 0 0 rotate 0 0 0
$attachment "Mane07" "Mane07" 0 0 0 rotate 0 0 0
$attachment "Tail01" "Tail01" 0 0 0 rotate 0 0 0
$attachment "Tail02" "Tail02" 0 0 0 rotate 0 0 0
$attachment "Tail03" "Tail03" 0 0 0 rotate 0 0 0
$attachment "head" "LrigScull" -3 -12 0 rotate 0 -10 90

$cbox 0 0 0 0 0 0

$bbox -29.793 -26.047 -0.802 23.794 26.045 53.808

$hboxset "default"
$hbox 3 "LrigPelvis" -8.94 -2.93 -6.62 4.44 9.87 6.62
$hbox 6 "Lrig_LEG_BL_Femur" -5.53 -5.31 -2.63 11.88 6.24 4.26
$hbox 6 "Lrig_LEG_BL_Tibia" -3.26 -3.61 -2.04 7.95 2.25 2.67
$hbox 6 "Lrig_LEG_BL_LargeCannon" -4.45 -2.55 -2.31 12.34 3.19 3.02
$hbox 6 "Lrig_LEG_BL_RearHoof" -3.46 -4.88 -3.2 2.91 4.29 3.48
$hbox 7 "Lrig_LEG_BR_Femur" -5.51 -5.3 -4.34 11.88 6.25 2.63
$hbox 7 "Lrig_LEG_BR_Tibia" -3.26 -3.61 -2.67 7.95 2.25 2.04
$hbox 7 "Lrig_LEG_BR_LargeCannon" -4.45 -2.55 -3.02 12.34 3.19 2.31
$hbox 7 "Lrig_LEG_BR_RearHoof" -3.46 -4.88 -3.49 2.91 4.29 3.2
$hbox 3 "LrigSpine1" 0 -3.16 -4.68 7.88 3.2 4.68
$hbox 2 "LrigSpine2" -0.99 -2.95 -2.68 5.38 0.33 2.68
$hbox 2 "Lrig_LEG_FL_Scapula" -3.22 -4.51 -2.6 12.59 3.6 2.48
$hbox 4 "Lrig_LEG_FL_Humerus" -0.83 -3.57 -2.62 7.8 2.96 3.03
$hbox 4 "Lrig_LEG_FL_Radius" -2.56 -3 -2.21 6.58 2.6 2.6
$hbox 4 "Lrig_LEG_FL_Metacarpus" 0 -3.75 -2.32 7.26 3.46 3.26
$hbox 4 "Lrig_LEG_FL_FrontHoof" -5.84 -5.24 -3.06 2.19 3.99 3.34
$hbox 2 "Lrig_LEG_FR_Scapula" -3.22 -4.51 -2.48 12.59 3.6 2.6
$hbox 5 "Lrig_LEG_FR_Humerus" -0.83 -3.57 -3.03 7.8 2.96 2.62
$hbox 5 "Lrig_LEG_FR_Radius" -2.56 -3 -2.44 6.58 2.6 2.21
$hbox 5 "Lrig_LEG_FR_Metacarpus" 0 -3.75 -3.26 7.26 3.46 2.32
$hbox 5 "Lrig_LEG_FR_FrontHoof" -5.84 -5.24 -3.34 2.19 3.99 3.06
$hbox 1 "LrigNeck1" -1.27 -1.2 -3.47 3.95 3.54 3.44
$hbox 1 "LrigNeck2" -6.49 -5.13 -3.52 3.86 2.97 3.38
$hbox 1 "LrigScull" -9.38 -11.29 -7.63 8.51 5.35 7.78

$definebone "LrigPelvis" "" -0.000003 3.335794 28.155224 2.120173 -89.999921 -89.999982 0 0 0 0 0 0
$definebone "LrigSpine1" "LrigPelvis" 0 -0.000004 0 0 -8.580847 0 0 0 0 0 0 0
$definebone "LrigSpine2" "LrigSpine1" 5.375031 -0.000006 -0.000009 0 -4.606979 0 0 0 0 0 0 0
$definebone "LrigRibcage" "LrigSpine2" 5.375031 -0.000015 -0.00001 0 11.432345 0 0 0 0 0 0 0
$definebone "LrigNeck1" "LrigRibcage" 0 -0.000002 0 0.537606 -60.235165 -0.269863 0 0 0 0 0 0
$definebone "LrigNeck2" "LrigNeck1" 3.952618 0.00005 -0.000003 0.798187 -30.647715 -0.061478 0 0 0 0 0 0
$definebone "LrigNeck3" "LrigNeck2" 3.861511 -0.000023 0.005197 0.097517 54.494818 -0.736996 0 0 0 0 0 0
$definebone "LrigScull" "LrigNeck3" 3.510599 0.000032 -0.000001 -0.077178 44.320333 -0.79389 0 0 0 0 0 0
$definebone "right_hand" "LrigScull" -0.002691 2.972797 -8.301614 89.101928 3.578868 101.888152 0 0 0 0 0 0
$definebone "Lrigweaponbone" "LrigScull" 3.364572 3.21003 -0.005423 0.840008 -0.987878 0.840243 0 0 0 0 0 0
$definebone "Lrig_LEG_FR_Scapula" "LrigRibcage" -1.359049 -0.502213 -2.742348 0.021887 88.502326 163.06115 0 0 0 0 0 0
$definebone "Lrig_LEG_FR_Humerus" "Lrig_LEG_FR_Scapula" 8.455881 0.000004 -0.000031 9.808757 -31.642361 8.818771 0 0 0 0 0 0
$definebone "Lrig_LEG_FR_Radius" "Lrig_LEG_FR_Humerus" 5.427908 -0.000021 -0.000017 -8.703748 36.952403 0.990666 0 0 0 0 0 0
$definebone "Lrig_LEG_FR_Metacarpus" "Lrig_LEG_FR_Radius" 6.225872 -0.000021 0.000374 -1.297176 -3.861144 1.146333 0 0 0 0 0 0
$definebone "Lrig_LEG_FR_PhalangesManus" "Lrig_LEG_FR_Metacarpus" 6.04926 0.000005 -0.000092 5.194418 23.317885 3.546048 0 0 0 0 0 0
$definebone "Lrig_LEG_FR_FrontHoof" "Lrig_LEG_FR_PhalangesManus" 3.457453 -0.323213 0.030643 0.626236 -27.173769 1.511649 0 0 0 0 0 0
$definebone "Lrig_LEG_FL_Scapula" "LrigRibcage" -1.359043 -0.502218 2.742352 -0.021887 88.502326 -163.060986 0 0 0 0 0 0
$definebone "Lrig_LEG_FL_Humerus" "Lrig_LEG_FL_Scapula" 8.455883 0.000006 0.000034 -9.792884 -31.649334 -8.825987 0 0 0 0 0 0
$definebone "Lrig_LEG_FL_Radius" "Lrig_LEG_FL_Humerus" 5.427906 -0.000062 -0.000002 8.684601 36.956706 -0.990091 0 0 0 0 0 0
$definebone "Lrig_LEG_FL_Metacarpus" "Lrig_LEG_FL_Radius" 6.225866 0.000041 0.000461 1.290869 -3.87663 -1.147766 0 0 0 0 0 0
$definebone "Lrig_LEG_FL_PhalangesManus" "Lrig_LEG_FL_Metacarpus" 6.049268 -0.000016 0.000836 -5.172253 23.364072 -3.548975 0 0 0 0 0 0
$definebone "Lrig_LEG_FL_FrontHoof" "Lrig_LEG_FL_PhalangesManus" 3.457455 -0.323214 -0.030612 -0.635696 -27.205642 -1.505689 0 0 0 0 0 0
$definebone "Jaw" "LrigScull" 5.364578 3.210033 -0.005423 0.840013 -89.98794 0.840243 0 0 0 0 0 0
$definebone "Ear_L" "LrigScull" -7.416153 -5.091698 6.119503 -83.093554 35.017338 126.349347 0 0 0 0 0 0
$definebone "Ear_R" "LrigScull" -7.226642 -5.079868 -5.994822 -81.509483 -151.281314 -47.294962 0 0 0 0 0 0
$definebone "Lrig_LEG_BR_Femur" "LrigPelvis" -3.647326 2.528301 -3.881336 3.104286 90.225153 -178.719003 0 0 0 0 0 0
$definebone "Lrig_LEG_BR_Tibia" "Lrig_LEG_BR_Femur" 8.783319 -0.000051 -0.001179 13.032493 -61.431562 -0.9047 0 0 0 0 0 0
$definebone "Lrig_LEG_BR_LargeCannon" "Lrig_LEG_BR_Tibia" 4.7637 -0.00001 0.000001 -12.395475 52.403547 -8.258341 0 0 0 0 0 0
$definebone "Lrig_LEG_BR_PhalanxPrima" "Lrig_LEG_BR_LargeCannon" 9.830597 -0.000036 -0.000093 1.243671 42.536728 11.693268 0 0 0 0 0 0
$definebone "Lrig_LEG_BR_RearHoof" "Lrig_LEG_BR_PhalanxPrima" 3.575821 0.247356 0.022604 0.010828 -26.953159 -0.005399 0 0 0 0 0 0
$definebone "Lrig_LEG_BL_Femur" "LrigPelvis" -3.647312 2.52832 3.881349 -3.109843 90.226813 178.723183 0 0 0 0 0 0
$definebone "Lrig_LEG_BL_Tibia" "Lrig_LEG_BL_Femur" 8.783323 0.00001 -0.000168 -13.052089 -61.429956 0.905618 0 0 0 0 0 0
$definebone "Lrig_LEG_BL_LargeCannon" "Lrig_LEG_BL_Tibia" 4.763699 0 0.000001 12.420516 52.400515 8.270361 0 0 0 0 0 0
$definebone "Lrig_LEG_BL_PhalanxPrima" "Lrig_LEG_BL_LargeCannon" 9.830591 0.000051 -0.000103 -1.313847 42.549121 -11.706092 0 0 0 0 0 0
$definebone "Lrig_LEG_BL_RearHoof" "Lrig_LEG_BL_PhalanxPrima" 3.575816 0.247352 -0.0226 0.040619 -26.946114 -0.020684 0 0 0 0 0 0
$definebone "Mane02" "LrigScull" -7.407091 -3.369225 0.062676 -19.706281 85.83634 -0.944292 0 0 0 0 0 0
$definebone "Mane03" "Mane02" 8.437847 0.000005 -0.000002 0.000041 -16.067914 -0.000276 0 0 0 0 0 0
$definebone "Mane03_tip" "Mane03" 7.714621 0.000068 -0.00001 19.419775 8.37497 -2.571302 0 0 0 0 0 0
$definebone "Mane04" "LrigScull" -6.087363 -9.362785 0.077451 27.537911 -11.500552 0.265624 0 0 0 0 0 0
$definebone "Mane05" "Mane04" 10.28127 -0.000011 0.00001 -0.000326 79.629487 -0.000056 0 0 0 0 0 0
$definebone "Mane06" "LrigScull" -2.003916 -8.936123 -5.835894 13.210084 85.320237 -0.913295 0 0 0 0 0 0
$definebone "Mane07" "Mane06" 9.869663 -0.000042 0.000011 -14.755662 -14.405129 2.7161 0 0 0 0 0 0
$definebone "Mane01" "LrigScull" -1.877639 -10.274139 0.142414 -0.893757 9.181554 -0.087605 0 0 0 0 0 0
$definebone "Tail01" "LrigPelvis" -6.846875 -0.42474 0.000011 -0.000115 -173.438447 0 0 0 0 0 0 0
$definebone "Tail02" "Tail01" 10.804911 -0.000118 -0.000001 0 -91.836026 0 0 0 0 0 0 0
$definebone "Tail03" "Tail02" 16.832985 -0.000084 -0.000006 0 -0.001203 0 0 0 0 0 0 0

$jigglebone "Mane02"
{
	is_flexible
	{
		length 20
		tip_mass 5
		pitch_stiffness 80
		pitch_damping 9
		yaw_stiffness 80
		yaw_damping 9
		along_stiffness 100
		along_damping 0
		angle_constraint 27.000001
	}
}
$jigglebone "Mane03"
{
	is_flexible
	{
		length 20
		tip_mass 5
		pitch_stiffness 80
		pitch_damping 9
		yaw_stiffness 80
		yaw_damping 5
		along_stiffness 100
		along_damping 0
		angle_constraint 39.999999
	}
}
$jigglebone "Mane03_tip"
{
	is_flexible
	{
		length 20
		tip_mass 5
		pitch_stiffness 80
		pitch_damping 9
		yaw_stiffness 80
		yaw_damping 5
		along_stiffness 100
		along_damping 0
		angle_constraint 39.999999
	}
}
$jigglebone "Mane04"
{
	is_flexible
	{
		length 20
		tip_mass 5
		pitch_stiffness 80
		pitch_damping 9
		yaw_stiffness 80
		yaw_damping 9
		along_stiffness 100
		along_damping 0
		angle_constraint 27.000001
	}
}
$jigglebone "Mane05"
{
	is_flexible
	{
		length 20
		tip_mass 5
		pitch_stiffness 80
		pitch_damping 9
		yaw_stiffness 80
		yaw_damping 5
		along_stiffness 100
		along_damping 0
		angle_constraint 39.999999
	}
}
$jigglebone "Mane06"
{
	is_flexible
	{
		length 20
		tip_mass 5
		pitch_stiffness 80
		pitch_damping 9
		yaw_stiffness 80
		yaw_damping 9
		along_stiffness 100
		along_damping 0
		angle_constraint 27.000001
	}
}
$jigglebone "Mane07"
{
	is_flexible
	{
		length 20
		tip_mass 5
		pitch_stiffness 80
		pitch_damping 9
		yaw_stiffness 80
		yaw_damping 5
		along_stiffness 100
		along_damping 0
		angle_constraint 39.999999
	}
}
$jigglebone "Mane01"
{
	is_flexible
	{
		length 20
		tip_mass 5
		pitch_stiffness 80
		pitch_damping 9
		yaw_stiffness 80
		yaw_damping 9
		along_stiffness 100
		along_damping 0
		angle_constraint 27.000001
	}
}
$jigglebone "Tail01"
{
	is_flexible
	{
		length 20
		tip_mass 5
		pitch_stiffness 80
		pitch_damping 9
		yaw_stiffness 80
		yaw_damping 9
		along_stiffness 100
		along_damping 0
		angle_constraint 27.000001
	}
}
$jigglebone "Tail02"
{
	is_flexible
	{
		length 20
		tip_mass 5
		pitch_stiffness 80
		pitch_damping 9
		yaw_stiffness 80
		yaw_damping 5
		along_stiffness 100
		along_damping 0
		angle_constraint 39.999999
	}
}
$jigglebone "Tail03"
{
	is_flexible
	{
		length 20
		tip_mass 5
		pitch_stiffness 80
		pitch_damping 9
		yaw_stiffness 80
		yaw_damping 9
		along_stiffness 100
		along_damping 0
		angle_constraint 30.000001
	}
}

$sequence "ragdoll" {
	"player_default_base_new_pantoneshift_anims\ragdoll.smd"
	activity "ACT_DIERAGDOLL" 1
	fadein 0.2
	fadeout 0.2
	fps 30
}

$includemodel "ppm/pony_anims_rev.mdl"

$collisionjoints "player_default_base_new_pantoneshift_physics.smd"
{
	$mass 2
	$inertia 2
	$damping 0.01
	$rotdamping 0.4
	$rootbone " "
	$concave
	$maxconvexpieces 1


	$jointconstrain "Lrig_LEG_BL_Femur" x limit 0 0 0.000064
	$jointconstrain "Lrig_LEG_BL_Femur" y limit -5 5 0.000064
	$jointconstrain "Lrig_LEG_BL_Femur" z limit -70 60 0.000064

	$jointconstrain "Lrig_LEG_BL_LargeCannon" x limit 0 0 0.000064
	$jointconstrain "Lrig_LEG_BL_LargeCannon" y limit 0 0 0.000064
	$jointconstrain "Lrig_LEG_BL_LargeCannon" z limit -40 90 0.000064

	$jointconstrain "Lrig_LEG_BR_Femur" x limit 0 0 0.000064
	$jointconstrain "Lrig_LEG_BR_Femur" y limit -5 5 0.000064
	$jointconstrain "Lrig_LEG_BR_Femur" z limit -70 60 0.000064

	$jointconstrain "Lrig_LEG_BR_LargeCannon" x limit 0 0 0.000064
	$jointconstrain "Lrig_LEG_BR_LargeCannon" y limit 0 0 0.000064
	$jointconstrain "Lrig_LEG_BR_LargeCannon" z limit -40 90 0.000064

	$jointconstrain "Lrig_LEG_BR_RearHoof" x limit 0 0 0.000064
	$jointconstrain "Lrig_LEG_BR_RearHoof" y limit -5 -5 0.000064
	$jointconstrain "Lrig_LEG_BR_RearHoof" z limit -20 20 0.000064

	$jointconstrain "LrigRibcage" x limit 0 0 0
	$jointconstrain "LrigRibcage" y limit 0 0 0
	$jointconstrain "LrigRibcage" z limit 0 0 0

	$jointconstrain "Lrig_LEG_FL_Radius" x limit 0 0 0.000064
	$jointconstrain "Lrig_LEG_FL_Radius" y limit -20 40 0.000064
	$jointconstrain "Lrig_LEG_FL_Radius" z limit -30 70 0.000064

	$jointconstrain "Lrig_LEG_FL_Metacarpus" x limit 0 0 0.000064
	$jointconstrain "Lrig_LEG_FL_Metacarpus" y limit 0 0 0.000064
	$jointconstrain "Lrig_LEG_FL_Metacarpus" z limit -90 0 0.000064

	$jointconstrain "Lrig_LEG_FR_Radius" x limit 0 0 0.000064
	$jointconstrain "Lrig_LEG_FR_Radius" y limit -40 20 0.000064
	$jointconstrain "Lrig_LEG_FR_Radius" z limit -30 70 0.000064

	$jointconstrain "Lrig_LEG_FR_Metacarpus" x limit 0 0 0.000064
	$jointconstrain "Lrig_LEG_FR_Metacarpus" y limit 0 0 0.000064
	$jointconstrain "Lrig_LEG_FR_Metacarpus" z limit -90 0 0.000064

	$jointconstrain "Lrig_LEG_FR_FrontHoof" x limit 0 0 0.000064
	$jointconstrain "Lrig_LEG_FR_FrontHoof" y limit -5 -5 0.000064
	$jointconstrain "Lrig_LEG_FR_FrontHoof" z limit -20 20 0.000064

	$jointconstrain "LrigNeck2" x limit -20 20 0.000064
	$jointconstrain "LrigNeck2" y limit -20 20 0.000064
	$jointconstrain "LrigNeck2" z limit -20 20 0.000064

	$jointconstrain "LrigScull" x limit -20 20 0.000064
	$jointconstrain "LrigScull" y limit -20 20 0.000064
	$jointconstrain "LrigScull" z limit -20 20 0.000064

	$jointconstrain "Lrig_LEG_FL_FrontHoof" x limit 0 0 0.000064
	$jointconstrain "Lrig_LEG_FL_FrontHoof" y limit -5 -5 0.000064
	$jointconstrain "Lrig_LEG_FL_FrontHoof" z limit -20 20 0.000064

	$jointconstrain "Lrig_LEG_BL_RearHoof" x limit 0 0 0.000064
	$jointconstrain "Lrig_LEG_BL_RearHoof" y limit -5 -5 0.000064
	$jointconstrain "Lrig_LEG_BL_RearHoof" z limit -20 20 0.000064
}
