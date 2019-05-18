/******************************************************
�ļ�����	PrvtProt_EcDc.h

������	��ҵ˽��Э�飨�㽭���ڣ�	

Data			  Vasion			author
2019/4/29		V1.0			liujian
*******************************************************/
#ifndef		_PRVTPROT_ECDC_H
#define		_PRVTPROT_ECDC_H
/*******************************************************
description�� include the header file
*******************************************************/


/*******************************************************
description�� macro definitions
*******************************************************/
/**********�꿪�ض���*********/

/**********�곣������*********/
#define PP_ECDC_DATA_LEN 	512//����

#define PP_ENCODE_DISBODY 	0x01//����dispatcher header
#define PP_ENCODE_APPDATA 	0x02//����app data


/***********�꺯��***********/

/*******************************************************
description�� struct definitions
*******************************************************/

/*******************************************************
description�� typedef definitions
*******************************************************/
/******enum definitions******/
typedef enum
{
	/*XCALL*/
	ECDC_XCALL_REQ = 0,//xcall request
    ECDC_XCALL_RESP,//xcall response

	/*remote config*/
	ECDC_RMTCFG_CHECK_REQ,//check remote config req
	ECDC_RMTCFG_GET_REQ,//get remote config req
	ECDC_RMTCFG_END_REQ,//remote config req end
	ECDC_RMTCFG_READ_REQ,//remote config read req
	ECDC_RMTCFG_CONN_RESP,//remote config conn resp
	ECDC_RMTCFG_READ_RESP,//remote config read req
	ECDC_APP_MID_MAX
} ECDC_APP_MID_TYPE;//Ӧ������
/*****struct definitions*****/

/******union definitions*****/

/*******************************************************
description�� variable External declaration
*******************************************************/

/*******************************************************
description�� function External declaration
*******************************************************/
extern int PrvtPro_msgPackageEncoding(uint8_t type,uint8_t *msgData,int *msgDataLen, \
							  void *disptrBody, void *appchoice);
extern int PrvtPro_decodeMsgData(uint8_t *LeMessageData,int LeMessageDataLen, \
										void *DisBody,void *appData);
#endif 