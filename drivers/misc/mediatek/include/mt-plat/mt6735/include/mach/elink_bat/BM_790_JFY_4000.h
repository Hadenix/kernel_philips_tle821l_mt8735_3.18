/********************************************************************/
/*************           company: JFY               *****************/
/*************           sign: 37100100             *****************/
/*************           compacity: 4000            *****************/
/*************           project: 790 series        *****************/
/********************************************************************/

/* Qmax for battery  */
#define BM_790_JFY_4000_Q_MAX_POS_50	3778
#define BM_790_JFY_4000_Q_MAX_POS_25	3778
#define BM_790_JFY_4000_Q_MAX_POS_0		3778
#define BM_790_JFY_4000_Q_MAX_NEG_10	3778

#define BM_790_JFY_4000_Q_MAX_POS_50_H_CURRENT	3778
#define BM_790_JFY_4000_Q_MAX_POS_25_H_CURRENT	3778
#define BM_790_JFY_4000_Q_MAX_POS_0_H_CURRENT	3778
#define BM_790_JFY_4000_Q_MAX_NEG_10_H_CURRENT	3778


// T2 25C
BATTERY_PROFILE_STRUCT bm_790_jfy_4000_battery_profile_t2[] =
{
	{0  , 4179},
	{1  , 4128},
	{2  , 4117},
	{3  , 4104},
	{5  , 4085},
	{6  , 4076},
	{7  , 4067},
	{8  , 4059},
	{9  , 4047},
	{10 , 4038},
	{11 , 4030},
	{12 , 4021},
	{13 , 4010},
	{14 , 4002},
	{15 , 3995},
	{16 , 3986},
	{17 , 3975},
	{18 , 3967},
	{19 , 3959},
	{20 , 3950},
	{21 , 3940},
	{22 , 3932},
	{23 , 3925},
	{24 , 3918},
	{25 , 3911},
	{26 , 3902},
	{27 , 3895},
	{28 , 3888},
	{29 , 3882},
	{30 , 3873},
	{31 , 3866},
	{32 , 3860},
	{33 , 3854},
	{34 , 3846},
	{35 , 3840},
	{36 , 3834},
	{37 , 3828},
	{38 , 3821},
	{39 , 3816},
	{40 , 3811},
	{41 , 3806},
	{42 , 3800},
	{43 , 3795},
	{44 , 3791},
	{45 , 3786},
	{46 , 3781},
	{47 , 3777},
	{48 , 3773},
	{49 , 3769},
	{50 , 3765},
	{51 , 3760},
	{52 , 3757},
	{53 , 3753},
	{54 , 3750},
	{55 , 3745},
	{56 , 3742},
	{57 , 3739},
	{58 , 3736},
	{59 , 3732},
	{60 , 3729},
	{61 , 3726},
	{62 , 3723},
	{63 , 3719},
	{64 , 3717},
	{65 , 3715},
	{66 , 3712},
	{67 , 3709},
	{68 , 3707},
	{69 , 3705},
	{70 , 3703},
	{71 , 3700},
	{72 , 3698},
	{73 , 3696},
	{74 , 3694},
	{75 , 3691},
	{76 , 3688},
	{77 , 3685},
	{78 , 3683},
	{79 , 3679},
	{80 , 3674},
	{81 , 3670},
	{82 , 3666},
	{83 , 3661},
	{84 , 3655},
	{85 , 3651},
	{86 , 3647},
	{87 , 3644},
	{88 , 3640},
	{89 , 3636},
	{90 , 3632},
	{91 , 3628},
	{92 , 3621},
	{93 , 3615},
	{94 , 3608},
	{95 , 3599},
	{96 , 3582},
	{97 , 3562},
	{98 , 3533},
	{99 , 3489},
	{100, 3426},
	{100, 3426}
};     

// ============================================================
// <Rbat, Battery_Voltage> Table
// ============================================================
// T2 25C
R_PROFILE_STRUCT bm_790_jfy_4000_r_profile_t2[] =
{
	{60 , 4179},
	{60 , 4128},
	{60 , 4117},
	{60 , 4104},
	{61 , 4085},
	{61 , 4076},
	{61 , 4067},
	{61 , 4059},
	{61 , 4047},
	{61 , 4038},
	{61 , 4030},
	{61 , 4021},
	{61 , 4010},
	{61 , 4002},
	{62 , 3995},
	{62 , 3986},
	{62 , 3975},
	{62 , 3967},
	{62 , 3959},
	{62 , 3950},
	{62 , 3940},
	{62 , 3932},
	{62 , 3925},
	{62 , 3918},
	{63 , 3911},
	{63 , 3902},
	{63 , 3895},
	{63 , 3888},
	{63 , 3882},
	{63 , 3873},
	{63 , 3866},
	{63 , 3860},
	{63 , 3854},
	{63 , 3846},
	{64 , 3840},
	{64 , 3834},
	{64 , 3828},
	{64 , 3821},
	{64 , 3816},
	{64 , 3811},
	{64 , 3806},
	{64 , 3800},
	{64 , 3795},
	{64 , 3791},
	{64 , 3786},
	{65 , 3781},
	{65 , 3777},
	{65 , 3773},
	{65 , 3769},
	{65 , 3765},
	{65 , 3760},
	{65 , 3757},
	{65 , 3753},
	{65 , 3750},
	{65 , 3745},
	{66 , 3742},
	{66 , 3739},
	{66 , 3736},
	{66 , 3732},
	{66 , 3729},
	{66 , 3726},
	{66 , 3723},
	{66 , 3719},
	{66 , 3717},
	{66 , 3715},
	{67 , 3712},
	{67 , 3709},
	{67 , 3707},
	{67 , 3705},
	{67 , 3703},
	{67 , 3700},
	{67 , 3698},
	{67 , 3696},
	{67 , 3694},
	{67 , 3691},
	{67 , 3688},
	{68 , 3685},
	{68 , 3683},
	{68 , 3679},
	{68 , 3674},
	{68 , 3670},
	{68 , 3666},
	{68 , 3661},
	{68 , 3655},
	{68 , 3651},
	{68 , 3647},
	{69 , 3644},
	{69 , 3640},
	{69 , 3636},
	{69 , 3632},
	{69 , 3628},
	{69 , 3621},
	{69 , 3615},
	{69 , 3608},
	{69 , 3599},
	{69 , 3582},
	{70 , 3562},
	{70 , 3533},
	{70 , 3489},
	{70 , 3426},
	{70 , 3426}
}; 

