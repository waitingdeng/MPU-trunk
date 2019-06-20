#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <netinet/in.h> 
#include <sys/types.h>  
#include <sys/socket.h> 
#include <pthread.h>

#include "timer.h"
#include "msg_parse.h"
#include "tbox_ivi_api.h"
#include "log.h"
#include "tcom_api.h"
#include "pwdg.h"
#include "cfg_api.h"
#include "fault_sync.h"
#include "tbox_ivi_pb.h"

static pthread_t ivi_tid;    /* thread id */
static timer_t ivi_timer;
static int signalpower;
int tcp_fd = -1;
ivi_client ivi_clients[MAX_IVI_NUM];

ivi_callrequest callrequest;

int gps_onoff = 0;
int network_onoff = 0;
static unsigned char ivi_msgbuf[1024];

unsigned char recv_buf[MAX_IVI_NUM][IVI_MSG_SIZE];

typedef void (*ivi_msg_proc)(unsigned char *msg, unsigned int len);
typedef void (*ivi_msg_handler)(unsigned char *msg, unsigned int len, void *para);

extern void gps_get_nmea(unsigned char * nmea);
extern int nm_get_net_type(void);
extern int at_get_sim_status(void);
extern int at_get_imei(char *imei);
extern int at_get_iccid(char *iccid);
extern void makecall(char *num);
extern void disconnectcall(void);
extern int wifi_disable(void);
extern int wifi_enable(void);
extern int nm_get_signal(void);
extern int assist_get_call_status(void);


int pb_bytes_set(ProtobufCBinaryData * des, uint8_t *buf, int len)
{
    if( len > 0 )
    {
        memcpy(des->data,buf,len);
        des->len = len;

        return 0;
    }

    return -1;
}

int str_find(const char * string, unsigned int strlen, const char *substring, int unsigned sublen)
{
	int i = 0;
	int j = 0;
	
	if( ( string == NULL ) || ( substring == NULL ) )
	{
        log_e(LOG_IVI,"str_find string is null!!!");
		return -1;
	}
	
	if ( strlen < sublen )
	{
        log_e(LOG_IVI,"strlen = %d, sublen = %d.",strlen,sublen);
		return -1;
	}
	
	for ( i = 0; i <= strlen - sublen; i++ )
	{
		for ( j = 0; j < sublen; j++ )
		{
			if ( string[i + j] != substring[j] )
			{
				break;
			}
		}
		
		if ( j == sublen )
		{
			return i;
		}
	}
	
	return -1;
}

#if 0
static inline unsigned char ivi_msg_checksum(const unsigned char *buf, int len, unsigned char cs)
{
    int i;
    volatile unsigned char checksum = cs;

    if (buf == NULL || len <= 0)
    {
        return 0;
    }

    for (i = 0; i < len; ++i)
    {
        checksum = checksum ^ buf[i];
    }

    return checksum;
}


static int ivi_msg_check(unsigned char *buf, int *len)
{
    int msg_len = *len;
    unsigned char cs = 0x0;

    /* dcom_checksum */
    cs = ivi_msg_checksum(buf, msg_len, cs);

    if (cs != buf[msg_len - IVI_PKG_E_MARKER_SIZE - 2])
    {
        log_e(LOG_IVI, "ivi_msg_checksum unmatch!, cs:%u,%u", buf[msg_len - IVI_PKG_E_MARKER_SIZE - 2], cs);
        return -1;
    }

    return 0 ;
}

#endif

int tbox_ivi_get_gps_onoff(void)
{
    return gps_onoff;
}

int tbox_ivi_get_network_onoff(void)
{
    return network_onoff;
}


void ivi_msg_decodex(MSG_RX *rx, ivi_msg_handler ivi_msg_proc, void *para)
{
    int ret, len, i;
    int r_pos = 0, start_pos = -1, end_pos = -1;

    while ( r_pos < rx->used )
    { 
        if( start_pos < 0 )
        {
            ret = str_find( (const char *)(rx->data + r_pos) ,rx->used - r_pos,IVI_PKG_MARKER,IVI_PKG_S_MARKER_SIZE);

            if( ret >= 0 )
            {
                start_pos = r_pos + ret;
                r_pos = start_pos + IVI_PKG_S_MARKER_SIZE;
            }
            else
            {
                r_pos = rx->used;
            }
        }

        /* nothing is found in the buffer, ignore it */
        if (-1 == start_pos)
        {
            rx->used = 0;
        }
        /* start tag is found, but the end tag is not found in a very long buffer, ignore it */
        else if ((r_pos - start_pos) >= (rx->size - 20))
        {
            rx->used = 0;
        }
        /* start tag is found */
        else
        {
            len = rx->used - start_pos;

            if (start_pos != 0)
            {
                for (i = 0; i < len; i++)
                {
                    rx->data[i] = rx->data[i + start_pos];
                }
            }

            rx->used = len;

            ret = str_find( (const char *)(rx->data + r_pos) ,rx->used - r_pos,IVI_PKG_ESC,IVI_PKG_E_MARKER_SIZE);

            if( ret >= 0 )
            {
                end_pos = r_pos + ret + IVI_PKG_E_MARKER_SIZE;
                r_pos = end_pos;
                len = end_pos - start_pos;

                rx->used -= len;

#if 0
                if (len <= 0 || 0 != ivi_msg_check(rx->data + start_pos, &len))
                {
                    start_pos = end_pos;
                    end_pos = -1;
                }
                else
#endif                    
                {
                    log_buf_dump(LOG_IVI, "ivi send", rx->data + start_pos, len);
                    ivi_msg_proc(rx->data + start_pos, len, para);
                    start_pos = -1;
                    end_pos = -1;
                }
            }
            else
            {
                r_pos = rx->used;
            }
        }

    }
    
}

