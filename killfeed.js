let client = Process.getModuleByName("client.so").base
// let onGameEvent = client.add("0x0119de80")
let fireGameEvent = client.add("0x01151a20")

let tf_damageTypes = [
	"TF_DMG_CUSTOM_NONE",
	"TF_DMG_CUSTOM_HEADSHOT",
	"TF_DMG_CUSTOM_BACKSTAB",
	"TF_DMG_CUSTOM_BURNING",
	"TF_DMG_WRENCH_FIX",
	"TF_DMG_CUSTOM_MINIGUN",
	"TF_DMG_CUSTOM_SUICIDE",
	"TF_DMG_CUSTOM_TAUNTATK_HADOUKEN",
	"TF_DMG_CUSTOM_BURNING_FLARE",
	"TF_DMG_CUSTOM_TAUNTATK_HIGH_NOON",
	"TF_DMG_CUSTOM_TAUNTATK_GRAND_SLAM",
	"TF_DMG_CUSTOM_PENETRATE_MY_TEAM",
	"TF_DMG_CUSTOM_PENETRATE_ALL_PLAYERS",
	"TF_DMG_CUSTOM_TAUNTATK_FENCING",
	"TF_DMG_CUSTOM_PENETRATE_NONBURNING_TEAMMATE",
	"TF_DMG_CUSTOM_TAUNTATK_ARROW_STAB",
	"TF_DMG_CUSTOM_TELEFRAG",
	"TF_DMG_CUSTOM_BURNING_ARROW",
	"TF_DMG_CUSTOM_FLYINGBURN",
	"TF_DMG_CUSTOM_PUMPKIN_BOMB",
	"TF_DMG_CUSTOM_DECAPITATION",
	"TF_DMG_CUSTOM_TAUNTATK_GRENADE",
	"TF_DMG_CUSTOM_BASEBALL",
	"TF_DMG_CUSTOM_CHARGE_IMPACT",
	"TF_DMG_CUSTOM_TAUNTATK_BARBARIAN_SWING",
	"TF_DMG_CUSTOM_AIR_STICKY_BURST",
	"TF_DMG_CUSTOM_DEFENSIVE_STICKY",
	"TF_DMG_CUSTOM_PICKAXE",
	"TF_DMG_CUSTOM_ROCKET_DIRECTHIT",
	"TF_DMG_CUSTOM_TAUNTATK_UBERSLICE",
	"TF_DMG_CUSTOM_PLAYER_SENTRY",
	"TF_DMG_CUSTOM_STANDARD_STICKY",
	"TF_DMG_CUSTOM_SHOTGUN_REVENGE_CRIT",
	"TF_DMG_CUSTOM_TAUNTATK_ENGINEER_GUITAR_SMASH",
	"TF_DMG_CUSTOM_BLEEDING",
	"TF_DMG_CUSTOM_GOLD_WRENCH",
	"TF_DMG_CUSTOM_CARRIED_BUILDING",
	"TF_DMG_CUSTOM_COMBO_PUNCH",
	"TF_DMG_CUSTOM_TAUNTATK_ENGINEER_ARM_KILL",
	"TF_DMG_CUSTOM_FISH_KILL",
	"TF_DMG_CUSTOM_TRIGGER_HURT",
	"TF_DMG_CUSTOM_DECAPITATION_BOSS",
	"TF_DMG_CUSTOM_STICKBOMB_EXPLOSION",
	"TF_DMG_CUSTOM_AEGIS_ROUND",
	"TF_DMG_CUSTOM_FLARE_EXPLOSION",
	"TF_DMG_CUSTOM_BOOTS_STOMP",
	"TF_DMG_CUSTOM_PLASMA",
	"TF_DMG_CUSTOM_PLASMA_CHARGED",
	"TF_DMG_CUSTOM_PLASMA_GIB",
	"TF_DMG_CUSTOM_PRACTICE_STICKY",
	"TF_DMG_CUSTOM_EYEBALL_ROCKET",
	"TF_DMG_CUSTOM_HEADSHOT_DECAPITATION",
	"TF_DMG_CUSTOM_TAUNTATK_ARMAGEDDON",
	"TF_DMG_CUSTOM_FLARE_PELLET",
	"TF_DMG_CUSTOM_CLEAVER",
	"TF_DMG_CUSTOM_CLEAVER_CRIT",
	"TF_DMG_CUSTOM_SAPPER_RECORDER_DEATH",
	"TF_DMG_CUSTOM_MERASMUS_PLAYER_BOMB",
	"TF_DMG_CUSTOM_MERASMUS_GRENADE",
	"TF_DMG_CUSTOM_MERASMUS_ZAP",
	"TF_DMG_CUSTOM_MERASMUS_DECAPITATION",
	"TF_DMG_CUSTOM_CANNONBALL_PUSH",
	"TF_DMG_CUSTOM_TAUNTATK_ALLCLASS_GUITAR_RIFF",
	"TF_DMG_CUSTOM_THROWABLE",
	"TF_DMG_CUSTOM_THROWABLE_KILL",
	"TF_DMG_CUSTOM_SPELL_TELEPORT",
	"TF_DMG_CUSTOM_SPELL_SKELETON",
	"TF_DMG_CUSTOM_SPELL_MIRV",
	"TF_DMG_CUSTOM_SPELL_METEOR",
	"TF_DMG_CUSTOM_SPELL_LIGHTNING",
	"TF_DMG_CUSTOM_SPELL_FIREBALL",
	"TF_DMG_CUSTOM_SPELL_MONOCULUS",
	"TF_DMG_CUSTOM_SPELL_BLASTJUMP",
	"TF_DMG_CUSTOM_SPELL_BATS",
	"TF_DMG_CUSTOM_SPELL_TINY",
	"TF_DMG_CUSTOM_KART",
	"TF_DMG_CUSTOM_GIANT_HAMMER",
	"TF_DMG_CUSTOM_RUNE_REFLECT",
	//
	// INSERT NEW ITEMS HERE TO AVOID BREAKING DEMOS
	//

	"TF_DMG_CUSTOM_END", // END

];

