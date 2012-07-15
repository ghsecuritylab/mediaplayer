#ifndef __RTL871X_SECURITY_H_
#define __RTL871X_SECURITY_H_


#include <drv_conf.h>
#include <osdep_service.h>
#include <drv_types.h>



#define _NO_PRIVACY_	0x0
#define _WEP40_			0x1
#define _TKIP_			0x2
#define _TKIP_WTMIC_	0x3
#define _AES_			0x4
#define _WEP104_		0x5


#define _WPA_IE_ID_	0xdd
#define _WPA2_IE_ID_	0x30

#ifndef Ndis802_11AuthModeWPA2
#define Ndis802_11AuthModeWPA2 (Ndis802_11AuthModeWPANone + 1)
#endif

#ifndef Ndis802_11AuthModeWPA2PSK
#define Ndis802_11AuthModeWPA2PSK (Ndis802_11AuthModeWPANone + 2)
#endif

union pn48	{
	
	u64	val;
	
#ifdef CONFIG_LITTLE_ENDIAN

struct {
  u8 TSC0;
  u8 TSC1;
  u8 TSC2;
  u8 TSC3;
  u8 TSC4;
  u8 TSC5;
  u8 TSC6;
  u8 TSC7;
} _byte_;
 
#elif CONFIG_BIG_ENDIAN

struct {
  u8 TSC7;
  u8 TSC6;
  u8 TSC5;
  u8 TSC4;
  u8 TSC3;
  u8 TSC2;
  u8 TSC1;
  u8 TSC0;
} _byte_;
 
#endif

};

union Keytype {
        u8   skey[16];
        u32    lkey[4];
};


typedef struct _RT_PMKID_LIST
{
	u8						bUsed;
	u8 						Bssid[6];
	u8						PMKID[16];
	u8						SsidBuf[33];
	u8*						ssid_octet;
	u16 						ssid_length;
} RT_PMKID_LIST, *PRT_PMKID_LIST;


struct	security_priv	{
	
	u32	dot11AuthAlgrthm;		// 802.11 auth, could be open, shared, and 8021x
	u32	dot11PrivacyAlgrthm;	// This specify the privacy for shared auth. algorithm.
	u32	dot11PrivacyKeyIndex;	// this is only valid for legendary wep, 0~3 for key id.
	union Keytype dot11DefKey[4];			// this is only valid for def. key	
	u32	dot11DefKeylen[4];
	u32	dot118021XGrpPrivacy;	// This specify the privacy algthm. used for Grp key 
	u32	dot118021XGrpKeyid;		// key id used for Grp Key
	union Keytype	dot118021XGrpKey[2];	// 802.1x Group Key, for inx0 and inx1	
	union Keytype	dot118021XGrptxmickey;
	union Keytype	dot118021XGrprxmickey;
	union pn48		dot11Grptxpn;			// PN48 used for Grp Key xmit.
	union pn48		dot11Grprxpn;			// PN48 used for Grp Key recv.
	u8	binstallGrpkey;
	u8	bcheck_grpkey;
	u8	bgrpkey_handshake;
	s32	sw_encrypt;
	s32	sw_decrypt;
	s32 	hw_decrypted;
	u32	ndisauthtype;
	u32	ndisencryptstatus;
	NDIS_WLAN_BSSID_EX sec_bss;  //for joinbss (h2c buffer) usage
//#ifdef PLATFORM_WINDOWS
	NDIS_802_11_WEP	 ndiswep;
//#endif
	u8	assoc_info[600];
	u8	szofcapability[256]; //for wpa2 usage
	u8	oidassociation[512]; //for wpa/wpa2 usage
	u8	authenticator_ie[256];  //store ap security information element
	u8	supplicant_ie[256];  //store sta security information element
	u32	last_mic_err_time;
	u8	btkip_countermeasure;
	u8	btkip_wait_report;
	u32	btkip_countermeasure_time;

	//---------------------------------------------------------------------------
	// For WPA2 Pre-Authentication.
	//---------------------------------------------------------------------------
//	u8				RegEnablePreAuth;				// Default value: Pre-Authentication enabled or not, from registry "EnablePreAuth". Added by Annie, 2005-11-01.
//	u8				EnablePreAuthentication;			// Current Value: Pre-Authentication enabled or not.
	RT_PMKID_LIST		PMKIDList[16];	// Renamed from PreAuthKey[NUM_PRE_AUTH_KEY]. Annie, 2006-10-13.
	u8				PMKIDIndex;
//	u32				PMKIDCount;						// Added by Annie, 2006-10-13.
//	u8				szCapability[256];				// For WPA2-PSK using zero-config, by Annie, 2005-09-20.
};

#define GET_ENCRY_ALGO(psecuritypriv, psta, encry_algo, bmcst)\
do{\
	switch(psecuritypriv->dot11AuthAlgrthm)\
	{\
		case 0:\
		case 1:\
			encry_algo = (u8)psecuritypriv->dot11PrivacyAlgrthm;\
			break;\
		case 2:\
			if(bmcst)\
				encry_algo = (u8)psecuritypriv->dot118021XGrpPrivacy;\
			else\
				encry_algo =(u8) psta->dot118021XPrivacy;\
			break;\
	}\
}while(0)


#define SET_ICE_IV_LEN( iv_len, icv_len, encrypt)\
do{\
	switch(encrypt)\
	{\
		case _WEP40_:\
		case _WEP104_:\
			iv_len = 4;\
			icv_len = 4;\
			break;\
		case _TKIP_:\
			iv_len = 8;\
			icv_len = 4;\
			break;\
		case _AES_:\
			iv_len = 8;\
			icv_len = 8;\
			break;\
		default:\
			iv_len = 0;\
			icv_len = 0;\
			break;\
	}\
}while(0)


#define GET_TKIP_PN(iv,dot11txpn)\
do{\
	dot11txpn._byte_.TSC0=iv[2];\
	dot11txpn._byte_.TSC1=iv[0];\
	dot11txpn._byte_.TSC2=iv[4];\
	dot11txpn._byte_.TSC3=iv[5];\
	dot11txpn._byte_.TSC4=iv[6];\
	dot11txpn._byte_.TSC5=iv[7];\
}while(0)


#define ROL32( A, n )	( ((A) << (n)) | ( ((A)>>(32-(n)))  & ( (1UL << (n)) - 1 ) ) )
#define ROR32( A, n ) 	ROL32( (A), 32-(n) )

struct mic_data
{
	u32  K0, K1;         // Key
	u32  L, R;           // Current state
	u32  M;              // Message accumulator (single word)
	u32     nBytesInM;      // # bytes in M
};

void secmicsetkey(struct mic_data *pmicdata, u8 * key );
void secmicappendbyte(struct mic_data *pmicdata, u8 b );
void secmicappend(struct mic_data *pmicdata, u8 * src, u32 nBytes );
void secgetmic(struct mic_data *pmicdata, u8 * dst );

void seccalctkipmic(
	u8 * key,
	u8 *header,
	u8 *data,
	u32 data_len,
	u8 *Miccode,
	u8   priority);

u32 aes_encrypt(_adapter *padapter, u8 *pxmitframe);
u32 tkip_encrypt(_adapter *padapter, u8 *pxmitframe);
void wep_encrypt(_adapter *padapter, u8  *pxmitframe);

u32 aes_decrypt(_adapter *padapter, u8  *precvframe);
u32 tkip_decrypt(_adapter *padapter, u8  *precvframe);
void wep_decrypt(_adapter *padapter, u8  *precvframe);
#endif	//__RTL871X_SECURITY_H_
