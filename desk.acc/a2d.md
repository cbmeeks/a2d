# MGTK API

There are three distinct API classes that need to be used:

* Main MGTK API - entry point $4000 AUX, called MLI-style (JSR followed by command type and address of param block)
* DeskTop Jump Table - simple JSR calls starting at $4003 MAIN, no arguments
* DeskTop API - another MLI-style interface starting at $8E00 AUX

## Main MGTK API

### Concepts

#### GrafPort

The _port_ block is reused in several places. It has the following structure:

```
  .word left           pixels from screen left edge
  .word top            pixels from screen top edge
  .addr addr           screen address $2000
  .word stride         screen stride $80
  .word hoff           pixels scrolled left
  .word voff           pixels scrolled up
  .word width          pixels wide
  .word height         pixels tall
```

Drawing state can be set to a port (SetPort) and then subsequent operations will occur
within the port: they will be clipped to the bounds and the offset will be taken into account.
For example, if hoffset is 15 and voffset is 5 then a pixel plotted at 40, 40 will appear
at 40 - 15 = 25 pixels from the left edge and 40 - 5 = 35 pixels from the top edge.

#### Pattern

A simple repeating 8x8 _pattern_ is defined by 8 bytes. All bits of each byte are used.

#### Window Parts

Windows have a _client area_ which has the requested dimensions. Above this is an optional
_title bar_ which in turn has an optional _close box_. Within the client area are an
optional _resize box_ and optional _scroll bars_.


### Desk Accessory Lifecycle

* Loaded/invoked at $800 MAIN (have through $1FFF available)
* Save stack
* Copy DA code from MAIN to AUX (e.g. using AUXMOVE)
* Transfer control to AUX
* Turn on ALTZP and LCBANK1
* Create window (OpenWindow)
* Draw everything
* Call $2B (no idea what this does!)
* Enter [input loop](#input-loop)
* ...
* Destroy window (CloseWindow)
* Redraw desktop icons (DESKTOP_REDRAW_ICONS)
* Switch control back to MAIN (RAMRDOFF/RAMWRTOFF)
* Ensure ALTZP and LCBANK1 are on
* Restore stack
* RTS

> NOTE: Movable windows must maintain an _offscreen_flag_. If a window is moved so that the
> client area is entirely offscreen then various operations should be skipped because
> the window's box coordinates will not be set correctly.

#### Input Loop

* Call GetEvent.
* If a key, then check modifiers (Open/Closed Apple) and key code, ignore or take action.
* If a click, call FindWindow.
* If target id not the window id, ignore.
* If target element is desktop or menu then ignore.
* If target element is close box then initiate [window close](#window-close).
* If target element is title bar then initiate [window drag](#window-drag).
* If target element is resize box then initiate [window resize](#window-resize).
* Otherwise, it is client area; call FindControl.
* If client part is a scrollbar then initiate a [scroll](#window-scroll).
* Otherwise, handle a client click using custom logic (e.g. hit testing buttons, etc)

#### Window Close

* Call TrackGoAway, which enters a modal loop to handle the mouse moving out/in the box.
* Result indicates clicked or canceled; if clicked, exit the DA, otherwise ignore.

#### Window Drag

* Call DragWindow.
* Call JUMP_TABLE_REDRAW_ALL.
* If _offscreen flag_ was not set, redraw desktop icons (DESKTOP_REDRAW_ICONS).
* Set _offscreen flag_ if window's `top` is greater than or equal to the screen bottom (191), clear otherwise.
* If _offscreen flag_ is not set, redraw window.

#### Window Resize

* Call GrowWindow.
* Call JUMP_TABLE_REDRAW_ALL.
* Call DESKTOP_REDRAW_ICONS.
* Call UpdateThumb if needed to adjust scroll bar settings. (Details TBD).
* Redraw window.

#### Window Scroll


### Drawing Operations



### Commands


## DeskTop Jump Table

Call from MAIN (RAMRDOFF/RAMWRTOFF). Call style:

```
   jsr $xxxx
```

> NOTE: Most of these calls have not been identified yet.

#### JUMP_TABLE_REDRAW_ALL ($4015)

Redraws all DeskTop windows. Required after a drag or resize. Follow with DESKTOP_REDRAW_ICONS call.

#### JUMP_TABLE_CLEAR_SEL ($401E)

Deselect all DeskTop icons (volumes/files).

#### JUMP_TABLE_CUR_POINTER ($4039)

Changes mouse cursor to the default pointer. Note that bitmap is in the language card memory so it must be swapped in.

#### JUMP_TABLE_CUR_WATCH ($403C)

Changes mouse cursor to a watch. Note that bitmap is in the language card memory so it must be swapped in.

## DeskTop API

Call from AUX (RAMRDON/RAMWRTON). Call style:

```
   jsr $8E00
   .byte command
   .addr params
```

> NOTE: Only a single call has been identified so far.

Commands:

#### DESKTOP_REDRAW_ICONS ($0C)

Parameters: none (pass $0000 as address)

Redraws the icons on the desktop (mounted volumes, trash). This call is required in these cases:

* After destroying a window (CloseWindow)
* After repainting the desktop (JUMP_TABLE_REDRAW_ALL) following a drag (DragWindow)
* After repainting the desktop (JUMP_TABLE_REDRAW_ALL) following a resize (GrowWindow)
