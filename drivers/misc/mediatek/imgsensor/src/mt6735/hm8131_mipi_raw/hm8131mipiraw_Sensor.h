/*******************************************************************************************/


/*******************************************************************************************/
#ifndef _HM8131MIPI_SENSOR_H
#define _HM8131MIPI_SENSOR_H

#define	__SW_VER__		"20140820V0.r1"
#define	__HW_VER__		"HM8131V1_Module"

#define	HM8131MIPI_FACTORY_START_ADDR		0
#define	HM8131MIPI_ENGINEER_START_ADDR	10
#define	FACTORY_START_ADDR					0
#define	ENGINEER_START_ADDR				10

typedef enum HM8131MIPI_group_enum {
	HM8131MIPI_PRE_GAIN = 0,
	HM8131MIPI_CMMCLK_CURRENT,
	HM8131MIPI_FRAME_RATE_LIMITATION,
	HM8131MIPI_REGISTER_EDITOR,
	HM8131MIPI_GROUP_TOTAL_NUMS
} HM8131MIPI_FACTORY_GROUP_ENUM;

typedef enum HM8131MIPI_engineer_index {
	HM8131MIPI_CMMCLK_CURRENT_INDEX	= HM8131MIPI_ENGINEER_START_ADDR,
	HM8131MIPI_ENGINEER_END
} HM8131MIPI_FACTORY_ENGINEER_INDEX;

typedef enum HM8131MIPI_register_index {
	HM8131MIPI_SENSOR_BASEGAIN		= HM8131MIPI_FACTORY_START_ADDR,
	HM8131MIPI_PRE_GAIN_R_INDEX,
	HM8131MIPI_PRE_GAIN_Gr_INDEX,
	HM8131MIPI_PRE_GAIN_Gb_INDEX,
	HM8131MIPI_PRE_GAIN_B_INDEX,
	HM8131MIPI_FACTORY_END_ADDR
} HM8131MIPI_FACTORY_REGISTER_INDEX;

typedef struct _sensor_data_struct {
	SENSOR_REG_STRUCT	Reg[HM8131MIPI_ENGINEER_END];
	SENSOR_REG_STRUCT	CCT[HM8131MIPI_FACTORY_END_ADDR];
} SENSOR_DATA_STRUCT, *PSENSOR_DATA_STRUCT;

typedef enum {
	SENSOR_MODE_INIT = 0,
	SENSOR_MODE_PREVIEW,
	SENSOR_MODE_VIDEO,
	SENSOR_MODE_CAPTURE
} HM8131MIPI_SENSOR_MODE;

typedef struct {
	kal_uint32 SlaveAddr;
	kal_uint32 SensorID;

	kal_uint32 PClk;

	kal_uint32 DummyPixels;
	kal_uint32 DummyLines;
	
	kal_uint16 LineLength;
	kal_uint16 FrameHeight;
	kal_uint32 MaxExposureLines;

	HM8131MIPI_SENSOR_MODE SensorMode;

	kal_uint32 Reg2Gain;
	kal_uint32 Gain2Reg;
	kal_uint32 BaseGain;//64
	kal_uint32 Shutter;

	kal_bool AutoFlickerMode;
	
	kal_uint32  SensorFps;

	kal_uint32	MirrorFlip;
	
} HM8131MIPI_PARA_STRUCT, *PHM8131MIPI_PARA_STRUCT;


/* Sensor Clock Info,UT: KHz */
#if 1
#define	HM8131MIPI_PREVIEW_CLK	 68000  
#define	HM8131MIPI_VIDEO_CLK    70000
#define	HM8131MIPI_CAPTURE_CLK	 68000  //68Mhz
#else
#define	HM8131MIPI_PREVIEW_CLK	 35000  //35 
#define	HM8131MIPI_VIDEO_CLK    35000   //35
#define	HM8131MIPI_CAPTURE_CLK	 35000  //68Mhz

#endif

/*First Bayer Color*/
#define HM8131MIPI_COLOR_FORMAT	SENSOR_OUTPUT_FORMAT_RAW_R	//SENSOR_OUTPUT_FORMAT_RAW_R

