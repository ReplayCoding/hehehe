#include <cdll_int.h>
#include <igameevents.h>
#include <interface.h>

#include <frida-gum.h>

const gpointer DOSTARTUPSHADERPRELOADING_OFFSET = (gpointer)0x60ae0;
const gpointer FIREGAMEEVENT_OFFSET = (gpointer)0x1154430;

typedef int printf_t(const char *format, ...);

static bool have_we_inited = false;
static IVEngineClient013 *engineClient;

GumInterceptor *interceptor = NULL;

enum _TestHookId : int { TEST_HOOK_DLOPEN, TEST_HOOK_FIREGAMEEVENT };
typedef enum _TestHookId TestHookId;

enum ETFDmgCustom {
  TF_DMG_CUSTOM_NONE = 0,
  TF_DMG_CUSTOM_HEADSHOT,
  TF_DMG_CUSTOM_BACKSTAB,
  TF_DMG_CUSTOM_BURNING,
  TF_DMG_WRENCH_FIX,
  TF_DMG_CUSTOM_MINIGUN,
  TF_DMG_CUSTOM_SUICIDE,
  TF_DMG_CUSTOM_TAUNTATK_HADOUKEN,
  TF_DMG_CUSTOM_BURNING_FLARE,
  TF_DMG_CUSTOM_TAUNTATK_HIGH_NOON,
  TF_DMG_CUSTOM_TAUNTATK_GRAND_SLAM,
  TF_DMG_CUSTOM_PENETRATE_MY_TEAM,
  TF_DMG_CUSTOM_PENETRATE_ALL_PLAYERS,
  TF_DMG_CUSTOM_TAUNTATK_FENCING,
  TF_DMG_CUSTOM_PENETRATE_NONBURNING_TEAMMATE,
  TF_DMG_CUSTOM_TAUNTATK_ARROW_STAB,
  TF_DMG_CUSTOM_TELEFRAG,
  TF_DMG_CUSTOM_BURNING_ARROW,
  TF_DMG_CUSTOM_FLYINGBURN,
  TF_DMG_CUSTOM_PUMPKIN_BOMB,
  TF_DMG_CUSTOM_DECAPITATION,
  TF_DMG_CUSTOM_TAUNTATK_GRENADE,
  TF_DMG_CUSTOM_BASEBALL,
  TF_DMG_CUSTOM_CHARGE_IMPACT,
  TF_DMG_CUSTOM_TAUNTATK_BARBARIAN_SWING,
  TF_DMG_CUSTOM_AIR_STICKY_BURST,
  TF_DMG_CUSTOM_DEFENSIVE_STICKY,
  TF_DMG_CUSTOM_PICKAXE,
  TF_DMG_CUSTOM_ROCKET_DIRECTHIT,
  TF_DMG_CUSTOM_TAUNTATK_UBERSLICE,
  TF_DMG_CUSTOM_PLAYER_SENTRY,
  TF_DMG_CUSTOM_STANDARD_STICKY,
  TF_DMG_CUSTOM_SHOTGUN_REVENGE_CRIT,
  TF_DMG_CUSTOM_TAUNTATK_ENGINEER_GUITAR_SMASH,
  TF_DMG_CUSTOM_BLEEDING,
  TF_DMG_CUSTOM_GOLD_WRENCH,
  TF_DMG_CUSTOM_CARRIED_BUILDING,
  TF_DMG_CUSTOM_COMBO_PUNCH,
  TF_DMG_CUSTOM_TAUNTATK_ENGINEER_ARM_KILL,
  TF_DMG_CUSTOM_FISH_KILL,
  TF_DMG_CUSTOM_TRIGGER_HURT,
  TF_DMG_CUSTOM_DECAPITATION_BOSS,
  TF_DMG_CUSTOM_STICKBOMB_EXPLOSION,
  TF_DMG_CUSTOM_AEGIS_ROUND,
  TF_DMG_CUSTOM_FLARE_EXPLOSION,
  TF_DMG_CUSTOM_BOOTS_STOMP,
  TF_DMG_CUSTOM_PLASMA,
  TF_DMG_CUSTOM_PLASMA_CHARGED,
  TF_DMG_CUSTOM_PLASMA_GIB,
  TF_DMG_CUSTOM_PRACTICE_STICKY,
  TF_DMG_CUSTOM_EYEBALL_ROCKET,
  TF_DMG_CUSTOM_HEADSHOT_DECAPITATION,
  TF_DMG_CUSTOM_TAUNTATK_ARMAGEDDON,
  TF_DMG_CUSTOM_FLARE_PELLET,
  TF_DMG_CUSTOM_CLEAVER,
  TF_DMG_CUSTOM_CLEAVER_CRIT,
  TF_DMG_CUSTOM_SAPPER_RECORDER_DEATH,
  TF_DMG_CUSTOM_MERASMUS_PLAYER_BOMB,
  TF_DMG_CUSTOM_MERASMUS_GRENADE,
  TF_DMG_CUSTOM_MERASMUS_ZAP,
  TF_DMG_CUSTOM_MERASMUS_DECAPITATION,
  TF_DMG_CUSTOM_CANNONBALL_PUSH,
  TF_DMG_CUSTOM_TAUNTATK_ALLCLASS_GUITAR_RIFF,
  TF_DMG_CUSTOM_THROWABLE,
  TF_DMG_CUSTOM_THROWABLE_KILL,
  TF_DMG_CUSTOM_SPELL_TELEPORT,
  TF_DMG_CUSTOM_SPELL_SKELETON,
  TF_DMG_CUSTOM_SPELL_MIRV,
  TF_DMG_CUSTOM_SPELL_METEOR,
  TF_DMG_CUSTOM_SPELL_LIGHTNING,
  TF_DMG_CUSTOM_SPELL_FIREBALL,
  TF_DMG_CUSTOM_SPELL_MONOCULUS,
  TF_DMG_CUSTOM_SPELL_BLASTJUMP,
  TF_DMG_CUSTOM_SPELL_BATS,
  TF_DMG_CUSTOM_SPELL_TINY,
  TF_DMG_CUSTOM_KART,
  TF_DMG_CUSTOM_GIANT_HAMMER,
  TF_DMG_CUSTOM_RUNE_REFLECT,
  //
  // INSERT NEW ITEMS HERE TO AVOID BREAKING DEMOS
  //

