#ifndef GT911_H
#define GT911_H

#include "stm32h7xx_hal.h"                  // Device header
#include "main.h"
/*
#include "GPIO.h"
#include "I2C.h"
#include "delay.h"*/

#define     GT911_I2C                         I2C1

/*----------------------------DEVICE ID----------------------------*/

#define     GT911_DEFAULT_ID                  0x28//  0xBA //0x28
//#define     GT911_DEFAULT_ID                  0x28 //0x28
#define     GT911_ALTERNATIVE_ID              0x14

typedef struct 
{
  uint8_t ConfigVersion;
  uint8_t XOutputMax_L;
  uint8_t XOutputMax_H;
  uint8_t YOutputMax_L;
  uint8_t YOutputMax_H;
  uint8_t TouchNumber;
  uint8_t ModuleSwitch[2];
  uint8_t ShakeCount;
  uint8_t Filter;
  uint8_t LargeTouch;
  uint8_t NoiseReduction;
  uint8_t ScreenTouchLvl;
  uint8_t ScreenLeaveLvl;
  uint8_t LowPowerControl;
  uint8_t RefreshRate;
  uint8_t xThreshold;
  uint8_t yThreshold;
	uint8_t xSpeedLimit;
	uint8_t ySpeedLimit;
  uint8_t Space[2];
  uint8_t MiniFilter;
  uint8_t StretchR0;
  uint8_t StretchR1;
  uint8_t StretchR2;
  uint8_t StretchRM;
  uint8_t DrvGroupAnum;
  uint8_t DrvGroupBnum;
  uint8_t SensorNum;
  uint8_t FreqAfactor;
  uint8_t FreqBfactor;
  uint8_t PannelBitFreq_L;
  uint8_t PannelBitFreq_H;
  uint8_t PannelSensorTime_L;
  uint8_t PannelSensorTime_H;
  uint8_t PannelTxGain;
  uint8_t PannelRxGain;
  uint8_t PannelDumpShift;
  uint8_t DrvFrameControl;
  uint8_t ChargingLvlUp;
  uint8_t ModuleSwitch3;
  uint8_t GestureDis;
  uint8_t GestureLongPressTime;
  uint8_t XorYSlopeAdjust;
  uint8_t GestureControl;
  uint8_t GestureSwitch1;
  uint8_t GestureSwitch2;
  uint8_t GestureRefreshRate;
  uint8_t GestureTouchLvl;
  uint8_t NewGreenWakeUpLvl;
  uint8_t FreqHoppStart;
  uint8_t FreqHoppEnd;
  uint8_t NoiseDetectTimes;
  uint8_t HoppingFlag;
  uint8_t HoppingThreshold;
  uint8_t NoiseThreshold;
  uint8_t NoiseMinThreshold;
  uint8_t reserved2;
  uint8_t HoppSensorGroup;
  uint8_t HoppSeg1Normalize;
  uint8_t HoppSeg1Factor;
  uint8_t MainClockAdjust;
  uint8_t HoppSeg2Normalize;
  uint8_t HoppSeg2Factor;
  uint8_t reserved3;
  uint8_t HoppSeg3Normalize;
  uint8_t HoppSeg3Factor;
  uint8_t reserved4;
  uint8_t HoppSeg4Normalize;
  uint8_t HoppSeg4Factor;
  uint8_t reserved5;
  uint8_t HoppSeg5Normalize;
  uint8_t HoppSeg5Factor;
  uint8_t reserved6;
  uint8_t HoppSeg6Normalize;
  uint8_t Key1;
  uint8_t Key2;
  uint8_t Key3;
  uint8_t Key4;
  uint8_t KeyArea;
  uint8_t KeyTouchLvl;
  uint8_t KeyLeaveLvl;
  uint8_t KeySens[2];
  uint8_t KeyRestrain;
  uint8_t KeyRestrainTime;
  uint8_t GestureLargeTouch;
  uint8_t reserved7[2];
  uint8_t HotKnotNoiseMap;
  uint8_t LinkThreshold;
  uint8_t PxyThreshold;
  uint8_t GHotDumpShift;
  uint8_t GHotRXGain;
  uint8_t FreqGain0;
  uint8_t FreqGain1;
  uint8_t FreqGain2;
  uint8_t FreqGain3;
  uint8_t reserved8[9];
  uint8_t CombineDis;
  uint8_t SplitSet;
  uint8_t reserved9[2];
  uint8_t SensorCH[14];
  uint8_t reserved10[16];
  uint8_t DriverCH[26];
  	  uint8_t reserved11[16];
  uint8_t ConfigChksum;
  uint8_t ConfigFresh;
} GT911ConfInfo_TypeDef;

