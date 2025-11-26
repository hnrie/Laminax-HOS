/**
 * LaminaxActionMode:
 * @Laminax_ACTION_MODE_NONE: block action
 * @Laminax_ACTION_MODE_NORMAL: allow action when in window mode,
 *     e.g. when the focus is in an application window
 * @Laminax_ACTION_MODE_OVERVIEW: allow action while the overview
 *     is active
 * @Laminax_ACTION_MODE_LOCK_SCREEN: allow action when the screen
 *     is locked, e.g. when the screen shield is shown
 * @Laminax_ACTION_MODE_UNLOCK_SCREEN: allow action in the unlock
 *     dialog
 * @Laminax_ACTION_MODE_LOGIN_SCREEN: allow action in the login screen
 * @Laminax_ACTION_MODE_SYSTEM_MODAL: allow action when a system modal
 *     dialog (e.g. authentication or session dialogs) is open
 * @Laminax_ACTION_MODE_LOOKING_GLASS: allow action in looking glass
 * @Laminax_ACTION_MODE_POPUP: allow action while a shell menu is open
 * @Laminax_ACTION_MODE_ALL: always allow action
 *
 * Controls in which Laminax states an action (like keybindings and gestures)
 * should be handled.
*/
typedef enum {
  Laminax_ACTION_MODE_NONE          = 0,
  Laminax_ACTION_MODE_NORMAL        = 1 << 0,
  Laminax_ACTION_MODE_OVERVIEW      = 1 << 1,
  Laminax_ACTION_MODE_LOCK_SCREEN   = 1 << 2,
  Laminax_ACTION_MODE_UNLOCK_SCREEN = 1 << 3,
  Laminax_ACTION_MODE_LOGIN_SCREEN  = 1 << 4,
  Laminax_ACTION_MODE_SYSTEM_MODAL  = 1 << 5,
  Laminax_ACTION_MODE_LOOKING_GLASS = 1 << 6,
  Laminax_ACTION_MODE_POPUP         = 1 << 7,

  Laminax_ACTION_MODE_ALL = ~0,
} LaminaxActionMode;

