/********************************************************************/
/*************           company: PT(Power Tech�����)***************/
/*************           sign: 37100100             *****************/
/*************           compacity: 3000            *****************/
/*************           project: 798 series        *****************/
/********************************************************************/

/* Qmax for battery  */
#define BM_798_PT_3000_Q_MAX_POS_50	3122
#define BM_798_PT_3000_Q_MAX_POS_25	3122
#define BM_798_PT_3000_Q_MAX_POS_0		3122
#define BM_798_PT_3000_Q_MAX_NEG_10	3122

#define BM_798_PT_3000_Q_MAX_POS_50_H_CURRENT	3122
#define BM_798_PT_3000_Q_MAX_POS_25_H_CURRENT	3122
#define BM_798_PT_3000_Q_MAX_POS_0_H_CURRENT	3122
#define BM_798_PT_3000_Q_MAX_NEG_10_H_CURRENT	3122


// T2 25C
BATTERY_PROFILE_STRUCT bm_798_pt_3000_battery_profile_t2[] =
{
	{0  , 4188},
	{1  , 4143},
	{2  , 4129},
	{3  , 4121},
	{4  , 4110},
	{5  , 4098},
	{6  , 4091},
	{7  , 4081},
	{8  , 4070},
	{9  , 4063},
	{10 , 4056},
	{11 , 4047},
	{12 , 4041},
	{13 , 4030},
	{14 , 4018},
	{15 , 4008},
	{16 , 3998},
	{17 , 3988},
	{18 , 3983},
	{19 , 3975},
	{20 , 3967},
	{21 , 3963},
	{22 , 3956},
	{23 , 3948},
	{24 , 3943},
	{25 , 3936},
	{26 , 3929},
	{27 , 3924},
	{28 , 3918},
	{29 , 3910},
	{30 , 3905},
	{31 , 3899},
	{32 , 3892},
	{33 , 3888},
	{34 , 3881},
	{35 , 3874},
	{36 , 3870},
	{37 , 3865},
	{38 , 3858},
	{39 , 3854},
	{40 , 3848},
	{41 , 3840},
	{42 , 3835},
	{43 , 3828},
	{44 , 3822},
	{45 , 3818},
	{46 , 3813},
	{47 , 3808},
	{48 , 3805},
	{49 , 3801},
	{50 , 3797},
	{51 , 3794},
	{52 , 3790},
	{53 , 3788},
	{54 , 3785},
	{55 , 3782},
	{56 , 3780},
	{57 , 3778},
	{58 , 3775},
	{59 , 3774},
	{60 , 3771},
	{61 , 3769},
	{62 , 3768},
	{63 , 3766},
	{64 , 3764},
	{65 , 3763},
	{66 , 3761},
	{67 , 3760},
	{68 , 3759},
	{69 , 3758},
	{70 , 3756},
	{71 , 3755},
	{72 , 3753},
	{73 , 3750},
	{74 , 3748},
	{75 , 3745},
	{76 , 3741},
	{77 , 3738},
	{78 , 3734},
	{79 , 3730},
	{80 , 3727},
	{81 , 3724},
	{82 , 3718},
	{83 , 3714},
	{84 , 3708},
	{85 , 3701},
	{86 , 3696},
	{87 , 3689},
	{88 , 3681},
	{89 , 3679},
	{90 , 3677},
	{91 , 3674},
	{92 , 3672},
	{93 , 3668},
	{94 , 3663},
	{95 , 3657},
	{96 , 3632},
	{97 , 3584},
	{98 , 3543},
	{99 , 3458},
	{100, 3331}
	
};     

// ============================================================
// <Rbat, Battery_Voltage> Table
// ============================================================
// T2 25C
R_PROFILE_STRUCT bm_798_pt_3000_r_profile_t2[] =
{
	{103, 4188},
	{103, 4143},
	{104, 4129},
	{104, 4121},
	{105, 4110},
	{104, 4098},
	{104, 4091},
	{105, 4081},
	{104, 4070},
	{104, 4063},
	{104, 4056},
	{104, 4047},
	{105, 4041},
	{104, 4030},
	{105, 4018},
	{104, 4008},
	{104, 3998},
	{104, 3988},
	{105, 3983},
	{106, 3975},
	{105, 3967},
	{106, 3963},
	{106, 3956},
	{107, 3948},
	{106, 3943},
	{106, 3936},
	{106, 3929},
	{106, 3924},
	{106, 3918},
	{105, 3910},
	{106, 3905},
	{105, 3899},
	{105, 3892},
	{107, 3888},
	{107, 3881},
	{106, 3874},
	{107, 3870},
	{108, 3865},
	{106, 3858},
	{107, 3854},
	{107, 3848},
	{107, 3840},
	{107, 3835},
	{108, 3828},
	{108, 3822},
	{108, 3818},
	{108, 3813},
	{108, 3808},
	{108, 3805},
	{109, 3801},
	{109, 3797},
	{109, 3794},
	{109, 3790},
	{109, 3788},
	{109, 3785},
	{109, 3782},
	{108, 3780},
	{108, 3778},
	{108, 3775},
	{107, 3774},
	{107, 3771},
	{106, 3769},
	{106, 3768},
	{108, 3766},
	{108, 3764},
	{109, 3763},
	{109, 3761},
	{108, 3760},
	{108, 3759},
	{108, 3758},
	{108, 3756},
	{107, 3755},
	{108, 3753},
	{109, 3750},
	{109, 3748},
	{108, 3745},
	{108, 3741},
	{109, 3738},
	{109, 3734},
	{109, 3730},
	{109, 3727},
	{110, 3724},
	{110, 3718},
	{110, 3714},
	{110, 3708},
	{110, 3701},
	{109, 3696},
	{110, 3689},
	{110, 3681},
	{111, 3679},
	{111, 3677},
	{109, 3674},
	{109, 3672},
	{109, 3668},
	{108, 3663},
	{108, 3657},
	{109, 3632},
	{109, 3584},
	{110, 3543},
	{111, 3458},
	{112, 3331}
	
}; 

