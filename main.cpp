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
		rewind(jsonFilePointer`);
		fread(jsonFileContents,1,fileSize,jsonFilePointer);

		JSON_Parse(jsonFileContents,&branch,NULL);

		struct Json_Results *key = NULL;

		#if 1
		key = FindByKey("movie", branch.subBranch, NULL);

		if (key)
		{
			for (int i=0;i < key->items.count;i++)
			{
				s32 *value = (s32*) GetFromMArray(&key->items,i);

				if (value)
				{
					printf("%s\n", value);
				}
			}
		}
		#endif

	//	JsonPrintAll(branch.subBranch);

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