// 0 & 1: CONSTRUCTOR AND DESTRUCTOR
// 2: virtual const char *GetName() const = 0;	// get event name
//
// 3: virtual bool  IsReliable() const = 0; // if event handled reliable
// 4: virtual bool  IsLocal() const = 0; // if event is never networked
// 5: virtual bool  IsEmpty(const char *keyName = NULL) = 0; // check if data field exists
// 
// // Data access
// 6: virtual bool  GetBool( const char *keyName = NULL, bool defaultValue = false ) = 0;
// 7: virtual int   GetInt( const char *keyName = NULL, int defaultValue = 0 ) = 0;
// 8: virtual float GetFloat( const char *keyName = NULL, float defaultValue = 0.0f ) = 0;
// 9: virtual const char *GetString( const char *keyName = NULL, const char *defaultValue = "" ) = 0;
//
// 10: virtual void SetBool( const char *keyName, bool value ) = 0;
// 11: virtual void SetInt( const char *keyName, int value ) = 0;
// 12: virtual void SetFloat( const char *keyName, float value ) = 0;
// 13: virtual void SetString( const char *keyName, const char *value ) = 0;
function getGameEventFunc(vtable, funcName) {
  let gameEventVtableFuncs = {
      "GetName": 2,
      "GetString": 9,
      "GetInt": 7,
      "SetInt": 11,
      "SetString": 13,
  };
  return vtable.add(Process.pointerSize * gameEventVtableFuncs[funcName]).readPointer();
}

Interceptor.attach(fireGameEvent, {
    onEnter: function(args) {
      // First arg is self
      let thisPtr = args[0];
      let gameEvent = args[1];
      let gameEventVtable = gameEvent.readPointer();

      let getNamePtr = getGameEventFunc(gameEventVtable, "GetName");
      let getStringPtr = getGameEventFunc(gameEventVtable, "GetString");
      let getIntPtr = getGameEventFunc(gameEventVtable, "GetInt");

      let setStringPtr = getGameEventFunc(gameEventVtable, "SetString");
      let setIntPtr = getGameEventFunc(gameEventVtable, "SetInt");

      let getName = new NativeFunction(getNamePtr, "pointer", ["pointer"]);
      let getString = new NativeFunction(getStringPtr, "pointer", ["pointer", "pointer", "pointer"]);
      let getInt = new NativeFunction(getIntPtr, "int", ["pointer", "pointer", "pointer"]);

      let setString = new NativeFunction(setStringPtr, "void", ["pointer", "pointer", "pointer"]);
      let setInt = new NativeFunction(setIntPtr, "void", ["pointer", "pointer", "int"]);

      if(getInt(gameEvent, Memory.allocUtf8String("customkill"), NULL) == tf_damageTypes.indexOf("TF_DMG_CUSTOM_BACKSTAB")) {
        setInt(gameEvent, Memory.allocUtf8String("customkill"), tf_damageTypes.indexOf("TF_DMG_CUSTOM_NONE"));
      }

      let event_name = getName(gameEvent);
      let weapon_name = getString(gameEvent, Memory.allocUtf8String("weapon"), ptr(0));
      let customkill = getInt(gameEvent, Memory.allocUtf8String("customkill"), ptr(0));

      console.log(`(${gameEvent})`);
      console.log(`
        event_name: ${event_name.readCString()}
        weapon_name: ${weapon_name.readCString()}
        customkill: ${tf_damageTypes[customkill]}
      `);
    },
})
