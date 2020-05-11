/******************************************************
�ļ�����	PrvtProt_rmtDiag.h

������	��ҵ˽��Э�飨�㽭���ڣ�	

Data			  Vasion			author
2019/04/16		   V1.0			    liujian
*******************************************************/
#ifndef		_PRVTPROT_RMTDIAG_H
#define		_PRVTPROT_RMTDIAG_H
/*******************************************************
description�� include the header file
*******************************************************/

/*******************************************************
description�� macro definitions
*******************************************************/
/**********�꿪�ض���*********/

/**********�곣������*********/
#define PP_DIAG_WAITTIME    2500//�ȴ�HU��Ӧʱ��
#define PP_DIAG_MAX_REPORT  80//һ������ϱ��Ĺ�������

#define PP_DIAGPWRON_WAITTIME    	5000//5s
#define PP_DIAGQUERY_WAITTIME    	5000//5s
#define PP_FAULTCODECLEAN_WAITTIME  5000//5s
#define PP_DIAGQUERYALL_WAITTIME	150000//150s
/***********�꺯��***********/
#define PP_DIAG_TYPE_TBOX	26
#define PP_DIAG_TBOX_CURRENTFAULT		0
#define PP_DIAG_TBOX_HISTORYFAULT		1

#define PP_DIAG_TBOX_PBLNETFAULTCODE	"T0002"
#define PP_DIAG_TBOX_PBLNETFAULTLOWBYTE	0

#define PP_DIAG_TBOX_HULINKFAULTCODE	"T0004"
#define PP_DIAG_TBOX_HULINKFAULTLOWBYTE	0

#define PP_ACTIVEDIAG_SELFDIAG		1
#define PP_ACTIVEDIAG_TSPDIAG		2

/*******************************************************
description�� struct definitions
*******************************************************/

/*******************************************************
description�� typedef definitions
*******************************************************/
/******enum definitions******/
typedef enum
{
	PP_RMTDIAG_RESP_REQ = 0,//
	PP_RMTDIAG_RESP_IMAGEACQREQ,//
	PP_RMTDIAG_STATUS,//
	PP_RMTDIAG_MAX_RESP
} PP_RMTDIAG_RESPTYPE;

typedef enum
{
	PP_DIAGRESP_IDLE = 0,//
	PP_DIAGRESP_VEHICOND,
	PP_DIAGRESP_QUERYFAILREQ,//�����ѯ����
	PP_DIAGRESP_QUERYWAIT,//�ȴ���ѯ���Ӧ��
	PP_DIAGRESP_DTCEAIT,//
	PP_DIAGRESP_QUERYUPLOAD,//��ѯ�ϱ�
	PP_DIAGRESP_END
} PP_RMTDIAG_DIAGRESP_ST;

typedef enum
{
	PP_IMAGEACQRESP_IDLE = 0,//
	PP_IMAGEACQRESP_INFORM_HU,//֪ͨHU
	PP_IMAGEACQRESP_WAITHURESP,//�ȴ�HU��Ӧ
	PP_IMAGEACQRESP_END
} PP_RMTDIAG_IMAGEACQRESP_ST;

typedef enum
{
	PP_LOGACQRESP_IDLE = 0,//
	PP_LOGACQRESP_INFORM_UPLOADLOG,
	PP_LOGACQRESP_INFORM_WAITING,
	PP_LOGACQRESP_INFORM_TSP,
	PP_LOGACQRESP_END,
} PP_RMTDIAG_LOGACQRESP_ST;

typedef enum
{
	PP_STOPLOG_IDLE = 0,//
	PP_STOPLOG_INFORM_STOP,
	PP_STOPLOG_INFORM_WAITING,
	PP_STOPLOG_INFORM_TSP,
	PP_STOPLOG_END
} PP_RMTDIAG_STOPLOGACQRESP_ST;

typedef enum
{
	PP_ACTIVEDIAG_IDLE = 0,//�ϵ�
	PP_ACTIVEDIAG_CHECKREPORTST,//����ϱ����
	PP_ACTIVEDIAG_CHECKOTACOND,
	PP_ACTIVEDIAG_CHECKVEHICOND,//��鳵��
	PP_ACTIVEDIAG_QUREYWAIT,//
	PP_ACTIVEDIAG_QUREYDTCWAIT,
	PP_ACTIVEDIAG_QUERYUPLOAD,//
	PP_ACTIVEDIAG_END
} PP_RMTDIAG_ACTIVEDIAG_ST;

