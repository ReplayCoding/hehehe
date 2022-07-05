#pragma once
#include <frida-gum.h>

class ListenerWrapper {
public:
  ListenerWrapper() {
    this->listener = gum_make_call_listener(
        this->on_enter_wrapper, this->on_leave_wrapper, this, nullptr);
  }
  virtual ~ListenerWrapper() { gum_object_unref(this->listener); }
  auto get_listener() { return this->listener; };

  virtual void on_enter(GumInvocationContext *context) = 0;
  virtual void on_leave(GumInvocationContext *context) = 0;

private:
  static void on_enter_wrapper(GumInvocationContext *context, void *user_data) {
    auto _this = static_cast<ListenerWrapper *>(user_data);
    _this->on_enter(context);
  };
  static void on_leave_wrapper(GumInvocationContext *context, void *user_data) {
    auto _this = static_cast<ListenerWrapper *>(user_data);
    _this->on_leave(context);
  };

protected:
  GumInvocationListener *listener{};
};
