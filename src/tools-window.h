#ifndef __TOOLS_WINDOW__
#define __TOOLS_WINDOW__

#include "loongson-utils.h"
G_BEGIN_DECLS

#define TOOLS_TYPE_WINDOW         (tools_window_get_type ())

G_DECLARE_FINAL_TYPE (ToolsWindow, tools_window, TOOLS, WINDOW, GtkWindow);

GtkWidget         *tools_window_new                     (void);

G_END_DECLS

#endif
