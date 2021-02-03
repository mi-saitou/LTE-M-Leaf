/**
 * @file arduino_secrets.h
 * @brief LPWA leaf setting <br>
 * 
 * @date  20200520 Preview-Alpha
 * @author kt-nakamura@kddi-tech.com
 */

/*
 * PDP 接続先
 */ 
#define SECRET_GPRS_APN "soracom.io" // replace your GPRS APN
#define SECRET_GPRS_LOGIN "sora"     // replace with your GPRS login
#define SECRET_GPRS_PASSWORD "sora"  // replace with your GPRS password
/*
 * Network 接続先
 * ENDPOINT_IP はカンマ区切りで記載
 */
#define ENDPOINT_URL "beam.soracom.io"
#define ENDPOINT_PORT 23080

#define LOCAL_PORT 8888