typedef enum
{
	PP_RMTDIAG_ERROR_NONE = 0,//
	PP_RMTDIAG_ERROR_VEHISPEED,//vehicle speed > 5km/h
	PP_RMTDIAG_ERROR_TIMEOUT,//time out
	PP_RMTDIAG_ERROR_FOTAING,
	PP_RMTDIAG_ERROR_FOTAECUDIAG,
	PP_RMTDIAG_ERROR_DIAGEVTCONFLICT,
	PP_RMTDIAG_ERROR_DIAGUNENABLE,//unenable
	PP_RMTDIAG_ERROR_ECUNORES,//ECU no response
	PP_RMTDIAG_ERROR_ECURESERRCODE,//ECU Response error code
	PP_RMTDIAG_ERROR_IGNOFF,//ign off
}PP_RMTDIAG_QUERYWRONGTYPE;

typedef enum
{
	PP_FAULTCODECLEAN_IDLE = 0,//
	PP_FAULTCODECLEAN_VEHICOND,//
	PP_FAULTCODECLEAN_REQ,//
	PP_FAULTCODECLEAN_WAIT,//
	PP_FAULTCODECLEAN_END
} PP_FAULTCODECLEAN_ST;

typedef enum
{
	PP_ECULOG_TBOX = 0,//
	PP_ECULOG_IHU,//
	PP_ECULOG_IRS,//
	PP_ECULOG_ICU,//
	PP_ECULOG_TAP,//
	PP_ECULOG_MAX
} PP_RMTDIAG_ECULOGTYPE;

/*****struct definitions*****/
typedef struct
{
	uint8_t  ecuType;
	uint8_t  logLevel;
	uint32_t startTime;
	uint16_t durationTime;
}__attribute__((packed)) PP_rmtDiag_logType_t; 

typedef struct
{
	uint8_t  ecuType;
	uint8_t  result;
	uint8_t failureType;
}__attribute__((packed)) PP_rmtDiag_logReqResp_t; 

typedef struct
{
	uint8_t  diagReq;
	uint8_t  diagType;
	long	 diageventId;
	long	 diagexpTime;
	uint8_t  ImageAcquisitionReq;
	long	 imagereqeventId;
	uint8_t  dataType;
	uint8_t  cameraName;
	uint16_t ImagedurationTime;
	//uint32_t effectiveTime;
	//uint32_t sizeLimit;
	uint8_t  result;//�ɼ�����֪ͨ״̬
	uint8_t	 activediagresult;
	uint8_t  failureType;//�ɼ�����ʧ������
	char     fileName[255];//�ɼ������ļ���
	uint8_t  diagrespSt;
	uint8_t  ImageAcqRespSt;
	uint8_t  waitSt;
	uint64_t waittime;
	uint64_t dtcwaittime;
	
	uint8_t  activeDiagSt;
	uint8_t  activeDiagFlag;
	uint8_t  mcurtcflag;
	uint64_t activeDiagdtcwait;
	uint64_t activeDiagdelaytime;
	uint8_t	 activeDiagWeek;
	long	 activeDiagEveId;
	long	 activeDiagexpTime;

	uint8_t  LogAcqRespSt;
	uint8_t  LogAcqReq;
	long	 logeventId;
	long	 logexpTime;
	PP_rmtDiag_logType_t	ecuLog[PP_ECULOG_MAX];

	PP_rmtDiag_logReqResp_t logReqResp[PP_ECULOG_MAX];
	uint8_t	 faultquerySt;
	uint8_t  faultdtcSt;

	uint8_t  StopLogAcqSt;
	uint8_t  StopLogAcqReq;
	long	 StoplogeventId;
	long	 StoplogexpTime;
	PP_rmtDiag_logReqResp_t StopLogResp;

	uint8_t  cleanfaultSt;
	uint8_t  cleanfaultReq;
	uint8_t  cleanfaultType;
	long	 cleanfaulteventId;
	long	 cleanfaultexpTime;

	uint8_t  cleanallfaultflag;
	uint8_t	 cleanECU;

	uint8_t	 faultCleanFinish;
	uint64_t faultcleanwaittime;
	uint8_t	 faultCleanResult;
	uint8_t	 faultCleanfailureType;
	uint8_t  sleepflag;
}__attribute__((packed)) PrvtProt_rmtDiagSt_t; /*�ṹ��*/