/*First Point Info For Output Image*/
#define HM8131MIPI_PV_X_START   4
#define HM8131MIPI_PV_Y_START   4
#define HM8131MIPI_VIDEO_X_START   2
#define HM8131MIPI_VIDEO_Y_START   2
#define HM8131MIPI_FULL_X_START    4
#define HM8131MIPI_FULL_Y_START    4

/*Image Size For Output*/
#define HM8131MIPI_IMAGE_SENSOR_PV_WIDTH		(3264-32)
#define HM8131MIPI_IMAGE_SENSOR_PV_HEIGHT		(2448-24)
#define HM8131MIPI_IMAGE_SENSOR_VIDEO_WIDTH	(1632-16)
#define HM8131MIPI_IMAGE_SENSOR_VIDEO_HEIGHT	(1224-12)
#define	HM8131MIPI_IMAGE_SENSOR_FULL_WIDTH		(3264-32)
#define	HM8131MIPI_IMAGE_SENSOR_FULL_HEIGHT	(2448-24)

/* Defualt Frame Info  Width & Height*/
#define HM8131MIPI_PV_PERIOD_PIXEL_NUMS		(888)
#define HM8131MIPI_PV_PERIOD_LINE_NUMS			(2534)
#define HM8131MIPI_VIDEO_PERIOD_PIXEL_NUMS 	(888)
#define HM8131MIPI_VIDEO_PERIOD_LINE_NUMS		(1306)
#define HM8131MIPI_FULL_PERIOD_PIXEL_NUMS		(888)
#define	HM8131MIPI_FULL_PERIOD_LINE_NUMS		(2534)


// TODO:
#define	HM8131MIPI_MIN_ANALOG_GAIN				1
#define	HM8131MIPI_MAX_ANALOG_GAIN				8		///16
#define	HM8131MIPI_ANALOG_GAIN_1X				0x00	///(0x0020)

#define	HM8131MIPI_MIN_LINE_LENGTH			2984
#define	HM8131MIPI_MIN_FRAME_LENGTH		2038
#define	HM8131MIPI_MAX_LINE_LENGTH			0xCCCC
#define	HM8131MIPI_MAX_FRAME_LENGTH		0xFFFF


/* Slave address */
#define		HM8131MIPI_WRITE_ID_0				(0x48)
#define		HM8131MIPI_READ_ID_0					(0x49)
//#define		HM8131MIPI_WRITE_ID_1					(0x68)
//#define		HM8131MIPI_READ_ID_1					(0x69)

/* Sensor ID, Move to kd_imgsensor.h*/
//#define	HM8131MIPI_SENSOR_ID		0x8131	





#define SETTING_END 0xFFFF
#define SETTING_DELAY 0xFFFE

//#define MIPI_4_lane_enable 1

static struct HM8131REG 
{
    unsigned int Reg;
    unsigned char Value;
} ;

#if 0

/*Sensor Settings*/
static struct HM8131REG HM8131_InitSettings[]={

	 {SETTING_DELAY,10},
	
	
	   {0x3519,0x00},
	   {0x351A,0x05},
	   {0x351B,0x1E},
	   {0x351C,0x90},
	   {0x351E,0x15},
	   {0x351D,0x15},
	   {0x4001,0x80},
		 {SETTING_DELAY,10},
	
		{0xBA93,0x01},
		 {SETTING_DELAY,10},
	
	
	   {0x412A,0x8A},
	   {0xBAA2,0xC0},
	   {0xBAA2,0xC0},
	   {0xBAA2,0xC0},
	   {0xBAA2,0x40},
	   {0x412A,0x8A},
	   {0x412A,0x8A},
	   {0x412A,0x0A},
	
	 {SETTING_DELAY,10},
	
	   {0xBA93,0x03},
	   {0xBA93,0x02},
	   {0xBA90,0x01},
				  
	   {0x4001,0x00},
	
	 {SETTING_DELAY,10},
	
		//mipi 2-lane 1216 version
		//{0x0303,0x01}, //2 1 ; PLL adc1 - enable PLL -> was 002A
		{0x0303,0x02},	//1028, for 2-lane
		{0x0305,0x0C}, //2 1 ; PLL N, mclk 24mhz
	
