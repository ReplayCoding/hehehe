#include "frida-gum.h"

const gpointer DOSTARTUPSHADERPRELOADING_OFFSET = 0x60ae0;
const gpointer FIREGAMEEVENT_OFFSET = 0x1154430;

typedef int (*printf_t)(const char *restrict format, ...);

static bool have_we_inited = false;

GumInterceptor *interceptor = NULL;
GumInvocationListener *listener = NULL;

struct _TestListener {
  GObject parent;
};

enum _TestHookId { TEST_HOOK_DLOPEN, TEST_HOOK_FIREGAMEEVENT };
typedef enum _TestHookId TestHookId;

static void test_listener_interface_init(gpointer g_iface, gpointer iface_data);

#define TEST_TYPE_LISTENER (test_listener_get_type())
G_DECLARE_FINAL_TYPE(TestListener, test_listener, TEST, LISTENER, GObject)
G_DEFINE_TYPE_WITH_CODE(TestListener, test_listener, G_TYPE_OBJECT,
                        G_IMPLEMENT_INTERFACE(GUM_TYPE_INVOCATION_LISTENER,
                                              test_listener_interface_init))

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

static void handleGameEvent_handler(const void *thisPtr,
                                    const void **gameEvent) {
  // In GNU C, sizeof void is 1, so pointer arithmetic just works like
  // regular arithmetic (i think?)
  // https://gcc.gnu.org/onlinedocs/gcc/Pointer-Arith.html#Pointer-Arith
  typedef char *(*GetName_t)(void *this);
  typedef char *(*GetString_t)(void *this, char *keyName, char *defaultValue);
  typedef int (*GetInt_t)(void *this, char *keyName, int defaultValue);
  typedef void (*SetInt_t)(void *this, char *keyName, int value);

  const GetName_t *getName = *gameEvent + (2 * sizeof(void *));
  const GetString_t *getString = *gameEvent + (9 * sizeof(void *));
  const GetInt_t *getInt = *gameEvent + (7 * sizeof(void *));
  const SetInt_t *setInt = *gameEvent + (11 * sizeof(void *));

  const int customkill = (*getInt)(gameEvent, "customkill", NULL);
  g_print("FireGameEvent(this: %p, event: %p)\n", thisPtr, gameEvent);
  g_print("\t event name: %s\n", (*getName)(gameEvent));
  g_print("\t weapon name: %s\n", (*getString)(gameEvent, "weapon", NULL));
  g_print("\t customkill: %d\n", customkill);
  if (customkill == TF_DMG_CUSTOM_BACKSTAB) {
    (*setInt)(gameEvent, "customkill", TF_DMG_CUSTOM_NONE);
  };
};

static void test_listener_on_enter(GumInvocationListener *self,
                                   GumInvocationContext *context) {
  TestHookId hook_id = GUM_IC_GET_FUNC_DATA(context, TestHookId);
  switch (hook_id) {
    case TEST_HOOK_DLOPEN:
      break;
    case TEST_HOOK_FIREGAMEEVENT: {
      const gpointer thisPtr =
          gum_invocation_context_get_nth_argument(context, 0);
      const void **gameEvent =
          gum_invocation_context_get_nth_argument(context, 1);
      handleGameEvent_handler(thisPtr, gameEvent);
    }
  }
};

static void doNothingLol(void *_this) { g_print("DOING NOTHING\n"); }

static void test_listener_on_leave(GumInvocationListener *self,
                                   GumInvocationContext *context) {
  TestHookId hook_id = GUM_IC_GET_FUNC_DATA(context, TestHookId);
  switch (hook_id) {
    case TEST_HOOK_DLOPEN: {
      const gchar *module_name =
          gum_invocation_context_get_nth_argument(context, 0);
      gum_module_ensure_initialized(module_name);
      const GumAddress module_base = gum_module_find_base_address(module_name);
      if (g_strrstr(module_name, "client.so")) {
        const gpointer fireGameEvent_ptr = module_base + FIREGAMEEVENT_OFFSET;
        g_print("\n---------------\n");
        g_print("MODULE BASE: %p\n", GSIZE_TO_POINTER(module_base));
        g_print("FIREGAMEEVENT_OFFSET: %p\n", FIREGAMEEVENT_OFFSET);
        g_print("COMBINED: %p\n", fireGameEvent_ptr);
        g_print("\n---------------\n");
        gum_interceptor_begin_transaction(interceptor);
        gum_interceptor_attach(interceptor, fireGameEvent_ptr, self,
                               GSIZE_TO_POINTER(TEST_HOOK_FIREGAMEEVENT));
        gum_interceptor_end_transaction(interceptor);
      } else if (g_strrstr(module_name, "shaderapidx9.so")) {
        const gpointer doStartupShaderPreloading_ptr =
            module_base + DOSTARTUPSHADERPRELOADING_OFFSET;
        gum_interceptor_begin_transaction(interceptor);
        gum_interceptor_replace(interceptor, doStartupShaderPreloading_ptr,
                                doNothingLol, NULL);
        gum_interceptor_end_transaction(interceptor);
      };
      break;
    }
    case TEST_HOOK_FIREGAMEEVENT:
      break;
  }
};

static void test_listener_interface_init(gpointer g_iface,
                                         gpointer iface_data) {
  GumInvocationListenerInterface *iface = g_iface;

  iface->on_enter = test_listener_on_enter;
  iface->on_leave = test_listener_on_leave;
}

static void test_listener_class_init(TestListenerClass *klass) {}
static void test_listener_init(TestListener *self) {}

void do_printf_shit(void) {
  printf_t printf_found =
      GSIZE_TO_POINTER(gum_module_find_export_by_name(NULL, "printf"));
  printf_found("\n\n------------LOADED------------\n");
  printf_found("printf is at 0x%X\n", printf_found);
  printf_found("FRIDA WORKS\n\n");
}

void do_dlopen_shit(void) {
  interceptor = gum_interceptor_obtain();
  listener = g_object_new(TEST_TYPE_LISTENER, NULL);
  GumAddress dlopen_address = gum_module_find_export_by_name(NULL, "dlopen");
  gum_interceptor_begin_transaction(interceptor);
  gum_interceptor_attach(interceptor, GSIZE_TO_POINTER(dlopen_address),
                         listener, GSIZE_TO_POINTER(TEST_HOOK_DLOPEN));
  gum_interceptor_end_transaction(interceptor);
}

__attribute__((constructor)) void pooter_init(void) {
  if (!have_we_inited) {
    gum_init_embedded();
    do_printf_shit();
    do_dlopen_shit();
  };
}

__attribute__((destructor)) void pooter_deinit(void) {
  if (have_we_inited) {
    g_object_unref(interceptor);
    g_object_unref(listener);
    gum_deinit_embedded();
  };
};