typedef struct
{
  uint8_t TrackID;
  uint16_t XCoordinate;
  uint16_t YCoordinate;
  uint16_t Size;
  uint8_t reserved;
} GT911Point_TypeDef;

typedef struct 
{
  uint16_t XCoordinate;
  uint16_t YCoordinate;
  uint16_t Size;
}GT911Touch_TypeDef;

typedef struct 
{
  uint8_t ProductID[4];
  uint8_t FirmwareVersion[2];
  uint8_t XCoordResolution[2];
  uint8_t YCoordResolution[2];
  uint8_t VendorID;
  uint8_t reserved1[3];
  uint8_t Status;
  //uint8_t TrackID1;
  //uint8_t PxyOK[2];
  //uint8_t reserved2[5];
  //uint8_t TrackID2;
  GT911Point_TypeDef Point[5];
	int8_t reserved2[8];
  uint8_t KeyValue;
} GT911CoordInfo_TypeDef;


/*------------------------REAL-TIME COMMAND------------------------*/

#define     COMMAND_ADDRESS                   0x8040
#define     ESD_CHECK_ADDRESS                 0x8041
#define     COMMAND_CHECK_ADDRESS             0x8046

/*--------------------CONFIGURATION INFORMATION--------------------*/

#define     CONFIG_VERSION                    0x8047
#define     X_OUTPUT_MAX_LOW_ADDRESS          0x8048
#define     X_OUTPUT_MAX_HIGH_ADDRESS         0x8049
#define     Y_OUTPUT_MAX_LOW_ADDRESS          0x804A
#define     Y_OUTPUT_MAX_HIGH_ADDRESS         0x804B
#define     TOUCH_NUMBER_ADDRESS              0x804C
#define     MODULE_SWITCH1_ADDRESS            0x804D
#define     MODULE_SWITCH2_ADDRESS            0x804E
#define     SHAKE_COUNT_ADDRESS               0x804F
#define     FILTER_ADDRESS                    0x8050
#define     LARGE_TOUCH_ADDRESS               0x8051
#define     NOISE_REDUCTION_ADDRESS           0x8052
#define     SCREEN_TOUCH_LEVEL_ADDRESS        0x8053
#define     SCREEN_LEAVE_LEVEL_ADDRESS        0x8054
#define	    LOW_POWER_CONTROL_ADDRESS         0x8055
#define	    REFRESH_RATE_ADDRESS              0x8056
#define	    X_THRESHOLD_ADDRESS               0x8057
#define	    Y_THRESHOLD_ADDRESS               0x8058
#define	    X_SPEED_LIMIT_ADDRESS             0x8059
#define	    Y_SPEED_LIMIT_ADDRESS             0x805A
#define	    SPACE_ADDRESS                     0x805B	
#define	    MINI_FILTER_ADDRESS               0x805D
#define	    STRETCH_R0_ADDRESS                0x805E
#define	    STRETCH_R1_ADDRESS                0x805F
#define	    STRETCH_R2_ADDRESS                0x8060
#define	    STRETCH_RM_ADDRESS                0x8061
#define	    DRV_GROUPA_NUM_ADDRESS            0x8062 
#define	    DRV_GROUPB_NUM_ADDRESS            0x8063
#define	    SENSOR_NUM_ADDRESS                0x8064
#define	    FREQA_FACTOR_ADDRESS              0x8065
#define	    FREQB_FACTOR_ADDRESS              0x8066
#define	    PANNEL_BITFREQL_ADDRESS           0x8067
#define	    PANNEL_BITFREQH_ADDRESS           0x8068
#define	    PANNEL_SENSOR_TIMEL_ADDRESS       0x8069
#define	    PANNEL_SENSOR_TIMEH_ADDRESS       0x806A
#define	    PANNEL_TX_Gain_ADDRESS            0x806B 
#define	    PANNEL_RX_Gain_ADDRESS            0x806C 
#define	    PANNEL_DUMP_Shift_ADDRESS         0x806D 
#define	    DRV_FRAME_Control_ADDRESS         0x806E 
#define	    CHARGING_LEVEL_UP_ADDRESS         0x806F
#define	    MODULE_Switch3_ADDRESS            0x8070 
#define	    GESTURE_DIS_ADDRESS               0x8071
#define	    GESTURE_LONG_PRESS_TIME_ADDRESS   0x8072
#define	    X_or_Y_SLOPE_ADJUST_ADDRESS       0x8073
#define	    GESTURE_CONTROL_ADDRESS           0x8074
#define	    GESTURE_SWITCH1_ADDRESS           0x8075
#define	    GESTURE_SWITCH2_ADDRESS           0x8076
#define	    GESTURE_REFRESH_RATE_ADDRESS      0x8077
#define	    GESTURE_TOUCH_LEVEL_ADDRESS       0x8078
#define	    NEWGREENWAKEUPLEVEL_ADDRESS       0x8079
#define	    FREQ_HOPPING_START_ADDRESS        0x807A 
#define	    FREQ_HOPPING_END_ADDRESS          0x807B
#define	    NOISE_DETECT_TIMES_ADDRESS        0x807C 
#define	    HOPPING_FLAG_ADDRESS              0x807D
#define	    HOPPING_THRESHOLD_ADDRESS         0x807E 
#define	    NOISE_THRESHOLD_ADDRESS           0x807F
#define	    NOISE_MIN_THRESHOLD_ADDRESS       0x8080 
#define	    HOPPING_SENSOR_GROUP_ADDRESS      0x8082
#define	    HOPPING_SEG1_NORMALIZE_ADDRESS    0x8083
#define	    HOPPING_SEG1_FACTOR_ADDRESS       0x8084
#define	    MAIN_CLOCK_AJDUST_ADDRESS         0x8085
#define	    HOPPING_SEG2_NORMALIZE_ADDRESS    0x8086
#define	    HOPPING_SEG2_FACTOR_ADDRESS       0x8087
#define	    HOPPING_SEG3_NORMALIZE_ADDRESS    0x8089
#define	    HOPPING_SEG3_FACTOR_ADDRESS       0x808A
#define	    HOPPING_SEG4_NORMALIZE_ADDRESS    0x808C
#define	    HOPPING_SEG4_FACTOR_ADDRESS       0x808D
#define	    HOPPING_SEG5_NORMALIZE_ADDRESS    0x808F
#define	    HOPPING_SEG5_FACTOR_ADDRESS       0x8090
#define	    HOPPING_SEG6_NORMALIZE_ADDRESS    0x8092
#define	    KEY_ADDRESS1                      0x8093
#define	    KEY_ADDRESS2                      0x8094
#define	    KEY_ADDRESS3                      0x8095
#define	    KEY_ADDRESS4                      0x8096
#define	    KEY_AREA_ADDRESS                  0x8097
#define	    KEY_TOUCH_LEVEL_ADDRESS           0x8098
#define	    KEY_LEAVE_LEVEL_ADDRESS           0x8099
#define	    KEY_SENS1to2_ADDRESS              0x809A
#define	    KEY_SENS3to4_ADDRESS              0x809B
#define	    KEY_RESTRAIN_ADDRESS              0x809C
#define	    KEY_RESTRAIN_TIME_ADDRESS         0x809D 
#define	    GESTURE_LARGE_TOUCH_ADDRESS       0x809E
#define	    HOTKNOT_NOISE_MAP_ADDRESS         0x80A1
#define	    LINK_THRESHOLD_ADDRESS            0x80A2
#define	    PXY_THRESHOLD_ADDRESS             0x80A3
#define	    GHOT_DUMP_SHIFT_ADDRESS           0x80A4
#define	    GHOT_RX_GAIN_ADDRESS              0x80A5
#define	    FREQ_GAIN0_ADDRESS                0x80A6
#define	    FREQ_GAIN1_ADDRESS                0x80A7
#define	    FREQ_GAIN2_ADDRESS                0x80A8
#define	    FREQ_GAIN3_ADDRESS                0x80A9
#define	    COMBINE_DIS_ADDRESS               0x80B3
#define	    SPLIT_SET_ADDRESS                 0x80B4
#define     SENSOR_CH0_ADDRESS                0x80B7
#define     SENSOR_CH1_ADDRESS                0x80B8
#define     SENSOR_CH2_ADDRESS                0x80B9
#define     SENSOR_CH3_ADDRESS                0x80BA
#define     SENSOR_CH4_ADDRESS                0x80BB
#define     SENSOR_CH5_ADDRESS                0x80BC
#define     SENSOR_CH6_ADDRESS                0x80BD
#define     SENSOR_CH7_ADDRESS                0x80BE
#define     SENSOR_CH8_ADDRESS                0x80BF
#define     SENSOR_CH9_ADDRESS                0x80C0
#define     SENSOR_CH10_ADDRESS               0x80C1
#define     SENSOR_CH11_ADDRESS               0x80C2
#define     SENSOR_CH12_ADDRESS               0x80C3
#define     SENSOR_CH13_ADDRESS               0x80C4
#define     DRIVER_CH0_ADDRESS                0x80D5
#define     DRIVER_CH1_ADDRESS                0x80D6
#define     DRIVER_CH2_ADDRESS                0x80D7
#define     DRIVER_CH3_ADDRESS                0x80D8
#define     DRIVER_CH4_ADDRESS                0x80D9
#define     DRIVER_CH5_ADDRESS                0x80DA
#define     DRIVER_CH6_ADDRESS                0x80DB
#define     DRIVER_CH7_ADDRESS                0x80DC
#define     DRIVER_CH8_ADDRESS                0x80DD
#define     DRIVER_CH9_ADDRESS                0x80DE
#define     DRIVER_CH10_ADDRESS               0x80DF
#define     DRIVER_CH11_ADDRESS               0x80E0
#define     DRIVER_CH12_ADDRESS               0x80E1
#define     DRIVER_CH13_ADDRESS               0x80E2
#define     DRIVER_CH14_ADDRESS               0x80E3
#define     DRIVER_CH15_ADDRESS               0x80E4
#define     DRIVER_CH16_ADDRESS               0x80E5
#define     DRIVER_CH17_ADDRESS               0x80E6
#define     DRIVER_CH18_ADDRESS               0x80E7
#define     DRIVER_CH19_ADDRESS               0x80E8
#define     DRIVER_CH20_ADDRESS               0x80E9
#define     DRIVER_CH21_ADDRESS               0x80EA
#define     DRIVER_CH22_ADDRESS               0x80EB
#define     DRIVER_CH23_ADDRESS               0x80EC
#define     DRIVER_CH24_ADDRESS               0x80ED
#define     DRIVER_CH25_ADDRESS               0x80EE
#define	    CONFIG_CHKSUM_ADDRESS             0x80FF
#define	    CONFIG_FRESH_ADDRESS              0x8100

