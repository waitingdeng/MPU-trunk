/******************************************************
�ļ�����PP_ChargeCtrl.h

������	����������

Data			  Vasion			author
2019/05/18		   V1.0			    liujian
*******************************************************/
#ifndef		_PP_CHARGE_CTRL_H
#define		_PP_CHARGE_CTRL_H
/*******************************************************
description�� include the header file
*******************************************************/

/*******************************************************
description�� macro definitions
*******************************************************/
/**********�꿪�ض���*********/

/**********�곣������*********/
#define PP_CHARGECTRL_APPOINTHOLDTIME   (6*60*60*1000)

#define PP_CHARGECTRL_IDLE   		0
#define PP_CHARGECTRL_REQSTART  	1
#define PP_CHARGECTRL_RESPWAIT   	2
#define PP_CHARGECTRL_END    		3

#define PP_COMAND_STARTCHARGE   	0x700
#define PP_COMAND_STOPCHARGE  		0x701
#define PP_COMAND_APPOINTCHARGE   	0x702
#define PP_COMAND_CANCELAPPOINTCHARGE    	0x703

#define PP_CHARGECTRL_OPEN   		2
#define PP_CHARGECTRL_CLOSE  		1


#define PP_APPOINTCHARGE_IDLE   	0
#define PP_APPOINTCHARGE_READY	    1
#define PP_APPOINTCHARGE_ONGOING   	2
#define PP_APPOINTCHARGE_SUCCESS  	3
#define PP_APPOINTCHARGE_UNCONNT  	4//充电枪未连接
#define PP_APPOINTCHARGE_SPORT		5//运动模式
#define PP_APPOINTCHARGE_ABNRSHUTDOWN	6//异常关闭
/***********�꺯��***********/

/*******************************************************
description�� struct definitions
*******************************************************/

/*******************************************************
description�� typedef definitions
*******************************************************/
/******enum definitions******/
typedef struct
{
	uint8_t req;
	uint8_t chargecmd;
	uint8_t bookingSt;//�Ƿ�ԤԼ
	uint8_t executSt;//ִ��״̬
	uint8_t CtrlSt;
	char 	style;//��ʽ��tsp-1��2-������3-HU��4-tbox
	uint64_t period;
	uint8_t  waitSt;
	uint64_t waittime;
	uint8_t  chargeSt;//���״̬��1-����У�0-δ���

	uint8_t  dataUpdata;

	uint8_t appointcharge;
	uint8_t appointchargeSt;
	uint64_t appointchargeTimer;
}__attribute__((packed))  PP_rmtChargeCtrlSt_t; /*remote control*/


typedef struct
{
	//tsp
	long 	reqType;
	long 	rvcReqCode;
	long 	bookingId;
}__attribute__((packed))  PP_rmtChargeCtrlPara_t; /*�ṹ��*/


typedef struct
{
	//ԤԼ��¼
	uint8_t  appointType;//ԤԼ��¼���ͣ�1-tsp;3-HU
	uint8_t  validFlg;
	uint32_t id;
	uint8_t  hour;
	uint8_t  min;
	uint8_t  targetSOC;
	uint8_t  period;
	uint32_t eventId;

	uint16_t rvcReqType;
	uint32_t huBookingTime;
	uint32_t HUbookingId;

	uint8_t	bookupdataflag;//ԤԼ��¼���µ�tsp��־:0-Ĭ�ϣ�1-�ϱ��У�2-�ϱ�ʧ�ܣ�3-�ϱ����
}__attribute__((packed))  PP_rmtCharge_AppointBook_t; /*�ṹ��*/

typedef struct
{
	//ԤԼ����
	uint8_t  week;
	uint8_t  mask;
} PP_rmtCharge_Appointperiod_t; /*�ṹ��*/
/******union definitions*****/

/*******************************************************
description�� variable External declaration
*******************************************************/

/*******************************************************
description�� function External declaration
*******************************************************/
extern void PP_ChargeCtrl_init(void);
extern int 	PP_ChargeCtrl_mainfunction(void *task);
extern void SetPP_ChargeCtrl_Request(char ctrlstyle,void *appdatarmtCtrl,void *disptrBody);

extern void PP_ChargeCtrl_SetCtrlReq(unsigned char req,uint16_t reqType);
extern void PP_ChargeCtrl_chargeStMonitor(void *task);
extern int PP_ChargeCtrl_start(void);
extern int PP_ChargeCtrl_end(void);
extern void PP_ChargeCtrl_ClearStatus(void);
extern void PP_ChargeCtrl_send_cb(void);
//extern void SetPP_ChargeCtrl_Awaken(void);
extern unsigned char GetPP_ChargeCtrl_Sleep(void);
#endif 
