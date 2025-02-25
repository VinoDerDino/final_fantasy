#include "jsonparser.h"

json_decoder getDecoderChars(PlayerList *list) {
    return (json_decoder) {
        .decodeError = decodeError,
        .willDecodeSublist = willDecodeSublistChar,
        .shouldDecodeTableValueForKey = shouldDecodeTableValueForKey,
        .didDecodeTableValue = didDecodeTableValueChar,
        .shouldDecodeArrayValueAtIndex = shouldDecodeArrayValueAtIndex,
        .didDecodeArrayValue = didDecodeArrayValue,
        .didDecodeSublist = didDecodeSublistChar,
        .userdata = list
    };
}

json_decoder getDecoderCharsPreview(PlayerList *list) {
    return (json_decoder) {
        .decodeError = decodeError,
        .willDecodeSublist = willDecodeSublistChar,
        .shouldDecodeTableValueForKey = shouldDecodeTableValueForKey,
        .didDecodeTableValue = didDecodeTableValueChar,
        .shouldDecodeArrayValueAtIndex = shouldDecodeArrayValueAtIndex,
        .didDecodeArrayValue = didDecodeArrayValue,
        .didDecodeSublist = didDecodeSublistChar,
        .userdata = list
    };
}

json_decoder getDecoderItems(ItemList *list) {
    return (json_decoder) {
        .decodeError = decodeError,
        .willDecodeSublist = willDecodeSublistItems,
        .shouldDecodeTableValueForKey = shouldDecodeTableValueForKey,
        .didDecodeTableValue = didDecodeTableValueItems,
        .shouldDecodeArrayValueAtIndex = shouldDecodeArrayValueAtIndex,
        .didDecodeArrayValue = didDecodeArrayValue,
        .didDecodeSublist = didDecodeSublistItems,
        .userdata = list
    };
}

void decodeItems(const char *path, ItemList *list) {
    json_decoder decoder = getDecoderItems(list);
    SDFile *file = pd->file->open(path, kFileRead);
    pd->json->decode(&decoder, (json_reader){ .read = readFile, .userdata = file }, NULL);
    pd->file->close(file);
}

void decodeChars(const char *path, PlayerList *list) {
    json_decoder decoder = getDecoderChars(list);
    SDFile *file = pd->file->open(path, kFileRead);
    pd->json->decode(&decoder, (json_reader){ .read = readFile, .userdata = file}, NULL);
    pd->file->close(file);
}

void decodeCharsPreview(const char *path, PlayerPreviewList *list) {
    json_decoder decoder = getDecoderCharsPreview(list);
    SDFile *file = pd->file->open(path, kFileRead);
    pd->json->decode(&decoder, (json_reader){ .read = readFile, .userdata = file}, NULL);
    pd->file->close(file);
}

int readFile(void* userdata, uint8_t* buf, int bufsize) {
    return pd->file->read((SDFile*)userdata, buf, bufsize);
}

void decodeError(json_decoder* decoder, const char* error, int linenum) {
    pd->system->logToConsole("JSON Error on line %d: %s", linenum, error);
}

