;;; ==================================================
;;; Mouse Graphics Tool Kit (w/ Graphics Primitives)
;;; ==================================================

.scope MGTK

MLI     := $4000
        ;; MLI-style call (jsr MLI ; .byte call ; .addr params)
        ;; Call from AUX (RAMRDON/RAMWRTON)

;;; ==================================================
;;; Graphics Primitives
;;; ==================================================

NoOp                := $00  ; No-op
        ;; (input length 0 bytes)

;;; --------------------------------------------------
;;; Initialization Commands

InitGraf        := $01

SetSwitches     := $02      ; Configure display switches
        ;; (input length 1 byte)
        ;; Turns on 80 col/DHR, and then:
        ;;   bit 0: LoRes if clear, HiRes if set
        ;;   bit 1: Page 1 if clear, Page 2 if set
        ;;   bit 2: Full screen if clear, split screen if set
        ;;   bit 3: Graphics if clear, text if set

;;; --------------------------------------------------
;;; GrafPort Commands

InitPort        := $03  ; Get screen state
        ;; (input length 0 bytes)
        ;; (output length 36 bytes)
        ;; .word left
        ;; .word top
        ;; .addr addr           screen_mapbits
        ;; .word stride         screen_mapwidth
        ;; .word hoff
        ;; .word voff
        ;; .word width          560-1
        ;; .word height         192-1
        ;; .res 8 pattern
        ;; .byte mskand         AND mask, default $FF
        ;; .byte mskor          ORA mask, default $00
        ;; .word xpos
        ;; .word ypos
        ;; .byte hthick         horizontal pen thickness
        ;; .byte vthick         vertical pen thickness
        ;; .byte 0              ??? fill mode?
        ;; .byte tmask          AND mask for text, default $7F
        ;; .addr font

SetPort   := $04          ; Set full drawing state
        ;; (input length 36 bytes)
        ;; .word left           pixels from screen edge
        ;; .word top
        ;; .addr addr           screen_mapbits
        ;; .word stride         screen_mapwidth
        ;; .word hoff           pixels scrolled
        ;; .word voff
        ;; .word width          pixels
        ;; .word height
        ;; .res 8 pattern
        ;; .byte mskand         AND mask, default $FF
        ;; .byte mskor          ORA mask, default $00
        ;; .word xpos
        ;; .word ypos
        ;; .byte hthick         horizontal pen thickness
        ;; .byte vthick         vertical pen thickness
        ;; .byte 0              ??? fill mode?
        ;; .byte tmask          AND mask for text, default $7F
        ;; .addr font

GetPort         := $05          ; Get pointer to active drawing state
        ;; (input length 0 bytes)
        ;; (output length 2 bytes)
        ;; .addr state          (out)

SetPortBits     := $06            ; Set just the drawing box, subset of full state
        ;; (input length 16 bytes)
        ;; .word left           pixels from screen edge
        ;; .word top
        ;; .addr addr           screen_mapbits ($2000)
        ;; .word stride         screen_mapwidth ($80)
        ;; .word hoff           pixels scrolled
        ;; .word voff
        ;; .word width          pixels
        ;; .word height

SetPenMode      := $07
        ;; (input length 1 byte)
        ;; .byte mode           (>=4 also sets eor mask to $7f)
        ;;    0 = white (???)
        ;;    2 = black (???)

SetPattern      := $08
        ;; (input length 8 bytes)
        ;; .res 8 pattern       8x8 pixel pattern for PaintRect calls

SetColorMasks   := $09
        ;; (input length 2 bytes)
        ;; .byte mskand
        ;; .byte mskor

SetPenSize      := $0A
        ;; (input length 2 bytes)
        ;; .byte hthick         horizontal pen thickness
        ;; .byte vthick         vertical pen thickness

SetFont         := $0B
        ;; (input length 2 bytes)
        ;; .addr font           font definition (see below)

SetTextBG       := $0C      ; Stored to background before XORing glyph
        ;; (input length 1 byte)
        ;; .byte mask

;;; --------------------------------------------------
;;; Drawing Commands