	//TEST OK: 2A_3B  31_45 3F_58 46_62
		//{0x0307,0x46}, //2 1 ; PLL M, pclk_raw=68mhz
		{0x0307,0x44}, //1028, for 2-lane
	   {0x030F,0x62}, //2 1 ; PLL M, pkt_clk=94mhz
	
		{0x030D, 0x0C},//2 1 ; PLL N,
		{0x0309, 0x02},
		{0x400D, 0x04},
		{0x0383, 0x03},
		{0x0387, 0x03},
		{0x0390, 0x01},//01

	 //analog
	  {0x414A, 0x02},
	  {0x4147, 0x03},
	  {0x4144, 0x03},
	  {0x4145, 0x31},
	  {0x4146, 0x51},
	  {0x4149, 0x57},
	  {0x4260, 0x00},
	  {0x4261, 0x00},
				 
	  {0x426A, 0x01},
				 
	  {0x4270, 0x08},
	  {0x4271, 0xBF},
	  {0x4272, 0x00},
	  {0x427D, 0x00},
	  {0x427E, 0x03},
	  {0x427F, 0x00},
	  {0x4380, 0xA6},
	  {0x4381, 0x7B},
	  {0x4383, 0x98},
	  {0x4387, 0x17},
	  {0x4386, 0x32},
	  {0x4382, 0x00},
	  {0x4388, 0x9F},
	  {0x4389, 0x15},
	  {0x438A, 0x00},
	  {0x438C, 0x0F},
	  {0x4384, 0x14},
	  {0x438B, 0x00},
	  {0x4385, 0xA5},
	  {0x438F, 0x00},
	  {0x438D, 0xA0},
				   
	  {0x4B11, 0x1F},
				   
	  {0x4B44, 0x00},//mipi enable phy to LDO: 1.5=>1.2
	  {0x4B46, 0x03},
	  {0x4B47, 0xC9},
				   
				   
				   
	  {0x44B0, 0x03},
	  {0x44B1, 0x01},
	  {0x44B2, 0x00},
	  {0x44B3, 0x04},
	  {0x44B4, 0x14},
	  {0x44B5, 0x24},
				   
				   
	  {0x44B8, 0x03},
	  {0x44B9, 0x01},
	  {0x44BA, 0x05},
	  {0x44BB, 0x15},
	  {0x44BC, 0x25},
	  {0x44BD, 0x35},
	 
	 //===========
	 
	  {0x44D0, 0xC0},
	  {0x44D1, 0x80},
	  {0x44D2, 0x40},
	  {0x44D3, 0x40},
	  {0x44D4, 0x40},
	  {0x44D5, 0x40},
	  {0x4B07, 0xF0},
	  {0x4131, 0x01},
				   
	  {0x060B, 0x01},
	 //analog
	 
	 {SETTING_DELAY,30},
	
	   {0x4B31,0x06}, //NEW!! 
	   {0x4274,0x33}, //2 1 ; [5] mask out bad frame due to mode (flip/mirror) change
	   {0x4002,0x23}, //2 1 ; output BPC
	   //{0x4B18,0x06}, //orig:6 //2 1 ; [7:0] FULL_TRIGGER_TIME
	   {0x4B18,0x28}, //1028, for 2-lane
	
//#if MIPI_4_lane_enable
		//{0x0111,0x01},
	
//#else
		{0x0111,0x01}, //2 1 ; B5: 1'b0:2lane, 1'b1:4lane (org 4B19)
	
//#endif    
	   {0x4B20,0xDE}, //2 1 ; clock always on(9E) / clock always on while sending packet(BE)
	   {0x4B0E,0x01},
	   {0x4B42,0x02},
	   {0x4B02,0x02}, //2 1 ; lpx_width
	   //{0x4B03,0x05}, //2 1 ; hs_zero_width
	   {0x4B03,0x0E}, //1028, for 2-lane
	   {0x4B04,0x02}, //2 1 ; hs_trail_width
	   {0x4B05,0x0C}, //2 1 ; clk_zero_width
	   {0x4B06,0x03}, //2 1 ; clk_trail_width
	   {0x4B0F,0x07}, //2 1 ; clk_back_porch
	   {0x4B31,0x06}, //2 1 ; clk_trail_width
	
	
	
