

#ifndef _INTERFACE_INOUT_H_
#define _INTERFACE_INOUT_H_

#include "../../src/utils/eloop.h"
#ifdef  __cplusplus
extern "C" {
#endif

typedef enum
{
	AUTH_TYPE_NONE_WAPI = 0,	/*no WAPI	*/
	AUTH_TYPE_WAPI,			/*Certificate*/
	AUTH_TYPE_WAPI_PSK,		/*Pre-PSK*/
}wapi_auth_t;

typedef enum
{
	KEY_TYPE_ASCII = 0,			/*ascii		*/
	KEY_TYPE_HEX,				/*HEX*/
}wapi_key_t;

typedef struct
{
	wapi_auth_t authType;		/*Authentication type*/
	union
	{
		struct
		{
			wapi_key_t kt;				/*Key type*/
			unsigned int  kl;			/*key length*/
			unsigned char kv[128];/*value*/
		} ;
		struct
		{
			unsigned char as[2048];	/*ASU Certificate*/
			unsigned char user[2048];/*User Certificate*/
		} ;
	}para;
}wapi_param_t;


typedef enum
{
	CONN_ASSOC = 0,
	CONN_DISASSOC,
}CONN_STATUS;
typedef struct
{
	unsigned char v[6];
	unsigned char pad[2];
}MAC_ADDRESS;

struct wapi_cb_ctx {
	void *ctx; /* pointer to arbitrary upper level context */

	/*face to mt592x*/
	int (*msg_send)(void *priv, const u8 *msg_in, int msg_in_len, 
							 u8 *msg_out, int *msg_out_len);

	/*for testing*/
	void (*wapi_printf)(int level, char *format, ...); 

	/*send output to wpa_ctrl*/
	void (*wpa_msg)(void *ctx, int level, const char *fmt, ...);

	/*set wapi key*/
	int (*set_key)(void *ctx, int alg,
		       const u8 *addr, int key_idx, int set_tx,
		       const u8 *seq, size_t seq_len,
		       const u8 *key, size_t key_len);

	/*send WAI frame*/
	int (*ether_send)(void *ctx, const u8* pbuf, int length);

	/*set wpa_supplicant state*/
	void (*set_state)(void *ctx, int state);

	/*get wpa_supplicant state*/
	int (*get_state)(void *ctx);

	/*send deauthenticateion*/
	void (*deauthenticate)(void *ctx, int reason_code);

	/*set one-shot timer*/
	int  (*set_timer)(unsigned int secs, unsigned int usecs,
			   eloop_timeout_handler handler,
			   void *eloop_data, void *user_data);

	/*clear one-shot timer*/
	int  (*cancel_timer)(eloop_timeout_handler handler,
			 void *eloop_data, void *user_data);
};

int  wapi_lib_init(struct wapi_cb_ctx* ctx);
int  wapi_lib_exit();
int  wapi_set_user(const wapi_param_t* pPar);
void wapi_set_msg(CONN_STATUS action, const MAC_ADDRESS* pBSSID, const MAC_ADDRESS* pLocalMAC, unsigned char *assoc_ie, unsigned char assoc_ie_len);
int  wapi_set_rx_wai(const u8* pbuf, int length);
int  wapi_get_state(void);

#ifdef  __cplusplus
}
#endif

#endif /* _INTERFACE_INOUT_H_  */

