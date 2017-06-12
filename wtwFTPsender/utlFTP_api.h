#ifndef __UTL_FTP_API_H__
#define __UTL_FTP_API_H__

namespace utlFTP
{
   struct wtwFtpFile
   {
      /** struct size */
      int            structSize;
      /** path to file */
      wchar_t         filePath[260];
      /** Contact to which the file will be sent, only contactID,
        * netClass and netId (sometimes called sID) are needed
        */
      wtwContactDef   contact;
      /** not used now, should be set to 0 */
      DWORD         flags;
   };

   /** wParam = wtwFtpFile*, lParam = NULL */
   static const wchar_t WTW_FTP_SEND_FILE[] = L"utlFTP/sendFile";
};

#endif // __UTL_FTP_API_H__