	   {0x4B39,0x02}, //2 1 ; clk_width_exit
	   //{0x4B3F,0x08}, //2 1 ; [3:0] mipi_req_dly
	   {0x4B3F,0x18}, //1028, for 2-lane
	   {0x4B42,0x02}, //2 1 ; HS_PREPARE_WIDTH
	   {0x4B43,0x02}, //2 1 ; CLK_PREPARE_WIDTH
	
	
	   {0x4024,0x40}, //2 1 ; enabled MIPI -> was 0024
	
//#if for_module 
		{0x0101,0x00}, //flip+mirror
//#else          
//	   {0x0101,0x03}, //flip+mirror
//#endif
	
	
	{0x0350,0x33},
	
	
	
	{0x3110,0x01},
	{0x3111,0x01},				
	{0x3130,0x01},
	{0x3131,0x26},
	
	 {0x0348,0x0C},//Image size X end Hb
	 {0x0349,0xCD},//Image size X end Lb
	 {0x034A,0x09},//Image size Y end Hb
	 {0x034B,0x9D},// Image size Y end Lb
	 {0x034C,0x06},//Image size X end Hb
	 {0x034D,0x68},//Image size X end Lb
	 {0x034E,0x04},//Image size Y end Hb
	 {0x034F,0xD0},// Image size Y end Lb
	
	   {0x0340,0x05},
	   {0x0341,0x1A},
	   {0x4800,0x00},
	   {0x0104,0x01},
	   {0x0104,0x00},
	   {0x4801,0x00},
	   {0x0000,0x00},
	   {0xBA94,0x01},
	   {0xBA94,0x00},
	   {0xBA93,0x02},


    {SETTING_END,0},
   
};
#endif 

static struct HM8131REG HM8131_PreviewSettings[]={
	 

 {SETTING_DELAY,10},//START
	
	
	   {0x3519,0x00},
	   {0x351A,0x05},
	   {0x351B,0x1E},
	   {0x351C,0x90},
	   {0x351E,0x15},
	   {0x351D,0x15},
	   {0x4001,0x80},
		 {SETTING_DELAY,10},
	
		{0xBA93,0x01},
		 {SETTING_DELAY,10},
	
	
	   {0x412A,0x8A},
	   {0xBAA2,0xC0},
	   {0xBAA2,0xC0},
	   {0xBAA2,0xC0},
	   {0xBAA2,0x40},
	   {0x412A,0x8A},
	   {0x412A,0x8A},
	   {0x412A,0x0A},
	
	 {SETTING_DELAY,10},
	
	   {0xBA93,0x03},
	   {0xBA93,0x02},
	   {0xBA90,0x01},
				  
	   {0x4001,0x00},
	
	 {SETTING_DELAY,10},
	
		
		{0x0303,0x01}, //2 1 ; PLL adc1 - enable PLL -> was 002A
		
		{0x0305,0x0C}, //2 1 ; PLL N, mclk 24mhz
		{0x0309,0x02}, //1028, for 2-lane

	{0x0307,0x46}, //2 1 ; PLL M, pclk_raw=35mhz
	
	{0x030D, 0x0C},
	//{0x030F,0x31}, //2 1 ; PLL M, pkt_clk=196mhz
	{0x030F,0x62}, //1028, for 2-lan
	 {SETTING_DELAY,10},
	 
		//{0x030D, 0x0C},//2 1 ; PLL N,
		//{0x0309, 0x02},
		//{0x400D, 0x04},
		//{0x0383, 0x03},
		//{0x0387, 0x03},
		//{0x0390, 0x01},//01
		//{0x0390,0x11}, //1028, for 2-lan

	 //analog
	  {0x414A, 0x02},
	  {0x4147, 0x03},
	  {0x4144, 0x03},
	  {0x4145, 0x31},
	  {0x4146, 0x51},
	  {0x4149, 0x57},
	  {0x4260, 0x00},
	  {0x4261, 0x00},
				 
	  {0x426A, 0x01},
				 
