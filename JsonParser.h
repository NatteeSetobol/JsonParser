#ifndef JSON_PARSER
#define JSON_PARSER
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include "required/intrinsic.h"
#include "required/nix.h"
#include "required/memory.h"
#include "required/platform.h"
#include "required/assert.h"
#include "marray.h"
#include "string.h"

enum Json_Type{ JNONE,JLIST, JDICTIONARY, JARRAY };

struct Json_Branch
{
	enum Json_Type type;
	char *key;
	char *value;
	struct Json_Branch *prev;
	struct Json_Branch *next;

	struct Json_Branch *head;
	struct Json_Branch *current;

	struct Json_Branch *prevBranch;
	struct Json_Branch *subBranch;
};

enum JSON_Token_Type
{
	DATA,
	LIST_START,
	LIST_END,
	ARRAY_START,
	ARRAY_END,
	IDENTIFIER,
	SPLITTER,
	NEXT,
	SPACE,
	STRING_TERM,
	ENTER
};

struct JSON_Token
{
	size_t length;
	enum JSON_Token_Type type;
	char *JsonData;
	char token;
};

struct JSON_Tokenizer
{
	char *at;
};

void JSON_Parse(char *json, struct Json_Branch* jBranch, struct Error_Handler *error);
struct Json_Branch* Json_Find(struct Json_Branch* branch,struct array* arrayOfSearch);
struct Json_Branch *Json_DelTree(struct Json_Branch* branch);
struct JSON_Token GetJSONToken(struct JSON_Tokenizer *jsonTokenizer);
struct Json_Branch* Json_DelBranch(struct Json_Branch* branch);
void Json_AddBranch(struct Json_Branch* branch, enum Json_Type type, char* key, char* value);
void Json_Free(struct Json_Branch* branch);
void GetBranchType(struct Json_Branch *branch);

struct Json_Branch *AddSubBranch(struct Json_Branch* realCurrent, enum Json_Type type)
{
	struct Json_Branch* subBranch = (struct Json_Branch*) Memory(struct Json_Branch);
	subBranch->type=type;
	subBranch->key=NULL;
	subBranch->value=NULL;
	subBranch->head=NULL;
	subBranch->current=NULL;
	subBranch->prevBranch=realCurrent;
	subBranch->subBranch=NULL;
	subBranch->head=NULL;
	subBranch->current=NULL;

	return subBranch;
}