  TF_DMG_CUSTOM_END // END
};

static void do_nothing_stub(void *_this) { g_print("DOING NOTHING\n"); }
static void handleGameEvent_handler(const void *thisPtr,
                                    IGameEvent *gameEvent) {
  const int customkill = gameEvent->GetInt("customkill", TF_DMG_CUSTOM_NONE);
  int i = 0;
  engineClient->Con_NPrintf(i++, "FireGameEvent(this: %p, event: %p)\n",
                            thisPtr, gameEvent);
  engineClient->Con_NPrintf(i++, "\t event name: %s\n", gameEvent->GetName());
  engineClient->Con_NPrintf(i++, "\t weapon name: %s\n",
                            gameEvent->GetString("weapon"));
  engineClient->Con_NPrintf(i++, "\t customkill: %d\n", customkill);
  if (customkill == TF_DMG_CUSTOM_BACKSTAB) {
    gameEvent->SetInt("customkill", TF_DMG_CUSTOM_NONE);
  };
};

class Listener {
public:
  Listener() {
    this->listener =
        gum_make_call_listener(this->on_enter, this->on_leave, this, NULL);
  }
  ~Listener() {
    gum_object_unref(this->listener);
  }
  GumInvocationListener *listener;
  int pootis{42};

private:
  static void on_enter(GumInvocationContext *context, void *user_data) {
    Listener* _this = (Listener*) user_data;
    g_print("Pootis is %d\n\n\n", _this->pootis);
    TestHookId hookid =
        (int)gum_invocation_context_get_listener_function_data(context);
    switch (hookid) {
      case TEST_HOOK_DLOPEN:
        break;
      case TEST_HOOK_FIREGAMEEVENT: {
        const gpointer thisPtr =
            gum_invocation_context_get_nth_argument(context, 0);
        const void *gameEvent =
            gum_invocation_context_get_nth_argument(context, 1);
        handleGameEvent_handler(thisPtr, (IGameEvent *)gameEvent);
      }
    }
  };

  static void on_leave(GumInvocationContext *context, void *user_data) {
    Listener* _this = (Listener*) user_data;
    TestHookId hookid =
        (int)gum_invocation_context_get_listener_function_data(context);
    switch (hookid) {
      case TEST_HOOK_DLOPEN: {
        const gchar *module_name =
            (gchar *)gum_invocation_context_get_nth_argument(context, 0);
        gum_module_ensure_initialized(module_name);
        const GumAddress module_base =
            gum_module_find_base_address(module_name);
        if (g_strrstr(module_name, "client.so")) {
          const gpointer fireGameEvent_ptr = module_base + FIREGAMEEVENT_OFFSET;
          g_print("\n---------------\n");
          g_print("MODULE BASE: %p\n", (void *)module_base);
          g_print("FIREGAMEEVENT_OFFSET: %p\n", FIREGAMEEVENT_OFFSET);
          g_print("COMBINED: %p\n", fireGameEvent_ptr);
          g_print("\n---------------\n");
          gum_interceptor_begin_transaction(interceptor);
          gum_interceptor_attach(interceptor, fireGameEvent_ptr, _this->listener,
                                 (void *)TEST_HOOK_FIREGAMEEVENT);
          gum_interceptor_end_transaction(interceptor);
        } else if (g_strrstr(module_name, "shaderapidx9.so")) {
          const gpointer doStartupShaderPreloading_ptr =
              module_base + DOSTARTUPSHADERPRELOADING_OFFSET;
          gum_interceptor_begin_transaction(interceptor);
          gum_interceptor_replace(interceptor, doStartupShaderPreloading_ptr,
                                  (gpointer)do_nothing_stub, NULL);
          gum_interceptor_end_transaction(interceptor);
        } else if (g_strrstr(module_name, "engine.so")) {
          const CreateInterfaceFn interfaceFactory =
              (void *)gum_module_find_export_by_name(module_name,
                                                     "CreateInterface");
          if (engineClient == nullptr) {
            engineClient = interfaceFactory("VEngineClient013", nullptr);
            g_print("Found engine client: %p\n", engineClient);
          };
        };
        break;
      }
      case TEST_HOOK_FIREGAMEEVENT:
        break;
    }
  };
};

void do_dlopen_shit(void) {
  interceptor = gum_interceptor_obtain();
  auto listener = new Listener();

  GumAddress dlopen_address = gum_module_find_export_by_name(NULL, "dlopen");
  gum_interceptor_begin_transaction(interceptor);
  gum_interceptor_attach(interceptor, (void *)dlopen_address, listener->listener,
                         (void *)TEST_HOOK_DLOPEN);
  gum_interceptor_end_transaction(interceptor);
}

__attribute__((constructor)) void pooter_init(void) {
  if (!have_we_inited) {
    gum_init_embedded();
    do_dlopen_shit();
  };
}

__attribute__((destructor)) void pooter_deinit(void) {
  if (have_we_inited) {
    gum_object_unref(interceptor);
    gum_deinit_embedded();
  };
};
