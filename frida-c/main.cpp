#include "listener.hpp"
#include "sdk.hpp"
#include <frida-gum.h>

const gpointer DOSTARTUPSHADERPRELOADING_OFFSET =
    reinterpret_cast<gpointer>(0x60ae0);
const gpointer FIREGAMEEVENT_OFFSET = reinterpret_cast<gpointer>(0x1154430);

typedef int printf_t(const char *format, ...);

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

class MyListener : public ListenerWrapper {
public:
  MyListener(auto interceptor) : ListenerWrapper(), interceptor(interceptor){};
  virtual void on_enter(GumInvocationContext *context) {
    TestHookId hookid = reinterpret_cast<int>(
        gum_invocation_context_get_listener_function_data(context));
    switch (hookid) {
      case TEST_HOOK_DLOPEN:
        break;
      case TEST_HOOK_FIREGAMEEVENT: {
        const gpointer thisPtr =
            gum_invocation_context_get_nth_argument(context, 0);
        const auto gameEvent = static_cast<IGameEvent *>(
            gum_invocation_context_get_nth_argument(context, 1));
        this->handleGameEvent_handler(thisPtr, gameEvent);
      }
    }
  };

  virtual void on_leave(GumInvocationContext *context) {
    TestHookId hookid = reinterpret_cast<int>(
        gum_invocation_context_get_listener_function_data(context));
    switch (hookid) {
      case TEST_HOOK_DLOPEN: {
        const gchar *module_name = static_cast<gchar *>(
            gum_invocation_context_get_nth_argument(context, 0));
        gum_module_ensure_initialized(module_name);
        const GumAddress module_base =
            gum_module_find_base_address(module_name);
        if (g_strrstr(module_name, "client.so")) {
          const gpointer fireGameEvent_ptr = module_base + FIREGAMEEVENT_OFFSET;
          gum_interceptor_begin_transaction(this->interceptor);
          gum_interceptor_attach(
              this->interceptor, fireGameEvent_ptr, this->listener,
              reinterpret_cast<void *>(TEST_HOOK_FIREGAMEEVENT));
          gum_interceptor_end_transaction(this->interceptor);
        } else if (g_strrstr(module_name, "shaderapidx9.so")) {
          const gpointer doStartupShaderPreloading_ptr =
              module_base + DOSTARTUPSHADERPRELOADING_OFFSET;
          gum_interceptor_begin_transaction(this->interceptor);
          gum_interceptor_replace(this->interceptor,
                                  doStartupShaderPreloading_ptr,
                                  (gpointer)do_nothing_stub, nullptr);
          gum_interceptor_end_transaction(this->interceptor);
        } else if (g_strrstr(module_name, "engine.so")) {
          const auto interfaceFactory = reinterpret_cast<CreateInterfaceFn>(
              gum_module_find_export_by_name(module_name, "CreateInterface"));
          if (this->engineClient == nullptr) {
            this->engineClient = static_cast<IVEngineClient013 *>(
                interfaceFactory(VENGINE_CLIENT_INTERFACE_VERSION_13, nullptr));
            g_print("Found engine client: %p\n", this->engineClient);
          };
        };
        break;
      }
      case TEST_HOOK_FIREGAMEEVENT:
        break;
    }
  };

private:
  IVEngineClient013 *engineClient{};
  GumInterceptor *interceptor{};

  static void do_nothing_stub() { g_print("DOING NOTHING\n"); }
  void handleGameEvent_handler(const void *thisPtr, IGameEvent *gameEvent) {
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
};

static bool have_we_inited = false;
GumInterceptor *interceptor{};
ListenerWrapper *listener{};

void do_dlopen_shit(void) {
  interceptor = gum_interceptor_obtain();
  listener = new MyListener(interceptor);

  auto dlopen_address = reinterpret_cast<void *>(
      gum_module_find_export_by_name(nullptr, "dlopen"));
  gum_interceptor_begin_transaction(interceptor);
  gum_interceptor_attach(interceptor, dlopen_address, listener->get_listener(),
                         reinterpret_cast<void *>(TEST_HOOK_DLOPEN));
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
    delete listener;
    gum_deinit_embedded();
  };
}