void JSON_Parse(char *json, struct Json_Branch* jBranch, struct Error_Handler *error)
{
	struct JSON_Tokenizer jTokenizer={0};
	struct JSON_Token jToken={0};
	int dataLen = Strlen(json);
	struct Json_Branch *current=NULL;
	bool isKey=false;
	int cnt=0;

	current = jBranch;

	jTokenizer.at = json;

	for (int i=0;i<dataLen;i++)
	{
		jToken = GetJSONToken(&jTokenizer);

		switch(jToken.type)
		{
			case SPLITTER:
			{
				if (isKey == true)
				{
					char* val=NULL;
					char* tempVal=NULL;
					
					do
					{
					jToken = GetJSONToken(&jTokenizer);

					switch(jToken.type)
					{
						case ENTER:
						case SPACE:
						{
							break;
						}
						case LIST_START:
						{
							struct Json_Branch *realCurrent=NULL;
							realCurrent = current;

							if (isKey == true) 
							{
								current = current->current;
								isKey = false;
							}

							if (current->type == JARRAY)
							{
								Json_AddBranch( current ,JLIST, NULL, NULL);
								current->current->prevBranch = current;
								current = current->current;

							}

							current->subBranch = AddSubBranch(realCurrent,JLIST);
							current = current->subBranch;
							break;
						}
						case ARRAY_START:
						{
							struct Json_Branch *realCurrent=NULL;
							realCurrent = current;

							if (isKey == true) 
							{
								current = current->current;
								isKey = false;
							}

							current->subBranch = AddSubBranch(realCurrent,JARRAY);
							current = current->subBranch;
							break;

						}
						default:
						{
							if (jToken.type == IDENTIFIER)
							{

								jToken = GetJSONToken(&jTokenizer);
								while (jToken.type != IDENTIFIER && jToken.type != STRING_TERM)
								{
									char v[3]={0};
									v[0] = jToken.token;


									if (val == NULL)
									{
										val = S32(v);

									} else {
										tempVal = S32Cat(val,1, v);

										Free(val);
										val=NULL;

										val = S32(tempVal);

										Free(tempVal);
										tempVal=NULL;
									}

									jToken = GetJSONToken(&jTokenizer);
								}

							} else {
								while (jToken.type !=LIST_END && jToken.type != NEXT && jToken.type != STRING_TERM)
								{
									char v[3]={0};
									v[0] = jToken.token;


									if (val == NULL)
									{
										val = S32(v);

									} else {
										tempVal = S32Cat(val,1, v);

										Free(val);
										val=NULL;

										val = S32(tempVal)

										if (StrCmp(val,"null"))
										{
											int breakhere = 0;
										}

										Free(tempVal);
										tempVal=NULL;
									}

									jToken = GetJSONToken(&jTokenizer);
								}
							}

							if (val)
							{
								current->current->value = S32(val);
								Free(val);

								if (jToken.type == LIST_END)
								{
									jTokenizer.at--;
								}
							}
							break;
						}
					}
					} while (jToken.type == SPACE);

					isKey = false; 
				}
				break;
			}
			case NEXT:
			{
				if (isKey == true)
				{

					isKey = false; 
				}
				break;
			}
			case SPACE:
			{
				break;
			}
			case LIST_START:
			{
				struct Json_Branch *realCurrent=NULL;
				realCurrent = current;

				if (isKey == true) 
				{
					current = current->current;
					isKey = false;
				}

				if (current->type == JARRAY)
				{
					Json_AddBranch( current ,JLIST, NULL, NULL);
					current->current->prevBranch = current;
					current = current->current;
					
				}

				current->subBranch = AddSubBranch(realCurrent,JLIST);
				current = current->subBranch;
				break;
			}
			case ARRAY_START:
			{
				struct Json_Branch *realCurrent=NULL;
				realCurrent = current;

				if (isKey == true) 
				{
					current = current->current;
					isKey = false;
				}

				current->subBranch = AddSubBranch(realCurrent,JARRAY);
				current = current->subBranch;

				break;
			}
			case LIST_END:
			{
				if (isKey == true) isKey = false;
				if (current->prevBranch)
				{
					current = current->prevBranch;
				}
				break;
			}
			case ARRAY_END:
			{
				if (isKey == true) isKey = false;

				if (current->prevBranch)
				{
					current = current->prevBranch;
				}
				break;
			}

			case IDENTIFIER:
			{
				char *value = NULL;
				bool isStringLoopDone = false;

				for (int j=0;j < dataLen && isStringLoopDone == false;j++)
				{
					jToken = GetJSONToken(&jTokenizer);

					switch (jToken.type)
					{
						#if 0
						case '\\':
						{
							jToken = GetJSONToken(&jTokenizer);
							break;
						}
						#endif
						case IDENTIFIER:
						{
							isStringLoopDone=true;
							break;
						} 
						default:
						{
							char temp[2]={0};
							temp[0] = jToken.token;
							temp[1] = '\0';

							if (value == NULL)
							{
								value = S32(temp);
							} else {
								char* tempValue = NULL;

								tempValue = S32Cat(value,1,temp);

								Free(value);
								value = NULL;

								value = S32(tempValue);
								
								Free(tempValue);
								tempValue=NULL;
							}

							break;
						}
					}
				}
				
				if (value)
				{
					if (isKey==false)
					{
						if (value)
						{
							Json_AddBranch( current ,JDICTIONARY , value, NULL);
							isKey = true;
						}
					} else {
						current->current->value = S32(value);
						isKey = false;
					}

					Free(value);
					value = NULL;
				}
				
				break;
			}
		}
	}
}


void JsonPrintAll(struct Json_Branch *branch)
{
	struct Json_Branch *result = branch;
	struct Json_Branch *current = branch;
	bool found = false;
	int count = 0;

	switch(current->type)
	{
		case JARRAY:
		{
			current = current->head;
			while (current)
			{
				if (current != NULL)
				{
					if (current->subBranch)
					{
						JsonPrintAll(current->subBranch);
					}

					current = current->next;
				}
			}
			break;
		}
		case JLIST:
		{
			if (current->head)
			{

				current = current->head;

				if (current->value)
				{
					printf("key: %s, value: %s\n", current->key, current->value);
				} else {
					printf("key: %s\n", current->key);
					if (current->subBranch)
					{
						JsonPrintAll(current->subBranch);
					}

				}

				JsonPrintAll(current);
			}
			break;
		}
		case JDICTIONARY:
		{
			if (current->next)
			{
				current = current->next;
				if (current->value)
				{
					printf("key: %s, value: %s\n", current->key, current->value);
				} else {
					printf("key: %s\n", current->key);
					if (current->subBranch)
					{
						JsonPrintAll(current->subBranch);
					}
				}

				JsonPrintAll(current);
			}
			break;
		}
	}

	return ;
}


