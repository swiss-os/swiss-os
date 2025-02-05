/*
	
	Small web server
	Copyright (c) 1998 O'ksi'D

 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 O'KSI'D BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 Except as contained in this notice, the name of the O'ksi'D shall
 not be used in advertising or otherwise to promote the sale, use or other
 dealings in this Software without prior written authorization from O'ksi'D.

 *
 * gcc -g -Wall -o httpd httpd.c
 */

#ifdef _WIN32
#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>

#define BUFSIZE 4096 
 
HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;

HANDLE g_hInputFile = NULL;
 
void CreateChildProcess(void); 
void WriteToPipe(void); 
void ReadFromPipe(void); 
void ErrorExit(PCTSTR); 
 
int _tmain(int argc, TCHAR *argv[]) 
{ 
   SECURITY_ATTRIBUTES saAttr; 
 
   printf("\n->Start of parent execution.\n");

// Set the bInheritHandle flag so pipe handles are inherited. 
 
   saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
   saAttr.bInheritHandle = TRUE; 
   saAttr.lpSecurityDescriptor = NULL; 

// Create a pipe for the child process's STDOUT. 
 
   if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) ) 
      ErrorExit(TEXT("StdoutRd CreatePipe")); 

// Ensure the read handle to the pipe for STDOUT is not inherited.

   if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
      ErrorExit(TEXT("Stdout SetHandleInformation")); 

// Create a pipe for the child process's STDIN. 
 
   if (! CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0)) 
      ErrorExit(TEXT("Stdin CreatePipe")); 

// Ensure the write handle to the pipe for STDIN is not inherited. 
 
   if ( ! SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0) )
      ErrorExit(TEXT("Stdin SetHandleInformation")); 
 
// Create the child process. 
   
   CreateChildProcess();

// Get a handle to an input file for the parent. 
// This example assumes a plain text file and uses string output to verify data flow. 
 
   if (argc == 1) 
      ErrorExit(TEXT("Please specify an input file.\n")); 

   g_hInputFile = CreateFile(
       argv[1], 
       GENERIC_READ, 
       0, 
       NULL, 
       OPEN_EXISTING, 
       FILE_ATTRIBUTE_READONLY, 
       NULL); 

   if ( g_hInputFile == INVALID_HANDLE_VALUE ) 
      ErrorExit(TEXT("CreateFile")); 
 
// Write to the pipe that is the standard input for a child process. 
// Data is written to the pipe's buffers, so it is not necessary to wait
// until the child process is running before writing data.
 
   WriteToPipe(); 
   printf( "\n->Contents of %S written to child STDIN pipe.\n", argv[1]);
 
// Read from pipe that is the standard output for child process. 
 
   printf( "\n->Contents of child process STDOUT:\n\n");
   ReadFromPipe(); 

   printf("\n->End of parent execution.\n");

// The remaining open handles are cleaned up when this process terminates. 
// To avoid resource leaks in a larger application, close handles explicitly. 

   return 0; 
} 
 
void CreateChildProcess()
// Create a child process that uses the previously created pipes for STDIN and STDOUT.
{ 
   TCHAR szCmdline[]=TEXT("child");
   PROCESS_INFORMATION piProcInfo; 
   STARTUPINFO siStartInfo;
   BOOL bSuccess = FALSE; 
 
// Set up members of the PROCESS_INFORMATION structure. 
 
   ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
 
// Set up members of the STARTUPINFO structure. 
// This structure specifies the STDIN and STDOUT handles for redirection.
 
   ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
   siStartInfo.cb = sizeof(STARTUPINFO); 
   siStartInfo.hStdError = g_hChildStd_OUT_Wr;
   siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
   siStartInfo.hStdInput = g_hChildStd_IN_Rd;
   siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
 
// Create the child process. 
    
   bSuccess = CreateProcess(NULL, 
      szCmdline,     // command line 
      NULL,          // process security attributes 
      NULL,          // primary thread security attributes 
      TRUE,          // handles are inherited 
      0,             // creation flags 
      NULL,          // use parent's environment 
      NULL,          // use parent's current directory 
      &siStartInfo,  // STARTUPINFO pointer 
      &piProcInfo);  // receives PROCESS_INFORMATION 
   
   // If an error occurs, exit the application. 
   if ( ! bSuccess ) 
      ErrorExit(TEXT("CreateProcess"));
   else 
   {
      // Close handles to the child process and its primary thread.
      // Some applications might keep these handles to monitor the status
      // of the child process, for example. 

      CloseHandle(piProcInfo.hProcess);
      CloseHandle(piProcInfo.hThread);
      
      // Close handles to the stdin and stdout pipes no longer needed by the child process.
      // If they are not explicitly closed, there is no way to recognize that the child process has ended.
      
      CloseHandle(g_hChildStd_OUT_Wr);
      CloseHandle(g_hChildStd_IN_Rd);
   }
}
 
