//locc.c - Lines of Code Counter
//Copyright (C) 2021  Ayman Wagih Mohsen
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include"util.h"
#include<Windows.h>
#include<conio.h>

int		file_is_readablew(const wchar_t *filename);//0: not readable, 1: regular file, 2: folder
size_t	file_size_w(const wchar_t *filename);

//windows-specific
CVec	text_from_clipboard_a()
{
	char *str;
	size_t len;
	CVec ret;

	OpenClipboard(0);
	str=(char*)GetClipboardData(CF_OEMTEXT);
	if(!str||!(len=strlen(str)))
	{
		CloseClipboard();
		printf("Can't paste from clipboard\n");
		return 0;
	}
	cv_ctor_data(char, ret, str, len+1);
	CloseClipboard();
	return ret;
}
CVec	text_from_clipboard_w()
{
	HANDLE hMem;
	wchar_t *str;
	size_t len;
	CVec ret;

	OpenClipboard(0);
	hMem=GetClipboardData(CF_UNICODETEXT);
	if(!hMem)
	{
		CloseClipboard();
		printf("Can't paste from clipboard\n");
		return 0;
	}
	str=(wchar_t*)GlobalLock(hMem);
	if(!str||!(len=wcslen(str)))
	{
		CloseClipboard();
		printf("Can't paste from clipboard\n");
		return 0;
	}
	cv_ctor_data(wchar_t, ret, str, len+1);
	CloseClipboard();
	return ret;
}
void	set_console_buffer_size(short w, short h)
{
	COORD coords={w, h};
	HANDLE handle=GetStdHandle(STD_OUTPUT_HANDLE);
	int success=SetConsoleScreenBufferSize(handle, coords);
	if(!success)
		printf("Failed to resize console buffer: %d\n\n", GetLastError());
}
//end windows-specific

CVec	str_a2w(const char *src, int len)
{
	int k;
	CVec ret;

	cv_ctor_zero(wchar_t, ret, len);
	for(k=0;k<len;++k)
		cv_at(wchar_t, ret, k)=src[k];
	return ret;
}
CVec	open_binary(const wchar_t *filename, int *psize)
{
	size_t size;
	int error;
	FILE *file;
	CVec text;

	size=file_size_w(filename);
	cv_ctor_zero(char, text, size);
	error=_wfopen_s(&file, filename, L"rb");
	if(error)
		return 0;
	fread(cv_data(char, text), 1, size, file);
	fclose(file);
	if(psize)
		*psize=size;
	return text;
}
void	path_filter_bslash(wchar_t *path, int len)
{
	int k;

	for(k=0;k<len;++k)
		if(path[k]==L'\\')
			path[k]=L'/';
}

const wchar_t pathtail[]=L"/*";
int		recursive=-1;//-1: uninitialized, 0: flat, 1: recursive
int		count_loc(const wchar_t *filename)
{
	CVec text;
	int k, size, loc;
	
	text=open_binary(filename, &size);//only ASCII files
	if(!text)
	{
		printf("Failed to open %s\n", filename);
		return 0;
	}

	loc=1;
	for(k=0;k<size;++k)
		loc+=cv_at(char, text, k)=='\n';
	printf("%6d\t%S\n", loc, filename);
	return loc;
}
int		count_loc_folder(CVec path)
{
	int k, loc;
	WIN32_FIND_DATAW data={0};
	void *hSearch=0;
	CVec p2;

	loc=0;
	hSearch=FindFirstFileW(cv_data(wchar_t, path), &data);
	if(hSearch==INVALID_HANDLE_VALUE)
	{
		printf("Error: failed to open %S\n", cv_data(wchar_t, path));
		scanf("%d", &k);
		abort();
	}
	//.
	FindNextFileW(hSearch, &data);
	//..
	while(FindNextFileW(hSearch, &data))
	{
		cv_ctor_copy(p2, path);
		k=wcslen(data.cFileName);
		cv_replace(p2, cv_size(p2)-2, 1, k, data.cFileName);//replace "path/*" with "path/name"
		if(data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			if(recursive==-1)
			{
				printf("Found a folder, press R for recursive counting.\n");
				recursive=_getch();
				recursive=(recursive&0xDF)=='R';
			}
			if(recursive)
			{
				cv_str_append(p2, 2, pathtail);//append "/*"
				k=count_loc_folder(p2);
				if(k)
					printf("%6d\t%S\n\n", k, cv_data(wchar_t, p2));
					//printf("%6d\t%S [folder]\n", k, data.cFileName);
				loc+=k;
			}
		}
		else
		{
			//get extension
			for(k=cv_size(p2)-1;k>=0&&cv_at(wchar_t, p2, k)!=L'.';--k);
			++k;
			if(	!_wcsicmp(cv_pat(wchar_t, p2, k), L"c")||
				!_wcsicmp(cv_pat(wchar_t, p2, k), L"cpp")||
				!_wcsicmp(cv_pat(wchar_t, p2, k), L"cxx")||
				!_wcsicmp(cv_pat(wchar_t, p2, k), L"cc")||
				!_wcsicmp(cv_pat(wchar_t, p2, k), L"hc")||
				!_wcsicmp(cv_pat(wchar_t, p2, k), L"h")||
				!_wcsicmp(cv_pat(wchar_t, p2, k), L"hpp")||
				!_wcsicmp(cv_pat(wchar_t, p2, k), L"hh")||
				!_wcsicmp(cv_pat(wchar_t, p2, k), L"asm")||
				!_wcsicmp(cv_pat(wchar_t, p2, k), L"s")||
				!_wcsicmp(cv_pat(wchar_t, p2, k), L"py")||
				!_wcsicmp(cv_pat(wchar_t, p2, k), L"java")||
				!_wcsicmp(cv_pat(wchar_t, p2, k), L"kt")
				)
			{
				k=count_loc(cv_data(wchar_t, p2));
				//printf("%6d\t%S\n", k, data.cFileName);
				loc+=k;
			}
		}
		cv_dtor(p2);
	}
	return loc;
}
int		main(int argc, const char **argv)
{
	CVec path;
	int type, loc;

	set_console_buffer_size(128, 4096);

	printf(
		"LoCC - Lines of Code Counter\n"
		"Supported file types: .c .cpp .cxx .cc .hc .h .hpp .hh .asm .s .py .java .kt\n\n"
		);

	if(argc>1)
		path=str_a2w(argv[1], strlen(argv[1]));
	else
	{
		printf("Copy the path to clipboard and press any key...\n");
		_getch();
		path=text_from_clipboard_w();
		printf("%S\n\n", cv_data(wchar_t, path));
	}

	while(!(type=file_is_readablew(cv_data(wchar_t, path))))
	{
		printf("Copy the path to clipboard and press any key...\n");
		_getch();
		path=text_from_clipboard_w();
		printf("%S\n\n", cv_data(wchar_t, path));
	}
	path_filter_bslash(cv_data(wchar_t, path), cv_size(path));

	if(type==1)
		loc=count_loc(cv_data(wchar_t, path));
	else
	{
		if(cv_back(wchar_t, path)==pathtail[0])
			cv_str_push_back(path, pathtail[1]);
		else
			cv_str_append(path, 2, pathtail);
		loc=count_loc_folder(path);
		printf("\n%6d lines total\n\n", loc);
	}
	_getch();
	return 0;
}