/*----------------------COORDINATE INFORMATION---------------------*/

#define	    PRODUCT_ID_FIRST_ADDRESS          0x8140
#define	    FIRMWARE_VERSION_ADDRESS          0x8144
#define	    X_COORD_RESOLUTION_ADDRESS        0x8146
#define	    Y_COORD_RESOLUTION_ADDRESS        0x8148
#define	    VENDOR_ID_ADDRESS                 0x814A
#define	    STATUS_ADDRESS                    0x814E

#define	    TRACK_ID1_ADDRESS                 0x814F

#define	    POINT1_X_ADDRESS                  0x8150
#define	    POINT1_Y_ADDRESS                  0x8152
#define	    POINT1_SIZE_ADDRESS               0x8154

#define	    TRACK_ID2_ADDRESS                 0x8156

#define	    POINT2_X_ADDRESS                  0x8158
#define	    POINT2_Y_ADDRESS                  0x815A
#define	    POINT2_SIZE_ADDRESS               0x815C

#define	    TRACK_ID3_ADDRESS                 0x815F

#define	    POINT3_X_ADDRESS                  0x8160
#define	    POINT3_Y_ADDRESS                  0x8162
#define	    POINT3_SIZE_ADDRESS               0x8164

#define	    TRACK_ID4_ADDRESS                 0x8167

