
#ifndef __BT_DUN_UTILS_H__
#define __BT_DUN_UTILS_H__

#if defined (__BTMTK__) && defined (__BT_DUN_PROFILE__)

typedef struct _GSList GSList;
typedef void* gpointer;
typedef const void *gconstpointer;

int ip_dun_create_netdev __P((char *, char *)); /* provide the raw socket initialization */
void ip_dun_remove_netdev __P((void));
void ip_dun_setfd __P((int));

int  pending_packet_list_len __P((void)); /* get length of PPP pending packet list */
int  read_pending_packet __P((u_char **)); /* Read PPP pending packet */
int  loop_packet __P((u_char *, int)); /* append a PPP packet to the pending packet list */
void  remove_pending_packet __P((void)); /* remove the first PPP pending packet */

int pppbt_decode_packet __P((unsigned char *, int)); /* process buffer from read_packet */
int pppbt_encode_packet __P((unsigned char *, int)); /* process buffer in output */

int g_slist_length __P((GSList *));
GSList* g_slist_last __P((GSList *));
GSList* g_slist_append __P((GSList *, gpointer));
GSList* g_slist_remove __P((GSList *, gconstpointer));
#endif

#endif