struct Json_Results
{
	bool32 success;
	struct marray items;
};

//NOTES(): Need to find a way to seperate each results for each key rather than bundling it up into one array.

struct Json_Results *JSON_FindByKeys(struct marray *keys,struct Json_Branch *branch, struct Json_Results *savedResult)
{
	struct Json_Results *result = NULL;
	struct Json_Branch *current = branch;
	bool found = false;
	int count = 0;

	if (savedResult)
	{
		result = savedResult;
	}

	switch(current->type)
	{
		case JARRAY:
		{
			current = current->head;
			while (current)
			{
				if (current != NULL)
				{
					if (current->subBranch)
					{
						result = JSON_FindByKeys(keys,current->subBranch, result);
					}

					current = current->next;
				}
			}
			break;
		}
		case JLIST:
		{
			if (current->head)
			{
				current = current->head;

				if (current->value)
				{	
					
					for (int keyIndex = 0; keyIndex < keys->count; keyIndex++)
					{
						s32 *key = (s32*) GetFromMArray(keys,keyIndex);

						if (StrCmp(current->key, key)) 
						{
							if (result  == NULL) 
							{
								result = (struct Json_Results*) MemoryRaw(sizeof(struct Json_Results));
								result->success = true;
							}

							if (current->value)
							{
								s32 *newValue = S32(current->value);
								AddToMArray(&result->items, newValue);

							} else {
								if (current->subBranch)
								{
									AddToMArray(&result->items,current->subBranch );
								}
							}

						}
					}
				} else {

					for (int keyIndex = 0; keyIndex < keys->count; keyIndex++)
					{
						s32 *key = (s32*) GetFromMArray(keys,keyIndex);

						if (StrCmp(current->key, key))
						{
							if (result  == NULL) 
							{
								result = (struct Json_Results*) MemoryRaw(sizeof(struct Json_Results));
								result->success = true;
							}

							if (current->value)
							{
								s32 *newValue = S32(current->value);
								AddToMArray(&result->items, newValue);

							} else {
								if (current->subBranch)
								{
									AddToMArray(&result->items,current->subBranch );
								}
							}

						}
					}
					if (current->subBranch)
					{
						result = JSON_FindByKeys(keys,current->subBranch,result);
					}

				}
				result = JSON_FindByKeys(keys,current,result);
			}
			break;
		}
		case JDICTIONARY:
		{
			if (current->next)
			{
				current = current->next;
				if (current->value)
				{

					for (int keyIndex = 0; keyIndex < keys->count; keyIndex++)
					{
						s32 *key = (s32*) GetFromMArray(keys,keyIndex);
						
						if (StrCmp(current->key, key))
						{
							if (result  == NULL) 
							{
								result = (struct Json_Results*) MemoryRaw(sizeof(struct Json_Results));
								result->success = true;
							}

							if (current->value)
							{
								s32 *newValue = S32(current->value);
								AddToMArray(&result->items, newValue);

							} else {
								if (current->subBranch)
								{
									AddToMArray(&result->items,current->subBranch );
								}
							}

						}
					}
					if (current->subBranch)
					{
						result = JSON_FindByKeys(keys,current->subBranch,result);
					}

				} else {

					for (int keyIndex = 0; keyIndex < keys->count; keyIndex++)
					{
						s32 *key = (s32*) GetFromMArray(keys,keyIndex);

						if (StrCmp(current->key, key)) 
						{
							if (result  == NULL) 
							{
								result = (struct Json_Results*) MemoryRaw(sizeof(struct Json_Results));
								result->success = true;
							}

							if (current->value)
							{
								s32 *newValue = S32(current->value);
								AddToMArray(&result->items, newValue);

							} else {
								if (current->subBranch)
								{
									AddToMArray(&result->items,current->subBranch );
								}
							}

						}
					}
					if (current->subBranch)
					{
						result = JSON_FindByKeys(keys,current->subBranch,result);
					}
				}

				result = JSON_FindByKeys(keys,current,result);
			}
			break;
		}
	}