#define	    POINT4_X_ADDRESS                  0x8168
#define	    POINT4_Y_ADDRESS                  0x816A
#define	    POINT4_SIZE_ADDRESS               0x816C

#define	    TRACK_ID5_ADDRESS                 0x816F

#define	    POINT5_X_ADDRESS                  0x8170
#define	    POINT5_Y_ADDRESS                  0x8172
#define	    POINT5_SIZE_ADDRESS               0x8174

#define	    KEY_VALUE_ADDRESS                 0x817F

/*-------------------------GESTURE FEATURES------------------------*/

#define	    GEST_ID_FIRST_ADDRESS             0x8140
#define	    GEST_FIRMWARE_VERSION_ADDRESS     0x8144
#define	    GEST_X_COORD_RESOLUTION_ADDRESS   0x8146
#define	    GEST_Y_COORD_RESOLUTION_ADDRESS   0x8148
#define	    GEST_TYPES_ADDRESS                0x814B
#define	    NUM_GEST_TOUCH_POINT_ADDRESS      0x814C
#define	    GEST_X_START_POINT_ADDRESS        0x814D
#define	    GEST_Y_START_POINT_ADDRESS        0x814F
#define	    GEST_X_END_POINT_ADDRESS          0x8151
#define	    GEST_Y_END_POINT_ADDRESS          0x8153

