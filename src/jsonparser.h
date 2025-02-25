#ifndef JSONPARSER_H_
#define JSONPARSER_H_

#define TARGET_EXTENSION 1

#include <stdbool.h>

#include "pd_api.h"
#include "player.h"
#include "item.h"
#include "game.h"

extern PlaydateAPI *pd;
extern Game game;

json_decoder getDecoderItems(ItemList *list);
void decodeItems(const char *path, ItemList *list);
void decodeChars(const char *path, PlayerList *list);
void decodeCharsPreview(const char *path, PlayerPreviewList *list);
int readFile(void* userdata, uint8_t* buf, int bufsize);
void willDecodeSublistItems(json_decoder* decoder, const char* name, json_value_type type);
void didDecodeTableValueItems(json_decoder* decoder, const char* key, json_value value);
void* didDecodeSublistItems(json_decoder* decoder, const char* name, json_value_type type);
void willDecodeSublistChar(json_decoder* decoder, const char* name, json_value_type type);
void didDecodeTableValueChar(json_decoder* decoder, const char* key, json_value value);
void* didDecodeSublistChar(json_decoder* decoder, const char* name, json_value_type type);
int shouldDecodeArrayValueAtIndex(json_decoder* decoder, int pos);
void decodeError(json_decoder* decoder, const char* error, int linenum);
int shouldDecodeTableValueForKey(json_decoder* decoder, const char* key);
void didDecodeArrayValue(json_decoder* decoder, int pos, json_value value);

#endif