	return result;
}

struct Json_Results *JSON_FindByKey(s32* key,struct Json_Branch *branch, struct Json_Results *savedResult)
{
	struct Json_Results *result = NULL;
	struct Json_Branch *current = branch;
	bool found = false;
	int count = 0;

	if (savedResult)
	{
		result = savedResult;
	}

	switch(current->type)
	{
		case JARRAY:
		{
			current = current->head;
			while (current)
			{
				if (current != NULL)
				{
					if (current->subBranch)
					{
						result = JSON_FindByKey(key,current->subBranch, result);
						/*
						if (result)
						{
							break;
						}
						*/
					}

					current = current->next;
				}
			}
			break;
		}
		case JLIST:
		{
			if (current->head)
			{
				current = current->head;

				if (current->value)
				{
				///	printf("key: %s, value: %s\n", current->key, current->value);
					if (StrCmp(current->key, key)) // && result == NULL)
					{
						if (result  == NULL) 
						{
							result = (struct Json_Results*) MemoryRaw(sizeof(struct Json_Results));
							result->success = true;
						}

						if (current->value)
						{
							s32 *newValue = S32(current->value);
							AddToMArray(&result->items, newValue);

						} else {
							if (current->subBranch)
							{
								AddToMArray(&result->items,current->subBranch );
							}
						}

					}
				} else {
					//printf("key: %s\n", current->key);
					if (StrCmp(current->key,key))
					{
						if (result  == NULL) 
						{
							result = (struct Json_Results*) MemoryRaw(sizeof(struct Json_Results));
							result->success = true;
						}
						if (current->value)
						{
							s32 *newValue = S32(current->value);
							AddToMArray(&result->items, newValue);

						} else {
							if (current->subBranch)
							{
								AddToMArray(&result->items,current->subBranch );
							}
						}


					}

					if (current->subBranch)
					{
						result = JSON_FindByKey(key,current->subBranch,result);
					}

				}
				result = JSON_FindByKey(key,current,result);
			}
			break;
		}
		case JDICTIONARY:
		{
			if (current->next)
			{
				current = current->next;
				if (current->value)
				{
				//	printf("key: %s, value: %s\n", current->key, current->value);
					if (StrCmp(current->key,key) )
					{
						if (result  == NULL) 
						{
							result = (struct Json_Results*) MemoryRaw(sizeof(struct Json_Results));
							result->success = true;
						}

						if (current->value)
						{
							s32 *newValue = S32(current->value);
							AddToMArray(&result->items, newValue);

						} else {
							if (current->subBranch)
							{
								AddToMArray(&result->items,current->subBranch );
							}
						}

					}

				} else {
					//printf("key: %s\n", current->key);
					if (StrCmp(current->key,key) )
					{
						if (result  == NULL) 
						{
							result = (struct Json_Results*) MemoryRaw(sizeof(struct Json_Results));
							result->success = true;
						}

						if (current->value)
						{
							s32 *newValue = S32(current->value);
							AddToMArray(&result->items, newValue);
						} else {
							if (current->subBranch)
							{
								AddToMArray(&result->items,current->subBranch );
							}
						}

					}

					if (current->subBranch)
					{
						result = JSON_FindByKey(key,current->subBranch,result);
					}
				}

				result = JSON_FindByKey(key,current,result);
			}
			break;
		}
	}

	return result;
}

struct JSON_Token GetJSONToken(struct JSON_Tokenizer *jsonTokenizer)
{
	struct JSON_Token jsonTokenResults={0};

	char c=0;
	i8* tokAddr=NULL;

	jsonTokenResults.length = 1;
	jsonTokenResults.JsonData = jsonTokenizer->at;

	c = jsonTokenizer->at[0];
	jsonTokenResults.token = c;
	if (c != '\0')
	{
		++jsonTokenizer->at;
	}