void ivi_msg_error_response_send( int fd ,Tbox__Net__Messagetype id,char *error_code)
{
    int i = 0;
    int ret = 0;
    size_t szlen = 0;

    char send_buf[4096] = {0};
    unsigned char pro_buf[2048] = {0};

    if( fd < 0 )
    {
        log_e(LOG_IVI,"ivi_msg_error_response_send fd = %d.",fd);
        return ;
    }
    
    Tbox__Net__TopMessage TopMsg ;
    Tbox__Net__MsgResult result;

    tbox__net__top_message__init( &TopMsg );
    tbox__net__msg_result__init( &result );

    TopMsg.message_type = id;
 
    result.result = false;
    pb_bytes_set( &result.error_code, (uint8_t *)error_code, strlen(error_code));
    
    TopMsg.msg_result = &result;

    szlen = tbox__net__top_message__get_packed_size( &TopMsg );

    tbox__net__top_message__pack(&TopMsg,pro_buf);
    
    memcpy(send_buf,IVI_PKG_MARKER,IVI_PKG_S_MARKER_SIZE);

    send_buf[IVI_PKG_S_MARKER_SIZE] = szlen >> 8;
    send_buf[IVI_PKG_S_MARKER_SIZE + 1] = szlen;

    for( i = 0; i < szlen; i ++ )
    {
        send_buf[ i + IVI_PKG_S_MARKER_SIZE + 2 ] = pro_buf[i];
    }

    memcpy(( send_buf + IVI_PKG_S_MARKER_SIZE + szlen + 2),IVI_PKG_ESC,IVI_PKG_E_MARKER_SIZE);

    ret = send(fd, send_buf, (IVI_PKG_S_MARKER_SIZE + IVI_PKG_E_MARKER_SIZE + IVI_PKG_MSG_LEN + szlen), 0);


    if (ret < (IVI_PKG_S_MARKER_SIZE + IVI_PKG_E_MARKER_SIZE + IVI_PKG_MSG_LEN + szlen))
    {
        log_e(LOG_IVI, "ivi msg error send response failed!!!");
    }
    else
    {
        log_i(LOG_IVI, "ivi msg error send response success");
    }

    return;

}

void ivi_remotediagnos_response_send( int fd )
{

    int i = 0;
    int ret = 0;
    size_t szlen = 0;

    char send_buf[4096] = {0};
    unsigned char pro_buf[2048] = {0};

    if( fd < 0 )
    {
        log_e(LOG_IVI,"ivi_remotediagnos_response_send fd = %d.",fd);
        return ;
    }
    
    Tbox__Net__TopMessage TopMsg ;
	Tbox__Net__TboxRemoteDiagnose diagnos;
	
    tbox__net__top_message__init( &TopMsg );
	tbox__net__tbox_remote_diagnose__init( &diagnos );
	
    TopMsg.message_type = TBOX__NET__MESSAGETYPE__REQUEST_TBOX_REMOTEDIAGNOSE;

	diagnos.vin = "111111111111111";
//	diagnos.eventid = ;
//	diagnos.timestamp = ;
	diagnos.datatype = TBOX__NET__DATA_TYPE_ENUM__PHOTO_TYPE;
	diagnos.cameraname = TBOX__NET__CAMERA_NAME_ENUM__DVR_TYPE;
	diagnos.aid = 110;
	diagnos.mid = 119;
	diagnos.effectivetime = 100;
	diagnos.sizelimit = 1000;
	
	TopMsg.tbox_remotedaignose = &diagnos;
	
    szlen = tbox__net__top_message__get_packed_size( &TopMsg );

    tbox__net__top_message__pack(&TopMsg,pro_buf);
    
    memcpy(send_buf,IVI_PKG_MARKER,IVI_PKG_S_MARKER_SIZE);

    send_buf[IVI_PKG_S_MARKER_SIZE] = szlen >> 8;
    send_buf[IVI_PKG_S_MARKER_SIZE + 1] = szlen;

    for( i = 0; i < szlen; i ++ )
    {
        send_buf[ i + IVI_PKG_S_MARKER_SIZE + 2 ] = pro_buf[i];
    }

    memcpy(( send_buf + IVI_PKG_S_MARKER_SIZE + szlen + 2),IVI_PKG_ESC,IVI_PKG_E_MARKER_SIZE);

    ret = send(fd, send_buf, (IVI_PKG_S_MARKER_SIZE + IVI_PKG_E_MARKER_SIZE + IVI_PKG_MSG_LEN + szlen), 0);


    if (ret < (IVI_PKG_S_MARKER_SIZE + IVI_PKG_E_MARKER_SIZE + IVI_PKG_MSG_LEN + szlen))
    {
        log_e(LOG_IVI, "ivi remotediagnos send response failed!!!");
    }
    else
    {
        log_i(LOG_IVI, "ivi remotediagnos send response success");
    }

    return;

}

