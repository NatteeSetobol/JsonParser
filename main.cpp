#include <stdio.h>
#include "jsonparser.h"

int main()
{
	struct Json_Branch branch = {}; 
	int fileSize = 0;
	s32 *jsonFileContents  = NULL;
	FILE *jsonFilePointer = fopen("news.json","rb");

	if (jsonFilePointer)
	{
		fseek(jsonFilePointer,0,SEEK_END);
		fileSize = ftell(jsonFilePointer);

		jsonFileContents = (s32*) MemoryRaw(fileSize+1);
		rewind(jsonFilePointer);
		fread(jsonFileContents,1,fileSize,jsonFilePointer);

		JSON_Parse(jsonFileContents,&branch,NULL);

		struct Json_Branch *key = NULL;

		#if 0
		key = FindByKey("_id", branch.subBranch);

		if (key->value)
		{
			printf("%s\n",key->value);
		}
		#endif

		JsonPrintAll(branch.subBranch);

		/*
		while(current)
		{
			printf("key: %s\n", current->key);
			current = current->next;
		}
		*/

		

		if (jsonFileContents)
		{
			Free(jsonFileContents);
			jsonFileContents=NULL;
		}
		fclose(jsonFilePointer);
	}

}