Move            := $0D ; Adjust start of subsequent DRAW_TEXT, DRAW_LINE
        ;; (input length 4 bytes)
        ;; .word xdelta
        ;; .word ydelta

MoveTo          := $0E          ; Start of subsequent DRAW_TEXT, DRAW_LINE
        ;; (input length 4 bytes)
        ;; .word xcoord
        ;; .word ycoord

Line            := $0F          ; Draw line (from SET_POS)
        ;; (input length 4 bytes)
        ;; .word xdelta         signed, delta in pixels
        ;; .word ydelta

LineTo          := $10        ; Draw line (from SET_POS)
        ;; (input length 4 bytes)
        ;; .word xcoord         end coords in pixels
        ;; .word ycoord

PaintRect       := $11          ; Fill rectangle with selected simple pattern/thickness
        ;; (input length 8 bytes)
        ;; .word left           (includes scroll pos)
        ;; .word top
        ;; .word right          pixels
        ;; .word bottom

FrameRect       := $12          ; Draw rectangle with selected simple pattern/thickness
        ;; (input length 8 bytes)
        ;; .word left           pixels
        ;; .word top
        ;; .word right
        ;; .word bottom

InRect          := $13          ; Is pos (via SET_POS) in bounds? Returns true/false in A
        ;; (input length 8 bytes)
        ;; .word left
        ;; .word top
        ;; .word right
        ;; .word bottom

PaintBits       := $14          ; Draw pattern
        ;; (input length 16 bytes)
        ;; .word left
        ;; .word top
        ;; .addr bitmap         bitmap is 7 bits per byte, 0 = black, 1 = white
        ;; .byte stride         bitmap width in bytes
        ;; .byte 0              ???
        ;; .word hoff           offset within bitmap definition
        ;; .word voff
        ;; .word width          pixels
        ;; .word height         pixels

PaintPoly       := $15
        ;; (input length 0 bytes)

FramePoly       := $16       ; Draw multiple closed polygons
        ;; (input length 0 bytes)
        ;; Address points at struct:
        ;; .byte points         count
        ;; .byte flag           high bit clear if this is last polygon, set if not
        ;; .word x1, y1
        ;; .word x2, y2
        ;; ...

        ;; $17 used in DeskTop but not DAs - maybe selection drag
        ;; (input length 0 bytes)

InPoly          := $16

;;; --------------------------------------------------
;;; Text Commands

TextWidth       := $18         ; Measure the width of a string in pixels
        ;; (input length 3 bytes)
        ;; .addr data
        ;; .byte length
        ;; .word width          result in pixels

DrawText        := $19          ; Drawn at last SET_POS as left, baseline
        ;; (input length 3 bytes)
        ;; .addr data
        ;; .byte length

;;; --------------------------------------------------
;;; Utility Commands

SetZP1          := $1A  ; Configure ZP usage by API (speed vs. convenience)
        ;; (input length 1 byte)
        ;; .byte flag           (AWS_CZP_*; high bit set = preserve ZP during calls)

SetZP2          := $1B  ; Stash or restore lower 128 bytes of ZP; calls are idempotent
        ;; (input length 1 byte)
        ;; .byte flag           (high bit set = stash ZP, clear = unstash ZP)

Version         := $1C          ; ???

;;; ==================================================
;;; Mouse Graphics Tool Kit Calls
;;; ==================================================

;;; --------------------------------------------------
;;; Initialization Calls

StartDeskTop    := $1D  ; Inits state, registers interrupt handler, draws desktop
        ;; (input length 12 byte)
        ;; ???

StopDeskTop     := $1E       ; Deallocates interrupt, hides cursor
        ;; (no parameters; pass $0000 as address)


;;; $1F ???

SetUserHook     := $20           ; Install mouse hook; A=0 on success, $95 if mouse disabled
        ;; NOTE: Doesn't set the internal flag - buggy ???
        ;; (input length 2 bytes)
        ;; (output length 2 bytes)
        ;; .addr hook           Mouse hook routine to install
        ;; .addr mouse_state    (out) Address of mouse state (.word x, y; .byte status)

ScaleMouse      := $21          ; ???

KeyboardMouse   := $22          ; ???

