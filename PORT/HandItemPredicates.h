#pragma once
#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/SaveGame.h"

namespace ravenloft::game {

class HandItemPredicates {
public:
    static int predicate23E58(int objectId,const data::ObjectDatabase& objects);
    static int characterHasClass23EAC(const data::CharacterRecord& character,int classId);
    static int predicate23EE4(int objectId);
    static int spellHandRange23F34(int objectId);
    static int predicate23F50(int objectId,const data::ObjectDatabase& objects);
    static int predicate23FC4(int objectId,const data::ObjectDatabase& objects);
    static int predicate24060(int objectId);
};

} // namespace ravenloft::game