void ivi_activestate_response_send( int fd )
{
    int i = 0;
    int ret = 0;
    size_t szlen = 0;

    char send_buf[4096] = {0};
    unsigned char pro_buf[2048] = {0};

    if( fd < 0 )
    {
        log_e(LOG_IVI,"ivi_activestate_response_send fd = %d.",fd);
        return ;
    }
    
    Tbox__Net__TopMessage TopMsg ;
	Tbox__Net__TboxActiveState state;
	
    tbox__net__top_message__init( &TopMsg );
	tbox__net__tbox_active_state__init( &state );
	
    TopMsg.message_type = TBOX__NET__MESSAGETYPE__RESPONSE_TBOX_ACTIVESTATE_RESULT;
	state.active_state = 1; //已激活
	
	TopMsg.tbox_activestate = &state ;
    szlen = tbox__net__top_message__get_packed_size( &TopMsg );

    tbox__net__top_message__pack(&TopMsg,pro_buf);
    
    memcpy(send_buf,IVI_PKG_MARKER,IVI_PKG_S_MARKER_SIZE);

    send_buf[IVI_PKG_S_MARKER_SIZE] = szlen >> 8;
    send_buf[IVI_PKG_S_MARKER_SIZE + 1] = szlen;

    for( i = 0; i < szlen; i ++ )
    {
        send_buf[ i + IVI_PKG_S_MARKER_SIZE + 2 ] = pro_buf[i];
    }

    memcpy(( send_buf + IVI_PKG_S_MARKER_SIZE + szlen + 2),IVI_PKG_ESC,IVI_PKG_E_MARKER_SIZE);

    ret = send(fd, send_buf, (IVI_PKG_S_MARKER_SIZE + IVI_PKG_E_MARKER_SIZE + IVI_PKG_MSG_LEN + szlen), 0);


    if (ret < (IVI_PKG_S_MARKER_SIZE + IVI_PKG_E_MARKER_SIZE + IVI_PKG_MSG_LEN + szlen))
    {
        log_e(LOG_IVI, "ivi activestate send response failed!!!");
    }
    else
    {
        log_i(LOG_IVI, "ivi activestate send response success");
    }

    return;

}

void ivi_signalpower_response_send(int fd  )
{
    int i = 0;
    int ret = 0;
    size_t szlen = 0;
    char send_buf[4096] = {0};
    unsigned char pro_buf[2048] = {0};
	int temp;
	temp = nm_get_signal();
	if(signalpower == temp)
	{
		return ;
	}
	else
	{
		signalpower = temp;
	}
    if( fd < 0 )
    {
       log_e(LOG_IVI,"ivi_signalpower_response_send fd = %d.",fd);
       return ; 
    }
	
    Tbox__Net__TopMessage TopMsg ;
    Tbox__Net__MsgResult result;
    tbox__net__top_message__init( &TopMsg );
    tbox__net__msg_result__init( &result );

	unsigned char nettype;
            
    nettype = nm_get_net_type();

    if( at_get_sim_status() == 2 )
    {
         TopMsg.signal_type = TBOX__NET__SIGNAL_TYPE__NONE_SIGNAL;
    }
    else
    {
         if(nettype == 0)
         {
             TopMsg.signal_type = TBOX__NET__SIGNAL_TYPE__GSM;
         }
         else if(nettype == 2)
         {
             TopMsg.signal_type = TBOX__NET__SIGNAL_TYPE__UMTS;
          }
          else if(nettype == 7)
          {
              TopMsg.signal_type = TBOX__NET__SIGNAL_TYPE__LTE;
          }
          else
          {
              TopMsg.signal_type = TBOX__NET__SIGNAL_TYPE__NONE_SIGNAL;
          }
     }		
    TopMsg.message_type = TBOX__NET__MESSAGETYPE__RESPONSE_NETWORK_SIGNAL_STRENGTH;
    result.result = true;
	TopMsg.signal_power = temp;
    TopMsg.msg_result = &result;
	log_o(LOG_IVI,"power = %d",TopMsg.signal_power);
    szlen = tbox__net__top_message__get_packed_size( &TopMsg );

    tbox__net__top_message__pack(&TopMsg,pro_buf);
    
    memcpy(send_buf,IVI_PKG_MARKER,IVI_PKG_S_MARKER_SIZE);

    send_buf[IVI_PKG_S_MARKER_SIZE] = szlen >> 8;
    send_buf[IVI_PKG_S_MARKER_SIZE + 1] = szlen;

    for( i = 0; i < szlen; i ++ )
    {
        send_buf[ i + IVI_PKG_S_MARKER_SIZE + 2 ] = pro_buf[i];
    }

    memcpy(( send_buf + IVI_PKG_S_MARKER_SIZE + szlen + 2),IVI_PKG_ESC,IVI_PKG_E_MARKER_SIZE);

    ret = send(fd, send_buf, (IVI_PKG_S_MARKER_SIZE + IVI_PKG_E_MARKER_SIZE + IVI_PKG_MSG_LEN + szlen), 0);


    if (ret < (IVI_PKG_S_MARKER_SIZE + IVI_PKG_E_MARKER_SIZE + IVI_PKG_MSG_LEN + szlen))
    {
        log_e(LOG_IVI, "ivi signalpower send response failed!!!");
    }
    else
    {
        log_i(LOG_IVI, "ivi signalpower send response success");
    }

    return;

}