GetIntHandler   := $23  ; Get address of interrupt handler
        ;; (input length 0)
        ;; (output length 2 bytes)
        ;; .addr handler        (out) Address of interrupt handler (after cld)

;;; --------------------------------------------------
;;; Cursor Manager Calls

SetCursor  := $24          ; Set cursor definition
        ;; (input not copied)
        ;; .res 24 bitmap       2x12 byte bitmap
        ;; .res 24 mask         2x12 byte mask
        ;; .byte hotx           hotspot coords
        ;; .byte hoty

ShowCursor := $25          ; Return cursor to visibility
        ;; (no parameters; pass $0000 as address)

HideCursor := $26          ; Cursor hidden until ShowCursor call
        ;; (no parameters; pass $0000 as address)

ObscureCursor := $27         ; Cursor hidden until moved
        ;; (no parameters; pass $0000 as address)

GetCursorAddr := $28           ; Get cursor definition
        ;; (input length 0 bytes)
        ;; (output length 2 bytes)
        ;; .addr definition     See SetCursor

;;; --------------------------------------------------
;;; Event Manager Calls

CheckEvents     := $29          ; ???

GetEvent        := $2A
        ;; (input length 0 bytes)
        ;; (output length 5 bytes)
        ;; .byte state
        ;; if state is key_down:
        ;;   .byte key          (ASCII code; high bit clear)
        ;;   .byte modifiers    (0=none, 1=open-apple, 2=closed-apple, 3=both)
        ;; if state otherwise:
        ;;   .word xcoord
        ;;   .word ycoord

FlushEvents     := $2B
        ;; (no parameters; pass $0000 as address)

        ;; $2C used in DeskTop but not DAs - icon drag
        ;; (input length 0 bytes)

PeekEvent       := $4C

PostEvent       := $2D          ; Set pending input state (mouse or keyboard)
        ;; (input length 5 bytes)
        ;; .byte state
        ;; if state is key_down:
        ;;   .byte key          ASCII code; high bit clear
        ;;   .byte modifiers    0=none, 1=open-apple, 2=closed-apple, 3=both
        ;; if state otherwise:
        ;;   .word xcoord
        ;;   .word ycoord

SetKeyEvent     := $2E       ; When set, keyboard is ignored in $29 calls
        ;; (input length 1 byte)
        ;; .byte flag           high bit set = ignore keyboard, otherwise check

;;; --------------------------------------------------
;;; Menu Manager Calls

InitMenu        := $2F

SetMenu         := $30             ; Configure (and draw) menu
        ;; (input not copied)
        ;; .word count          Number of top-level menus
        ;;
        ;; .word menu_id        Menu identifier
        ;; .addr label          Address of pascal (length-prefixed) string
        ;; .addr menu_defn      Address of menu definition
        ;; .word 0,0,0          (overwritten with ???, left edge, right edge)
        ;; ...
        ;;
        ;; Menu definitions are:
        ;; .word count          Number of items in menu

        ;; Menu items are:
        ;; .word 0,0            ???
        ;; .word has_shortcut   1 if has keyboard shortcuts, 0 otherwise
        ;; .byte shortcut1      ASCII code of shortcut #1 (e.g. uppercase B); or 0
        ;; .byte shortcut2      ASCII code of shortcut #2 (e.g. uppercase b, or same); or 0
        ;; .addr label          Address of pascal (length-prefixed) string
        ;; or for a separator:
        ;; .word $40, $13, $0

MenuSelect      := $31           ; Enter modal loop for handling mouse-down on menu bar
        ;; (input not copied)
        ;; (output 2 bytes)
        ;; .byte menu_id        Top level menu identifier, or 0 if none
        ;; .byte item_num       Index (1-based) of item in menu, or 0 if none

MenuKey         := $32
HiliteMenu      := $33
DisableMenu     := $34
DisableItem     := $35
CheckItem       := $36
SetMark         := $37

;;; --------------------------------------------------
;;; Window Manager Calls