	switch(c)
	{
		case '\0':
		{
			jsonTokenResults.type = STRING_TERM;
			break;
		}
		case 0x20:
		{
			jsonTokenResults.type = SPACE;
			break;
		}
		case '{':
		{
			jsonTokenResults.type = LIST_START;
			break;
		}
		case '}':
		{
			jsonTokenResults.type = LIST_END;
			break;
		}
		case '[':
		{
			jsonTokenResults.type = ARRAY_START;
			break;
		}
		case ']':
		{
			jsonTokenResults.type = ARRAY_END;
			break;
		}
		case '"':
		{
			jsonTokenResults.type = IDENTIFIER;
			break;
		}
		case ':':
		{
			jsonTokenResults.type = SPLITTER;
			break;
		}
		case '\n':
		{
			jsonTokenResults.type = ENTER;
			break;
		}
		case ',':
		{
			jsonTokenResults.type = NEXT;
			break;
		}

	};

	return jsonTokenResults;
}

struct Json_Branch *Json_DelTree(struct Json_Branch* branch)
{
	struct Json_Branch *retTree=NULL;
	if (branch->subBranch)
	{
		retTree=  Json_DelTree(branch->subBranch);

	} 
	if (branch->head)
	{
		struct Json_Branch *retBranch = Json_DelBranch(branch->head);

		if (retBranch != NULL)
		{
			if (retBranch->key)
			{
				Free(retBranch->key);
				retBranch->key=NULL;
			}
			if (retBranch->value)
			{
				Free(retBranch->value);
				retBranch->value=NULL;
			}
			Free(retBranch);
			retBranch=NULL;
		}
	}

	if (retTree!= NULL)
	{
		if (retTree->key)
		{
			Free(retTree->key);
			retTree->key=NULL;
		}
		if (retTree->value)
		{
			Free(retTree->value);
			retTree->value=NULL;
		}

		Free(retTree);
		retTree=NULL;
	}

	return branch;
}

struct Json_Branch* Json_DelBranch(struct Json_Branch* branch)
{
	struct Json_Branch *retTree=NULL;

	if (branch->subBranch)
	{
		retTree=  Json_DelTree(branch->subBranch);
	}
	if (branch->next)
	{
		struct Json_Branch *retBranch = Json_DelBranch(branch->next);

		if (retBranch != NULL)
		{
			if (retBranch->key)
			{
				Free(retBranch->key);
				retBranch->key=NULL;
			}
			if (retBranch->value)
			{
				Free(retBranch->value);
				retBranch->value=NULL;
			}
			Free(retBranch);
			retBranch=NULL;
		}
	}

	if (retTree!= NULL)
	{
		if (retTree->key)
		{
			Free(retTree->key);
			retTree->key=NULL;
		}
		if (retTree->value)
		{
			Free(retTree->value);
			retTree->value=NULL;
		}
		Free(retTree);
		retTree=NULL;
	}
	return branch;
}

void GetBranchType(struct Json_Branch *branch)
{
	switch(branch->type)
	{
		case JNONE:
		{
			printf("NO TYPE\n");
			break;
		}
		case JLIST:
		{
			printf("Json type\n");
			break;
		}
		case JDICTIONARY:
		{
			printf("Dictionary type\n");
			break;
		}
		case JARRAY:
		{
			printf("JArray type\n");
			break;
		}
	}

	return ;
}

void Json_Free(struct Json_Branch* branch)
{
	Json_DelTree(branch);
}

void Json_AddBranch(struct Json_Branch* branch, enum Json_Type type, char* key, char* value)
{
	if (branch->head == NULL)
	{
		branch->head = (struct Json_Branch*) Memory(struct Json_Branch);
		branch->head->type = type;
		if (key != NULL)
		{
			branch->head->key = S32(key);
		} else {
			branch->head->key = NULL;
		}

		if (value != NULL)
		{
			branch->head->value= S32(value);
		} else {
			branch->head->value=NULL;
		}
		branch->head->next= NULL;
		branch->head->prev= NULL;
		branch->head->head= NULL;
		branch->head->current= NULL;
		branch->current = branch->head;
	} else {
		branch->current->next = (struct Json_Branch*) Memory(struct Json_Branch);
		branch->current->next->type = type;
		if (key)
		{
			branch->current->next->key = S32(key);
		} else {
			branch->current->next->key = NULL;
		}
		if (value)
		{
			branch->current->next->value= S32(value);;
		} else {
			branch->current->next->value= NULL;;
		}
		branch->current->next->prev= branch->current;
		branch->current->next->head= NULL;
		branch->current->next->current= NULL;
		branch->current = branch->current->next;
	}
}


#endif
