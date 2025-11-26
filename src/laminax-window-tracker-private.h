/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_WINDOW_TRACKER_PRIVATE_H__
#define __Laminax_WINDOW_TRACKER_PRIVATE_H__

#include "Laminax-window-tracker.h"

void _Laminax_window_tracker_add_child_process_app (LaminaxWindowTracker *tracker,
                                                  GPid                pid,
                                                  LaminaxApp           *app);

#endif