OpenWindow      := $38
        ;; (input not copied)
        ;; .byte id
        ;; .byte options                option_*
        ;; .addr title
        ;; .byte hscroll                scroll_option_*
        ;; .byte vscroll                scroll_option_*
        ;; .byte hsmax
        ;; .byte hspos
        ;; .byte vsmax
        ;; .byte vspos
        ;; .byte ???
        ;; .byte ???
        ;; .word width_a        ??? possibly size of scroll area within window of scroll area?
        ;; .word height_a       ???
        ;; .word width_b        (of scroll area?)
        ;; .word height_b       (of scroll area?)
        ;;  - next section is identical to that for SetPort
        ;; .word left           pixels from screen edge
        ;; .word top
        ;; .word screen_addr
        ;; .word screen_stride
        ;; .word hoff          pixels scrolled
        ;; .word voff
        ;; .word width          pixels
        ;; .word height
        ;; .res 8 pattern
        ;; .byte mskand         AND mask, default $FF
        ;; .byte mskor          ORA mask, default $00
        ;; .word xpos
        ;; .word ypos
        ;; .byte hthick
        ;; .byte vthick
        ;; .byte 0              ??? fill mode?
        ;; .byte tmask          AND mask for text, default $7F
        ;; .addr font
        ;; .addr next           address of next lower window in stack (filled in by call)

CloseWindow     := $39
        ;; (input length 1 byte)
        ;; .byte id

CloseAll        := $3A

GetWinPtr       := $3B         ; Get pointer to window params by id; A=0 on success
        ;; (input length 1 byte)
        ;; (output length 3 bytes)
        ;; .byte id             of window
        ;; .addr window         (out) window params

GetWinPort      := $3C     ; get drawing state of window
        ;; (input length 3 bytes)
        ;; .byte id             window
        ;; .addr state          state definition to populate, like SetPort

SetWinPort      := $3D     ; Update drawing state by passed window id
        ;; ** Implementation appears buggy - or maybe just really cryptic ??? **
        ;; (input length 2 bytes)
        ;; .byte id             window
        ;; .byte ???


BeginUpdate     := $3E
        ;; (input length 1 byte)
        ;; .byte id

EndUpdate       := $3F

FindWindow      := $40
        ;; (input length 4 bytes)
        ;; .word queryx         relative to screen
        ;; .word queryy
        ;; .byte element        (out) area_*
        ;; .byte id             (out) of window

FrontWindow     := $41            ; Get id of top window
        ;; (input length 0 bytes)
        ;; (output length 1 byte)
        ;; .byte id             (out) window, or 0 if none

SelectWindow    := $42       ; Make window topmost
        ;; (input length 1 byte)
        ;; .byte id             window

TrackGoAway     := $43
        ;; (input length 0 bytes)
        ;; .byte clicked        (out) 0 = cancelled, 1 = clicked
        ;; .byte ??             (out)
        ;; .byte ??             (out)

DragWindow      := $44
        ;; (input length 5 bytes)
        ;; .byte id             window
        ;; .word xcoord         mouse coords
        ;; .word ycoord
        ;; .byte moved          high bit set if moved, clear if not

GrowWindow      := $45
        ;; (input length 5 bytes)
        ;; .byte id             window
        ;; .word xcoord         mouse coords
        ;; .word ycoord
        ;; .byte ??             likely: moved? 0 = no change, 1 = moved

ScreenToWindow  := $46          ; Map screen coords to client coords
        ;; (input length 5 bytes)
        ;; .byte window_id
        ;; .word screenx
        ;; .word screeny
        ;; .word clientx
        ;; .word clienty

WindowToScreen  := $47

;;; --------------------------------------------------
;;; Control Manager Calls

FindControl     := $48
        ;; (input length 4 bytes)
        ;; .word xcoord
        ;; .word ycoord
        ;; .byte part           ctl_*
        ;; .byte scroll         part_*

SetCtlMax       := $49        ; ???
        ;; (input length 3 bytes)
        ;; .byte ???            maybe part (i.e. HSCROLL or VSCROLL) ???
        ;; .byte ???            width fraction ??
        ;; .byte ???