	  {0x4270, 0x08},
	  {0x4271, 0xBF},
	  {0x4272, 0x00},
	  //{0x427D, 0x00},
	  //{0x427E, 0x03},
	  //{0x427F, 0x00},
	  //{0x4380, 0xA6},
	  //{0x4381, 0x7B},
	  {0x4383, 0x98},
	  {0x4387, 0x17},
	  {0x4386, 0x32},
	  //{0x4382, 0x00},
	  //{0x4388, 0x9F},
	  //{0x4389, 0x15},
	  {0x438A, 0x00},
	  
	  {0x427D, 0x00},
	  {0x427E, 0x03},
	  {0x427F, 0x00},
	  {0x4380, 0xA6},
	  {0x4381, 0x7B},
	  {0x4382, 0x00},
	  {0x4388, 0x9F},
	  {0x4389, 0x15},
	  
	  {0x438C, 0x0F},
	  {0x4384, 0x14},
	  {0x438B, 0x00},
	  {0x4385, 0xA5},
	  {0x438F, 0x00},
	  {0x438D, 0xA0},
				   
	  {0x4B11, 0x1F},
				   
	  {0x4B44, 0x00},//mipi enable phy to LDO: 1.5=>1.2
	  {0x4B46, 0x03},
	  {0x4B47, 0xC9},
				   
				   
				   
	  {0x44B0, 0x03},
	  {0x44B1, 0x01},
	  {0x44B2, 0x00},
	  {0x44B3, 0x04},
	  {0x44B4, 0x14},
	  {0x44B5, 0x24},
				   
				   
	  {0x44B8, 0x03},
	  {0x44B9, 0x01},
	  {0x44BA, 0x05},
	  {0x44BB, 0x15},
	  {0x44BC, 0x25},
	  {0x44BD, 0x35},
	 
	 //===========
	 
	  {0x44D0, 0xC0},
	  {0x44D1, 0x80},
	  {0x44D2, 0x40},
	  {0x44D3, 0x40},
	  {0x44D4, 0x40},
	  {0x44D5, 0x40},
	  {0x4B07, 0xF0},
	  {0x4131, 0x01},
				   
	  {0x060B, 0x01},
	 //analog
	 
	 {SETTING_DELAY,30},
	{0x4274, 0x33},
	{0x400D, 0x04},
	{0x3110, 0x03},
	{0x3111, 0x01},
	{0x3130, 0x01},
	{0x3131, 0x26},
	{0x0383, 0x03},
	{0x0387, 0x03},
	{0x0390, 0x11},
	{SETTING_DELAY,30},
	
{0x0348,0x0C},
{0x0349,0xCD},//3277
{0x034A,0x09},
{0x034B,0x9D},//2461
	   {0x0340,0x05},
	   {0x0341,0x1A},
	
	   {0x4B31,0x06}, //NEW!! 
{0x034C,0x06},
{0x034D,0x68},//1632
{0x034E,0x04},
{0x034F,0xD0},//1224
	   //{0x4274,0x33}, //2 1 ; [5] mask out bad frame due to mode (flip/mirror) change
	   //{0x4002,0x23}, //2 1 ; output BPC
	   {0x4B18,0x06}, //orig:6 //2 1 ; [7:0] FULL_TRIGGER_TIME
	   
	
//#if MIPI_4_lane_enable
		//{0x0111,0x01},
	
//#else
		{0x0111,0x01}, //2 1 ; B5: 1'b0:2lane, 1'b1:4lane (org 4B19)
	
//#endif    
	   {0x4B20,0xDE}, //2 1 ; clock always on(9E) / clock always on while sending packet(BE)
	   {0x4B0E,0x01},
	   {0x4B42,0x02},
	   {0x4B02,0x02}, //2 1 ; lpx_width
	   {0x4B03,0x05}, //2 1 ; hs_zero_width
	   {0x4B04,0x02}, //2 1 ; hs_trail_width
	   {0x4B05,0x0C}, //2 1 ; clk_zero_width
	   {0x4B06,0x03}, //2 1 ; clk_trail_width
	   {0x4B0F,0x07}, //2 1 ; clk_back_porch
	   {0x4B31,0x06}, //2 1 ; clk_trail_width
	
	
	
