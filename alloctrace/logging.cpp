// Copyright (c) 2007-2009 Google Inc.
// Copyright (c) 2006-2007 Jaiku Ltd.
// Copyright (c) 2002-2006 Mika Raento and Renaud Petit
//
// This software is licensed at your choice under either 1 or 2 below.
//
// 1. MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// 2. Gnu General Public license 2.0
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//
// This file is part of the JaikuEngine mobile client.

#include <e32base.h>
#include "app_context_impl.h"
#include <windows.h>

HANDLE LogStartAny(TInt aOp) {
	CApp_context *c=(CApp_context *)GetContext();
	if (!c) return INVALID_HANDLE_VALUE;
	HANDLE logfile=c->iHookData;
	if (!logfile) {
		TBuf<100> filen=_L("c:\\");
		filen.Append(c->Name());
		filen.Append(_L("-alloctrace.dat"));
		logfile=CreateFile( filen.PtrZ(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
		c->iHookData=logfile;
	}
	if (logfile==INVALID_HANDLE_VALUE) return logfile;
	DWORD written;
	BOOL ret;
	DWORD err;
	ret=WriteFile(logfile, &aOp, sizeof(aOp), &written, 0);
	if (!ret) err=GetLastError();
	const TDesC8& stack=c->FullCallStackBuffer();
	TInt len=stack.Length();
	ret=WriteFile(logfile, &len, sizeof(len), &written, 0);
	ret=WriteFile(logfile, stack.Ptr(), stack.Length(), &written, 0);
	return logfile;
}

void LogAlloc(TAny* aResult, TInt aSize)
{
	HANDLE logfile=LogStartAny(1);
	if (logfile==INVALID_HANDLE_VALUE) return;
	TInt op;
	DWORD written;
	BOOL ret;
	op=aSize;
	ret=WriteFile(logfile, &op, sizeof(op), &written, 0);
	op=(TInt)aResult;
	ret=WriteFile(logfile, &op, sizeof(op), &written, 0);
}

void LogFree(TAny* aResult)
{
	HANDLE logfile=LogStartAny(2);
	if (logfile==INVALID_HANDLE_VALUE) return;
	TInt op;
	DWORD written;
	BOOL ret;
	op=(TInt)aResult;
	ret=WriteFile(logfile, &op, sizeof(op), &written, 0);
}

void LogReAlloc(TAny* aResult, TInt aSize, TAny* aCell)
{
	HANDLE logfile=LogStartAny(3);
	if (logfile==INVALID_HANDLE_VALUE) return;
	TInt op;
	DWORD written;
	BOOL ret;
	op=aSize;
	ret=WriteFile(logfile, &op, sizeof(op), &written, 0);
	op=(TInt)aResult;
	ret=WriteFile(logfile, &op, sizeof(op), &written, 0);
	op=(TInt)aCell;
	ret=WriteFile(logfile, &op, sizeof(op), &written, 0);
}