TrackThumb      := $4A
        ;; (input length 5 bytes)
        ;; .byte type           ctl_horizontal_scroll_bar or ctl_vertical_scroll_bar
        ;; .word mouse xcoord
        ;; .word mouse ycoord
        ;; .byte position       0...255
        ;; .byte moved          0 = no change, 1 = moved

UpdateThumb     := $4B
        ;; (input length 3 bytes)
        ;; .byte type           ctl_horizontal_scroll_bar or ctl_vertical_scroll_bar
        ;; .byte pos            new position 0...250
        ;; .byte ???

ActivateCtl     := $4C          ; ???

;;; $4E is last call

;;; ==================================================
;;; Graphics Primitives Constants

;;; Used in GetWinPort / SetPortBits
screen_mapbits     := $2000    ; Screen address
screen_mapwidth   := $80      ; Stride in bytes

;;; Used in SetPenMode
pencopy          := 0
notpenXOR             := 6

;;; Used in SetZP1
zp_overwrite       := 0
zp_preserve        := 1<<7

;;; Used in various state blocks
colormask_and      := $FF
colormask_or       := $00
textbg_white       := $7F

;;; ==================================================
;;; Mouse Graphics Tool Kit Constants

;;; Used in GetEvent
no_event        := 0    ; No mouse or keypress
button_down     := 1    ; Mouse button was depressed
button_up       := 2    ; Mouse button was released
key_down        := 3    ; Key was pressed
drag            := 4    ; Mouse button still down
apple_key       := 5    ; Mouse button was depressed, modifier key down

;;; Used in FindWindow
area_desktop    := 0
area_menubar    := 1
area_content    := 2            ; Includes scroll bars
area_dragbar    := 3
area_grow_box   := 4
area_close_box  := 5

;;; Used in FindControl, TrackThumb, UpdateThumb
ctl_not_a_control               := 0
ctl_vertical_scroll_bar         := 1
ctl_horizontal_scroll_bar       := 2

;;; Used in FindControl
part_up_arrow           := 1
part_left_arrow         := 1
part_down_arrow         := 2
part_right_arrow        := 2
part_page_up            := 3
part_page_left          := 3
part_page_down          := 4
part_page_right         := 4
part_thumb              := 5

;;; Used in OpenWindow
option_dialog_box       := 1 << 0
option_go_away_box      := 1 << 1
option_grow_box         := 1 << 2

scroll_option_none      := 0
scroll_option_present   := 1 << 7
scroll_option_thumb     := 1 << 6
scroll_option_active    := 1 << 0
scroll_option_normal    := scroll_option_present | scroll_option_thumb | scroll_option_active

.endscope ; MGTK

;;; ==================================================
;;; Macros

;;; Call an MGTK entry point:
;;;     MGTK_CALL n                      - params is $0000
;;;     MGTK_CALL n, params_addr
;;;     MGTK_CALL m, params_addr, label  - params_addr is labeled for modifying

.macro  MGTK_CALL    op, addr, label
        jsr MGTK::MLI
        .byte op

.if .paramcount > 2
        label := *
.endif

.if .paramcount > 1
        .addr addr
.else
        .addr 0
.endif
.endmacro

;;; ------------------------------------

.macro DEFINE_STRING str, label ; String definition, for use with TextWidth/DrawText
        .local  data            ; Call as DEFINE_STRING "abc"
        .local  end             ; Can include control chars by using:
        .addr   data            ; DEFINE_STRING {"abc",$0D,"def"}

.if .paramcount > 1
label:
.endif
        .byte   end - data
data:   .byte   str
end:
.endmacro


;;; Define pattern for PaintBits - low 7 bits are reversed
;;; e.g. .byte px(%1000000)
;;; px() has high bit clear, PX() has high bit set
.define px(bits) (((bits&$40)>>6)|((bits&$20)>>4)|((bits&$10)>>2)|(bits&$8)|((bits&$4)<<2)|((bits&$2)<<4)|((bits&$1)<<6))
.define PX(bits) (((bits&$40)>>6)|((bits&$20)>>4)|((bits&$10)>>2)|(bits&$8)|((bits&$4)<<2)|((bits&$2)<<4)|((bits&$1)<<6)|$80)