	   {0x4B39,0x02}, //2 1 ; clk_width_exit
	   {0x4B3F,0x08}, //2 1 ; [3:0] mipi_req_dly
	   
	   {0x4B42,0x02}, //2 1 ; HS_PREPARE_WIDTH
	   {0x4B43,0x02}, //2 1 ; CLK_PREPARE_WIDTH
	
	
	   {0x4024,0x40}, //2 1 ; enabled MIPI -> was 0024
	
//#if for_module 
//		{0x0101,0x00}, //flip+mirror
//#else          
	   {0x0101,0x03}, //flip+mirror
//#endif
	
	//{0x0350,0x33},
	
	//{0x3110,0x01},
	//{0x3111,0x01},				
	//{0x3130,0x01},
	//{0x3131,0x26},

//Soso Modify
//{0x0344,0x00}, 
//{0x0345,0x00},	 
//{0x0346,0x00},
//{0x0347,0x00},
//{0x0348,0x0C},
//{0x0349,0xCD},//3277
//{0x034A,0x09},
//{0x034B,0x9D},//2461

//{0x034C,0x06},
//{0x034D,0x68},//1632
//{0x034E,0x04},
//{0x034F,0xD0},//1224


	   //{0x0340,0x05},
	   //{0x0341,0x1A},
	   {0x4800,0x00},
	   {0x0104,0x01},
	   {0x0104,0x00},
	   {0x4801,0x00},
	   {0x0000,0x00},
	   {0xBA94,0x01},
	   {0xBA94,0x00},
	   {0xBA93,0x02},

    {SETTING_END,0},
};

#if 1
#define HM8131_VideoSettings HM8131_PreviewSettings
#else
static struct HM8131REG HM8131_VideoSettings[]={




	{SETTING_END,0},
};
#endif

static struct HM8131REG HM8131_FullSizeSettings[]={
	   {0x3519,0x00},
	   {0x351A,0x05},
	   {0x351B,0x1E},
	   {0x351C,0x90},
	   {0x351E,0x15},
	   {0x351D,0x15},
	   {0x4001,0x80},
	   {SETTING_DELAY,10},
	
		{0xBA93,0x01},
	   {SETTING_DELAY,10},
	
	
		{0x412A,0x8A},
		{0xBAA2,0xC0},
		{0xBAA2,0xC0},
		{0xBAA2,0xC0},
		{0xBAA2,0x40},
		{0x412A,0x8A},
		{0x412A,0x8A},
		{0x412A,0x0A},
	
	{SETTING_DELAY,10},
	
		{0xBA93,0x03},
		{0xBA93,0x02},
		{0xBA90,0x01},
		
		{0x4001,0x00},
	
	{SETTING_DELAY,10},
	
	   {0x0303,0x01}, //2 1 ; PLL adc1 - enable PLL -> was 002A
	   
	   {0x0305,0x0C}, //2 1 ; PLL N, mclk 24mhz
	   {0x0307,0x44}, //2 1 ; PLL M, pclk_raw=68mhz
	   {0x030D,0x0C}, //2 1 ; PLL N,
	   {0x030F,0x5E}, //2 1 ; PLL M, pkt_clk=94mhz


		//analog();

		 {0x414A, 0x02},
		 {0x4147, 0x03},
		 {0x4144, 0x03},
		 {0x4145, 0x31},
		 {0x4146, 0x51},
		 {0x4149, 0x57},
		 {0x4260, 0x00},
		 {0x4261, 0x00},
					
		 {0x426A, 0x01},
					
		 {0x4270, 0x08},
		 {0x4271, 0xBF},
		 {0x4272, 0x00},
		 {0x427D, 0x00},
		 {0x427E, 0x03},
		 {0x427F, 0x00},
		 {0x4380, 0xA6},
		 {0x4381, 0x7B},
		 {0x4383, 0x98},
		 {0x4387, 0x17},
		 {0x4386, 0x32},
		 {0x4382, 0x00},
		 {0x4388, 0x9F},
		 {0x4389, 0x15},
		 {0x438A, 0x00},
		 {0x438C, 0x0F},
		 {0x4384, 0x14},
		 {0x438B, 0x00},
		 {0x4385, 0xA5},
		 {0x438F, 0x00},
		 {0x438D, 0xA0},
					  