void ivi_callstate_response_send(int fd  )
{
    int i = 0;
    int ret = 0;
    size_t szlen = 0;
    char send_buf[4096] = {0};
    unsigned char pro_buf[2048] = {0};
	
    if( fd < 0 )
    {
       log_e(LOG_IVI,"ivi_signalpower_response_send fd = %d.",ivi_clients[0].fd);
       return ;
    }
	
    Tbox__Net__TopMessage TopMsg ;
    Tbox__Net__CallStatus callstate;
    tbox__net__top_message__init( &TopMsg );
	tbox__net__call_status__init( &callstate );
    
	TopMsg.message_type = TBOX__NET__MESSAGETYPE__RESPONSE_CALL_STATUS;
	if( 1 == callrequest.ecall)
	{
		callstate.type = TBOX__NET__CALL_TYPE__ECALL;
	}
	else if ( 1 == callrequest.bcall )
	{
		callstate.type = TBOX__NET__CALL_TYPE__BCALL;
	}
	else
	{
		callstate.type = TBOX__NET__CALL_TYPE__ICALL;
	}
	
	if( 0x01 == assist_get_call_status())
	{
		//来电
		callstate.call_status = TBOX__NET__CALL_STATUS_ENUM__CALL_IN;
	}
	else if( 0x03 == assist_get_call_status())
	{
		//去电
		callstate.call_status = TBOX__NET__CALL_STATUS_ENUM__CALL_OUT;
	}
	else if( 0x04 == assist_get_call_status())
	{
		//接通
		callstate.call_status = TBOX__NET__CALL_STATUS_ENUM__CALL_CONNECTED;
	}
	else 
	{
		//空闲(挂断)
		callstate.call_status = TBOX__NET__CALL_STATUS_ENUM__CALL_DISCONNECTED;
	}
		

	TopMsg.call_status = &callstate;
    
    szlen = tbox__net__top_message__get_packed_size( &TopMsg );

    tbox__net__top_message__pack(&TopMsg,pro_buf);
    
    memcpy(send_buf,IVI_PKG_MARKER,IVI_PKG_S_MARKER_SIZE);

    send_buf[IVI_PKG_S_MARKER_SIZE] = szlen >> 8;
    send_buf[IVI_PKG_S_MARKER_SIZE + 1] = szlen;

    for( i = 0; i < szlen; i ++ )
    {
        send_buf[ i + IVI_PKG_S_MARKER_SIZE + 2 ] = pro_buf[i];
    }

    memcpy(( send_buf + IVI_PKG_S_MARKER_SIZE + szlen + 2),IVI_PKG_ESC,IVI_PKG_E_MARKER_SIZE);

    ret = send(fd, send_buf, (IVI_PKG_S_MARKER_SIZE + IVI_PKG_E_MARKER_SIZE + IVI_PKG_MSG_LEN + szlen), 0);


    if (ret < (IVI_PKG_S_MARKER_SIZE + IVI_PKG_E_MARKER_SIZE + IVI_PKG_MSG_LEN + szlen))
    {
        log_e(LOG_IVI, "ivi signalpower send response failed!!!");
    }
    else
    {
        log_i(LOG_IVI, "ivi signalpower send response success");
    }

    return;

}

void ivi_gps_response_send( int fd )
{
    int i = 0;
    int ret = 0;
    size_t szlen = 0;

    char send_buf[4096] = {0};
    unsigned char pro_buf[2048] = {0};
    unsigned char nmea[1024] = {0};

    if( fd < 0 )
    {
        log_e(LOG_IVI,"ivi_gps_response_send fd = %d.",fd);
        return ;
    }

    memset(nmea,0,sizeof(nmea));
    
    gps_get_nmea( nmea );

    if( nmea[0] == 0 )
    {
        return;
    }

    log_i(LOG_IVI, "nmea = %s.",nmea);
    
    Tbox__Net__TopMessage TopMsg ;
    Tbox__Net__TboxGPSInfo gps;
    Tbox__Net__MsgResult result;

    tbox__net__top_message__init( &TopMsg );
    tbox__net__tbox_gpsinfo__init(&gps);
    tbox__net__msg_result__init( &result );

    gps.nmea = (char *)nmea;
    TopMsg.tbox_gpsinfo = &gps;

    TopMsg.message_type = TBOX__NET__MESSAGETYPE__RESPONSE_TBOX_GPSINFO_RESULT;

    result.result = true;
    TopMsg.msg_result = &result;

    szlen = tbox__net__top_message__get_packed_size( &TopMsg );

    tbox__net__top_message__pack(&TopMsg,pro_buf);
    
    memcpy(send_buf,IVI_PKG_MARKER,IVI_PKG_S_MARKER_SIZE);

    send_buf[IVI_PKG_S_MARKER_SIZE] = szlen >> 8;
    send_buf[IVI_PKG_S_MARKER_SIZE + 1] = szlen;

    for( i = 0; i < szlen; i ++ )
    {
        send_buf[ i + IVI_PKG_S_MARKER_SIZE + 2 ] = pro_buf[i];
    }

    memcpy(( send_buf + IVI_PKG_S_MARKER_SIZE + szlen + 2),IVI_PKG_ESC,IVI_PKG_E_MARKER_SIZE);

    ret = send(fd, send_buf, (IVI_PKG_S_MARKER_SIZE + IVI_PKG_E_MARKER_SIZE + IVI_PKG_MSG_LEN + szlen), 0);


    if (ret < (IVI_PKG_S_MARKER_SIZE + IVI_PKG_E_MARKER_SIZE + IVI_PKG_MSG_LEN + szlen))
    {
        log_e(LOG_IVI, "ivi gps send response failed!!!");
    }
    else
    {
        log_i(LOG_IVI, "ivi gps send response success");
    }

    return;

}