// Hier die Parameter für die einzelnen Items hinzufügen
void didDecodeTableValueItems(json_decoder* decoder, const char* key, json_value value) {
    Item* item = (Item*)decoder->userdata;
    
    if (strcmp(key, "id") == 0) {
        item->id = value.data.intval;
        item->icon = newSpriteFromTable(game.itemtable, value.data.intval, pd, 0, 0);
    } else if (strcmp(key, "name") == 0) {
        strncpy(item->name, value.data.stringval, MAX_NAME_LEN);
    } else if (strcmp(key, "description") == 0) {
        strncpy(item->description, value.data.stringval, MAX_DESC_LEN);
    } else if (strcmp(key, "type") == 0) {
        if (strcmp(value.data.stringval, "potion") == 0) item->type = ITEM_TYPE_POTION;
        else if (strcmp(value.data.stringval, "weapon") == 0) item->type = ITEM_TYPE_WEAPON;
        else if (strcmp(value.data.stringval, "armor") == 0) item->type = ITEM_TYPE_ARMOR;
        else if (strcmp(value.data.stringval, "accessory") == 0) item->type = ITEM_TYPE_ACCESSORY;
        else item->type = ITEM_TYPE_MISC;
    } else if (strcmp(key, "attack") == 0) {
        item->attack = value.data.intval;
    } else if (strcmp(key, "crit_chance") == 0) {
        item->crit_chance = value.data.intval;
    } else if (strcmp(key, "defense") == 0) {
        item->defense = value.data.intval;
    } else if (strcmp(key, "set_id") == 0) {
        item->set_id = value.data.intval;
    } else if (strcmp(key, "heal_value") == 0) {
        item->heal_value = value.data.intval;
    } else if (strcmp(key, "effect") == 0) {
        if (strcmp(value.data.stringval, "none") == 0) item->effect = EFFECT_NONE;
        else if (strcmp(value.data.stringval, "damage_boost") == 0) item->effect = EFFECT_DAMAGE_BOOST;
        else if (strcmp(value.data.stringval, "set_bonus") == 0) item->effect = EFFECT_SET_BONUS;
        else if (strcmp(value.data.stringval, "lifesteal") == 0) item->effect = EFFECT_LIFESTEAL;
        else if (strcmp(value.data.stringval, "crit_boost") == 0) item->effect = EFFECT_CRIT_BOOST;
        else if (strcmp(value.data.stringval, "defense_stack") == 0) item->effect = EFFECT_DEFENSE_STACK;
    } else if (strcmp(key, "effect_value") == 0) {
        item->effect_value = value.data.intval;
    } else if (strcmp(key, "value") == 0) {
        item->value = value.data.intval;
    }
}


// Hier die Parameter für die einzelnen Chars einfügen
void didDecodeTableValueChar(json_decoder* decoder, const char* key, json_value value) {
    Player *player = (Player*)decoder->userdata;

    if (strcmp(key, "id") == 0) {
        player->id = value.data.intval;
    } else if (strcmp(key, "name") == 0) {
        strncpy(player->name, value.data.stringval, MAX_NAME_LENGTH);
    } else if (strcmp(key, "health")) {
        player->health = value.data.intval;
    } else if (strcmp(key, "maxHealth") == 0) {
        player->maxHealth = value.data.intval;
    } else if (strcmp(key, "power") == 0) {
        player->power = value.data.intval;
    }
}

void* didDecodeSublistItems(json_decoder* decoder, const char* name, json_value_type type) {
    ItemList* list = (ItemList*)decoder->userdata;
    
    if (strcmp(name, "items") == 0) {
        return list;
    }
    
    if (type == kJSONTable) {
        if (list->current_index < MAX_ITEMS) {
            return &list->items[list->current_index];
        } else {
            pd->system->logToConsole("ItemList ist voll!");
            return NULL;
        }
    }
    
    return NULL;
}

void* didDecodeSublistChar(json_decoder* decoder, const char* name, json_value_type type) {
    PlayerList* list = (PlayerList*)decoder->userdata;

    if (strcmp(name, "characters") == 0) {
        return list;
    }

    if (type == kJSONTable) {
        if (list->current_index < TOTAL_PLAYER_COUNT) {
            return &list->chars[list->current_index];
        } else {
            pd->system->logToConsole("PlayerList ist voll!");
            return NULL;
        }
    }
    
    return NULL;
}

void willDecodeSublistItems(json_decoder* decoder, const char* name, json_value_type type) {
    ItemList* list = (ItemList*)decoder->userdata;
    if (type == kJSONTable && strcmp(name, "_root") != 0) {
        if (list->current_index < MAX_ITEMS) {
            decoder->userdata = &list->items[list->current_index++];
        }
    }
}

void willDecodeSublistChar(json_decoder* decoder, const char* name, json_value_type type) {
    PlayerList* list = (PlayerList*)decoder->userdata;
    if (type == kJSONTable && strcmp(name, "_root") != 0) {
        if (list->current_index < TOTAL_PLAYER_COUNT) {
            decoder->userdata = &list->chars[list->current_index++];
        }
    }
}

int shouldDecodeTableValueForKey(json_decoder* decoder, const char* key) {
    return 1;
}

int shouldDecodeArrayValueAtIndex(json_decoder* decoder, int pos) {
    return 1;
}

void didDecodeArrayValue(json_decoder* decoder, int pos, json_value value) {
    return;
}