typedef struct
{
	uint32_t datetime;
	uint8_t  diagflag;//bit 1-7 ��ʾ ����1~7
}__attribute__((packed)) PP_rmtDiag_datetime_t; /*�ṹ��*/

typedef struct
{
	uint8_t  week;
	uint8_t  mask;
}__attribute__((packed)) PP_rmtDiag_weekmask_t; /*�ṹ��*/

/* application data struct */
/***********************************

***********************************/
/* remote Diagnostic*/
typedef struct
{
	long	diagType;
}__attribute__((packed)) PP_DiagnosticReq_t;

typedef struct
{
	uint8_t diagCode[5];
	uint8_t diagCodelen;
	uint8_t faultCodeType;
	uint8_t lowByte;
	long 	diagTime;
}__attribute__((packed)) PP_DiagCode_t;

typedef struct
{
	long	diagType;
	int		result;
	long	failureType;
	PP_DiagCode_t		diagCode[255];
	uint8_t diagcodenum;
}__attribute__((packed)) PP_DiagnosticResp_t;

typedef struct
{
	PP_DiagnosticResp_t		diagStatus[255];
	uint8_t diagobjnum;
}__attribute__((packed)) PP_DiagnosticStatus_t;

typedef struct
{
	long dataType;
	long durationTime;
	long cameraName;
	//long effectiveTime;
	//long sizeLimit;
}__attribute__((packed)) PP_ImageAcquisitionReq_t;

typedef struct
{
	int result;
	long failureType ;
	uint8_t fileName[255];
	uint8_t fileNamelen;
}__attribute__((packed)) PP_ImageAcquisitionResp_t;

typedef struct
{
	long ecuType;
	long logLevel;
	long startTime;
	long durationTime;
}__attribute__((packed)) PP_LogAcquisitionReq_t;

typedef struct
{
	long	diagType;
}__attribute__((packed)) PP_FaultCodeClearanceReq_t;

typedef struct
{
	long	diagType;
	int		result;
	long	failureType;
}__attribute__((packed)) PP_FaultCodeClearanceResp_t;

typedef struct
{
	long	durationTime;
}__attribute__((packed)) PP_CanBusMessageCollectReq_t;

typedef struct
{
	long ecuType;
}__attribute__((packed)) PP_StopLogAcquisitionReq_t;

typedef struct
{
	long	ecuType;
	int		result;
	long	failureType;
}__attribute__((packed)) PP_LogAcqEcuResp_t;

typedef struct
{
	uint8_t	ecuNum;
	PP_LogAcqEcuResp_t EcuLog[PP_ECULOG_MAX];
}__attribute__((packed)) PP_LogAcquisitionResp_t;

typedef struct
{
	PP_DiagnosticReq_t  		DiagnosticReq;
	PP_DiagnosticResp_t 		DiagnosticResp;
	PP_DiagnosticStatus_t 		DiagnosticSt;
	PP_ImageAcquisitionReq_t 	ImageAcquisitionReq;
	PP_ImageAcquisitionResp_t 	ImageAcquisitionResp;
	PP_LogAcquisitionReq_t		LogAcquisitionReq[PP_ECULOG_MAX];
	PP_StopLogAcquisitionReq_t	StopLogAcquisitionReq;
	PP_LogAcquisitionResp_t		LogAcquisitionResp;
	PP_LogAcqEcuResp_t			StopLogResp;
	PP_FaultCodeClearanceReq_t	FaultCodeClearanceReq;
	PP_FaultCodeClearanceResp_t	FaultCodeClearanceResp;
	PP_CanBusMessageCollectReq_t CanBusMessageCollectReq;
}__attribute__((packed)) PP_App_rmtDiag_t;



/******union definitions*****/

/*******************************************************
description�� variable External declaration
*******************************************************/

/*******************************************************
description�� function External declaration
*******************************************************/
extern void PP_rmtDiag_init(void);
extern int PP_rmtDiag_mainfunction(void *task);
extern void PP_diag_SetdiagReq(unsigned char diagType,unsigned char reqtype);
extern void PP_diag_rmtdiagtest(unsigned char diagType,unsigned char sueecss,unsigned char faultNum);
#endif 