void ivi_msg_response_send( int fd ,Tbox__Net__Messagetype id)
{
    int i = 0;
    int ret = 0;
    size_t szlen = 0;

    unsigned char send_buf[4096] = {0};
    unsigned char pro_buf[2048] = {0};

    if( fd < 0 )
    {
        log_e(LOG_IVI,"ivi_msg_response_send fd = %d.",fd);
        return ;
    }
    
    Tbox__Net__TopMessage TopMsg;
    Tbox__Net__MsgResult result;
		
    tbox__net__top_message__init( &TopMsg );
    tbox__net__msg_result__init( &result );
	
    switch( id )
    {
        case TBOX__NET__MESSAGETYPE__REQUEST_RESPONSE_NONE:
        {
            break;
        }

        case TBOX__NET__MESSAGETYPE__REQUEST_HEARTBEAT_SIGNAL:
        {
            TopMsg.message_type = TBOX__NET__MESSAGETYPE__RESPONSE_HEARTBEAT_RESULT;
            result.result = true;
            break;
        }

        case TBOX__NET__MESSAGETYPE__REQUEST_NETWORK_SIGNAL_STRENGTH:
        {
            unsigned char nettype;
            
            TopMsg.message_type = TBOX__NET__MESSAGETYPE__RESPONSE_NETWORK_SIGNAL_STRENGTH;

            nettype = nm_get_net_type();

            if( at_get_sim_status() == 2 )
            {
                TopMsg.signal_type = TBOX__NET__SIGNAL_TYPE__NONE_SIGNAL;
            }
            else
            {
                if(nettype == 0)
                {
                    TopMsg.signal_type = TBOX__NET__SIGNAL_TYPE__GSM;
                }
                else if(nettype == 2)
                {
                    TopMsg.signal_type = TBOX__NET__SIGNAL_TYPE__UMTS;
                }
                else if(nettype == 7)
                {
                    TopMsg.signal_type = TBOX__NET__SIGNAL_TYPE__LTE;
                }
                else
                {
                    TopMsg.signal_type = TBOX__NET__SIGNAL_TYPE__NONE_SIGNAL;
                }
				signalpower = nm_get_signal();
				TopMsg.signal_power = nm_get_signal();
            }
			
            result.result = true;
            
            break;
        }

        case TBOX__NET__MESSAGETYPE__REQUEST_CALL_ACTION:
        {
            TopMsg.message_type = TBOX__NET__MESSAGETYPE__RESPONSE_CALL_ACTION_RESULT;

            Tbox__Net__CallActionResult call_request;

            tbox__net__call_action_result__init( &call_request );

      		if(1 == callrequest.ecall)
      		{
      			call_request.type = TBOX__NET__CALL_TYPE__ECALL;	
      		}
			else if( 1 == callrequest.bcall)
			{
				call_request.type = TBOX__NET__CALL_TYPE__BCALL;
			}
			else
			{
				call_request.type = TBOX__NET__CALL_TYPE__ICALL;
			}

			if( 1 == callrequest.action)
			{
				call_request.action = TBOX__NET__CALL_ACTION_ENUM__START_CALL;	
			}
			else
			{
				call_request.action = TBOX__NET__CALL_ACTION_ENUM__END_CALL;	
			}
			call_request.result = TBOX__NET__CALL_ACTION_RESULT_ENUM__CALL_ACTION_SUCCESS; 
			TopMsg.call_result = &call_request;
			result.result = true;
            break;
        }
		
		case TBOX__NET__MESSAGETYPE__REQUEST_TBOX_INFO:
		{

			TopMsg.message_type = TBOX__NET__MESSAGETYPE__RESPONSE_TBOX_INFO;

			Tbox__Net__TboxInfo tboxinfo;
			tbox__net__tbox_info__init(&tboxinfo);
			
			tboxinfo.software_version = "1234567890";
			tboxinfo.hardware_version = "1234567890";
			tboxinfo.iccid  = "1234567890";
			tboxinfo.pdid = "1234567890";
			tboxinfo.imei = "1234567890";
			tboxinfo.vin = "1234567890";

			TopMsg.tbox_info = &tboxinfo;

			log_o(LOG_IVI, "tbox info");
			result.result = true;
			break;
		}
		
        case TBOX__NET__MESSAGETYPE__REQUEST_TBOX_GPS_SET:
        {
            TopMsg.message_type = TBOX__NET__MESSAGETYPE__RESPONSE_TBOX_GPS_SET_RESULT;
            result.result = true;
            break;
        }
		
#if 0
		case TBOX__NET__MESSAGETYPE__REQUEST_TBOX_REMOTEDIAGNOSE:
		{
			TopMsg.message_type = TBOX__NET__MESSAGETYPE__RESPONSE_TBOX_REMOTEDIAGNOSE_RESUL;
			result.result = true;
			break;
		}
#endif       
        default:
        {

        }
    }

    TopMsg.msg_result = &result;

    szlen = tbox__net__top_message__get_packed_size( &TopMsg );
	
    tbox__net__top_message__pack(&TopMsg,pro_buf);

    memcpy(send_buf,IVI_PKG_MARKER,IVI_PKG_S_MARKER_SIZE);

    send_buf[IVI_PKG_S_MARKER_SIZE] = szlen >> 8;
    send_buf[IVI_PKG_S_MARKER_SIZE + 1] = szlen;

    for( i = 0; i < szlen; i ++ )
    {
        send_buf[ i + IVI_PKG_S_MARKER_SIZE + 2 ] = pro_buf[i];
    }
	
    memcpy(( send_buf + IVI_PKG_S_MARKER_SIZE + szlen + 2),IVI_PKG_ESC,IVI_PKG_E_MARKER_SIZE);
    ret = send(fd, send_buf, (IVI_PKG_S_MARKER_SIZE + IVI_PKG_E_MARKER_SIZE + IVI_PKG_MSG_LEN + szlen), 0);
	
    if (ret < (IVI_PKG_S_MARKER_SIZE + IVI_PKG_E_MARKER_SIZE + IVI_PKG_MSG_LEN + szlen))
    {
        log_e(LOG_IVI, "ivi msg send response failed!!!");
    }
    else
    {
        log_i(LOG_IVI, "ivi msg send response success.");
    }
	

}


