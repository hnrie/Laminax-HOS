/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_SCREENSHOT_H__
#define __Laminax_SCREENSHOT_H__

/**
 * SECTION:Laminax-screenshot
 * @short_description: Grabs screenshots of areas and/or windows
 *
 * The #LaminaxScreenshot object is used to take screenshots of screen
 * areas or windows and write them out as png files.
 *
 */

typedef struct _LaminaxScreenshot      LaminaxScreenshot;
typedef struct _LaminaxScreenshotClass LaminaxScreenshotClass;

#define Laminax_TYPE_SCREENSHOT              (Laminax_screenshot_get_type ())
#define Laminax_SCREENSHOT(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), Laminax_TYPE_SCREENSHOT, LaminaxScreenshot))
#define Laminax_SCREENSHOT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), Laminax_TYPE_SCREENSHOT, LaminaxScreenshotClass))
#define Laminax_IS_SCREENSHOT(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), Laminax_TYPE_SCREENSHOT))
#define Laminax_IS_SCREENSHOT_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), Laminax_TYPE_SCREENSHOT))
#define Laminax_SCREENSHOT_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), Laminax_TYPE_SCREENSHOT, LaminaxScreenshotClass))

GType Laminax_screenshot_get_type (void) G_GNUC_CONST;

LaminaxScreenshot *Laminax_screenshot_new (void);

typedef void (*LaminaxScreenshotCallback)  (LaminaxScreenshot *screenshot,
                                           gboolean success,
                                           cairo_rectangle_int_t *screenshot_area);

typedef void (*LaminaxScreenshotPickColorCallback)  (LaminaxScreenshot *screenshot,
                                                      gboolean success,
                                                      ClutterColor *color);

void    Laminax_screenshot_screenshot_area      (LaminaxScreenshot *screenshot,
                                                gboolean include_cursor,
                                                int x,
                                                int y,
                                                int width,
                                                int height,
                                                const char *filename,
                                                LaminaxScreenshotCallback callback);

void    Laminax_screenshot_screenshot_window    (LaminaxScreenshot *screenshot,
                                                gboolean include_frame,
                                                gboolean include_cursor,
                                                const char *filename,
                                                LaminaxScreenshotCallback callback);

void    Laminax_screenshot_screenshot           (LaminaxScreenshot *screenshot,
                                                gboolean include_cursor,
                                                const char *filename,
                                                LaminaxScreenshotCallback callback);

void    Laminax_screenshot_pick_color         (LaminaxScreenshot *screenshot,
                                                int x,
                                                int y,
                                                LaminaxScreenshotPickColorCallback callback);

#endif /* ___Laminax_SCREENSHOT_H__ */
