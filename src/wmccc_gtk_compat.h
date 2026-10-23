/*
 * GTK+ 3 compatibility helpers for the Glade 2 generated wmccc interface.
 *
 * The interface was generated for GTK+ 2 and still uses a few widgets and
 * entry points that were removed in GTK+ 3.  Keeping the small compatibility
 * layer here lets the generated code stay close to its original form while
 * building against the modern, Wayland-capable toolkit.
 */

#ifndef WMCCC_GTK_COMPAT_H
#define WMCCC_GTK_COMPAT_H

#include <gtk/gtk.h>

/* GtkTooltips was replaced by the per-widget tooltip API. */
typedef struct _WmcccTooltips GtkTooltips;

static inline GtkTooltips *
wmccc_tooltips_new (void)
{
  return NULL;
}

static inline void
wmccc_tooltips_set_tip (GtkTooltips *tooltips,
                        GtkWidget   *widget,
                        const gchar *tip_text,
                        const gchar *tip_private)
{
  (void) tooltips;
  (void) tip_private;
  gtk_widget_set_tooltip_text (widget, tip_text);
}

#define gtk_tooltips_new        wmccc_tooltips_new
#define gtk_tooltips_set_tip    wmccc_tooltips_set_tip

/* GTK+ 3 removed the GTK+ 1/2 object macros. */
#define GTK_OBJECT(object)      G_OBJECT(object)
#define gtk_widget_ref          g_object_ref
#define gtk_widget_unref        g_object_unref

/* GTK_WIDGET_SET_FLAGS() disappeared; the generated dialogs only set
 * GTK_CAN_DEFAULT on their buttons. */
#ifndef GTK_CAN_DEFAULT
#define GTK_CAN_DEFAULT 0
#endif

#define GTK_WIDGET_SET_FLAGS(widget, flags)                 \
  do {                                                      \
    (void) (flags);                                         \
    gtk_widget_set_can_default (GTK_WIDGET (widget), TRUE); \
  } while (0)

/* GtkOptionMenu was replaced by GtkComboBox.  The generated code still
 * creates a GtkMenu and hands it over; turn the menu labels into combo box
 * rows so the rest of wmccc can keep using history indices. */
static inline GtkWidget *
wmccc_option_menu_new (void)
{
  return gtk_combo_box_text_new ();
}

static inline void
wmccc_option_menu_set_menu (GtkComboBox *option_menu, GtkWidget *menu)
{
  GList *children, *l;

  children = gtk_container_get_children (GTK_CONTAINER (menu));
  for (l = children; l; l = l->next) {
    const gchar *label;
    gchar *text, *src, *dst;

    if (!GTK_IS_MENU_ITEM (l->data))
      continue;
    label = gtk_menu_item_get_label (GTK_MENU_ITEM (l->data));
    if (!label)
      continue;

    /* GtkMenu labels may contain mnemonic underscores. */
    text = g_strdup (label);
    for (src = text, dst = text; *src; ) {
      if (src[0] == '_' && src[1] == '_') {
        *dst++ = '_';
        src += 2;
      } else if (src[0] == '_') {
        ++src;
      } else {
        *dst++ = *src++;
      }
    }
    *dst = '\0';

    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (option_menu), text);
    g_free (text);
  }
  g_list_free (children);

  gtk_combo_box_set_active (GTK_COMBO_BOX (option_menu), 0);
}

static inline gint
wmccc_option_menu_get_history (GtkComboBox *option_menu)
{
  return gtk_combo_box_get_active (GTK_COMBO_BOX (option_menu));
}

static inline void
wmccc_option_menu_set_history (GtkComboBox *option_menu, gint index)
{
  gtk_combo_box_set_active (GTK_COMBO_BOX (option_menu), index);
}

#define gtk_option_menu_new         wmccc_option_menu_new
#define gtk_option_menu_set_menu    wmccc_option_menu_set_menu
#define gtk_option_menu_get_history wmccc_option_menu_get_history
#define gtk_option_menu_set_history wmccc_option_menu_set_history
#define GTK_OPTION_MENU(widget)     GTK_COMBO_BOX(widget)

/* GtkCombo was removed in GTK+ 3.  GtkComboBoxText with an embedded entry
 * provides the same editable combo box behaviour. */
static inline GtkWidget *
wmccc_combo_new (void)
{
  return gtk_combo_box_text_new_with_entry ();
}

static inline void
wmccc_combo_set_popdown_strings (GtkWidget *combo, GList *items)
{
  GList *l;

  for (l = items; l; l = l->next)
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo),
                                    (const gchar *) l->data);
}

#define gtk_combo_new                   wmccc_combo_new
#define gtk_combo_set_popdown_strings   wmccc_combo_set_popdown_strings

#endif /* WMCCC_GTK_COMPAT_H */