#define	    GESTURE_WIDTH_ADDRESS             0x8155
#define	    GESTURE_HEIGHT_ADDRESS            0x8157
#define	    GESTURE_MID_X_ADDRESS             0x8159
#define	    GESTURE_MID_Y_ADDRESS             0x815B
#define	    GESTURE_P1_X_ADDRESS              0x815D
#define	    GESTURE_P1_Y_ADDRESS              0x815F
#define	    GESTURE_P2_X_ADDRESS              0x8161
#define	    GESTURE_P2_Y_ADDRESS              0x8163
#define	    GESTURE_P3_X_ADDRESS              0x8165
#define	    GESTURE_P3_Y_ADDRESS              0x8167
#define	    GESTURE_P4_X_ADDRESS              0x8169
#define	    GESTURE_P4_Y_ADDRESS              0x816B

/*------------------GESTURE COORDINATE INFORMATION-----------------*/

#define	    GEST_POINT_X_COORDINATE_ADDRESS   0x9420
#define	    GEST_POINT_Y_COORDINATE_ADDRESS   0x9422

#define	    GEST_POINT_X_COORDINATE(NUM)   GEST_POINT_X_COORDINATE_ADDRESS + 4*NUM
#define	    GEST_POINT_Y_COORDINATE(NUM)   GEST_POINT_Y_COORDINATE_ADDRESS + 4*NUM

/*---------------------COMMAND STATUS REGISTERS--------------------*/

#define	    GT911_STATUS_ADDRESS              0x81A8
#define	    GT911_STATUS_BACKUP_ADDRESS       0x81A9

#define 		FREQ_CRC_SEED 										0x1021

void GT911_Read(uint16_t RegisterAddress, uint8_t* ReceivedBuffer, uint8_t Size);
void GT911_Write(uint16_t RegisterAddress, uint8_t* TransmitedBuffer, uint8_t Size);

uint8_t GT911_ReadTouch(GT911Touch_TypeDef * GT911Touch);

void GT911_Init(void);

extern void InitEXTI(void);

#endif /*GT911_H*/
