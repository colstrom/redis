#include "server.h"
#include <mruby.h>

void mrbScriptingInit(int setup) {
  mrb_state *mrb = mrb_open();

  if (setup) server.mrb_client = NULL;

  if (server.mrb_client == NULL) {
    server.mrb_client = createClient(-1);
    server.mrb_client->flags |= CLIENT_LUA;
  }

  server.mrb = mrb;
}
