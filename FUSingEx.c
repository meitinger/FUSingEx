/* Copyright (C) 2012, Manuel Meitinger
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Windows.h>
#include <Winwlx.h>
#include <Wtsapi32.h>

#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Advapi32.lib")

BOOL WINAPI LibMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
		DisableThreadLibraryCalls(hInstance);
	return TRUE;
}

BOOL EnsureFastUserSwitching()
{
	LPCTSTR lpSubKey = TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon");
	LPCTSTR lpValueName = TEXT("AllowMultipleTSSessions");
	LONG lResult;
	DWORD dwValue;
	DWORD dwSize;
	HKEY hKey;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		dwValue = 0;
		dwSize = sizeof(dwValue);
		lResult = RegQueryValueEx(hKey, lpValueName, 0, NULL, (LPBYTE)&dwValue, &dwSize);
		RegCloseKey(hKey);
		if (lResult == ERROR_SUCCESS && dwValue != 0)
			return TRUE;
	}
	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS)
	{
		dwValue = 1;
		lResult = RegSetValueEx(hKey, lpValueName, 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue));
		RegCloseKey(hKey);
		if (lResult == ERROR_SUCCESS)
			return TRUE;
	}
	return FALSE;
}

VOID CALLBACK RundllOEMLink(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, INT nCmdShow)
{
	DWORD sessionId;

	if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == WTSGetActiveConsoleSessionId())
		LockWorkStation();
	else
		WTSDisconnectSession(WTS_CURRENT_SERVER_HANDLE, WTS_CURRENT_SESSION, FALSE);
}

VOID CALLBACK WinlogonEventLock(PWLX_NOTIFICATION_INFO pInfo)
{
	DWORD sessionId;

	if
	(
		ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
		sessionId == WTSGetActiveConsoleSessionId() &&
		(!EnsureFastUserSwitching() || !WTSDisconnectSession(WTS_CURRENT_SERVER_HANDLE, sessionId, FALSE))
	)
		ExitWindowsEx(EWX_LOGOFF|EWX_FORCE, 0);
}