void WriteToPipe(void) 

// Read from a file and write its contents to the pipe for the child's STDIN.
// Stop when there is no more data. 
{ 
   DWORD dwRead, dwWritten; 
   CHAR chBuf[BUFSIZE];
   BOOL bSuccess = FALSE;
 
   for (;;) 
   { 
      bSuccess = ReadFile(g_hInputFile, chBuf, BUFSIZE, &dwRead, NULL);
      if ( ! bSuccess || dwRead == 0 ) break; 
      
      bSuccess = WriteFile(g_hChildStd_IN_Wr, chBuf, dwRead, &dwWritten, NULL);
      if ( ! bSuccess ) break; 
   } 
 
// Close the pipe handle so the child process stops reading. 
 
   if ( ! CloseHandle(g_hChildStd_IN_Wr) ) 
      ErrorExit(TEXT("StdInWr CloseHandle")); 
} 
 
void ReadFromPipe(void) 

// Read output from the child process's pipe for STDOUT
// and write to the parent process's pipe for STDOUT. 
// Stop when there is no more data. 
{ 
   DWORD dwRead, dwWritten; 
   CHAR chBuf[BUFSIZE]; 
   BOOL bSuccess = FALSE;
   HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

   for (;;) 
   { 
      bSuccess = ReadFile( g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
      if( ! bSuccess || dwRead == 0 ) break; 

      bSuccess = WriteFile(hParentStdOut, chBuf, 
                           dwRead, &dwWritten, NULL);
      if (! bSuccess ) break; 
   } 
} 
 
void ErrorExit(PCTSTR lpszFunction) 

// Format a readable error message, display a message box, 
// and exit from the application.
{ 
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(1);
}
#else
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <linux/in.h>
#include <errno.h>

#define BUF_LEN 4096
int readhead (int , char *, int);
int readdata (int , char *, int);
char buf[BUF_LEN];
int printfile (int, char *);
int server (void); 
 
int main (int argc, char *argv[]) 
{
	
	for (;;) {
		server ();
		printf ("SERVER ERROR !!!\n");
	}
}

int server (void) 
{

	int one = 1;
	struct protoent *proto;
	int svr_sock;
	struct sockaddr_in addr;
	struct sockaddr client;
	int clen;
	char *cp1, *cp2; 
	int cli_sock = -1;
	struct stat st;
	int cgiarg;
	int i = 0;

	for (i = 0; i < BUF_LEN; i++) {
		buf[i] = '\0';
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons (80);
	addr.sin_addr.s_addr = htonl (INADDR_LOOPBACK);
	/*addr.sin_addr.s_addr = htonl (INADDR_ANY);*/
	proto = getprotobyname ("tcp");
	svr_sock = socket (AF_INET, SOCK_STREAM, proto->p_proto);
	setsockopt (svr_sock, SOL_SOCKET, SO_REUSEADDR,
			(char *) &one, sizeof (int));
	bind (svr_sock, (struct sockaddr *) &addr, sizeof (struct sockaddr_in));
	listen (svr_sock, 1);
	for (;;) {
		clen = sizeof (client);
		cli_sock = accept (svr_sock, &client, &clen);
		if (cli_sock < 0 ) {
			goto error;
		}
		if (fcntl (cli_sock, F_SETFL, O_NONBLOCK) < 0) {
			goto error;
		}

		if (readhead (cli_sock, buf, BUF_LEN-1) > -1) {
			int fi = 0;
			printf ("%s\n", buf);
			/* remove "GET" of the arg line */
			cp1 = strchr (buf, ' ');
			cp1++;
			/* null mark the end of the file name */
			cp2 = strchr (cp1, ' ');
			*cp2 = 0;
			/* is it at cgi with some args ? */
			cp2 = strchr (cp1, '?');
			cgiarg = 0;
			if (cp2 > 0) { 
				*cp2 = 0;
				cgiarg = 1;
			} else { 
				cp2 = strchr (cp1, (char) 0);
			}
			st.st_size = 0;
			fi = stat (cp1, &st);
			if (fi > -1 ) { 
				fi = 1;
			}
			if (*(cp2-1) == '/') {
				int std;
				write (cli_sock, "HTTP/1.1 200 OK\n", 16);
				dprintf (cli_sock,
					"Content-type: text/html\n\n");
				dprintf (cli_sock, "<HTML><BODY><PRE>");
				dprintf (cli_sock, 
					"<B><U>Directory : \"%s\"</U></B>\n", 
					cp1);
				std = dup (1);
			 	close (1);
				dup (cli_sock);
				/* we can do that because there is the 
				   memory alocated for "GET" before the
				   file name ... */
				*(cp1-3) = 'l';
				*(cp1-2) = 's';
				system (cp1-3);
				close (1);
				dup (std);
				close (std);
				dprintf (cli_sock, "</PRE></BODY></HTML>");
			} else if (strcmp (cp2-4, ".cgi") == 0) {
				int std;
				setenv ("REQUEST_METHOD", "GET", 1);
				if (cgiarg > 0) {
					cp2++;
					setenv ("QUERY_STRING" , cp2, 1);
				} else {
					setenv ("QUERY_STRING", "", 1);
				}
				write (cli_sock, "HTTP/1.1 200 OK\n", 16);
				std = dup (1);
				close (1);
				dup (cli_sock);
				if (system (cp1) != 0) {
					dprintf(cli_sock,
                                                "Content-type: text/html\n\n"
						"<HTML><BODY><PRE>\n"
						"<FONT COLOR=RED>"
						"<B>ERROR :</B> cgi fails !!!"
						"</FONT>\n"
						"</PRE></BODY></HTML>");	
				}
				close (1);
				dup (std);
				close (std);	
			} else if ((strcmp (cp2-5, ".html") == 0) && fi > 0) {
				write (cli_sock, "HTTP/1.1 200 OK\n", 16);
				dprintf (cli_sock, "Content-length: %ld\n",
					(long) st.st_size);
				dprintf (cli_sock,
					"Content-type: text/html\n\n");
				printfile (cli_sock, cp1);
			} else if ((strcmp (cp2-4, ".png") == 0) && fi > 0) {
				errno = 0;
                                write (cli_sock, "HTTP/1.1 200 OK\n", 16);
                                dprintf (cli_sock, "Content-length: %ld\n",
                                        (long) st.st_size);
				errno = 0;
                                dprintf (cli_sock,
                                        "Content-type: image/png\n\n");
				if (errno) {
			//		goto error;
				}
                                if (printfile (cli_sock, cp1) < 0) {
					goto error;
				}
			} else if ((strcmp (cp2-4, ".gif") == 0) && fi > 0) {
				write (cli_sock, "HTTP/1.1 200 OK\n", 16);
				dprintf (cli_sock, "Content-length: %ld\n",
                                        (long) st.st_size);
                                dprintf (cli_sock,
                                        "Content-type: image/gif\n\n");
                                printfile (cli_sock, cp1);
			} else if ((strcmp (cp2-4, ".jpg") == 0) && fi > 0) {
				write (cli_sock, "HTTP/1.1 200 OK\n", 16);
                                dprintf (cli_sock, "Content-length: %ld\n",
                                        (long) st.st_size);
                                dprintf (cli_sock,
                                        "Content-type: image/jpeg\n\n");
                                printfile (cli_sock, cp1);
			} else if (fi > 0) {
				write (cli_sock, "HTTP/1.1 200 OK\n", 16);
				dprintf (cli_sock,
					"Content-type: text/plain\n\n");
				printfile (cli_sock, cp1);
			} else {
				dprintf (cli_sock, "HTTP/1.1 404 Not Found\n"
					"Connection: close\n"
					"Content-Type: text/html\n\n"
					"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD"
					" HTML 2.0//EN\">\n"
					"<HTML><HEAD>\n"
					"<TITLE>404 Not Found</TITLE>\n"
					"</HEAD><BODY>"
					"<H1>Not Found</H1>\n"
					"The requested URL was not found on"
					" this server.<P>"
					"\n<HR>\n"
					"<ADDRESS>Mini Server </ADDRESS>"
					"</BODY></HTML>\n");
			}		
		} 
		close (cli_sock);
	}
	close (svr_sock);
	return (0);
error:
	shutdown (svr_sock, 2);
	shutdown (cli_sock, 2);
	close (cli_sock);
	close (svr_sock);
	return (-1);
}

int printfile (int sock, char * file) {
	int fd;
	ssize_t si;

	fd = open (file, O_RDONLY);
	si = read (fd, buf, BUF_LEN);
	if (write (sock, buf, si) < 0) {
		close (fd);
		return (-1);
	}
	if (si < 1) {
//		if (dprintf (sock, "ERROR cannot open file ...\n") < 0) {
			close (fd);
			return (-1);
//		}
	}
	while (si > 0) {
		si = read (fd, buf, BUF_LEN);
		if (write (sock, buf, si) < 0) {
			close (fd);
			return (-1);
		}
	}
	close (fd);
	return (0);		
}

int readhead (int sock, char *buf, int size)
{
	int i = 0;

	memset (buf, 0, size);
        while (strstr (buf, "\r\n\r\n") == NULL &&
                i < size - 1)
        {
                int j;
                j = read (sock, buf+i, (size - 1) - i);
                if (i > 0 && j > 0) { 
                        i += j; 
                } else if (i < 0) {
                        return (-1);
                }
        }
	return (0);
}
#endif


