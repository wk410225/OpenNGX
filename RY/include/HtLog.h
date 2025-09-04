#ifndef __HT_LOG_H
#define __HT_LOG_H
#define LOG_NAME_LEN_MAX	      64	
#define HT_LOG_MODE_OFF				0
#define HT_LOG_MODE_ERROR			1
#define HT_LOG_MODE_NORMAL			2
#define HT_LOG_MODE_DEBUG			3

#define CONFIG "lyy_posp/etc/pospcfg.ini"

#define LOG_SEQ_ID_LEN          4
extern char gsLogSeqId[LOG_SEQ_ID_LEN+1];
extern char gsLogFileName[LOG_NAME_LEN_MAX];
/**********
#define HT_LOG_MODE_OFF				0
#define HT_LOG_MODE_ERROR			0
#define HT_LOG_MODE_NORMAL			0
#define HT_LOG_MODE_DEBUG			0
***/
extern char  gsLogFile[LOG_NAME_LEN_MAX];
/*****************************************************************************/
/* FUNC:   int HtLog (char *sLogName, int nLogMode, char *sFileName,         */
/*                    int nLine, char *sFmt, ...);                           */
/* INPUT:  sLogName: ��־��, ����·��                                        */
/*         nLogMode: ��־����,                                               */
/*                   HT_LOG_MODE_ERROR,HT_LOG_MODE_NORMAL,HT_LOG_MODE_DEBUG  */
/*         sFileName: �����Դ������                                         */
/*         nLine: �����Դ������к�                                         */
/*         sFmt: ������Ϣ                                                    */
/* OUTPUT: ��                                                                */
/* RETURN: 0: �ɹ�, ����: ʧ��                                               */
/* DESC:   ����LOG_MODE, ���ü���֮�µ���־��¼����־�ļ���,                 */
/*****************************************************************************/
int HtLog (char *sLogName, int nLogMode, char *sFileName, int nLine, char *sFmt, ...);

/*****************************************************************************/
/* FUNC:   int HtDebugString (char *sLogName, int nLogMode, char *sFileName, */
/*                            int nLine, char *psBuf, int nBufLen);          */
/* INPUT:  sLogName: ��־��, ����·��                                        */
/*         nLogMode: ��־����,                                               */
/*                   HT_LOG_MODE_ERROR,HT_LOG_MODE_NORMAL,HT_LOG_MODE_DEBUG  */
/*         sFileName: �����Դ������                                         */
/*         nLine: �����Դ������к�                                         */
/*         psBuf: �������buffer                                             */
/*         nBufLen: buffer�ĳ���                                             */
/* OUTPUT: ��                                                                */
/* RETURN: 0: �ɹ�, ����: ʧ��                                               */
/* DESC:   ����LOG_MODE, ���ü���֮�µ���־��¼����־�ļ���,                 */
/*         ���������buffer��16����ֵ                                        */
/*****************************************************************************/
int HtDebugString (char *sLogName, int nLogMode, char *sFileName, int nLine, char *psBuf, int nBufLen);

#endif