void ivi_msg_request_process(unsigned char *data, int len,int fd)
{
    int i = 0;
    short msg_len = 0;
    short msg_len1 = 0;
    short msg_len2 = 0;
    unsigned char proto_buf[2048] = {0};

    Tbox__Net__TopMessage *TopMsg;
    
    log_buf_dump(LOG_IVI, "IVI MSG", (const uint8_t *)data, len);

    msg_len1 = data[ IVI_PKG_S_MARKER_SIZE ];
    msg_len2 = data[ IVI_PKG_S_MARKER_SIZE + 1 ];

    msg_len = (msg_len1 << 8) + msg_len2;

    log_o(LOG_IVI,"msg_len = %d.",msg_len);

    for(i = 0; i < msg_len; i ++ )
    {
        proto_buf[i] = data[ i + IVI_PKG_S_MARKER_SIZE + IVI_PKG_MSG_LEN ];
    }

    log_buf_dump(LOG_IVI, "PROTO BUF", proto_buf, msg_len);

    TopMsg = tbox__net__top_message__unpack(NULL, msg_len , proto_buf);

    if( NULL == TopMsg )
    {
        log_e(LOG_IVI,"tbox__net__top_message__unpack failed !!!");
        return;
    }

    log_o(LOG_IVI,"TopMsg->message_type = %d.",TopMsg->message_type);

    switch( TopMsg->message_type )
    {
        case TBOX__NET__MESSAGETYPE__REQUEST_RESPONSE_NONE:
        {
            break;
        }
		
        case TBOX__NET__MESSAGETYPE__REQUEST_HEARTBEAT_SIGNAL:
        {
            ivi_msg_response_send( fd ,TBOX__NET__MESSAGETYPE__REQUEST_HEARTBEAT_SIGNAL);
            break;
        }
		
        case TBOX__NET__MESSAGETYPE__REQUEST_NETWORK_SIGNAL_STRENGTH:
        {
        	network_onoff = 1;
            ivi_msg_response_send( fd ,TBOX__NET__MESSAGETYPE__REQUEST_NETWORK_SIGNAL_STRENGTH);
            break;
        }
		
        case TBOX__NET__MESSAGETYPE__REQUEST_CALL_ACTION:
        {
        	memset(&callrequest,0 ,sizeof(ivi_callrequest));
            switch( TopMsg->call_action->type )
            {
                case TBOX__NET__CALL_TYPE__ECALL:  //车机语音触发ECALL
                {
                	callrequest.ecall = 1;
                    if( TBOX__NET__CALL_ACTION_ENUM__START_CALL == TopMsg->call_action->action )
                    {
                   		callrequest.action = 1;  
                    }
                    else if( TBOX__NET__CALL_ACTION_ENUM__END_CALL == TopMsg->call_action->action )
                    {
                     	callrequest.action = 2;
                    }
                    ivi_msg_response_send( fd ,TBOX__NET__MESSAGETYPE__REQUEST_CALL_ACTION);
					ivi_remotediagnos_response_send( fd ); //ECALL触发，下发远程诊断
                    break;
                }

                case TBOX__NET__CALL_TYPE__BCALL:
                {
                	callrequest.bcall = 1;
                    if( TBOX__NET__CALL_ACTION_ENUM__START_CALL == TopMsg->call_action->action )
                    {
                    	callrequest.action = 1;
                    }
                    else if( TBOX__NET__CALL_ACTION_ENUM__END_CALL == TopMsg->call_action->action )
                    {
                        callrequest.action = 2;
                    }
                    ivi_msg_response_send( fd ,TBOX__NET__MESSAGETYPE__REQUEST_CALL_ACTION);
                    break;
                }

                case TBOX__NET__CALL_TYPE__ICALL:
                {
                	callrequest.icall = 1;
                    if( TBOX__NET__CALL_ACTION_ENUM__START_CALL == TopMsg->call_action->action )
                    {
                        callrequest.action = 1;
                    }
                    else if( TBOX__NET__CALL_ACTION_ENUM__END_CALL == TopMsg->call_action->action )
                    {
                        callrequest.action = 2;
                    }

                    ivi_msg_response_send( fd ,TBOX__NET__MESSAGETYPE__REQUEST_CALL_ACTION);
                    break;
                }

                default:
                {
                    log_e(LOG_IVI,"unkonw call action type!!!");
                    break;
                }
            }
            break;
        }
		case TBOX__NET__MESSAGETYPE__REQUEST_TBOX_INFO:
		{
			ivi_msg_response_send( fd ,TBOX__NET__MESSAGETYPE__REQUEST_TBOX_INFO);
			break;
		}
        
        case TBOX__NET__MESSAGETYPE__REQUEST_TBOX_GPS_SET:
        {   
            log_o(LOG_IVI,"onoff sta %d.",TopMsg->tbox_gps_ctrl->onoff);
            switch ( TopMsg->tbox_gps_ctrl->onoff )
            {
                case TBOX__NET__GPS__SEND__ON_OFF__GPS_ON:
                {
                    gps_onoff = 1;
                    log_o(LOG_IVI,"gps onoff open...");
                    
                    ivi_msg_response_send( fd ,TBOX__NET__MESSAGETYPE__REQUEST_TBOX_GPS_SET);

                    break;
                }

                case TBOX__NET__GPS__SEND__ON_OFF__GPS_OFF:
                {
                    gps_onoff = 0;

                    log_o(LOG_IVI,"gps onoff stop...");
                    
                    ivi_msg_response_send( fd ,TBOX__NET__MESSAGETYPE__REQUEST_TBOX_GPS_SET);

                    break;
                }

                case TBOX__NET__GPS__SEND__ON_OFF__GPS_ONCE:
                {
                    log_o(LOG_IVI,"gps nmea send once...");
                    ivi_gps_response_send( fd );

                    break;
                }

                default:
                {
                    log_e(LOG_IVI,"unkonw gps onoff type!!!");
                    break;
                }
            }
            break;
        }
		//车机回复TBOX远程诊断结果
       case TBOX__NET__MESSAGETYPE__RESPONSE_TBOX_REMOTEDIAGNOSE_RESULT:
	   {
	   		if(TopMsg->msg_result->result == true)
	   		{
				log_o(LOG_IVI,"remotediagnos success...");
	   		}
			break;
	   }
         
        default:
        {
            log_e(LOG_IVI,"recv ivi unknown message type!!!");
            break;
        }
    }
    
}


