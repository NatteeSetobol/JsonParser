#include <stdio.h>
#include "jsonparser.h"

int main()
{
	struct Json_Branch branch = {}; 
	int fileSize = 0;
	s32 *jsonFileContents  = NULL;
	FILE *jsonFilePointer = fopen("example_1.json","rb");

	if (jsonFilePointer)
	{
		fseek(jsonFilePointer,0,SEEK_END);
		fileSize = ftell(jsonFilePointer);

		jsonFileContents = (s32*) MemoryRaw(fileSize+1);
		rewind(jsonFilePointer);
		fread(jsonFileContents,1,fileSize,jsonFilePointer);

		JSON_Parse(jsonFileContents,&branch,NULL);

		if (branch.subBranch)
		{
			struct Json_Branch *items =  branch.subBranch->head;

			while (items != NULL)
			{
				printf("%s\n", items->value);
				items = items->next;
			}

		}

		if (jsonFileContents)
		{
			Free(jsonFileContents);
			jsonFileContents=NULL;
		}
		fclose(jsonFilePointer);
	}

}
