#include"util.h"
#include<stdio.h>
#include<sys/stat.h>
const char *ext[]=
{
	"c",	"h",
	"cpp",	"hpp",
	"cxx",
	"cc",	"hh",
	"hc",
	"asm",	"s",
	"py",
	"java",
	"kt",
};
int main(int argc, char **argv)
{
	if(argc!=2)
	{
		printf("Usage:  %s  path", *argv);
		return 0;
	}
	const char *path=argv[1];
	int totallines=0;
	ArrayHandle filenames=get_filenames(path, ext, _countof(ext), 1);
	for(int k=0;k<(int)filenames->count;++k)
	{
		ArrayHandle *fn0=(ArrayHandle*)array_at(&filenames, k);
		ArrayHandle text=load_file((char*)fn0[0]->data, 0, 0, 1);
		int nlines=1;
		for(int k2=0;k2<(int)text->count;++k2)
			nlines+=text->data[k2]=='\n';
		printf("%8d %s\n", nlines, (char*)fn0[0]->data);
		totallines+=nlines;
		array_free(&text);
	}
	printf("\n%8d Total\n", totallines);
	array_free(&filenames);
	return 0;
}