void ivi_tcp_protobuf_process(unsigned char *data, unsigned int datalen, void *para)
{
    int fd = *(int *)para;

    log_o(LOG_IVI,"ivi_tcp_protobuf_process fd = %d.",fd);

    ivi_msg_request_process( data, datalen ,fd);

    return;
}



int tbox_ivi_create_tcp_socket(void)
{
    int i = 0;
    struct sockaddr_in serv_addr;

    socklen_t serv_addr_len = 0;

    for (i = 0; i < MAX_IVI_NUM; i++)
    {
        ivi_clients[i].fd = -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    tcp_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (tcp_fd < 0)
    {
        log_e(LOG_IVI, "Fail to socket,error:%s", strerror(errno));
        return -1;
    }

    bzero(&serv_addr, sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(IVI_SERVER_PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    unsigned int value = 1;

    if (setsockopt(tcp_fd, SOL_SOCKET, SO_REUSEADDR,(void *)&value, sizeof(value)) < 0)
    {
        log_e(LOG_IVI, "Fail to setsockop, terror:%s", strerror(errno));
        return -2;
    }


    serv_addr_len = sizeof(serv_addr);

    if (bind(tcp_fd, (struct sockaddr *)&serv_addr, serv_addr_len) < 0)
    {
        log_e(LOG_IVI, "Fail to bind,error:%s", strerror(errno));
        return -3;
    }

    if (listen(tcp_fd, MAX_IVI_NUM) < 0)
    {
        log_e(LOG_IVI, "Fail to listen,error:%s", strerror(errno));
        return -4;
    }

    int flags = fcntl(tcp_fd, F_GETFL, 0);  
    fcntl(tcp_fd, F_SETFL, flags | O_NONBLOCK);

    log_o(LOG_IVI, "IVI module create server socket success");

    return 0;
}


int ivi_init(INIT_PHASE phase)
{
    int ret = 0;
    int i = 0;
    switch (phase)
    {
        case INIT_PHASE_INSIDE:
            {
                for (i = 0; i < MAX_IVI_NUM; i++)
                {
                    ivi_clients[i].fd = -1;
                }

                break;
            }

        case INIT_PHASE_RESTORE:
            break;

        case INIT_PHASE_OUTSIDE:
            {
                ret = tm_create(TIMER_REL, IVI_MSG_GPS_EVENT, MPU_MID_IVI, &ivi_timer);

                if (ret != 0)
                {
                    log_e(LOG_IVI, "create timer IVI_MSG_GPS_EVENT failed ret=0x%08x", ret);
                    return ret;
                }
                break;
            }
    }

    return 0;
}

void *ivi_main(void)
{
    int max_fd, tcom_fd, ret;
    TCOM_MSG_HEADER msghdr;
    fd_set read_set;
    static MSG_RX rx_msg[MAX_IVI_NUM];
	struct timeval timeout;

    short i = 0;
    struct sockaddr_in cli_addr;
    int new_conn_fd = -1;

	timeout.tv_sec = 0;
	timeout.tv_usec = 10000;  //select函数10ms延时
    prctl(PR_SET_NAME, "IVI");

    FD_ZERO(&read_set);

    memset(&cli_addr, 0, sizeof(cli_addr));

    for (i = 0; i < MAX_IVI_NUM; i++)
    {
        msg_init_rx(&rx_msg[i], recv_buf[i], sizeof(recv_buf[i]));
    }

    tcom_fd = tcom_get_read_fd(MPU_MID_IVI);

    if (tcom_fd  < 0)
    {
        log_e(LOG_IVI, "tcom_get_read_fd failed");
        return NULL;
    }

    ret = tbox_ivi_create_tcp_socket();

    if( ret != 0 )
    {
        if (tcp_fd < 0)
        {
            close(tcp_fd);
            tcp_fd = -1;

            log_e(LOG_IVI,"tbox_ivi_create_tcp_socket failed!!!");

            return NULL;
        }
    }

    while (1)
    {
        FD_ZERO(&read_set);
        FD_SET(tcom_fd, &read_set);

        if( tcp_fd > 0 )
        {
            FD_SET(tcp_fd, &read_set);
        }

        max_fd = tcom_fd > tcp_fd ? tcom_fd : tcp_fd;

        for (i = 0; i < MAX_IVI_NUM; i++)
        {
            if (ivi_clients[i].fd <= 0)
            {
                continue;
            }

            FD_SET(ivi_clients[i].fd, &read_set);

            if (max_fd < ivi_clients[i].fd)
            {
                max_fd = ivi_clients[i].fd;
            }

            log_i(LOG_IVI, "client_fd[%d]=%d", i, ivi_clients[i].fd);
        }

        /* monitor the incoming data */
        ret = select(max_fd + 1, &read_set, NULL, NULL, &timeout);

        /* the file deccriptor is readable */
        if (ret > 0)
        {
            if(FD_ISSET(tcom_fd, &read_set))
            {
                ret = tcom_recv_msg(MPU_MID_IVI, &msghdr, ivi_msgbuf);

                if (ret != 0)
                {
                    log_e(LOG_IVI, "tcom_recv_msg failed,ret:0x%08x", ret);
                    continue;
                }

                if (MPU_MID_TIMER == msghdr.sender)
                {
                    if( IVI_MSG_GPS_EVENT == msghdr.msgid )
                    {

                    }
                }
                else if (MPU_MID_MID_PWDG == msghdr.msgid)
                {
                    pwdg_feed(MPU_MID_IVI);
                }
            }

            if (FD_ISSET(tcp_fd, &read_set))
            {
                socklen_t len = sizeof(cli_addr);

                new_conn_fd = accept(tcp_fd, (struct sockaddr *)&cli_addr, &len);
                
                log_o(LOG_IVI, "new client comes ,fd = %d", new_conn_fd);

                if (new_conn_fd < 0)
                {
                    log_e(LOG_IVI, "socket accept failed!!!");
                    continue;
                }
                else
                {
                    for (i = 0; i < MAX_IVI_NUM; i++)
                    {
                        if (ivi_clients[i].fd == -1)
                        {
                            ivi_clients[i].fd = new_conn_fd;
                            ivi_clients[i].addr = cli_addr;
                            ivi_clients[i].lasthearttime = tm_get_time();
                            log_o(LOG_IVI, "add client_fd[%d] = %d", i, ivi_clients[i].fd);
                            break;
                        }
                    }

                    if (i >= MAX_IVI_NUM)
                    {
                        close(new_conn_fd);
                    }
                }
            }

            {
                for (i = 0; i < MAX_IVI_NUM; i++)
                {
                    int num = 0;

                    if (-1 == ivi_clients[i].fd)
                    {
                        continue;
                    }

                    if (tm_get_time() - ivi_clients[i].lasthearttime > 30000)
                    {
                        close(ivi_clients[i].fd);
                        ivi_clients[i].fd = -1;
                    }
					
					
                    if (FD_ISSET(ivi_clients[i].fd, &read_set))
                    {
                        log_i(LOG_IVI, "start read Client(%d) :%d\n", i, ivi_clients[i].fd);

                        if (rx_msg[i].used >= rx_msg[i].size)
                        {
                            rx_msg[i].used =  0;
                        }
						
                        num = recv(ivi_clients[i].fd, (rx_msg[i].data + rx_msg[i].used), rx_msg[i].size - rx_msg[i].used, 0);
					
                        if (num > 0)
                        {
                            ivi_clients[i].lasthearttime = tm_get_time();
                            rx_msg[i].used += num;
                            log_buf_dump(LOG_IVI, "tcp recv", rx_msg[i].data, rx_msg[i].used);
                            ivi_msg_decodex(&rx_msg[i], ivi_tcp_protobuf_process, &ivi_clients[i].fd);
                        }
                        else
                        {
                            if (num == 0 && (EINTR != errno))
                            {
                                log_e(LOG_IVI, "TCP client disconnect!!!!");
                            }

                            log_e(LOG_IVI, "Client(%d) exit\n", ivi_clients[i].fd);
                            close(ivi_clients[i].fd);
                            ivi_clients[i].fd = -1;
                        }
						
                    }
                }
            }

        }
        else if (0 == ret)   /* timeout */
        {
            //continue;   /* continue to monitor the incomging data */
        }
        else
        {
            if (EINTR == errno)  /* interrupted by signal */
            {
                continue;
            }

            log_e(LOG_IVI, "ivi_main exit, error:%s", strerror(errno));
            break;  /* thread exit abnormally */
        }
		if( 2 == flt_get_by_id(SOSBTN))
		{
			memset(&callrequest,0 ,sizeof(ivi_callrequest));
			callrequest.ecall = 1;
			callrequest.action =1;
			if(ivi_clients[0].fd > 0)
			{
				//按键触发ECALL，下发远程诊断命令
				ivi_remotediagnos_response_send( ivi_clients[0].fd );
			}
			log_o(LOG_IVI, "SOS trigger!!!!!");
		}
		if(ivi_clients[0].fd > 0)  //轮询任务：信号强度、电话状态、绑车激活、远程诊断、
		{
			
			if( 1 == tbox_ivi_get_network_onoff() ) //已经请求网络制式
			{
				ivi_signalpower_response_send( ivi_clients[0].fd ); //如果信号强度变化，传给车机
			}
//			if()   //电话成功拨出
//			{
//				ivi_callstate_response_send(ivi_clients[0].fd );  //电话状态变化，传给车机
//			}
//			if()  //判断TSP是否下发激活信息
//			{
//				ivi_activestate_response_send( ivi_clients[0].fd ); //通知车机激活成功
//			}
//			if()   //TSP是否下发远程命令
//			{
//				ivi_remotediagnos_response_send( ivi_clients[0].fd );
//			}
				
		}
	
    }

    return NULL;
}

/****************************************************************
function:     assist_run
description:  startup data communciation module
input:        none
output:       none
return:       0 indicates success;
              others indicates failed
*****************************************************************/
int ivi_run(void)
{
    int ret;
    pthread_attr_t ta;

    pthread_attr_init(&ta);
    pthread_attr_setdetachstate(&ta, PTHREAD_CREATE_DETACHED); //分离线程属性

    /* create thread and monitor the incoming data */
    ret = pthread_create(&ivi_tid, &ta, (void *)ivi_main, NULL);

    if (ret != 0)
    {
        log_e(LOG_IVI, "pthread_create failed, error:%s", strerror(errno));
        return ret;
    }

    return 0;
}
