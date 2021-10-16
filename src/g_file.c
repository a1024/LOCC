//g_file.c - File-related queries
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

#include		<sys/stat.h>
#if !defined __linux__
#define	S_ISREG(m)	(((m)&S_IFMT)==S_IFREG)
#endif
int				file_is_readablea(const char *filename)//0: not readable, 1: regular file, 2: folder
{
	struct stat info;
	int error=stat(filename, &info);
	if(error)
		return 0;
	return 1+!S_ISREG(info.st_mode);
}
int				file_is_readablew(const wchar_t *filename)//0: not readable, 1: regular file, 2: folder
{
	struct _stat32 info;
	int error=_wstat32(filename, &info);
	if(error)
		return 0;
	return 1+!S_ISREG(info.st_mode);
}
size_t			file_size_w(const wchar_t *filename)
{
	struct _stat32 info;
	int error=_wstat32(filename, &info);
	if(error)
		return -1;
	return info.st_size;
}