		 {0x4B11, 0x1F},
					  
		 {0x4B44, 0x00},//mipi enable phy to LDO: 1.5=>1.2
		 {0x4B46, 0x03},
		 {0x4B47, 0xC9},
					  
					  
					  
		 {0x44B0, 0x03},
		 {0x44B1, 0x01},
		 {0x44B2, 0x00},
		 {0x44B3, 0x04},
		 {0x44B4, 0x14},
		 {0x44B5, 0x24},
					  
					  
		 {0x44B8, 0x03},
		 {0x44B9, 0x01},
		 {0x44BA, 0x05},
		 {0x44BB, 0x15},
		 {0x44BC, 0x25},
		 {0x44BD, 0x35},
		
		//===========
		
		 {0x44D0, 0xC0},
		 {0x44D1, 0x80},
		 {0x44D2, 0x40},
		 {0x44D3, 0x40},
		 {0x44D4, 0x40},
		 {0x44D5, 0x40},
		 {0x4B07, 0xF0},
		 {0x4131, 0x01},
					  
		 {0x060B, 0x01},

		//analog
		{SETTING_DELAY,30},
	
	   {0x4274,0x33},
	   {0x3110,0x21},
	   {0x3111,0x00},
	   {0x3130,0x01},
	   {0x3131,0x80},
	   {0x4B31,0x06}, //new
	   //{0x4274,0x33}, //2 1 ; [5] mask out bad frame due to mode (flip/mirror) change
	   {0x4002,0x23}, //2 1 ; output BPC
	   {0x034C,0x0C},
	   {0x034D,0xC0},
	   {0x034E,0x09},
	   {0x034F,0xA0},
	   {0x4B18,0x17}, //2 1 ; [7:0] FULL_TRIGGER_TIME
	   

//#if MIPI_4_lane_enable
		//{0x0111,0x01},
//#else	
		{0x0111,0x01}, //2 1 ; B5: 1'b0:2lane, 1'b1:4lane (org 4B19)
//#endif    
		 {0x4B20,0xDE}, //2 1 ; clock always on(9E) / clock always on while sending packet(BE)
		{0x4B0E,0x03},
		{0x4B42,0x05},
		//{0x4B03,0x0E},
		{0x4B04,0x05},
		{0x4B06,0x06},
		{0x4B3F,0x12},
		

		{0x4024,0x40}, //2 1 ; enabled MIPI -> was 0024
	
//#if for_module
//		{0x0101,0x00}, //flip+mirror
//#else
		{0x0101,0x03}, //flip+mirror
//#endif
	
	
	
	//{0x0350,0x33},
	
	//{0x3110,0x23},
	//{0x3111,0x00},
	//{0x3130,0x01},
	//{0x3131,0x80},

//Soso modify
//{0x0344,0x00}, 
//{0x0345,0x00},	 
//{0x0346,0x00},
//{0x0347,0x00},
//{0x0348,0x0C},
//{0x0349,0xCF},//3279
//{0x034A,0x09},
//{0x034B,0x9F},//2463

//{0x034C,0x0C},
//{0x034D,0xC0},//3264
//{0x034E,0x09},
//{0x034F,0x90},//2448


		{0x4800,0x00},
		{0x0104,0x01},
		{0x0104,0x00},
		{0x4801,0x00},
		{0x0000,0x00},
	
	   {0xBA94,0x01},
	   {0xBA94,0x00},
	   {0xBA93,0x02},

	{SETTING_END,0},
};



////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

//export functions
UINT32 HM8131MIPIOpen(void);
UINT32 HM8131MIPIControl(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
UINT32 HM8131MIPIFeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId, UINT8 *pFeaturePara,UINT32 *pFeatureParaLen);
UINT32 HM8131MIPIGetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_INFO_STRUCT *pSensorInfo, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
UINT32 HM8131MIPIGetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution);
UINT32 HM8131MIPIClose(void);

#endif /* _HM8131MIPI_SENSOR_H_ */
