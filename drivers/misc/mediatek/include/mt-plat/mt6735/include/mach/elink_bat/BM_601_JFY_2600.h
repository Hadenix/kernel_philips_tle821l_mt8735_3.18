/********************************************************************/
/*************           company: JFY               *****************/
/*************           sign:                      *****************/
/*************           compacity: 2600            *****************/
/*************           project: 601 series        *****************/
/********************************************************************/

/* Qmax for battery  */
#define BM_601_JFY_2600_Q_MAX_POS_50	2489
#define BM_601_JFY_2600_Q_MAX_POS_25	2489
#define BM_601_JFY_2600_Q_MAX_POS_0		2489
#define BM_601_JFY_2600_Q_MAX_NEG_10	2489

#define BM_601_JFY_2600_Q_MAX_POS_50_H_CURRENT	2459
#define BM_601_JFY_2600_Q_MAX_POS_25_H_CURRENT	2459
#define BM_601_JFY_2600_Q_MAX_POS_0_H_CURRENT	2459
#define BM_601_JFY_2600_Q_MAX_NEG_10_H_CURRENT	2459


// T2 25C
BATTERY_PROFILE_STRUCT bm_601_jfy_2600_battery_profile_t2[] =
{
	{0  , 4163},
	{1  , 4121},
	{2  , 4104},
	{3  , 4097},
	{4  , 4089},
	{5  , 4076},
	{6  , 4064},
	{7  , 4051},
	{8  , 4039},
	{9  , 4033},
	{10 , 4027},
	{11 , 4016},
	{12 , 4005},
	{13 , 3993},
	{14 , 3988},
	{15 , 3982},
	{16 , 3972},
	{17 , 3961},
	{18 , 3951},
	{19 , 3940},
	{20 , 3936},
	{21 , 3931},
	{22 , 3921},
	{23 , 3911},
	{24 , 3902},
	{25 , 3892},
	{26 , 3888},
	{27 , 3883},
	{28 , 3874},
	{29 , 3865},
	{30 , 3856},
	{31 , 3848},
	{32 , 3844},
	{33 , 3840},
	{34 , 3832},
	{35 , 3824},
	{36 , 3817},
	{37 , 3809},
	{38 , 3806},
	{39 , 3802},
	{40 , 3796},
	{41 , 3789},
	{42 , 3782},
	{43 , 3777},
	{44 , 3774},
	{45 , 3770},
	{46 , 3765},
	{47 , 3760},
	{48 , 3755},
	{49 , 3750},
	{50 , 3748},
	{51 , 3746},
	{52 , 3742},
	{53 , 3738},
	{54 , 3735},
	{55 , 3731},
	{56 , 3730},
	{57 , 3728},
	{58 , 3725},
	{59 , 3723},
	{60 , 3721},
	{61 , 3718},
	{62 , 3717},
	{63 , 3716},
	{64 , 3714},
	{65 , 3712},
	{66 , 3710},
	{67 , 3709},
	{68 , 3707},
	{69 , 3705},
	{70 , 3702},
	{71 , 3700},
	{72 , 3696},
	{73 , 3695},
	{74 , 3693},
	{75 , 3689},
	{76 , 3685},
	{77 , 3680},
	{78 , 3675},
	{79 , 3672},
	{80 , 3669},
	{81 , 3661},
	{82 , 3654},
	{83 , 3645},
	{84 , 3634},
	{85 , 3628},
	{86 , 3622},
	{87 , 3609},
	{88 , 3595},
	{89 , 3580},
	{90 , 3564},
	{91 , 3556},
	{92 , 3547},
	{93 , 3531},
	{94 , 3516},
	{95 , 3501},
	{96 , 3486},
	{97 , 3478},
	{98 , 3470},
	{99 , 3452},
	{100, 3430}
};     

// ============================================================
// <Rbat, Battery_Voltage> Table
// ============================================================
// T2 25C
R_PROFILE_STRUCT bm_601_jfy_2600_r_profile_t2[] =
{
	{65 , 4163},
	{65 , 4121},
	{65 , 4104},
	{65 , 4097},
	{65 , 4089},
	{65 , 4076},
	{65 , 4064},
	{65 , 4051},
	{66 , 4039},
	{66 , 4033},
	{66 , 4027},
	{66 , 4016},
	{66 , 4005},
	{66 , 3993},
	{66 , 3988},
	{66 , 3982},
	{66 , 3972},
	{66 , 3961},
	{66 , 3951},
	{66 , 3940},
	{66 , 3936},
	{66 , 3931},
	{66 , 3921},
	{66 , 3911},
	{66 , 3902},
	{67 , 3892},
	{67 , 3888},
	{67 , 3883},
	{67 , 3874},
	{67 , 3865},
	{67 , 3856},
	{67 , 3848},
	{67 , 3844},
	{67 , 3840},
	{67 , 3832},
	{67 , 3824},
	{67 , 3817},
	{67 , 3809},
	{67 , 3806},
	{67 , 3802},
	{67 , 3796},
	{67 , 3789},
	{68 , 3782},
	{68 , 3777},
	{68 , 3774},
	{68 , 3770},
	{68 , 3765},
	{68 , 3760},
	{68 , 3755},
	{68 , 3750},
	{68 , 3748},
	{68 , 3746},
	{68 , 3742},
	{68 , 3738},
	{68 , 3735},
	{68 , 3731},
	{68 , 3730},
	{68 , 3728},
	{68 , 3725},
	{69 , 3723},
	{69 , 3721},
	{69 , 3718},
	{69 , 3717},
	{69 , 3716},
	{69 , 3714},
	{69 , 3712},
	{69 , 3710},
	{69 , 3709},
	{69 , 3707},
	{69 , 3705},
	{69 , 3702},
	{69 , 3700},
	{69 , 3696},
	{69 , 3695},
	{69 , 3693},
	{69 , 3689},
	{70 , 3685},
	{70 , 3680},
	{70 , 3675},
	{70 , 3672},
	{70 , 3669},
	{70 , 3661},
	{70 , 3654},
	{70 , 3645},
	{70 , 3634},
	{70 , 3628},
	{70 , 3622},
	{70 , 3609},
	{70 , 3595},
	{70 , 3580},
	{70 , 3564},
	{70 , 3556},
	{70 , 3547},
	{71 , 3531},
	{71 , 3516},
	{71 , 3501},
	{71 , 3486},
	{71 , 3478},
	{71 , 3470},
	{71 , 3452},
	{71 , 3430}
}; 

