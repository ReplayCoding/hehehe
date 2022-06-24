#include "frida-gum.h"

typedef int (*printf_t)(const char *restrict format, ...);

static bool have_we_inited = false;

GumInterceptor *interceptor = NULL;
GumInvocationListener *listener = NULL;

struct _TestListener {
  GObject parent;
};

enum _TestHookId { TEST_HOOK_DLOPEN };
typedef enum _TestHookId TestHookId;

static void test_listener_interface_init(gpointer g_iface, gpointer iface_data);

#define TEST_TYPE_LISTENER (test_listener_get_type())
G_DECLARE_FINAL_TYPE(TestListener, test_listener, TEST, LISTENER, GObject)
G_DEFINE_TYPE_WITH_CODE(TestListener, test_listener, G_TYPE_OBJECT,
                        G_IMPLEMENT_INTERFACE(GUM_TYPE_INVOCATION_LISTENER,
                                              test_listener_interface_init))

static void test_listener_on_enter(GumInvocationListener *self,
                                   GumInvocationContext *context){};

static void test_listener_on_leave(GumInvocationListener *self,
                                   GumInvocationContext *context) {
  TestHookId hook_id = GUM_IC_GET_FUNC_DATA(context, TestHookId);
  switch (hook_id) {
  case TEST_HOOK_DLOPEN: {
    const gchar *module_name =
        gum_invocation_context_get_nth_argument(context, 0);
    if (g_strrstr(module_name, "client.so") &&
        gum_module_ensure_initialized(module_name)) {
    };
    break;
  }
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
